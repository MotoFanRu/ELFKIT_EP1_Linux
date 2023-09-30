#include "app.h"
#include "icons.h"

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "MultiFont";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
	{ EV_GRANT_TOKEN,		        HandleUITokenGranted        },
	{ STATE_HANDLERS_END,           NULL        			    },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_DONE,                      HandleReqExit               },
    { EV_DIALOG_DONE,               HandleReqExit               },
    { EV_REQUEST_LIST_ITEMS,        HandleListReq               },
    { EV_SELECT,                    HandleSelect                },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { APP_STATE_ANY,
      NULL,
      NULL,
      any_state_handlers
    },

    { APP_STATE_INIT,
      NULL,
      NULL,
      init_state_handlers
    },

    { APP_STATE_MAIN,
      MainStateEnter,
      MainStateExit,
      main_state_handlers
    }
};

//iTunes item resources
UINT32 iTunes_EvCdResID = 0x14000076;
UINT32 iTunes_Resource[] = { 0x000C0CC0, LANG_FONT_TYPE, 0x01000674, 0x120007cd, 0x00000000, 0x120007ce, 0x120007cc };

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params)
{
	UINT32 status = RESULT_OK;

    elf = ela;
    elf->name = (char *)app_name;

    //Check if application is already loaded
    if(ldrIsLoaded((char *)app_name))
    {
        PFprintf("%s: Application already loaded!\n", app_name);
        return RESULT_FAIL;
    }

    //Set new evcode
    elf->evbase = iTunes_Resource[0];

    //Register application
    status = APP_Register(&elf->evbase, 1, state_handling_table, APP_STATE_MAX, (void*)ELF_Start);
    if(status == RESULT_OK)
    {
        PFprintf("%s: Application has been registered successfully!\n", app_name);

        //Set new resources
        DRM_SetResource(iTunes_EvCdResID, iTunes_Resource, sizeof(UINT32)*sizeof(iTunes_Resource));
        DRM_SetResource(iTunes_Resource[3], (void*)FONT_TYPE_ICON_LIST, 1106);
        DRM_SetResource(iTunes_Resource[5], (void*)FONT_TYPE_ICON_STAT, 2215);
        DRM_SetResource(iTunes_Resource[6], (void*)FONT_TYPE_ICON_ANI , 2529);

        //Set font
        UINT32 current_font_id = 0;
        ReadCfg(&current_font_id);
        SetFont(&current_font_id);
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

    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
		return RESULT_OK;
	}

	//Initialize application
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Initialize application resources
    InitApp(&app->apt);

    //Starting application
    status = APP_Start(ev_st, &app->apt, APP_STATE_INIT, state_handling_table, ELF_Exit, app_name, 0);
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
	APP_ExitStateAndApp(ev_st, app, 0);
	return RESULT_OK;
}

UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    APP_T *papp = (APP_T *)app;

    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    ReadCfg(&papp->current_font_id);
    Update(ev_st, app, 0);

	return RESULT_OK;
}

UINT32 MainStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    APP_T *papp = (APP_T *)app;

	device_Free_mem_fn(papp->fonts);
    APP_UtilUISDialogDelete(&app->dialog);

	return RESULT_OK;
}


UINT32 Update (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem)
{
    APP_T *papp = (APP_T *)app;
    UINT32 starting_num = 0;

    if (app->dialog != NULL)
        APP_UtilUISDialogDelete(&app->dialog);

    app->dialog = UIS_CreateList(&app->port, 0, papp->entries_num, 0, &starting_num, 0, 2, NULL, papp->caption);
    if(app->dialog == 0) return RESULT_FAIL;

    if (sItem != 0)
    {
        APP_UtilAddEvUISListChange(ev_st, app, 0, sItem, papp->entries_num, TRUE, 2, FBF_LEAVE, NULL, NULL);
        UIS_HandleEvent(app->dialog, ev_st);
    }

    SendListItems(ev_st, app, 1, starting_num);
    return RESULT_OK;
}

UINT32 HandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	UINT32 start, num;

	if(!app->focused) return RESULT_OK;
	EVENT_T	*event = AFW_GetEv(ev_st);

	start = event->data.list_items_req.begin_idx;
	num = event->data.list_items_req.count;

	APP_ConsumeEv(ev_st, app);
	return SendListItems( ev_st, app, start, num );
}

UINT32 HandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
    EVENT_T *event = AFW_GetEv(ev_st);

    APP_ConsumeEv(ev_st, app);
    papp->current_font_id = event->data.index - 1;

    WriteCfg(&papp->current_font_id);
    SetFont(&papp->current_font_id);

    return Update(ev_st, app, papp->current_font_id + 1);
}

