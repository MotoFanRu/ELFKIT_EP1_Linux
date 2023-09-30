#include "app.h"

DL_FS_MID_T *pids = NULL;       // массив физических идентификаторов для отправки

UINT32 SELECTED_IDX = 0;        // выделенный пункт списка
UINT32 ENTRIES_NUM = 0;         // кол-во пунктов в списке
UINT32 MAX_COUNT_MIDLETS = 0;   // максимальное кол-во мидлетов

MIDLET *midlets = NULL;         // список мидлетов
BOOL TransFlash = FALSE;        // TRUE = устройство хранения - флешка
                                // FALSE = устройство хранения - телефон
ldrElf *elf = NULL;

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit            },
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked },
    { STATE_HANDLERS_END,           NULL                     },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { EV_GRANT_TOKEN,               HandleUITokenGranted     },
    { STATE_HANDLERS_END,           NULL                     },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { STATE_HANDLERS_RESERVED,      HandleSend               },
    { STATE_HANDLERS_RESERVED,      HandleSwitchDevice       },
    { EV_REQUEST_LIST_ITEMS,        HandleListReq            },
    { EV_LIST_NAVIGATE,             HandleNavigate           },
    { EV_DIALOG_DONE,               HandleDialogDone         },
    { EV_SELECT, 					HandleSelect             },
    { EV_DONE,                      HandleDone               },
    { STATE_HANDLERS_END,           NULL                     },
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
      NULL,
      main_state_handlers
    }
};

//+++ Точка входа
UINT32 ELF_Entry(ldrElf *ela, WCHAR *params)
{
    UINT32 status = RESULT_OK;
    UINT32 evcode_base;

    elf = ela;
    elf->name = (char *)app_name;

	if(ldrIsLoaded((char*)app_name)) return RESULT_FAIL;

	evcode_base = elf->evbase + 1;
	evcode_base = ldrInitEventHandlersTbl((EVENT_HANDLER_ENTRY_T *)any_state_handlers, evcode_base);
	evcode_base = ldrInitEventHandlersTbl((EVENT_HANDLER_ENTRY_T *)main_state_handlers, evcode_base);

    status = APP_Register(&elf->evbase, 1, state_handling_table, HW_STATE_MAX, (void*)ELF_Start);
    if(status == RESULT_OK) ldrSendEvent(elf->evbase);

    return status;
}

//+++
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info)
{
    APP_T *app = NULL;
    UINT32 status = RESULT_OK;

    if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
    {
        print("Already loaded");
        return RESULT_OK; // Нам не нужен второй экземпляр
    }

    app = (APP_T*)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0 );
    if(!app)
    {
        print("Initialization failed");
        ldrUnloadElf(elf);
        return RESULT_OK;
    }
    elf->app = (APPLICATION_T *)app;

    status = APP_Start(ev_st, &app->apt, HW_STATE_INIT, state_handling_table, ELF_Exit, app_name, 0);
    if(status == RESULT_OK)
    {
        print("Started");
    }
    else
    {
        print("Start failed");
		APP_HandleFailedAppStart(ev_st, (APPLICATION_T*)app, 0);
		ldrUnloadElf(elf);
    }

    return RESULT_OK;
}

//+++
UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    free(pids);
    free(midlets);

    RemoveResources();
    APP_UtilUISDialogDelete(&app->dialog);

    UINT32 status = APP_ExitStateAndApp(ev_st, app, 0);
    ldrUnloadElf(elf);

    return status;
}

//+++
UINT32 MainStateEnter(EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    if(type!=ENTER_STATE_ENTER) return RESULT_OK;

    decodeJ2MEST();
    Update(ev_st, app, SELECTED_IDX);
    return RESULT_OK;
}

//+++
UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

//+++
UINT32 HandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st, app);
    app->exit_status = TRUE;
    return RESULT_OK;
}

//+++
UINT32 HandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

