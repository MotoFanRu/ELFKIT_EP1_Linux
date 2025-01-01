#include "app.h"

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "SkinManager";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   ELF_Exit                    },
    { STATE_HANDLERS_RESERVED,      ELF_APPShow                 },
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { STATE_HANDLERS_RESERVED,      MainHandleInfo              },
    { STATE_HANDLERS_RESERVED,      MainHandleSelect            },
    { EV_SELECT,                    MainHandleSelect            },
    { EV_DONE,                      MainHandleDone              },
    { EV_DIALOG_DONE,               MainHandleDialogDone        },
    { EV_LIST_NAVIGATE,             MainHandleNavigate          },
    { EV_REQUEST_LIST_ITEMS,        MainHandleListReq           },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static EVENT_HANDLER_ENTRY_T prev_state_handlers[] =
{
    { EV_KEY_PRESS,                 PrevHandleKeyPress          },
    { EV_DIALOG_DONE,               PrevHandleDialogDone        },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static EVENT_HANDLER_ENTRY_T conf_state_handlers[] =
{
    { EV_YES,                       ConfHandleYesNo             },
    { EV_NO,                        ConfHandleYesNo             },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static EVENT_HANDLER_ENTRY_T info_state_handlers[] =
{
    { EV_DONE,                      InfoHandleDone              },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { HW_STATE_ANY,
      NULL,
      NULL,
      any_state_handlers
    },

    { HW_STATE_INIT,
      NULL,
      NULL,
      init_state_handlers
    },

    { HW_STATE_MAIN,
      MainStateEnter,
      MainStateExit,
      main_state_handlers
    },

    { HW_STATE_PREV,
      PrevStateEnter,
      PrevStateExit,
      prev_state_handlers
    },

    { HW_STATE_CONF,
      ConfStateEnter,
      ConfStateExit,
      conf_state_handlers
    },

    { HW_STATE_INFO,
      InfoStateEnter,
      InfoStateExit,
      info_state_handlers
    }
};

UINT32 Themes_EvCdResID = 0x14000052;
UINT32 Themes_Resource[] = { THEME_MAIN_REGISTER, 0x010013FA /*Themes_NameResID*/, 0x010013ba, 0x120001f6, 0x00000000, 0x120001f5, 0x120001f4};

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Entry (ldrElf *ela, WCHAR *params)
{
	UINT32 status = RESULT_OK;
	UINT32 reserved;

    elf = ela;
    elf->name = (char *)app_name;

    //Check if application is already loaded
    if(ldrIsLoaded((char *)app_name))
    {
        PFprintf("%s: Application already loaded!\n", app_name);
        return RESULT_FAIL;
    }

    //Initialize state handling table
    reserved = elf->evbase + 1;
    reserved = ldrInitEventHandlersTbl(any_state_handlers, reserved);
    reserved = ldrInitEventHandlersTbl(main_state_handlers, reserved);
    elf->evbase = THEME_MAIN_REGISTER;

    //Set new resources
    DRM_SetResource(Themes_EvCdResID, Themes_Resource, sizeof(UINT32) * sizeof(Themes_Resource));

    //Register application
    status = APP_Register(&elf->evbase, 1, state_handling_table, HW_STATE_MAX, (void*)ELF_Start);
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

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info)
{
    APP_T *app = NULL;
    UINT32 status = RESULT_OK;

    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		EVENT_CODE_T ev_code = ldrFindEventHandlerTbl(any_state_handlers, ELF_APPShow);
		return ldrSendEventToAppEmpty(elf->app->afw_id, ev_code);
	}

	//Initialize application
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEventPrepost, sizeof(APP_T), reg_id, 0, 1, 1, 1, 0, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Initialize application data

    app->isShow = FALSE;
    app->sInfo = NULL;
    app->Resources = NULL;
    app->EntriesNum = 0;
    app->SelectedId = UIS_GetCurrentSkinIndex() + 1;
    app->SkinFolder = UIS_GetSkinStorageFolder(1);

    //Starting application
    status = APP_Start(ev_st, &app->apt, HW_STATE_INIT, state_handling_table, ELF_APPHide, app_name, 0);
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

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status = RESULT_OK;

    free(thisapp(app)->sInfo);
    Util_RemoveResources(app);

    APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

	status = APP_ExitStateAndApp(ev_st, app, 0);
	APP_MMC_THEME_MainRegister();

	ldrUnloadElf(elf);
	return status;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_ChangeRStack (EVENT_STACK_T *ev_st,  APPLICATION_T *app, BOOL show)
{
    APP_T *papp = (APP_T*)app;
	UINT32 status;

	if (papp->isShow != show)
	{
        papp->isShow = show;
        status = APP_ChangeRoutingStack(app, ev_st, (void *)(papp->isShow ? APP_HandleEvent : APP_HandleEventPrepost), (papp->isShow ? 1 : 0), 0, 1, 1);
	}

	return status;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_APPHide (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(app->state != HW_STATE_INIT)
    {
        thisapp(app)->SelectedId = UIS_GetCurrentSkinIndex() + 1;
        APP_UtilChangeState(HW_STATE_INIT, ev_st, app);
    }

	ELF_ChangeRStack(ev_st, app, FALSE);
	return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_APPShow (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	if (app->state != HW_STATE_MAIN)
	    APP_UtilChangeState(HW_STATE_MAIN, ev_st, app);

	ELF_ChangeRStack(ev_st, app, TRUE);
    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    UINT32 status;

    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    if(thisapp(app)->Resources == NULL)
    {
        status = Util_InitResources(app);
        if(status == RESULT_FAIL)
        {
            status = ELF_APPHide(ev_st, app);
            return status;
        }
    }

    status = MainHandleUpdate(ev_st, app, thisapp(app)->SelectedId);
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    UINT32 status = RESULT_OK;

    if(type != EXIT_STATE_EXIT)
        return RESULT_OK;

    status = APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st, app);
    return ELF_APPHide(ev_st, app);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return ELF_APPHide(ev_st, app);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleInfo (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilConsumeEvChangeState(HW_STATE_INFO, ev_st, app);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status = RESULT_OK;

    if(thisapp(app)->SelectedId > 0)
    {
        WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];

        u_strcpy(uri, L"file:/");
        u_strcat(uri, thisapp(app)->SkinFolder);
        u_strcat(uri, thisapp(app)->sInfo[thisapp(app)->SelectedId - 1].SkinName);
        u_strcat(uri, L"/");
        u_strcat(uri, SKIN_PREVIEW_FILE_NAME);

        status = APP_UtilConsumeEvChangeState((DL_FsFFileExist(uri) ? HW_STATE_PREV : HW_STATE_CONF), ev_st, app);
    }

    return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status, start, num;
    if(!app->focused) return RESULT_OK;

    EVENT_T *ev = AFW_GetEv(ev_st);
	start = ev->data.list_items_req.begin_idx;
	num = ev->data.list_items_req.count;

	APP_ConsumeEv(ev_st, app);
	status = MainHandleSendListItems(ev_st, app, start, num);

	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EVENT_T *ev = AFW_GetEv(ev_st);
    thisapp(app)->SelectedId = ev->data.index;
    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleUpdate (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem)
{
    ACTIONS_T actions;
    CONTENT_T content;
    UINT32 starting_num, status;

    APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

    Util_GetSkins(app);
    Util_InitDialogActions(&actions, app);

    app->dialog = UIS_CreateList(&app->port, 0, thisapp(app)->EntriesNum, 0, &starting_num, 0, 2, &actions, thisapp(app)->Resources[RES_LIST_CAPTION]);
    if(app->dialog == 0)
    {
        status = ELF_APPHide(ev_st, app);
    }
    else
    {
        if(sItem != 0)
        {
            APP_UtilAddEvUISListChange(ev_st, app, 0, sItem, thisapp(app)->EntriesNum, TRUE, 2, FBF_LEAVE, NULL, NULL);
            UIS_HandleEvent(app->dialog, ev_st);
        }
        status = MainHandleSendListItems(ev_st, app, 1, starting_num);
    }

    return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainHandleSendListItems (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num)
{
	UINT32 i, index, status = RESULT_OK;
	LIST_ENTRY_T *plist = NULL;

	if(num == 0) return RESULT_FAIL;

    plist = (LIST_ENTRY_T*)device_Alloc_mem(num, sizeof(LIST_ENTRY_T));
    if(plist == NULL) return ELF_APPHide(ev_st, app);

	for(index=0, i=start; (i<start+num) && (i<=thisapp(app)->EntriesNum); i++, index++)
	{
		plist[index].editable = FALSE;
		plist[index].content.static_entry.formatting = 1;

        UIS_MakeContentFromString( ((thisapp(app)->sInfo[i - 1].isCurrent == TRUE) ? "q0Sp1" : "q0"), &(plist[index].content.static_entry.text), thisapp(app)->sInfo[i - 1].SkinName, DRM_SELECTED);
	}

	status = APP_UtilAddEvUISListData(ev_st, app, 0, start, num, FBF_LEAVE, sizeof(LIST_ENTRY_T) * num, plist);

	if(status != RESULT_FAIL)
		status = UIS_HandleEvent(app->dialog, ev_st);

	device_Free_mem_fn(plist);
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 PrevStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    UINT32 status;

    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    if(thisapp(app)->SelectedId > 0)
    {
        WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];

        u_strcpy(uri, L"file:/");
        u_strcat(uri, thisapp(app)->SkinFolder);
        u_strcat(uri, thisapp(app)->sInfo[thisapp(app)->SelectedId - 1].SkinName);
        u_strcat(uri, L"/");
        u_strcat(uri, SKIN_PREVIEW_FILE_NAME);

        app->dialog = UIS_CreateScreenSaver(&app->port, uri);
        if(app->dialog == 0) return RESULT_FAIL;
    }

    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 PrevStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    UINT32 status = RESULT_OK;

    if(type != EXIT_STATE_EXIT)
        return RESULT_OK;

    status = APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

    return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 PrevHandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilConsumeEvChangeState(HW_STATE_CONF, ev_st, app);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 PrevHandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilConsumeEvChangeState(HW_STATE_CONF, ev_st, app);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ConfStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    UINT32 status;
    CONTENT_T content;

    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    if(thisapp(app)->SelectedId > 0)
    {
        WCHAR str[FS_MAX_URI_NAME_LENGTH + 1];

        u_strcpy(str, thisapp(app)->sInfo[thisapp(app)->SelectedId - 1].SkinName);
        u_strcat(str, L"?");

        UIS_MakeContentFromString("MCs0 q1", &content, 0x01000E0A, str);
        app->dialog = UIS_CreateConfirmation(&app->port, &content);

        if(app->dialog == 0)
            return RESULT_FAIL;
    }

    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ConfStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    UINT32 status = RESULT_OK;

    if(type != EXIT_STATE_EXIT)
        return RESULT_OK;

    status = APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

    return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ConfHandleYesNo (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(AFW_GetEvCode(ev_st) == EV_YES)
        if(thisapp(app)->SelectedId > 0)
            Util_SetSkin(thisapp(app)->SelectedId - 1, app);

    return APP_UtilConsumeEvChangeState(HW_STATE_MAIN, ev_st, app);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 InfoStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    UINT32 status;
    CONTENT_T content;

    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    if(thisapp(app)->SelectedId > 0)
    {
        //Get skin size
        //----------------------------------------------------------------------
        UINT16 i, result, count = 1, size = 0;
        WCHAR filter[FS_MAX_URI_NAME_LENGTH + 1];

        FS_SEARCH_PARAMS_T      fs_params;
        FS_SEARCH_HANDLE_T      fs_handle;
        FS_SEARCH_RESULT_T      fs_result;

        fs_params.flags = FS_SEARCH_RECURSIVE_AND_SORT_BY_NAME;
        fs_params.attrib = 0x0000;
        fs_params.mask = 0x0000;

        u_strcpy(filter, L"file:/");
        u_strcat(filter, thisapp(app)->SkinFolder);
        u_strcat(filter, thisapp(app)->sInfo[thisapp(app)->SelectedId - 1].SkinName);
        u_strcat(filter, L"/\xFFFE*.*");

        DL_FsSSearch(fs_params, filter, &fs_handle, &result, 0x0);
        for(i=0; i<result; i++)
        {
            DL_FsSearchResults(fs_handle, i, &count, &fs_result);
            size += DL_FsSGetFileSize(fs_result.name, 0);
        }
        DL_FsSearchClose(fs_handle);
        size = size >> 10;
        //----------------------------------------------------------------------

        u_strcpy(filter, thisapp(app)->SkinFolder);
        u_strcat(filter, thisapp(app)->sInfo[thisapp(app)->SelectedId - 1].SkinName);

        UIS_MakeContentFromString("s0NTs1 q2NTs3 i4s5NTs6 q7", &content, 0x01000693, 0x010007B9, thisapp(app)->sInfo[thisapp(app)->SelectedId - 1].SkinName, 0x01000808, size, 0x01000BC9, 0x01001140, filter);

        app->dialog = UIS_CreateViewer(&app->port, &content, NULL);
        if(app->dialog == 0) return RESULT_FAIL;
    }

    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 InfoStateExit (EVENT_STACK_T *ev_st,  APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    UINT32 status = RESULT_OK;

    if(type != EXIT_STATE_EXIT)
        return RESULT_OK;

    status = APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

    return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 InfoHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilConsumeEvChangeState(HW_STATE_MAIN, ev_st, app);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 Util_InitResources (APPLICATION_T *app)
{
    UINT32 status = RESULT_OK;
    RES_ACTION_LIST_ITEM_T action[2];

    thisapp(app)->Resources = (RESOURCE_ID*)device_Alloc_mem((RES_MAX + 1), sizeof(RESOURCE_ID));
    if(thisapp(app)->Resources == NULL) return RESULT_FAIL;

    thisapp(app)->Resources[RES_LIST_CAPTION] = 0x010013FA;   //Оболочка экрана
    thisapp(app)->Resources[RES_LIST_LABEL0] = LANG_SELECT;        //Выбрать
    thisapp(app)->Resources[RES_LIST_LABEL1] = 0x01000693;         //Информация

    action[0].softkey_label = 0;
    action[0].list_label = thisapp(app)->Resources[RES_LIST_LABEL0];
    action[0].softkey_priority = 0;
    action[0].list_priority = 1;
    action[0].isExit = FALSE;
    action[0].sendDlgDone = FALSE;

    status = DRM_CreateResource( &thisapp(app)->Resources[RES_LIST_ACTION0], RES_TYPE_ACTION, ( void * )&action[0], sizeof(RES_ACTION_LIST_ITEM_T) );
	if( status!=RESULT_OK ) DRM_ClearResource( thisapp(app)->Resources[RES_LIST_ACTION0] );

    action[1].softkey_label = 0;
    action[1].list_label = thisapp(app)->Resources[RES_LIST_LABEL1];
    action[1].softkey_priority = 0;
    action[1].list_priority = 0;
    action[1].isExit = FALSE;
    action[1].sendDlgDone = FALSE;

    status = DRM_CreateResource( &thisapp(app)->Resources[RES_LIST_ACTION1], RES_TYPE_ACTION, ( void * )&action[1], sizeof(RES_ACTION_LIST_ITEM_T) );
	if( status!=RESULT_OK ) DRM_ClearResource( thisapp(app)->Resources[RES_LIST_ACTION1] );

    return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 Util_RemoveResources (APPLICATION_T *app)
{
	UINT32 i;
	UINT32 status = RESULT_OK;

	for(i=0;i<RES_MAX;i++)
		status |= DRM_ClearResource(thisapp(app)->Resources[i]);

    device_Free_mem_fn(thisapp(app)->Resources);
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 Util_InitDialogActions (ACTIONS_T *actions, APPLICATION_T *app)
{
    actions->action[0].operation = ACTION_OP_ADD;
    actions->action[0].event = ldrFindEventHandlerTbl(main_state_handlers, MainHandleSelect);
    actions->action[0].action_res = thisapp(app)->Resources[RES_LIST_ACTION0];

    actions->action[1].operation = ACTION_OP_ADD;
    actions->action[1].event = ldrFindEventHandlerTbl(main_state_handlers, MainHandleInfo);
    actions->action[1].action_res = thisapp(app)->Resources[RES_LIST_ACTION1];

    actions->count = 2;

    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 Util_SetSkin (UINT32 idx, APPLICATION_T *app)
{
    //Change wallpaper with skin
    WCHAR wallpaper_path[FS_MAX_URI_NAME_LENGTH + 1];

    u_strcpy(wallpaper_path, L"file:/");
    u_strcat(wallpaper_path, thisapp(app)->SkinFolder);
    u_strcat(wallpaper_path, thisapp(app)->sInfo[idx].SkinName);
    u_strcat(wallpaper_path, L"/");
    u_strcat(wallpaper_path, thisapp(app)->sInfo[idx].SkinName);
    u_strcat(wallpaper_path, SKIN_WALLPAPER_FILE_EXT);

    if(DL_FsFFileExist(wallpaper_path))
    {
        SETWALLPAPER_SETTINGS_T WallPaper;
        WallPaper.url = wallpaper_path;
        WallPaper.layout = WALLPAPER_LAYOUT_FIT_TO_SCREEN;

        DL_FS_MID_T wallpaper_file_id;
        DL_FsGetIDFromURI(wallpaper_path, &wallpaper_file_id);

        DL_DbFeatureStoreBlock(0xB7E, (UINT8 *)&wallpaper_file_id, 8);
        UIS_SetWallpaper(&WallPaper);
    }

    u_strcpy(mma_memory_str, L"file:/");
    u_strcat(mma_memory_str, thisapp(app)->SkinFolder);
    u_strcat(mma_memory_str, thisapp(app)->sInfo[idx].SkinName);
    u_strcat(mma_memory_str, L"/mma_ucp");

    if(!DL_FsFFileExist(mma_memory_str))
        u_strcpy(mma_memory_str, DEFAUL_MMA_UCP_URI);

    UIS_SetSkin(idx);
    return RESULT_OK;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 Util_GetSkins (APPLICATION_T *app)
{
    UINT16 i, CurrentSkinId;

    CurrentSkinId = UIS_GetCurrentSkinIndex();
    thisapp(app)->EntriesNum = UIS_GetNumSkins();
    if(thisapp(app)->sInfo != NULL) mfree(thisapp(app)->sInfo);

    thisapp(app)->sInfo = (SKIN_INFO_T *)malloc(thisapp(app)->EntriesNum * sizeof(SKIN_INFO_T));
    if(thisapp(app)->sInfo == NULL) return RESULT_FAIL;

    for(i=0; i<thisapp(app)->EntriesNum; i++)
    {
        thisapp(app)->sInfo[i].SkinName = UIS_GetSkinName(i);
        thisapp(app)->sInfo[i].isCurrent = (i == CurrentSkinId ? TRUE : FALSE);
    }

    return RESULT_OK;
}
