#include "app.h"

Config Cfg = {0, 0, 0};
FILE_HANDLE_T LogFile = NULL;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "EventsLog";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_KEY_PRESS,                 HandleKeypress              },
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

    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
		return RESULT_OK;
	}

	//Initialize application
	app = (APP_T*)APP_InitAppData((void *)HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 2, 0, 0);
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

        //Read Config
        Util_ReadConfig(&elf->id);
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
    if(Cfg.UseFile) Util_CloseLog(LogFile);
	APP_ExitStateAndApp(ev_st, app, 0);
	ldrUnloadElf(elf);
	return RESULT_OK;
}

void HandleEvent (EVENT_STACK_T *ev_st, APPLICATION_T *app, APP_ID_T appid, REG_ID_T regid)
{
	EVENT_T  *event = AFW_GetEv(ev_st);

	if (event->code >= Cfg.F1 && event->code <= Cfg.F2)
	{
	    char string[50];
	    sprintf(string, "ev_code=0x%08X, ev_att_size=%d b\n", event->code, event->att_size);

	    PFprintf("%s", string);
	    if(Cfg.UseFile)
        {
            strcat(string, "\r");
            Util_SendLog(string);
        }
	}

    APP_HandleEventPrepost(ev_st, app, appid, regid);
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleKeypress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    char string[20];
    UINT8 key_code = GET_KEY(ev_st);

    sprintf(string, "Code Keypress=%d\n\n", key_code);
	PFprintf("%s", string);

	if(Cfg.UseFile)
	{
        strcat(string, "\r");
        Util_SendLog(string);
	}

    if(key_code == KEY_STAR)
        app->exit_status = TRUE;

	return RESULT_OK;
}

UINT32 Util_ReadConfig (DL_FS_MID_T *id)
{
	UINT32 R;
	FILE_HANDLE_T hFile;

    //Get path to .cfg
    WCHAR CFGFile[FS_MAX_URI_NAME_LENGTH + 1] = L"file:/\0";

    DL_FsGetURIFromID(id, (CFGFile + 6));
    WCHAR *ptr = CFGFile + u_strlen(CFGFile);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"eventslog.cfg\0");

    //Read config
	if(DL_FsFFileExist(CFGFile))
	{
		hFile = DL_FsOpenFile(CFGFile, FILE_READ_MODE, 0);
		DL_FsReadFile(&Cfg, sizeof(Config), 1, hFile, &R);
		DL_FsCloseFile(hFile);

		PFprintf("Filter1 = 0x%x\nFilter2 = 0x%x\nUseFile = %d\n\n", Cfg.F1, Cfg.F2, Cfg.UseFile);
	}

    CFGFile[u_strlen(CFGFile)-3] = 0;
    u_strcat(CFGFile, L"log");

    if(Cfg.UseFile) Util_OpenLog(CFGFile);
    return RESULT_OK;
}

UINT32 Util_OpenLog (WCHAR *uri)
{
	DL_FsDeleteFile(uri,  0);
	LogFile = DL_FsOpenFile(uri, FILE_WRITE_MODE, 0);
	return RESULT_OK;
}

UINT32 Util_SendLog (char *str)
{
	UINT32 written;
	DL_FsWriteFile((void *)str, strlen(str), 1, LogFile, &written);
	return RESULT_OK;
}