//+++
UINT32 Update (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem)
{
	UINT32 starting_num;
	ACTIONS_T action_list;

    RemoveResources();
    InitResources();

	InitDlgActions( &action_list );
    if (app->dialog != NULL) APP_UtilUISDialogDelete(&app->dialog);

	app->dialog = UIS_CreateList(&app->port, 0, ENTRIES_NUM, 0,&starting_num, 1, 2, &action_list, Resources[RES_LIST_CAPTION]);
    if(app->dialog == 0) return RESULT_FAIL;

	if(sItem != 0)
	{
		APP_UtilAddEvUISListChange(ev_st, app, 0, sItem, ENTRIES_NUM, TRUE, 2, FBF_LEAVE, NULL, NULL);
		UIS_HandleEvent(app->dialog, ev_st);
	}

	SendListItems(ev_st, app, 1, starting_num);
	return RESULT_OK;
}

//+++
UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status = APP_HandleUITokenGranted(ev_st, app);

    if((status == RESULT_OK) && (app->token_status == 2))
        status = APP_UtilChangeState(HW_STATE_MAIN, ev_st, app);

    return status;
}

//+++
UINT32 HandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    EVENT_T	*event;
    UINT32 start, num;

    if(!app->focused) return RESULT_OK;
    event = AFW_GetEv(ev_st);

    start = event->data.list_items_req.begin_idx;
    num = event->data.list_items_req.count;

    APP_ConsumeEv(ev_st, app);
    return SendListItems(ev_st, app, start, num);
}

//+++
UINT32 HandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	EVENT_T *event = AFW_GetEv(ev_st);
	SELECTED_IDX = event->data.index;
	return RESULT_OK;
}

UINT32 HandleSwitchDevice (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    TransFlash = !TransFlash;
	APP_ConsumeEv(ev_st, app);

	if(decodeJ2MEST() == RESULT_OK) Update(ev_st, app, SELECTED_IDX);
	else TransFlash = !TransFlash;

	return RESULT_OK;
}

//+++
UINT32 HandleSend (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 i, cnt;
    APP_ConsumeEv(ev_st, app);

    cnt = 0;
    if(SELECTED_IDX > 0)
    {
        if(pids != NULL)
        {
            for(i=0;i<ENTRIES_NUM;i++)
            {
                if(midlets[i].selected == TRUE)
                {
                    pids[cnt++] = midlets[i].midlet_app_id;
                }
            }
        }

        if(cnt == 0) SendViaBT(&midlets[SELECTED_IDX - 1].midlet_app_id, 1);
        else SendViaBT(pids, cnt);
    }

    return RESULT_OK;
}

//+++
UINT32 HandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(pids != NULL)
    {
        EVENT_T *event = AFW_GetEv(ev_st);
        midlets[event->data.index - 1].selected = (midlets[event->data.index - 1].selected ? FALSE : TRUE);
        Update(ev_st, app, event->data.index);
    }
    return RESULT_OK;
}

//+++
UINT32 SendListItems (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num)
{
	LIST_ENTRY_T			*plist; // Буффер для элементов списка
	UINT32					i, index, status=RESULT_OK;
    LIST_IMAGE_ELEMENT_T    *img_element;

    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
    if (midlets == NULL) return RESULT_FAIL;

	plist = (LIST_ENTRY_T*)device_Alloc_mem( sizeof(LIST_ENTRY_T), num);
	if(plist == NULL) return RESULT_FAIL;

    img_element = (LIST_IMAGE_ELEMENT_T*)device_Alloc_mem( sizeof(LIST_IMAGE_ELEMENT_T), num);
    if(img_element == NULL) return RESULT_FAIL;

	// Заполняем данные
	for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM); i++, index++)
	{
		plist[index].editable = FALSE;			// Поле нередактируемо
		plist[index].content.static_entry.formatting = 1;

        img_element[index].image.file_id = midlets[i-1].midlet_icon_id;
        img_element[index].image_type = LIST_IMAGE_DL_FS_MID_T;
        img_element[index].image_index = i;

        UIS_MakeContentFromString((midlets[i - 1].selected ? "j0Lq1Sp2" : "j0Lq1"), &(plist[index].content.static_entry.text), &img_element[index], midlets[i-1].midlet_name, DRM_SELECTED);
    }

	status = APP_UtilAddEvUISListData(ev_st, app, 0, start, num, FBF_LEAVE, sizeof(LIST_ENTRY_T)*num, plist);
    if( status!=RESULT_FAIL )
		status = UIS_HandleEvent(app->dialog, ev_st);

    device_Free_mem_fn(plist);
    device_Free_mem_fn(img_element);

	return status;
}

