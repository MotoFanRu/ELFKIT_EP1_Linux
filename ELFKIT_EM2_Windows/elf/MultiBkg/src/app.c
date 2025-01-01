#include "app.h"

FILEINFO*	flist    = NULL;
UINT32		bkgs     = 0;
UINT32		time	 = 0;
UINT32		current  = 0;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "MultiBkg";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit   },
	{ EV_TIMER_EXPIRED,	            HandleTimer     },
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
    PFprintf("%s: Loading configs...\n", app_name);
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
        if(ListBkgs(&app->apt) == RESULT_FAIL) app->apt.exit_status = TRUE;
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
    device_Free_mem_fn(flist);
	APP_Exit(ev_st, app, 0);
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
	if(AFW_GetEvSeqn(ev_st) == app->timer_handle)
	{
	    SETWALLPAPER_SETTINGS_T wp;

        //fit image to screen
	    wp.layout = WALLPAPER_LAYOUT_FIT_TO_SCREEN;
	    wp.url = flist[current].image_uri;

        //set wallpaper
		UIS_SetWallpaper(&wp);

		if(current < bkgs-1) current++;
		else current=0;

		APP_UtilStopTimer(app);
		APP_UtilStartTimer(time, 0, app);
	}

    return RESULT_OK;
}

UINT32 ListBkgs (APPLICATION_T *app)
{
	UINT32 i;
	FS_SEARCH_PARAMS_T fs_param;
	FS_SEARCH_HANDLE_T fs_handle;
	FS_SEARCH_RESULT_T fs_result;

	WCHAR filter[] = L"file://e/mobile/picture/multi/\xFFFE*.jpg\xFFFE*.png\xFFFE*.gif\xFFFE*.bmp";

	UINT16 res_count, count = 1;
	fs_param.flags = 0x1C;
	fs_param.attrib = 0x0000;
	fs_param.mask = 0x0000;

	DL_FsSSearch(fs_param, filter, &fs_handle, &res_count, 0x0);

	bkgs = res_count;

	if(bkgs <= 0)
        return RESULT_FAIL;

	if(flist != NULL) device_Free_mem_fn(flist);
	flist = (FILEINFO*)device_Alloc_mem(bkgs, sizeof(FILEINFO));
	if(flist == NULL) return RESULT_FAIL;

	for(i=0; i<bkgs; i++)
	{
		DL_FsSearchResults(fs_handle, i, &count, &fs_result);

		u_strncmp(fs_result.name, (filter + 6), 3) ? u_strncpy(flist[i].image_uri, filter, 9) : u_strncpy(flist[i].image_uri, filter, 6);
		u_strcat(flist[i].image_uri, fs_result.name);
	}

    APP_UtilStopTimer(app);
    APP_UtilStartTimer(time, 0, app);

	DL_FsSearchClose(fs_handle);
    return RESULT_OK;
}

UINT32 ReadConfig (DL_FS_MID_T *id)
{
	//Read multibkg.cfg
    UINT32 R;
	FILE_HANDLE_T hFile;

    //Get path to .cfg
    WCHAR CFGFile[FS_MAX_URI_NAME_LENGTH + 1] = L"file:/\0";

    DL_FsGetURIFromID(id, (CFGFile + 6));
    WCHAR *ptr = CFGFile + u_strlen(CFGFile);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"multibkg.cfg\0");

	if(DL_FsFFileExist(CFGFile))
	{
		hFile = DL_FsOpenFile(CFGFile, FILE_READ_MODE, 0);
		DL_FsReadFile(&time, sizeof(UINT32), 1, hFile, &R);
		DL_FsCloseFile(hFile);
	}

	if(time < 10000) time = 60000;

	return RESULT_OK;
}
