#include "app.h"

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "FlashLight";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_KEY_PRESS,                 HandleKeyPress              },
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

    //Initialize application data
    app->state = 0;
    app->counter = 0;
    app->keycode = 0;

    //Read config
    Util_ReadConfig(&elf->id, &app->keycode);
    PFprintf("%s: keycode = '%d'\n", app_name, app->keycode);

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

UINT32 HandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT8 key  = GET_KEY(ev_st);

    if(key == thisapp(app)->keycode) //Проверяем клавишу
    {
        thisapp(app)->counter++;
    }
    else
    {
        thisapp(app)->counter = 0;
    }

    if(thisapp(app)->counter == 2)   //Проверяем кол-во нажатий
    {
        thisapp(app)->counter = 0;

        if(thisapp(app)->state == 0) //Включаем вспышку
        {
            thisapp(app)->state = 1;
            set_nvidia_flash_control(1);
        }
        else                        //Выключаем вспышку
        {
            thisapp(app)->state = 0;
            set_nvidia_flash_control(0);
        }
    }

    return RESULT_OK;
}

UINT32 Util_ReadConfig (DL_FS_MID_T *id, UINT8 *keycode)
{
    UINT32 r, fSize;
    FILE_HANDLE_T hFile;
    WCHAR *ptr, uri[FS_MAX_URI_NAME_LENGTH + 1];

    u_strcpy(uri, L"file:/");
    DL_FsGetURIFromID(id, (uri + 6));

    ptr = uri + u_strlen(uri);
    while(*ptr != L'/') ptr--;

    ptr++;
    u_strcpy(ptr, L"flashlight.cfg");

    hFile = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    fSize = DL_FsGetFileSize(hFile);

    if(hFile == FILE_HANDLE_INVALID || fSize <= 0)
    {
        PFprintf("%s: flashlight.cfg not found!\n", app_name);
        DL_FsCloseFile(hFile);
        return RESULT_FAIL;
    }

    DL_FsReadFile(keycode, sizeof(UINT8), 1, hFile, &r);
    DL_FsCloseFile(hFile);
    return RESULT_OK;
}