//+++
UINT32 InitResources( )
{
    UINT32 status;
    CONTENT_T dst;
    RES_ACTION_LIST_ITEM_T action[2];

    UIS_MakeContentFromString( "Mp0Mq1", &dst, (TransFlash ? DRM_EXTERNAL_MEM : DRM_INTERNAL_MEM), L"JavaTransfer");

	/* Ресурс для заголовка списка */
    status = DRM_CreateResource( &Resources[RES_LIST_CAPTION], RES_TYPE_CONTENT, (void*)&dst, sizeof(dst) );
	if( status!=RESULT_OK ) return status;

	/* Ресурс для действия */
    action[0].softkey_label = 0;
    action[0].list_label = 0x01001150;
    action[0].softkey_priority = 0;
    action[0].list_priority = 1;
    action[0].isExit = FALSE;
    action[0].sendDlgDone = FALSE;

    status = DRM_CreateResource( &Resources[RES_ACTION1], RES_TYPE_ACTION, (void*)&action[0], sizeof(RES_ACTION_LIST_ITEM_T));
	if( status!=RESULT_OK ) DRM_ClearResource( Resources[RES_LIST_CAPTION] );

	/* Ресурс для действия */
    action[1].softkey_label = 0;
    action[1].list_label = 0x01000356;
    action[1].softkey_priority = 0;
    action[1].list_priority = 2;
    action[1].isExit = FALSE;
    action[1].sendDlgDone = FALSE;

    status = DRM_CreateResource( &Resources[RES_ACTION2], RES_TYPE_ACTION, (void*)&action[1], sizeof(RES_ACTION_LIST_ITEM_T));
	if( status!=RESULT_OK ) DRM_ClearResource( Resources[RES_LIST_CAPTION] );

    return status;
}

//+++
UINT32 RemoveResources( )
{
	UINT32 i, status = RESULT_OK;

	for(i=0;i<RES_MAX;i++)
		status |= DRM_ClearResource(Resources[i]);

	return status;
}

//+++
UINT32 InitDlgActions( ACTIONS_T *action_list )
{
    action_list->action[0].operation = ACTION_OP_ADD;
    action_list->action[0].event = ldrFindEventHandlerTbl(main_state_handlers, HandleSwitchDevice);
    action_list->action[0].action_res = Resources[RES_ACTION1];

    action_list->action[1].operation = ACTION_OP_ADD;
    action_list->action[1].event = ldrFindEventHandlerTbl(main_state_handlers, HandleSend);
    action_list->action[1].action_res = Resources[RES_ACTION2];

    action_list->count = 2;

    return RESULT_OK;
}

