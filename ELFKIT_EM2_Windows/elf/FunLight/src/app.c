#include "app.h"

UINT8 InCall;
UINT32 Count;

UINT32 *THandles;
FNL_Record *FNLRecords;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "FunLight";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_KEY_PRESS,                 HandleKeyPress              },
    { EV_TIMER_EXPIRED,             HandleTimer                 },
    { EV_CALL_RECEIVED,             HandleCallReceived          },
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

    //Read config
    if(Util_ReadConfig(&elf->id) != 0)
        return RESULT_FAIL;

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
    //Stop all timers
    Util_StopTimers(app);

    //Free memory
    device_Free_mem_fn(THandles);
    device_Free_mem_fn(FNLRecords);

    //Exit app
	APP_ExitStateAndApp(ev_st, app, 0);

	//Unload elf
	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleCallReceived (EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 i;
    InCall = 1;

    IFACE_DATA_T IFDATA;
    IFDATA.port = app->port;

    //Create timers
    for(i=0; i<Count; i++)
    {
        if(THandles[i] == 0)
        {
            DL_ClkStartCyclicalTimer(&IFDATA, FNLRecords[i].period, i);
            THandles[i] = IFDATA.handle;
        }
    }

    return RESULT_OK;
}

UINT32 HandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(InCall == 1)
    {
        UINT8 key = GET_KEY(ev_st);
        if(key == KEY_VOL_UP || key == KEY_VOL_DOWN)
        {
            InCall = 0;
            Util_StopTimers(app);
            Util_ResetState();
        }
    }
    return RESULT_OK;
}

UINT32 HandleTimer (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 id, handle;
    CALL_STATES_T CallStates;

    //Check incoming call
    if(InCall == 0)
        return RESULT_OK;

    //Check timer
    handle = AFW_GetEvSeqn(ev_st);
    if(!Util_IsFnLTimer(handle))
        return RESULT_OK;

    //Get incoming call state
    DL_SigCallGetCallStates(&CallStates);
    if(CallStates.number_of_calls == 0 || CallStates.overall_call_state == 2 || CallStates.overall_call_state == 16)
    {
        if(InCall == 1)
        {
            InCall = 0;
            Util_StopTimers(app);
            Util_ResetState();
        }
        return RESULT_OK;
    }

    //Get timer ID
    id = GET_TIMER_ID(ev_st);
    FNLRecords[id].state = !FNLRecords[id].state;

    if(FNLRecords[id].events & FNL_FLASH)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_FLASHLIGHT);

    if(FNLRecords[id].events & FNL_KEYPAD)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_KEYPAD);

    if((FNLRecords[id].events & FNL_DISPLAY) && flip_state == 1)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_DISPLAY);

    if((FNLRecords[id].events & FNL_CLI) && flip_state == 0)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_CLI);

    if(FNLRecords[id].events & FNL_LED)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_CHARGING_LED);

    if(FNLRecords[id].events & FNL_KPTOP)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_KEYPAD_TOP);

    if(FNLRecords[id].events & FNL_KPBOT)
        if(THandles[id] != 0)
            ll_call(FNLRecords[id].state, FUNCTION_KEYPAD_BOTTOM);

    return RESULT_OK;
}

UINT32 Util_IsFnLTimer (UINT32 timer_handle)
{
    UINT32 i;

    if(THandles != NULL)
        for(i=0; i<Count; i++)
            if(THandles[i] == timer_handle)
                return 1;

    return 0;
}

UINT32 Util_StopTimers (APPLICATION_T *app)
{
    UINT32 i;

    IFACE_DATA_T if_data;
    if_data.port = app->port;

    if(THandles != NULL)
    {
        for(i=0; i<Count; i++)
        {
            if(THandles[i] != 0)
            {
                if_data.handle = THandles[i];
                DL_ClkStopTimer(&if_data);
            }
        }
        memset(THandles, 0, Count*sizeof(UINT32));
    }

    return RESULT_OK;
}

UINT32 Util_ReadConfig (DL_FS_MID_T *id)
{
    UINT32 Head, fSize, R;
    FILE_HANDLE_T f;
    WCHAR config[FS_MAX_URI_NAME_LENGTH + 1] = L"file:/\0";

    //Get path to .cfg
    DL_FsGetURIFromID(id, (config + 6));
    WCHAR *ptr = config + u_strlen(config);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"funlight.pat\0");

    //Open file
    f = DL_FsOpenFile(config, FILE_READ_MODE, 0);
    if(f == FILE_HANDLE_INVALID) return 1;

    //Get file size
    fSize = DL_FsGetFileSize(f);
    if(fSize <= 0)
    {
        DL_FsCloseFile(f);
        return 2;
    }

    Count = (fSize - sizeof(FNL_Header)) / sizeof(FNL_Record);
    if(Count <= 0)
    {
        DL_FsCloseFile(f);
        return 3;
    }

    DL_FsReadFile(&Head, sizeof(FNL_Header), 1, f, &R);
    if(Head != 0x464E4C01) //FNL(01)
    {
        DL_FsCloseFile(f);
        return 4;
    }

    //Memory allocation!
    FNLRecords  = (FNL_Record *)device_Alloc_mem(Count, sizeof(FNL_Record));
    THandles = (UINT32*)device_Alloc_mem(Count, sizeof(UINT32));

    if(FNLRecords == NULL || THandles == NULL)
    {
        device_Free_mem_fn(FNLRecords);
        device_Free_mem_fn(THandles);
        DL_FsCloseFile(f);
        return 5;
    }

    //Read config
    DL_FsReadFile(FNLRecords, Count, sizeof(FNL_Record), f, &R);
    DL_FsCloseFile(f);
    return 0;
}

void Util_ResetState (void)
{
    ll_call(STATE_OFF, FUNCTION_FLASHLIGHT);
    ll_call(STATE_OFF, FUNCTION_CHARGING_LED);

    if(flip_state == 1)
    {
        ll_call(STATE_OFF, FUNCTION_CLI);
        ll_call(STATE_ON, FUNCTION_DISPLAY);
        ll_call(STATE_ON, FUNCTION_KEYPAD);
        DAL_EnableDisplay(0);
    }
    else
    {
        ll_call(STATE_OFF, FUNCTION_DISPLAY);
        ll_call(STATE_OFF, FUNCTION_KEYPAD);
        ll_call(STATE_ON, FUNCTION_CLI);
    }
}
