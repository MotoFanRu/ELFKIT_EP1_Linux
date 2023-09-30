int beginning( );

int _main( )
{
    return beginning( );
}

#include "___gui___.h"
#include "ldr_autorun.h"
#include "ldr_core.h"
#include "ldr_elflist.h"
#include "ldr_library.h"
#include "ldr_features.h"

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { EV_REVOKE_TOKEN,        APP_HandleUITokenRevoked },
    { STATE_HANDLERS_END,     NULL                     },
};

const EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { EV_GRANT_TOKEN,         HandleUITokenGranted     },
    { STATE_HANDLERS_END,     NULL                     },
};

const EVENT_HANDLER_ENTRY_T em_state_handlers[] =
{
    { EV_DONE,                EMHandleDone             },
    { EV_DIALOG_DONE,         EMHandleDone             },

    { EV_LIST_NAVIGATE,       EMHandleNavigate         },
    { EV_REQUEST_LIST_ITEMS,  EMHandleListReq          },
    { EV_KEY_PRESS,           EMHandleKeyPress         }, 

    { EV_SELECT,              EMHandleAction           },
    { EVENT_RUN,              EMHandleAction           },
    { EVENT_DEL,              EMHandleAction           },
    { EVENT_LIB,              EMHandleAction           },
    { EVENT_PM,               EMHandleAction           },

    { STATE_HANDLERS_END,     NULL                     },
};

const EVENT_HANDLER_ENTRY_T pm_state_handlers[] =
{
    { EV_DONE,                PMHandleDone             },
    { EV_PM_ELF_LIST_CHANGED, PMElfListChanged         },

    { EV_REQUEST_LIST_ITEMS,  PMHandleListReq          }, 
    { EV_LIST_NAVIGATE,       PMHandleNavigate         },

    { EV_SELECT,              PMHandleSelect           },
    { EVENT_DEL,              PMHandleEndElf           },

    { STATE_HANDLERS_END,     NULL                     },
};

const EVENT_HANDLER_ENTRY_T pi_state_handlers[] =
{
    { EV_DONE,                PIHandleDone             },
    { STATE_HANDLERS_END,     NULL                     },
};

const STATE_HANDLERS_ENTRY_T state_handling_table[] =
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

    { APP_STATE_EM,
      EMStateEnter,
      StateExit,
      em_state_handlers
    },

    { APP_STATE_PM,
      PMStateEnter,
      StateExit,
      pm_state_handlers
    },

    { APP_STATE_PI,
      PIStateEnter,
      StateExit,
      pi_state_handlers
    }
};

int beginning ()
{
    UINT32 InFusio_EvCdResID = 0x1400006F;
    UINT32 InFusio_NewResource[] = {0x0000EEED, 0x01001701, 0x01001701, 0x1200058C, 0x00000000, 0x1200058D, 0x1200058B};

    DRM_SetResource(InFusio_EvCdResID, InFusio_NewResource, sizeof(UINT32)*sizeof(InFusio_NewResource));
    APP_Register(&InFusio_NewResource[0], 1, state_handling_table, APP_STATE_MAX, (void*)APPStart);

    if(get_key_code((KPP_T *)0x219000) != ___KEY_0)
    {
        ldrLoadDefLibrary();
        ldrAutorunAction(NULL, AUTORUN_ACTION_LOAD);
    }

    return RESULT_OK;
}

UINT32 APPStart (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info)
{
    APP_T *app;
    UINT32 status;

    app = (APP_T*)APP_InitAppData((void*)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0);
    if(!app)
    {
        PFprintf("%s: Can't initialize application data\n", app_name);
        return RESULT_FAIL;
    }

    status = APPInitResources();
    if(status != RESULT_OK)
    {
        PFprintf("%s: Can't initialize application resources\n", app_name);
        return status;
    }

    status = APP_Start(ev_st, &app->apt, APP_STATE_INIT, state_handling_table, APPExit, app_name, 0);
    if(status != RESULT_OK)
    {
        PFprintf("%s: Can't start application!\n", app_name);
        APPFreeResources();
        APP_HandleFailedAppStart(ev_st, (APPLICATION_T*)app, 0);
        return status;
    }
    
    return status;
}

UINT32 APPExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EMSelectedID = 0;
    PMSelectedID = 0;

    APPFreeResources();
    APP_ExitStateAndApp(ev_st, app, NULL);

    return RESULT_OK;
}

