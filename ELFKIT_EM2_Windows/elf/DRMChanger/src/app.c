#include "app.h"

//app resources
UINT16 res_cnt = 0;
UINT8 **res_img = NULL;
RESOURCE_ID *res_id = NULL;

//skin resources
WCHAR *skin_folder = NULL;
UINT16 skin_index = 0xFFFF;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "DRMChanger";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_UIS_FORCE_REFRESH,         HandleUISRefresh            },
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
        UIS_ForceRefresh();
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
    Util_RemoveResources();
    UIS_ForceRefresh();

	APP_ExitStateAndApp(ev_st, app, 0);

	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 HandleUISRefresh (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT16 curr_skin_index =  UIS_GetCurrentSkinIndex();
    if(curr_skin_index == skin_index) return RESULT_OK;
    else skin_index = curr_skin_index;

    skin_folder = UIS_GetSkinStorageFolder(1);
    if(res_id) Util_RemoveResources();

    //Get filter string.
    //Format: skin_folder/skin_name/drm/*.gif
    //Example: file://a/mobile/skins/Moto/drm/*.gif
    WCHAR fs_filter[FS_MAX_URI_NAME_LENGTH + 1];
    u_strcpy(fs_filter, skin_folder);
    u_strcat(fs_filter, UIS_GetSkinName(skin_index));
    u_strcat(fs_filter, L"/drm/\xFFFE*.gif");

    //search images in skin folder
    FS_SEARCH_PARAMS_T fs_param;
    FS_SEARCH_HANDLE_T fs_handle;
    FS_SEARCH_RESULT_T fs_result;

    fs_param.flags = 0xD;
    fs_param.attrib = 0x0;
    fs_param.mask = 0x0;

    DL_FsSSearch(fs_param, fs_filter, &fs_handle, &res_cnt, 0x0);

    if(res_cnt)
    {
        res_img = (UINT8 **)malloc(res_cnt * sizeof(UINT8 *));
        if(res_img == NULL)
        {
            return RESULT_FAIL;
        }

        res_id = (RESOURCE_ID *)malloc(res_cnt * sizeof(RESOURCE_ID));
        if(res_id == NULL)
        {
            free(res_img);
            return RESULT_FAIL;
        }
    }

    UINT16 index, count = 1;
    for(index = 0; index < res_cnt; index += 1)
    {
        WCHAR u_name[32];
        DL_FsSearchResults(fs_handle, index, &count, &fs_result);
        DL_FsSGetFileName(fs_result.name, u_name);

        UINT32 number = u_atol(u_name);
        if(number)
        {
            //image size in bytes
            UINT32 size = DL_FsSGetFileSize(fs_result.name, 0);
            if(size <= 0)
            {
                index -= 1;
                res_cnt -= 1;
                continue;
            }

            //alloc memory for image
            res_img[index] = (UINT8 *)malloc(size * sizeof(UINT8));
            if(res_img == NULL)
            {
                index -= 1;
                res_cnt -= 1;
                continue;
            }

            //read image in memory
            UINT32 R;
            FILE_HANDLE_T hFile = DL_FsOpenFile(fs_result.name, FILE_READ_MODE, 0);
            DL_FsReadFile(res_img[index], size, 1, hFile, &R);
            DL_FsCloseFile(hFile);

            //save resources id
            res_id[index] = 0x12000000 + number;

            //apply new resource
            DRM_SetResource(res_id[index], (void *)res_img[index], size);
        }
    }
    DL_FsSearchClose(fs_handle);
    UIS_ForceRefresh();

    return RESULT_OK;
}

UINT32 Util_RemoveResources()
{
    UINT32 index, status = RESULT_OK;

    for(index = 0; index < res_cnt; index += 1)
    {
        free(res_img[index]);
        status |= DRM_ClearResource(res_id[index]);
    }

    free(res_img);
    free(res_id);
    res_cnt = 0;

    return status;
}
