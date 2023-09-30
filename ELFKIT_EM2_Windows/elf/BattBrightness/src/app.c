#include "app.h"

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "BBrightness";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_BATTERY_LOW,               HandleBatteryLow            },
    { EV_BATTERY_POWER_CHANGE,      HandleBatteryPowerChange    },
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

    //Load config
    PFprintf("%s: Loading config...\n", app_name);
    Util_ReadConfig(&elf->id, &app->cfg);

    //Checking battery status
    HandleBatteryPowerChange(ev_st, &app->apt);

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
    HandleBatteryPowerChange(ev_st, app);
	APP_ExitStateAndApp(ev_st, app, 0);
	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleBatteryLow (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    PFprintf("%s: HandleBatteryLow\n", app_name);

    //Change brightness for low battery
    UIS_SetBacklightWithIntensity(0xFF, thisapp(app)->cfg.low_brightness_value);
    Util_SetDispBrightness(thisapp(app)->cfg.low_brightness_value);
    DAL_EnableDisplay(0);

    return RESULT_OK;
}

UINT32 HandleBatteryPowerChange (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    PFprintf("%s: HandleBatteryPowerChange\n", app_name);

    //Set high value of brightness if charger is attached
    if(DL_AccIsExternalPowerActive() && DL_PwrGetChargingMode() != 0)
    {
        UIS_SetBacklightWithIntensity(0xFF, thisapp(app)->cfg.high_brightness_value);
        Util_SetDispBrightness(thisapp(app)->cfg.high_brightness_value);
    }
    else
    {
        //Set low value of brightness if charger is not attach & battery is low
        if(DL_PwrGetLowBatteryStatus())
        {
            UIS_SetBacklightWithIntensity(0xFF, thisapp(app)->cfg.low_brightness_value);
            Util_SetDispBrightness(thisapp(app)->cfg.low_brightness_value);
        }
    }

    DAL_EnableDisplay(0);
    return RESULT_OK;
}

UINT32 Util_ReadConfig (DL_FS_MID_T *id, CONFIG_T *cfg)
{
    UINT32 r;
    FILE_HANDLE_T hFile;
    WCHAR *ptr, uri[FS_MAX_URI_NAME_LENGTH + 1];

    //Get path to config
    u_strcpy(uri, L"file:/");
    DL_FsGetURIFromID(id, (uri + 6));

    ptr = uri + u_strlen(uri);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr, L"/config.cfg");

    //Set default values
    memcpy(cfg, &default_cfg, sizeof(CONFIG_T));

    //Read config
    hFile = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if(hFile != FILE_HANDLE_INVALID)
    {
        DL_FsReadFile(cfg, 1, 3, hFile, &r);
        DL_FsCloseFile(hFile);
    }

    //Check values
    if(cfg->use_high_value_from_phone) Util_GetDispBrightness(&cfg->high_brightness_value);
    if(cfg->low_brightness_value > 6) cfg->low_brightness_value = 6;
    if(cfg->high_brightness_value > 6) cfg->high_brightness_value = 6;


    PFprintf("%s: low_brightness_value = %d, high_brightness_value = %d, use_high_value_from_phone = %d\n", app_name, cfg->low_brightness_value, cfg->high_brightness_value, cfg->use_high_value_from_phone);
    return RESULT_OK;
}

UINT32 Util_GetDispBrightness (UINT8 *Bval)
{
    PFprintf("%s: Util_GetDispBacklight\n", app_name);
	if(Bval == NULL) return RESULT_FAIL;

	UINT8 data[0x10];
	DL_DbFeatureGetBlock(ID_DISP_BACKLIGHT, data);
	*Bval = data[7];

	return RESULT_OK;
}

UINT32 Util_SetDispBrightness (UINT8 Bval)
{
    PFprintf("%s: Util_SetDispBacklight\n", app_name);
	if(Bval > 6) return RESULT_FAIL;

	UINT8 data[0x10];
	DL_DbFeatureGetBlock(ID_DISP_BACKLIGHT, data);

	data[7] = Bval;
	DL_DbFeatureStoreBlock(ID_DISP_BACKLIGHT, data, 0x10);

	return RESULT_OK;
}