UINT32 StateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    if(app->state == APP_STATE_EM)
    {
        free(filelist);
    }
    else if(app->state == APP_STATE_PM)
    {
        free(Elfs);
    }

    return APP_UtilUISDialogDelete(&app->dialog);
}

UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status = APP_HandleUITokenGranted(ev_st, app);

    if((status == RESULT_OK) && (app->token_status == 2))
    {
        status = APP_UtilChangeState(APP_STATE_EM, ev_st, app);
    }

    return status;
}

//action_type: 0 - actions for APP_STATE_EM, else - APP_STATE_PM
UINT32 APPInitActions (ACTIONS_T *action_list, UINT8 action_type)
{
    if(action_type == 0)
    {
        action_list->action[0].operation = ACTION_OP_ADD; 
        action_list->action[0].event = EVENT_RUN;
        action_list->action[0].action_res = APPResources[APP_RES_ACTION_RUN]; //Запустить

        action_list->action[1].operation = ACTION_OP_ADD; 
        action_list->action[1].event = EVENT_DEL;
        action_list->action[1].action_res = APPResources[APP_RES_ACTION_DEL]; //Удалить

        action_list->action[2].operation = ACTION_OP_ADD; 
        action_list->action[2].event = EVENT_LIB;
        action_list->action[2].action_res = APPResources[APP_RES_ACTION_LIB]; //Перезагрузить lib.def

        action_list->action[3].operation = ACTION_OP_ADD; 
        action_list->action[3].event = EVENT_PM;
        action_list->action[3].action_res = APPResources[APP_RES_ACTION_PM]; //Process Manager
    
        action_list->count = 4;
    }
    else
    {
        action_list->action[0].operation = ACTION_OP_ADD; 
        action_list->action[0].event = EVENT_DEL;
        action_list->action[0].action_res = APPResources[APP_RES_ACTION_END]; //Завершить

        action_list->count = 1;
    }
    return RESULT_OK;
}

UINT32 APPInitResources ()
{
    UINT32 status;
    RES_ACTION_LIST_ITEM_T action[5];
    WCHAR RL_STRING[] = L"Reload lib";

    //Begin init resources
    //STRINGS
    APPResources[APP_RES_STRING_PM]  = LANG_PM;
    APPResources[APP_RES_STRING_RUN] = LANG_RUN;
    APPResources[APP_RES_STRING_DEL] = LANG_DEL1;
    APPResources[APP_RES_STRING_END] = LANG_END;

    status = DRM_CreateResource(&APPResources[APP_RES_STRING_LIB], RES_TYPE_STRING, (void *)RL_STRING, (u_strlen(RL_STRING) + 1) * sizeof(WCHAR));
    if(status != RESULT_OK) return status;

    //ACTIONS
    action[0].softkey_label = 0;
    action[0].list_label = APPResources[APP_RES_STRING_RUN];
    action[0].softkey_priority = 0;
    action[0].list_priority = 3;
    action[0].isExit = FALSE;
    action[0].sendDlgDone = FALSE;

    action[1].softkey_label = 0;
    action[1].list_label = APPResources[APP_RES_STRING_DEL];
    action[1].softkey_priority = 0;
    action[1].list_priority = 2;
    action[1].isExit = FALSE;
    action[1].sendDlgDone = FALSE;

    action[2].softkey_label = 0;
    action[2].list_label = APPResources[APP_RES_STRING_LIB];
    action[2].softkey_priority = 0;
    action[2].list_priority = 1;
    action[2].isExit = FALSE;
    action[2].sendDlgDone = FALSE;

    action[3].softkey_label = 0;
    action[3].list_label = APPResources[APP_RES_STRING_PM];
    action[3].softkey_priority = 0;
    action[3].list_priority = 0;
    action[3].isExit = FALSE;
    action[3].sendDlgDone = FALSE;

    action[4].softkey_label = APPResources[APP_RES_STRING_END];
    action[4].list_label = APPResources[APP_RES_STRING_END];
    action[4].softkey_priority = 1;
    action[4].list_priority = 0;
    action[4].isExit = FALSE;
    action[4].sendDlgDone = FALSE;

    status = DRM_CreateResource(&APPResources[APP_RES_ACTION_RUN], RES_TYPE_ACTION, (void *)&action[0], sizeof(RES_ACTION_LIST_ITEM_T));
    if(status != RESULT_OK) return status;

    status = DRM_CreateResource(&APPResources[APP_RES_ACTION_DEL], RES_TYPE_ACTION, (void *)&action[1], sizeof(RES_ACTION_LIST_ITEM_T));
    if(status != RESULT_OK) return status;
    
    status = DRM_CreateResource(&APPResources[APP_RES_ACTION_LIB], RES_TYPE_ACTION, (void *)&action[2], sizeof(RES_ACTION_LIST_ITEM_T));
    if(status != RESULT_OK) return status;

    status = DRM_CreateResource(&APPResources[APP_RES_ACTION_PM],  RES_TYPE_ACTION, (void *)&action[3], sizeof(RES_ACTION_LIST_ITEM_T));
    if(status != RESULT_OK) return status;

    status = DRM_CreateResource(&APPResources[APP_RES_ACTION_END], RES_TYPE_ACTION, (void *)&action[4], sizeof(RES_ACTION_LIST_ITEM_T));
    if(status != RESULT_OK) return status;
    //End init resources

    return status;
}