UINT32 SendListItems (EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
	APP_T *papp = (APP_T *)app;
	UINT32 i, index, status=RESULT_OK;

	if(num == 0) return RESULT_FAIL;

	LIST_ENTRY_T *plist = (LIST_ENTRY_T*)device_Alloc_mem(num, sizeof(LIST_ENTRY_T));
	if(plist == NULL) return RESULT_FAIL;

	for(index=0, i=start; (i<start+num) && (i<=papp->entries_num); i++, index++)
	{
		plist[index].editable = FALSE;
		plist[index].content.static_entry.formatting = 1;

        UIS_MakeContentFromString(((papp->current_font_id == (i - 1)) ? "q0Sp1" : "q0"), &(plist[index].content.static_entry.text), papp->fonts[i - 1]->font_name, DRM_SELECTED);
	}

  	status = APP_UtilAddEvUISListData(ev_st, app, 0, start, num, FBF_LEAVE, sizeof(LIST_ENTRY_T) * num, plist);
	if( status != RESULT_FAIL )
		status = UIS_HandleEvent(app->dialog, ev_st);

	device_Free_mem_fn(plist);
	return status;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	if(AFW_GetEvCode(ev_st) == EV_DONE)
		APP_ConsumeEv(ev_st, app);

    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT32 status = APP_HandleUITokenGranted(ev_st, app);

	if(status == RESULT_OK && app->token_status == 2)
		status = APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);

	return status;
}

///Utils
UINT32 InitApp (APPLICATION_T *app)
{
    UINT8 i;
    APP_T *papp = (APP_T *)app;
    FONT_TABLE_DESCRIPTOR_T *descriptor;

    if(papp == NULL) return RESULT_FAIL;

    //Заголовок списка
    papp->caption = LANG_FONT_TYPE;

    //Получаем кол-во шрифтов
	descriptor = (FONT_TABLE_DESCRIPTOR_T *)(CG4 + 0x23c);
	__rt_memcpy(&i, (void *)descriptor->end_descriptor_address, 1);
	papp->entries_num = i;

    //Выделяем память для списка шрифтов
    papp->fonts = (FONT_ENTRY_T **)device_Alloc_mem(papp->entries_num, sizeof(FONT_ENTRY_T *));
    if(papp->fonts == NULL) return RESULT_FAIL;

    //Заполняем список шрифтов
    for(i=0; i<papp->entries_num; i++)
        papp->fonts[i] = (FONT_ENTRY_T *)(descriptor->start_address + (i * sizeof(FONT_ENTRY_T)));

    //Получаем индекс текущего шрифта
    papp->current_font_id = 0;
    ReadCfg(&papp->current_font_id);

	return RESULT_OK;
}

UINT32 SetFont (UINT32 *font_id)
{
    UINT8 index;
    UINT8 *TABLE = (UINT8 *)FONT_TABLE_RAM_ADDR;
    FONT_TABLE_DESCRIPTOR_T *descriptor;

    //Копируем таблицу шрифтов в RAM
    if(font_id == NULL) return RESULT_FAIL;
    __rt_memcpy((void *)FONT_TABLE_RAM_ADDR, (void *)FONT_TABLE, FONT_TABLE_SIZE);

    //Проверяем индекс шрифта. Если он больше чем шрифтов в лэнге, выбираем первый шрифт (индекс = 0)
	descriptor = (FONT_TABLE_DESCRIPTOR_T *)(CG4 + 0x23c);
	__rt_memcpy(&index, (void *)descriptor->end_descriptor_address, 1);
	if(*font_id > index) *font_id = 0;

    //Изменяем таблицу шрифтов
    for(index=0; index<FONT_TABLE_SIZE; index+=6)
        TABLE[index] = *font_id;

    DRM_GetCurrentLanguage(&index);
    UIS_SetLanguage(index);

    return RESULT_OK;
}

UINT32 ReadCfg (UINT32 *font_id)
{
    UINT32 fSize, r;
    FILE_HANDLE_T hFile;

    if(font_id == NULL) return RESULT_FAIL;

    hFile = DL_FsOpenFile(FONT_CONFIG_URI, FILE_READ_MODE, 0);
    fSize = DL_FsGetFileSize(hFile);

    if(fSize <= 0 || hFile == FILE_HANDLE_INVALID)
    {
        DL_FsCloseFile(hFile);
        return RESULT_FAIL;
    }

    DL_FsReadFile(font_id, sizeof(UINT32), 1, hFile, &r);
    DL_FsCloseFile(hFile);

    return RESULT_OK;
}

UINT32 WriteCfg (UINT32 *font_id)
{
    UINT32 r;
    FILE_HANDLE_T hFile;

    if(font_id == NULL) return RESULT_FAIL;

    hFile = DL_FsOpenFile(FONT_CONFIG_URI, FILE_WRITE_MODE, 0);
    if(hFile == FILE_HANDLE_INVALID) return RESULT_FAIL;

    DL_FsWriteFile(font_id, sizeof(UINT32), 1, hFile, &r);
    DL_FsCloseFile(hFile);

    return RESULT_OK;
}
