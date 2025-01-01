#include "app.h"

BLACKLIST_CONFIG config = {0,0,0};
BOOL checked = FALSE;

VOID *attach = NULL;
UINT16 att_size = 0;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "BlackList";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_CALL_RECEIVED,             HandleCallReceived          },
    { EV_PBK_READ_RECORD,           HandlePBKReadRecord         },
	{ STATE_HANDLERS_END,           NULL                        },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { APP_STATE_ANY,
      NULL,
      NULL,
      any_state_handlers
    }
};

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params)
{
	UINT32 status = RESULT_OK;
    UINT32 reserve;

    elf = ela;
    elf->name = (char *)app_name;

    //Check if application is already loaded
    if(ldrIsLoaded((char *)app_name))
    {
        PFprintf("%s: Application already loaded!\n", app_name);
        elf = ldrFindElf((char *)app_name);
        ldrSendEvent(elf->evbase);
        return RESULT_FAIL;
    }

    //Initialize state handling table
    reserve = elf->evbase + 1;
    reserve = ldrInitEventHandlersTbl(any_state_handlers, reserve);

    //Register application
    status = APP_Register(&elf->evbase, 1, state_handling_table, APP_STATE_MAX, (void*)ELF_Start);
    if(status == RESULT_OK)
    {
        PFprintf("%s: Application has been registered successfully!\n", app_name);

        //Run application immediatly
        ldrSendEvent(elf->evbase);
    }
    else
    {
        PFprintf("%s: Can't register application!\n", app_name);
    }

	return status;
}

UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info)
{
    APP_T *app = NULL;
    UINT32 status = RESULT_OK;

    Util_LoadConfig();

    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
		PFprintf("%s: Config has been reloaded...\n", app_name);
		return RESULT_OK;
	}

	//Initialize application
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEventPrepost, sizeof(APP_T), reg_id, 0, 1, 1, 2, 0, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Starting application
    status = APP_Start(ev_st, &app->apt, APP_STATE_ANY, state_handling_table, ELF_Exit, app_name, 0);
    if(status == RESULT_OK)
    {
        PFprintf("%s: Application has been started successfully!\n", app_name);
    }
    else
    {
		PFprintf("%s: Can't start application!\n", app_name);
		APP_HandleFailedAppStart(ev_st, (APPLICATION_T*)app, 0);
		ldrUnloadElf(elf);
		return RESULT_OK;
    }

	return RESULT_OK;
}

UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    free(attach);
	APP_ExitStateAndApp(ev_st, app, 0);
	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleCallReceived (EVENT_STACK_T* ev_st, APPLICATION_T *app)
{
    PFprintf("%s: EV_CALL_RECEIVED\n", app_name);

    //Если режим "не блокировать"
    if(config.type == BLACKLIST_TYPE_NONE)
        return RESULT_OK;

    EVENT_T *ev = (EVENT_T *)AFW_GetEv(ev_st);
	CALL_ATT_T *att = (CALL_ATT_T *)ev->attachment;

	IFACE_DATA_T if_data;
	if_data.port = app->port;

	if(checked == TRUE)
	{
		checked = FALSE;
		return RESULT_OK;
	}

    //Если режим "Блокировать все"
    if(config.type == BLACKLIST_TYPE_ALL)
    {
        APP_ConsumeEv(ev_st, app);
        DL_SigCallReleaseReq(&if_data, att->call.call_id, config.cause);
        Util_BlackListLogger(att->number);
        return RESULT_OK;
    }

    //Размер аттача EV_CALL_RECEIVED
    att_size = ev->att_size;

    //Выделяем память для аттача
	if(attach != NULL) free(attach);
	attach = malloc(att_size);
	if(attach == NULL) return RESULT_OK;

    //сохраняем аттач ивента EV_CALL_RECEIVED
	memcpy(attach, ev->attachment, att_size);


    //Ищем запись в справочнике
	APP_ConsumeEv(ev_st, app);
	DL_DbPhoneBookFindRecordByTelno(&if_data, 0, att->number, 0);

	return RESULT_OK;
}

UINT32 HandlePBKReadRecord (EVENT_STACK_T* ev_st, APPLICATION_T *app)
{
    PFprintf("%s: EV_PBK_READ_RECORD\n", app_name);

    checked = TRUE;

	EVENT_T *ev = AFW_GetEv(ev_st);
	if(ev->att_size == 0) return RESULT_FAIL;

    IFACE_DATA_T if_data;
	if_data.port = app->port;

    CALL_ATT_T *att = (CALL_ATT_T *)attach;
	PHONEBOOK_RECORD_T *PBKRecord = (PHONEBOOK_RECORD_T*)ev->attachment;

    //Если режим "Блокировать категорию"
    if(config.type == BLACKLIST_TYPE_CATEGORY)
    {
        if(PBKRecord->group == config.group)
        {
            checked = FALSE;
            APP_ConsumeEv(ev_st, app);
            DL_SigCallReleaseReq(&if_data, att->call.call_id, config.cause);
            Util_BlackListLogger(PBKRecord->number);

            //костыль для FunLight
            AFW_AddEvNoD(ev_st, EV_CALL_END);
            return RESULT_OK;
        }
    }

	AFW_CreateInternalQueuedEvAux(EV_CALL_RECEIVED, FBF_LEAVE, att_size, attach);
	return RESULT_OK;
}

UINT32 Util_LoadConfig (DL_FS_MID_T *id)
{
	//Read BlackList.cfg
    UINT32 R;
	FILE_HANDLE_T hFile;

    //Read config
	if(DL_FsFFileExist(BLACKLIST_CONFIG_FILE_URI))
	{
		hFile = DL_FsOpenFile(BLACKLIST_CONFIG_FILE_URI, FILE_READ_MODE, 0);
		DL_FsReadFile(&config, sizeof(BLACKLIST_CONFIG), 1, hFile, &R);
		DL_FsCloseFile(hFile);

        if(config.type > BLACKLIST_TYPE_CATEGORY) config.type = default_config.type;
        if(config.group > 9) config.group = default_config.group;
	}
	else
	{
	    memcpy(&config, &default_config, sizeof(BLACKLIST_CONFIG));
	}

    PFprintf("%s: Config loaded. type=%d group=%d cause=%d\n", app_name, config.type, config.group, config.cause);
	return RESULT_OK;
}

UINT32 Util_BlackListLogger (WCHAR *number)
{
    char *ptr, line[100];

	UINT32 Written = 0;
	FILE_HANDLE_T hFile;

	hFile = DL_FsOpenFile(BLACKLIST_LOG_FILE_URI, FILE_APPEND_PLUS_MODE, NULL);
	if(hFile == FILE_HANDLE_INVALID) return RESULT_FAIL;

    ptr = line;
    u_utoa(number, ptr);

    CLK_TIME_T time;
    CLK_DATE_T date;
    DL_ClkGetTime(&time);
    DL_ClkGetDate(&date);

    ptr = line + strlen(line);
    sprintf(ptr, " %d:%02d:%02d %d.%02d.%04d\n\0", time.hour, time.minute, time.second, date.day, date.month, date.year);

	DL_FsWriteFile(line, strlen(line), 1, hFile, &Written);
	DL_FsCloseFile(hFile);

	return RESULT_OK;
}