UINT32 APPFreeResources ()
{
    UINT32 i, status = RESULT_OK;

    for(i=0; i<APP_RES_MAX; i++)
    {
        status |= DRM_ClearResource(APPResources[i]);
    }

    return status;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 EMStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    if(type != ENTER_STATE_ENTER) return RESULT_OK;
    if(EMSelectedID > EMEntriesCnt) EMSelectedID = 0;

    return EMUpdate(ev_st, app, EMSelectedID, TRUE);
}

UINT32 EMHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(AFW_GetEvCode(ev_st) == EV_DONE)
        APP_ConsumeEv(ev_st, app);

    app->exit_status = TRUE;
    return RESULT_OK;
}

UINT32 EMHandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(!app->focused) return RESULT_OK;

    EVENT_T *event = AFW_GetEv(ev_st);
    UINT32 start = event->data.list_items_req.begin_idx;
    UINT32 num = event->data.list_items_req.count;

    APP_ConsumeEv(ev_st, app);
    return EMSendListItems(ev_st, app, start, num);
}

UINT32 EMSendListItems (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num)
{
    LIST_ENTRY_T *plist;
    UINT32 i, index, status=RESULT_OK;
   
    if(num == 0) return RESULT_FAIL;

    plist = (LIST_ENTRY_T*)device_Alloc_mem(num, sizeof(LIST_ENTRY_T));
    if(plist == NULL) return RESULT_FAIL;

    for(index=0, i=start; (i<start+num) && (i<=EMEntriesCnt); i++, index++)
    {
        plist[index].editable = FALSE;
        plist[index].content.static_entry.formatting = 1;

        UIS_MakeContentFromString((filelist[i-1].in_autorun != FALSE ? "p0q1Sp2" : "p0q1"), &(plist[index].content.static_entry.text), 0x1200058C, filelist[i-1].u_name, DRM_SELECTED);
    }

    status = APP_UtilAddEvUISListData(ev_st, app, 0, start, num, FBF_LEAVE, sizeof(LIST_ENTRY_T)*num, plist);
    if(status != RESULT_FAIL) status = UIS_HandleEvent(app->dialog, ev_st);

    device_Free_mem_fn(plist);
    return status;
}

UINT32 EMUpdate (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem, BOOL UpdateFileList)
{
    UINT32 starting_num;
    ACTIONS_T action_list;
    
    if(app->dialog != NULL) APP_UtilUISDialogDelete(&app->dialog);
    
    APPInitActions(&action_list, 0);
    if(UpdateFileList == TRUE) EMGetElfList();

    app->dialog = UIS_CreateList(&app->port, 0, EMEntriesCnt, 0, &starting_num, 0, 2, &action_list, LANG_APPLICATIONS);
    if(app->dialog == 0) return RESULT_FAIL;

    if (sItem != 0)
    {
        APP_UtilAddEvUISListChange(ev_st, app, 0, sItem, EMEntriesCnt, TRUE, 2, FBF_LEAVE, NULL, NULL);    
        UIS_HandleEvent(app->dialog, ev_st);        
    }

    EMSendListItems(ev_st, app, 1, starting_num);
    return RESULT_OK;
}

UINT32 EMHandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EVENT_T *event = AFW_GetEv(ev_st);
    EMSelectedID = event->data.index;
    return RESULT_OK;
}

