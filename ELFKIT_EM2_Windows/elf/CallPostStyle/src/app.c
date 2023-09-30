#include "app.h"

UINT8 Calls = 0;
UINT8 HASH_KEY_MODE = 3;

BOOL RStyleHasBeenChanged = FALSE;
UINT8 CountOfMissedCalls = 0;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "CallPostStyle";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_CALL_END,                  HandleCallMissed            },
    { EV_CALL_ANSWERED,             HandleCallAnswered          },
    { EV_CHANGE_RINGSTYLE,          HandleRStyleChanged         },
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

    //Read Config
    PFprintf("%s: Load config\n", app_name);
    ReadConfig(&elf->id);

    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
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
	APP_ExitStateAndApp(ev_st, app, 0);
	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleCallMissed (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	UINT8 CurrentRStyle;
	UINT8 CurrentVolume;

	DL_DbFeatureGetCurrentState(0x5CB, &CurrentRStyle); //Get current ring style
    DL_AudGetVolumeSetting(0, &CurrentVolume);          //Get current volume level (calls)

    if(CurrentVolume < 4 || CurrentRStyle == 2 || CurrentRStyle == 4)
    {
        CountOfMissedCalls++;
    }

    if(CountOfMissedCalls >= Calls)
    {
        CountOfMissedCalls = 0;
        RStyleHasBeenChanged = TRUE;

        // 3 - vibrate & ring
        if(CurrentRStyle == 3)
        {
            AFW_CreateInternalQueuedEvAux(EV_CHANGE_RINGSTYLE, 0x65, 0, 0);
        }
        else
        {
            //Сохранить текущий стиль звонка для переключения
            DL_DbFeatureGetCurrentState(0x778, &HASH_KEY_MODE);

            //Установить стиль звонка для переключения на "Вибро и звонок"
            DL_DbFeatureStoreState(0x85A, CurrentRStyle);
            DL_DbFeatureStoreState(0x778, 0x3);
            DL_DbFeatureStoreState(0x77A, CurrentRStyle);

            //IdleHandleLongPressHashKey. This function must be enabled!
            AFW_CreateInternalQueuedEvAux(0x21AB, 0x65, 0, 0);
        }
    }
    return RESULT_OK;
}

UINT32 HandleCallAnswered (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    RStyleHasBeenChanged = FALSE;
    CountOfMissedCalls = 0;
    return RESULT_OK;
}

UINT32 HandleRStyleChanged (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(RStyleHasBeenChanged == TRUE)
    {
        CountOfMissedCalls = 0;
        RStyleHasBeenChanged = FALSE;
        DL_AudSetVolumeSetting(0, 7);

        //Восстановить стиль звонка для переключения
        DL_DbFeatureStoreState(0x778, HASH_KEY_MODE);
    }
    return RESULT_OK;
}

VOID ReadConfig (DL_FS_MID_T *id)
{
    UINT32 R;
	FILE_HANDLE_T hFile;

    //Get path to .cfg
    WCHAR CFGFile[FS_MAX_URI_NAME_LENGTH + 1] = L"file:/\0";

    DL_FsGetURIFromID(id, (CFGFile + 6));
    WCHAR *ptr = CFGFile + u_strlen(CFGFile);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"callpoststyle.cfg\0");

    //Read config
	if(DL_FsFFileExist(CFGFile))
	{
		hFile = DL_FsOpenFile(CFGFile, FILE_READ_MODE, 0);
		DL_FsReadFile(&Calls, sizeof(UINT8), 1, hFile, &R);
		DL_FsCloseFile(hFile);
	}

	if(Calls <= 0) Calls = 3;
}
