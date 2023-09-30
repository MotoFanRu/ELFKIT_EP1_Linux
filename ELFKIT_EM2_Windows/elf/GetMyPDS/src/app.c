#include "app.h"

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "GetMyPDS";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { EV_GRANT_TOKEN,               HandleUITokenGranted        },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_DIALOG_DONE,               HandleReqExit               },
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
      NULL,
      main_state_handlers
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
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

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
    APP_UtilUISDialogDelete(&app->dialog);
	APP_ExitStateAndApp(ev_st, app, 0);
	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32 status = APP_HandleUITokenGranted(ev_st, app);

    if( (status == RESULT_OK) && (app->token_status == 2) )
        status = APP_UtilChangeState( APP_STATE_MAIN, ev_st, app );

    return status;
}

UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    CONTENT_T content;
    UIS_DIALOG_T dialog = 0;

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;
    //Backup
    //Generate output name
    WCHAR pds[FS_MAX_URI_NAME_LENGTH] = L"file:/";
	const WCHAR done[] = L"PDS Saved to ";

    DL_FsGetURIFromID(&elf->id, (pds + 6));
    WCHAR *ptr = pds + u_strlen(pds);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"pds.bak\0");
    //Backup PDS
    UINT32 w;
    FILE_HANDLE_T hFile = DL_FsOpenFile(pds, FILE_WRITE_PLUS_MODE, NULL);
    DL_FsWriteFile((char *)PDS_AREA, PDS_SIZE, 1, hFile, &w);
    DL_FsCloseFile(hFile);

    //Create notice
    UIS_MakeContentFromString("MCq0q1", &content, done, pds);
    dialog = UIS_CreateTransientNotice(&app->port, &content, NOTICE_TYPE_OK);

    if(dialog == 0) return RESULT_FAIL;
    app->dialog = dialog;

	return RESULT_OK;
}