UINT32 EMHandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT8 key = GET_KEY(ev_st);
    APP_ConsumeEv(ev_st, app);

    if(key == KEY_0)
    {   
        AUTORUN_RECORD_T Record = {0, 0, 0};
        DL_FsGetIDFromURI(filelist[EMSelectedID - 1].u_fullname, &Record.file_id);

        if(filelist[EMSelectedID - 1].in_autorun == TRUE)
        {
            filelist[EMSelectedID - 1].in_autorun = FALSE;
            ldrAutorunAction(&Record, AUTORUN_ACTION_DELETE);
        }
        else
        {
            filelist[EMSelectedID - 1].in_autorun = !ldrAutorunAction(&Record, AUTORUN_ACTION_ADD);
        }

        EMUpdate(ev_st, app, EMSelectedID, FALSE);
    }

    return RESULT_OK;
}

UINT32 EMHandleAction (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EVENT_CODE_T evcode = AFW_GetEvCode(ev_st);
    APP_ConsumeEv(ev_st, app);

    switch(evcode)
    {
        case EV_SELECT:
        case EVENT_RUN:
            if(EMSelectedID > 0) 
            {
                ldrLoadElf(filelist[EMSelectedID - 1].u_fullname, 0, 0);
                app->exit_status = TRUE;
                break;
            }
            break;

        case EVENT_DEL:
            if(EMSelectedID > 0)
            {
                DL_FsDeleteFile(filelist[EMSelectedID - 1].u_fullname, NULL);
                EMUpdate(ev_st, app, EMSelectedID - 1, TRUE);
            }
            break;


        case EVENT_LIB:
            ldrLoadDefLibrary();
            break;

        case EVENT_PM:
            APP_UtilChangeState(APP_STATE_PM, ev_st, app);
            break;
    }

    return RESULT_OK;
}

UINT32 EMGetElfList(void)
{
    UINT32 i;
    WCHAR filter[] = L"file://a/elf/\xFFFE*.elf";

    FS_SEARCH_PARAMS_T fs_param;
    FS_SEARCH_HANDLE_T fs_handle;
    FS_SEARCH_RESULT_T fs_result; 

    UINT16 count = 1;
    UINT16 ent_count = 0;
 
    fs_param.flags = 0x0D;
    fs_param.attrib = 0x0000;
    fs_param.mask = 0x0000; 

    DL_FsSSearch(fs_param, filter, &fs_handle, &ent_count, 0x0); 
    EMEntriesCnt = ent_count;

    filelist = (FILEINFO*)malloc(EMEntriesCnt * sizeof(FILEINFO));

    for(i = 0; i < EMEntriesCnt; i++)
    {
        DL_FsSearchResults( fs_handle, i, &count, &fs_result );

        //checking existing of string "file:/"
        u_strncmp(fs_result.name, (filter + 6), 3) ? u_strncpy(filelist[i].u_fullname, filter, 9) : u_strncpy(filelist[i].u_fullname, filter, 6);
        
        //file uri
        u_strcat(filelist[i].u_fullname, fs_result.name);

        //file name with extension
        filelist[i].u_name = filelist[i].u_fullname + u_strlen(filelist[i].u_fullname);
        while(*filelist[i].u_name != '/') filelist[i].u_name--;
        filelist[i].u_name++;

        //check if elf in autorun
        AUTORUN_RECORD_T Record = {0, 0, 0};
        DL_FsGetIDFromURI(filelist[i].u_fullname, &Record.file_id);
        filelist[i].in_autorun = !ldrAutorunAction(&Record, AUTORUN_ACTION_FIND);
    }
    DL_FsSearchClose(fs_handle);

    return RESULT_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 PIStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    if(type != ENTER_STATE_ENTER) return RESULT_OK;

    ldrElf *elf = ldrFindElf(SelectedElf.a_name);
    if(elf == NULL) return APP_UtilChangeState(APP_STATE_PM, ev_st, app);

    CONTENT_T content;
    UIS_MakeContentFromString("s0NTq1 q2NTq3 g4NTq5 g6NTq7 g8", &content, LANG_INFORMATION, L"name:", SelectedElf.u_name, L"evbase:", elf->evbase, L"afw id:", elf->app->afw_id, L"reg id:", elf->app->reg_id);

    app->dialog = UIS_CreateViewer(&app->port, &content, NULL);
    if(app->dialog == 0) return RESULT_FAIL;

    return RESULT_OK;
}

UINT32 PIHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilChangeState(APP_STATE_PM, ev_st, app);
}

UINT32 PMStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    if(type != ENTER_STATE_ENTER) return RESULT_OK;
    return PMUpdate(ev_st, app, PMSelectedID, TRUE);
}

UINT32 PMHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilChangeState(APP_STATE_EM, ev_st, app);
}