//+++ декодирование J2MEST
UINT32 decodeJ2MEST(void)
{
    int i1 = 0; // кол-во мидлетов - 1
    int j = 0;

	UINT32 readen;
	FILE_HANDLE_T f;
    UINT32 fSize; // размер файла

    WCHAR IMEI[17];
    WCHAR midlet_idx[4];
    WCHAR midlet_uri[FS_MAX_URI_NAME_LENGTH + 1];
    WCHAR path_J2MEST[FS_MAX_URI_NAME_LENGTH + 1];

    J2MEST_RECORD_T rec;

    if (TransFlash)
    {
        DL_DbFeatureGetValueString(DB_FEATURE_IMEI, IMEI);
        u_strcpy(path_J2MEST, L"file://b/");
        u_strcat(path_J2MEST, kJavaDir);
        u_strcat(path_J2MEST, IMEI);
        u_strcat(path_J2MEST, L"/J2MEST");
    }
    else
    {
        u_strcpy(path_J2MEST, L"file://a/");
        u_strcat(path_J2MEST, kJavaDir);
        u_strcat(path_J2MEST, L"J2MEST");
    }

	// читаем J2MEST
	if (DL_FsFFileExist( path_J2MEST )) // проверяем есть ли файл
	{
	    ENTRIES_NUM = 0;
	    SELECTED_IDX = 0;
		f = DL_FsOpenFile(path_J2MEST, FILE_READ_MODE, 0);

        fSize = DL_FsGetFileSize(f);
        MAX_COUNT_MIDLETS = fSize/J2MEST_STEP;

        print("fSize = %d", fSize);
        print("MAX_COUNT_MIDLETS (fSize/J2MEST_STEP) = %d", MAX_COUNT_MIDLETS);

        free(midlets);
        midlets = malloc(MAX_COUNT_MIDLETS * sizeof(MIDLET));
        if (midlets == NULL) return RESULT_FAIL;
        memclr(midlets, MAX_COUNT_MIDLETS*sizeof(MIDLET));

		for(i1 = 0; i1 < MAX_COUNT_MIDLETS & i1*J2MEST_STEP < fSize ; i1++)
		{
            DL_FsFSeekFile(f, i1*J2MEST_STEP, SEEK_WHENCE_SET);
            DL_FsReadFile(&rec, sizeof(J2MEST_RECORD_T), 1, f, &readen);

			if((rec.unk1[1] & 0xF) != 0)
			{
                //Getting midlet name
				DL_Char_convUTF8toUCS2String(rec.midlet_name, 38, midlets[ENTRIES_NUM].midlet_name, 38);

				//Getting midlet index
                midlets[ENTRIES_NUM].midlet_index = i1;

                //Getting midlet icon id
                midlets[ENTRIES_NUM].midlet_icon_id = rec.midlet_icon;

                //Getting midlet uri
                if(TransFlash)
                {
                    u_strcpy(midlet_uri, L"file://b/");
                    u_strcat(midlet_uri, kJavaDir);
                    u_strcat(midlet_uri, IMEI);
                }
                else
                {
                    u_strcpy(midlet_uri, L"file://e/");
                    u_strcat(midlet_uri, kJavaDir);
                }

                u_strcat(midlet_uri, L"j2me");
                u_ltou(midlets[ENTRIES_NUM].midlet_index, midlet_idx);
                u_strcat(midlet_uri, midlet_idx);
                u_strcat(midlet_uri, L".jar");

                //Getting midlet fs id
                if(DL_FsFFileExist(midlet_uri) == TRUE)
                    DL_FsGetIDFromURI(midlet_uri, &midlets[ENTRIES_NUM].midlet_app_id);

                ENTRIES_NUM++;
            }
		}

		//Память для передачи нескольких приложений
        free(pids);
        pids = malloc(ENTRIES_NUM * sizeof(DL_FS_MID_T));
        if(pids != NULL) memclr(pids, ENTRIES_NUM * sizeof(DL_FS_MID_T));

        DL_FsCloseFile(f);
    }
    else
    {
        return RESULT_FAIL;
    }

    return RESULT_OK;
}

//+++
UINT32 SendViaBT(DL_FS_MID_T *ids, UINT32 count)
{
    RAINBOW_POG_SEND_BT_ATT att;
    memclr(&att, sizeof(RAINBOW_POG_SEND_BT_ATT));

    att.b_vobjectid = OBEX_OBJECT_FILE;
    att.obex_action = OBEX_ACTION_COPY;
    att.p_ids = ids;
    att.num_files = count;
    att.selected_transport_type = OBEX_TRANSPORT_BT;
    att.transport_list_size = 0;

    AFW_CreateInternalQueuedEvAux(EV_BT_SEND, FBF_LEAVE, sizeof(RAINBOW_POG_SEND_BT_ATT), &att);
    return RESULT_OK;
}
