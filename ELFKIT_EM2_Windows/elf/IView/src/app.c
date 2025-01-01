#include "app.h"

//в ELF_Entry, вторым параметром (params) идёт физический идентификатор файла (картинки)
DL_FS_MID_T f_id;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "IView";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
    { EV_KEY_PRESS,                 HandleKeyPress              },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
	{ EV_GRANT_TOKEN,		        HandleUITokenGranted        },
	{ STATE_HANDLERS_END,           NULL        			    },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_KEY_PRESS,                 HandleKeyPress              },
    { EV_DIALOG_DONE,               HandleDialogDone            },
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
    reserve = ldrInitEventHandlersTbl(main_state_handlers, reserve);

    //Save args
    memcpy(&f_id, params, sizeof(DL_FS_MID_T));

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
    app->dialog = NULL;

    APP_ExitStateAndApp(ev_st, app, 0);

	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    UINT32 result;
    WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];

    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    DL_FsGetURIFromID(&f_id, uri);
    app->dialog = UIS_CreateScreenSaver(&app->port, uri);

    if(app->dialog == 0)
    {
        app->exit_status = TRUE;
        return RESULT_FAIL;
    }

	return RESULT_OK;
}

UINT32 MainStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    APP_UtilUISDialogDelete(&app->dialog);
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

	if(status == RESULT_OK && app->token_status == 2)
		status = APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);

	return status;
}

UINT32 HandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT8 key = GET_KEY(ev_st);

	if(key == KEY_RED )
	{
	    app->exit_status = TRUE;
		APP_ConsumeEv(ev_st, app);
	}

	return RESULT_OK;
}

UINT32 HandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
	APP_ConsumeEv(ev_st, app);
	return RESULT_OK;
}