UINT32 PMHandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(!app->focused) return RESULT_OK;
    EVENT_T *event = AFW_GetEv(ev_st);

    UINT32 start = event->data.list_items_req.begin_idx;
    UINT32 num = event->data.list_items_req.count;

    APP_ConsumeEv(ev_st, app);
    return PMSendListItems(ev_st, app, start, num);
}

UINT32 PMSendListItems (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num)
{
    LIST_ENTRY_T *plist;
    UINT32 i, index, status=RESULT_OK;
   
    if(num == 0) return RESULT_FAIL;

    plist = (LIST_ENTRY_T*)device_Alloc_mem(num, sizeof(LIST_ENTRY_T));
    if(plist == NULL) return RESULT_FAIL;

    for(index=0, i=start; (i<start+num) && (i<=PMEntriesCnt); i++, index++)
    {
        plist[index].editable = FALSE;
        plist[index].content.static_entry.formatting = 1;

        UIS_MakeContentFromString("q0", &(plist[index].content.static_entry.text), Elfs[i-1].u_name);
    }

    status = APP_UtilAddEvUISListData(ev_st, app, 0, start, num, FBF_LEAVE, sizeof(LIST_ENTRY_T)*num, plist);
    if(status != RESULT_FAIL) status = UIS_HandleEvent(app->dialog, ev_st);

    device_Free_mem_fn(plist);
    return status;
}

UINT32 PMUpdate (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem, BOOL UpdateFileList)
{
    UINT32 starting_num;
    ACTIONS_T action_list;
    
    if(app->dialog != NULL)
    {    
        APP_UtilUISDialogDelete(&app->dialog);
        app->dialog = NULL;
    }
    
    APPInitActions(&action_list, 1);
    if(UpdateFileList == TRUE) PMGetElfList();

    app->dialog = UIS_CreateList(&app->port, 0, PMEntriesCnt, 0, &starting_num, 0, 2, &action_list, APPResources[APP_RES_STRING_PM]);
    if(app->dialog == 0) return RESULT_FAIL;

    if (sItem != 0)
    {
        APP_UtilAddEvUISListChange(ev_st, app, 0, sItem, PMEntriesCnt, TRUE, 2, FBF_LEAVE, NULL, NULL);    
        UIS_HandleEvent(app->dialog, ev_st);        
    }

    PMSendListItems(ev_st, app, 1, starting_num);
    return RESULT_OK;
}

UINT32 PMHandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EVENT_T *event = AFW_GetEv(ev_st);
    PMSelectedID = event->data.index;
    return RESULT_OK;
}

UINT32 PMHandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st, app);
    if(PMSelectedID > 0) 
    {
        memcpy(&SelectedElf, &Elfs[PMSelectedID - 1], sizeof(ELF_LIST_NAMES_T));
        APP_UtilChangeState(APP_STATE_PI, ev_st, app);
    }
    return RESULT_OK;
}

UINT32 PMHandleEndElf (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st, app);
    if(PMSelectedID > 0)
    {
        ldrElf *elf = ldrFindElf(Elfs[PMSelectedID - 1].a_name);
        if(elf != NULL)
        {
            if(elf->app && AFW_InquireRoutingStackByRegId( elf->app->reg_id ) == RESULT_OK)
            {
                //APP запущено и способно обрабатывать сообщения
                PFprintf("[*** PM ***]: App '%s' is alive :) Sending event EV_PM_API_EXIT!\n", elf->name);
                ldrSendEventToAppEmpty(elf->app->afw_id, EV_PM_API_EXIT);
            }
            else
            {
                //APP не загружено или не способно обрабатывать сообщения
                PFprintf("[*** PM ***]: App '%' is dead :( Trying to unload...\n", elf->name);
                ldrUnloadElf(elf);
            }
        }
    }
    return RESULT_OK;
}

UINT32 PMElfListChanged (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return PMUpdate(ev_st, app, PMSelectedID, TRUE);
}

UINT32 PMGetElfList()
{
    UINT32 i = 0;
    ldrElf *p = ldrGetElfsList(&PMEntriesCnt);

    Elfs = (ELF_LIST_NAMES_T *)malloc(PMEntriesCnt * sizeof(ELF_LIST_NAMES_T));
    while(p)
    {
        Elfs[i].a_name = p->name;
        u_atou(Elfs[i].a_name, Elfs[i].u_name);
        i++;
        p = p->next;
    }

    if(PMSelectedID > PMEntriesCnt) PMSelectedID = 0;

    return RESULT_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
