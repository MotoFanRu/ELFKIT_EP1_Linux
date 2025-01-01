#include "app.h"

BOOL bPressExit = FALSE;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "iTVolume";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit   },
    { EV_KEY_PRESS,                 HandleKeypress  },
	{ STATE_HANDLERS_END,           NULL            },
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

    elf = ela;
    elf->name = (char *)app_name;

    //Check if application is already loaded
    if(ldrIsLoaded((char *)app_name))
    {
        PFprintf("%s: Application already loaded!\n", app_name);
        return RESULT_FAIL;
    }

    //Initialize state handling table
    ldrInitEventHandlersTbl(any_state_handlers, elf->evbase);

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

    DL_AudPlayTone(1, 1);

	return RESULT_OK;
}

UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(bPressExit == TRUE)
    {
        DL_AudPlayTone(1, 1);
    	APP_ExitStateAndApp(ev_st, app, 0);
        ldrUnloadElf(elf);
        return RESULT_OK;
    }
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    bPressExit = TRUE;
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleKeypress(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT8 volume = 0;
    UINT8 Volume_id = 4;
    UINT8 runoff;

    EVENT_T *event = AFW_GetEv(ev_st);
    UIS_GetActiveDialogType(&runoff);

    if(runoff == DialogType_Dynamic_Idle)
    {
        if(event->data.key_pressed == 23 || event->data.key_pressed == 24)
        {
            if(bPressExit == FALSE) bPressExit=TRUE;
            else bPressExit=FALSE;

        }
        if(bPressExit == TRUE)
        {
            if(event->data.key_pressed == 2)
            {
                DL_AudGetVolumeSetting(Volume_id, &volume);
                if(volume < 7) volume++;
                DL_AudSetVolumeSetting(Volume_id, volume);
            }
            if(event->data.key_pressed==8)
            {
                DL_AudGetVolumeSetting(Volume_id, &volume);
                if(volume > 0) volume--;
                DL_AudSetVolumeSetting(Volume_id, volume);
            }
        }
    }

    return RESULT_OK;
}
