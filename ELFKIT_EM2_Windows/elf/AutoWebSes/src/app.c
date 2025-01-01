#include "app.h"

WCHAR u_spn[0x25];

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "AutoWebSes";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_TIMER_EXPIRED,             HandleTimer                 },
    { EV_NPN_CHANGE,                HandleNetworkStatus         },
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
    APP_UtilStopTimer(app);
	APP_ExitStateAndApp(ev_st, app, 0);

	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleTimer (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(GET_TIMER_ID(ev_st) == app->timer_handle)
    {
        APP_ConsumeEv(ev_st, app);
        Util_SetWebSession(app);
    }
    return RESULT_OK;
}

UINT32 HandleNetworkStatus (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EVENT_T *event = AFW_GetEv(ev_st);
    WCHAR *attach = (WCHAR *)event->attachment;

    if(attach && u_strlen(attach))
    {
        u_strcpy(u_spn, attach);
        Util_SetWebSession(app);
    }

    return RESULT_OK;
}

UINT32 Util_SetWebSession (APPLICATION_T *app)
{
    UINT8 index;
    UINT32 status;

    char a_spn[0x25];
    u_utoa(u_spn, a_spn);

    status = DL_DbWebSessionsFindAbsIndexByName(u_spn, &index, 4);
    PFprintf("%s: DL_DbWebSessionsFindAbsIndexByName -> spn=%s, index=%d, status=%d\n", app_name, a_spn, index, status);

    if(status == 0 || status == 1)
    {
		if (DL_DbWebSessionsSetDefaultSessionIndex(&app->port, index) == 1)
		{
			PFprintf("%s: DL_DbWebSessionsSetDefaultSessionIndex(%d) success\n", app_name, index);
		}
		else
		{
			PFprintf("%s: DL_DbWebSessionsSetDefaultSessionIndex(%d) fail\n", app_name, index);
		}
    }
    else
    {
        if (status == 2)
        {
            APP_UtilStopTimer(app);
            APP_UtilStartTimer(5*1000, 1, app);
        }
        else
        {
            if (status == 4)
            {
                PFprintf("%s: FindAbsIndex: spn not found\n", app_name);
            }
            else
            {
                PFprintf("%s: DL_DbWebSessionsFindAbsIndexByName(%s) fail\n", app_name, a_spn);
            }
        }
    }

    return status;
}
