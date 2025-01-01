#include "app.h"
#include "usb_utils.h"

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "USBMode";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   ELF_Exit                    },
    { STATE_HANDLERS_RESERVED,      ELF_APPShow                 },
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
    { EV_DEVICE_ATTACH,             HandleConnectDevice         },
    { EV_DEVICE_DETACH,             HandleDisconnectDevice      },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T bkg_state_handlers[] =
{
    { STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T gui_state_handlers[] =
{
    { EV_DONE,                      HandleDone                  },
	{ EV_SELECT,                    HandleSelect                },
	{ EV_REQUEST_LIST_ITEMS,        HandleListReq	            },
	{ EV_FLIP_CLOSE,                ELF_APPHide 	            },
	{ EV_FLIP_OPEN,                 ELF_APPHide   	            },
	{ EV_DIALOG_DONE,               ELF_APPHide                 },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { APP_STATE_ANY,
      NULL,
      NULL,
      any_state_handlers
    },

    { APP_STATE_BKG,
      NULL,
      NULL,
      bkg_state_handlers
    },

    { APP_STATE_GUI,
      GuiStateEnter,
      GuiStateExit,
      gui_state_handlers
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
    elf->evbase = USB_SETTINGS_EV;

    //Register application
    status = APP_Register(&elf->evbase, 2, state_handling_table, APP_STATE_MAX, (void*)ELF_Start);
    if(status == RESULT_OK)
    {
        PFprintf("%s: Application has been registered successfully!\n", app_name);
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
    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
	    //Show app
        EVENT_CODE_T ev_code = ldrFindEventHandlerTbl(any_state_handlers, ELF_APPShow);
        return ldrSendEventToAppEmpty(elf->app->afw_id, ev_code);
	}

    APP_T *app = NULL;
    UINT32 status = RESULT_OK;

	//Initialize application
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEventPrepost, sizeof(APP_T), reg_id, 0, 1, 1, 1, 0, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Init resources
    USBReadConfig(&app->cfg);
    USBDefaulMode(&app->curr_mode, USB_MODE_ACTION_GET);

    app->entries_num = 4;
    app->item = (LINE *)device_Alloc_mem(app->entries_num, sizeof(LINE));

    app->item[0].drm_id = 0x120000DA;
    app->item[0].str_id = 0x01001A8B;       //Карта памяти
    app->item[0].value = app->cfg.MCARDID;  //Режим "Карта памяти"

    app->item[1].drm_id = 0x120000D9;
    app->item[1].str_id = 0x01001A8C;       //Модем
    app->item[1].value = app->cfg.ModemID;  //Режим модема

    app->item[2].drm_id = 0x120000E2;
    app->item[2].str_id = 0x01000E62;       //Система
    app->item[2].value = app->cfg.P2KID;    //Режим P2K

    app->item[3].drm_id = 0x120000D0;
    app->item[3].str_id = 0x01000793;       //Загрузить Java
    app->item[3].value = USB_MODE_SJAL;     //Режим "Загрузить JAVA" (на самом деле это режим модема)

    app->isShow = FALSE;
    app->USBstatus = FALSE;

    //Starting application
    status = APP_Start(ev_st, &app->apt, APP_STATE_BKG, state_handling_table, ELF_APPHide, app_name, 0);
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if(AFW_GetEvCode(ev_st) == EV_DONE)
        APP_ConsumeEv(ev_st, app);

	APP_ExitStateAndApp(ev_st, app, 0);
    APP_USBSETTINGS_MainRegister();

	ldrUnloadElf(elf);
	return RESULT_OK;
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
    if(app->state != APP_STATE_BKG)
        APP_UtilChangeState(APP_STATE_BKG, ev_st, app);

    ELF_ChangeRStack(ev_st, app, FALSE);
	return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_APPShow (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	if (app->state == APP_STATE_BKG)
	    APP_UtilChangeState(APP_STATE_GUI, ev_st, app);

    ELF_ChangeRStack(ev_st, app, TRUE);
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 GuiStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    UINT32 i;
    APP_T *papp = (APP_T*)app;
	UINT32 starting_num;

    if(type == ENTER_STATE_ENTER)
    {
        app->dialog = UIS_CreateList(&app->port, 0, papp->entries_num, 0, &starting_num, 0, 2, NULL, LANG_USB_SETTINGS);
        if(app->dialog == 0) return RESULT_FAIL;
    }

    USBDefaulMode(&papp->curr_mode, USB_MODE_ACTION_GET);

    for(i=1; i<=papp->entries_num; i++)
        if(papp->item[i-1].value == papp->curr_mode)
            break;

    if(i > papp->entries_num) i = 1;

    APP_UtilAddEvUISListChange(ev_st, app, 0, i, papp->entries_num, TRUE, 2, FBF_LEAVE, NULL, NULL);
    HandleSendListItems(ev_st, app, 1, starting_num);

	return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 GuiStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    if(type == EXIT_STATE_EXIT) APP_UtilUISDialogDelete(&app->dialog);
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st, app);
    return ELF_APPHide(ev_st, app);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
    EVENT_T *event = AFW_GetEv(ev_st);

    BOOL SJAL = FALSE;
    USB_MODE_T val = papp->item[event->data.index - 1].value;

    //Получение режима из seem.
    USBDefaulMode(&papp->curr_mode, USB_MODE_ACTION_GET);
    APP_ConsumeEv(ev_st, app);

    //Если выбран новый режим, то устанавливаем его, иначе ничего не делаем.
    if(papp->curr_mode != val)
    {
        //Если выбрали режим "Загрузить JAVA", то устанавливаем флаг в TRUE и меняем режим на режим модема.
        if(val == USB_MODE_SJAL)
        {
            SJAL = TRUE;
            val = papp->cfg.ModemID;
        }
        //Устанавливаем новый режим в симах
        USBDefaulMode(&val, USB_MODE_ACTION_SET);

        //Если кабель подключён, то меняем режим "на лету".
        if(USB_CABLE_ATTACHED(papp->cfg.CableID))
        {
            //Если был выбран режим "Загрузить JAVA", то запускаем SJAL
            if(SJAL == TRUE) SJAL_Start();
            //Меняем режим
            USB_CHANGE_MODE(val);

            return ELF_APPHide(ev_st, app);
        }

        //Иначе просто обновляем диалог
        return GuiStateEnter(ev_st, app, ENTER_STATE_RESUME);
    }

    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 start, num;
	EVENT_T *event = AFW_GetEv(ev_st);

	if(!app->focused) return RESULT_OK;

	start = event->data.list_items_req.begin_idx;
	num = event->data.list_items_req.count;

	APP_ConsumeEv(ev_st, app);
	return HandleSendListItems(ev_st, app, start, num);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleConnectDevice (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
	EVENT_T	*event = AFW_GetEv(ev_st);

	if (IS_USB_CABLE_EVENT(event, papp->cfg.CableID))
	{
        APP_ConsumeEv(ev_st, app);
        papp->USBstatus = TRUE;

        if (papp->cfg.CableReaction == TRUE)
            ELF_APPShow(ev_st, app);
	}

    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleDisconnectDevice (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
	EVENT_T	*event = AFW_GetEv(ev_st);

	if (IS_USB_CABLE_EVENT(event, papp->cfg.CableID))
	{
        APP_ConsumeEv(ev_st, app);
        papp->USBstatus = FALSE;

        if (papp->cfg.CableReaction == TRUE)
            ELF_APPHide(ev_st, app);
	}
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleSendListItems (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APP_T *papp = (APP_T*)app;
	UINT32 i, index, status = RESULT_OK;
	LIST_ENTRY_T *plist;

    if(num == 0) return RESULT_FAIL;

	plist = (LIST_ENTRY_T *)device_Alloc_mem(num, sizeof(LIST_ENTRY_T)); //calloc
	if(plist == NULL) return RESULT_FAIL;

	for(index=0, i=start; (i<start+num) && (i<=papp->entries_num); i++, index++)
	{
		plist[index].editable = FALSE;			// Поле нередактируемо
		plist[index].content.static_entry.formatting = 1;

		UIS_MakeContentFromString( papp->item[i-1].value == papp->curr_mode ? "Mp0s1Sp2" : "Mp0s1", &(plist[index].content.static_entry.text), papp->item[i-1].drm_id, papp->item[i-1].str_id, DRM_SELECTED);
	}

	status = APP_UtilAddEvUISListData(ev_st, app, 0, start, num, FBF_LEAVE, sizeof(LIST_ENTRY_T)*num, plist);
	if(status != RESULT_FAIL) status = UIS_HandleEvent(app->dialog, ev_st);

	device_Free_mem_fn(plist);
	return status;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 USBReadConfig (USBConfig *cfg)
{
    UINT32 Size, R;
    FILE_HANDLE_T hFile;

    if(cfg == NULL)
        return 0;

    hFile = DL_FsOpenFile(USB_CFG_PATH, FILE_READ_MODE, 0);
    Size = DL_FsGetFileSize(hFile);

    if(hFile == FILE_HANDLE_INVALID || Size == 0)
    {
        //Configurate cable for RAZR V3x R252211LD_U_85.9B.E6P
        memcpy(cfg, &default_config, sizeof(USBConfig));
        return 1;
    }

    DL_FsReadFile(cfg, sizeof(USBConfig), 1, hFile, &R);
    DL_FsCloseFile(hFile);
    return 2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 USBDefaulMode (USB_MODE_T *mode, USB_MODE_ACTION_T Action)
{
    UINT8 i, data = 0;
    UINT32 status = RESULT_OK;
	SEEM_ELEMENT_DATA_CONTROL_T	seem;

	seem.seem_element_id = SEEM_USB_CDC_CONFIG;
	seem.seem_record_number = 1;
	seem.seem_offset = 0;
	seem.seem_size = 1;

    switch(Action)
    {
        case USB_MODE_ACTION_GET:
            status = SEEM_ELEMENT_DATA_read(&seem, &data, TRUE);
            *mode = (usb_def_cfg)[data & 0x0F];
            break;

        case USB_MODE_ACTION_SET:
            for (i = 0; i <= 0x0F; i++)
                if (*mode == (usb_def_cfg)[i])
                    break;

            data = (i <= 0x0F) ? i : 0x0F;

            data |= 0xF0;
            status = SEEM_ELEMENT_DATA_write(&seem, &data);
            break;
    }

    return status;
}
