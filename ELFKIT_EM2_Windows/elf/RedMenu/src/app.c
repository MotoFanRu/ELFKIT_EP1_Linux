#include "app.h"

ldrElf *elf = NULL;
WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];  //Путь к графике
const char app_name[APP_NAME_LEN] = "RedMenu";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   ELF_Exit                    },
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
    { EV_POWERDOWN,                 HandlePowerDown             },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
	{ EV_FLIP_CLOSE,                ELF_APPHide 	            },
	{ EV_FLIP_OPEN,                 ELF_APPHide   	            },
	{ EV_DONE,                      ELF_APPHide                 },
	{ EV_DIALOG_DONE,               ELF_APPHide                 },
	{ EV_SELECT,                    HandleSelect                },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
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

    { APP_STATE_MAIN,
      MainStateEnter,
      NULL,
      main_state_handlers
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

        //Getting path to graphics
        u_strcpy(uri, L"file:/\0");
        DL_FsGetURIFromID(&elf->id, (uri + 6));
        WCHAR *ptr = uri + u_strlen(uri);
        while(*ptr != L'/') ptr--;
        u_strcpy(ptr, L"/graphics/\0");

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
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEventPrepost, sizeof(APP_T), reg_id, 0, 1, 1, 1, 0, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Init resources
    app->plist = NULL;
    app->ilist = NULL;
    app->isShow = FALSE;
    app->EntriesNum = 5;

    //Starting application
    status = APP_Start(ev_st, &app->apt, APP_STATE_INIT, state_handling_table, ELF_APPHide, app_name, 0);
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
	APP_UtilUISDialogDelete(&app->dialog);
	APP_ExitStateAndApp(ev_st, app, 0);

	ldrUnloadElf(elf);
	return RESULT_OK;
}

UINT32 ELF_ChangeRStack (EVENT_STACK_T *ev_st, APPLICATION_T *app, BOOL show)
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

UINT32 ELF_APPHide (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	ELF_ChangeRStack(ev_st, app, FALSE);
	return RESULT_OK;
}

UINT32 ELF_APPShow (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	if (app->state != APP_STATE_MAIN)
        APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);

	ELF_ChangeRStack(ev_st, app, TRUE);
    return RESULT_OK;
}

UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    APP_T *papp = (APP_T*)app;
	if(type != ENTER_STATE_ENTER) return RESULT_OK;

    //Выделяем память
    papp->plist = device_Alloc_mem(papp->EntriesNum, sizeof(LIST_ENTRY_T));
    if(papp->plist == NULL) return RESULT_FAIL;

    papp->ilist = device_Alloc_mem(papp->EntriesNum, sizeof(LIST_IMAGE_ELEMENT_T));
    if(papp->ilist == NULL) return RESULT_FAIL;

    //Создаём пункты
    Util_MakeItems(papp->plist, papp->ilist, papp->EntriesNum);

    //Создаём список
	app->dialog = UIS_CreateStaticList(&app->port, 0, papp->EntriesNum, 0, papp->plist, 0, 2, NULL, LANG_SELECTION_MENU);
	if(app->dialog == 0) return RESULT_FAIL;

	device_Free_mem_fn(papp->plist);
    device_Free_mem_fn(papp->ilist);

	return RESULT_OK;
}

UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	UINT32 status = APP_HandleUITokenGranted(ev_st, app);

	if((status == RESULT_OK) && (app->token_status == 2))
		status = APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);

    return status;
}

UINT32 HandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	EVENT_T *event = AFW_GetEv(ev_st);
	APP_ConsumeEv(ev_st, app);

	switch(event->data.index)
	{
		case 1:
			DL_PudSoftReset();
			break;
		case 2:
            pu_main_powerdown(0);
			break;
		case 3:
			AFW_CreateInternalQueuedEvAux(0x73D, FBF_FREE, 0, NULL);
			break;
	    case 4:
            AFW_CreateInternalQueuedEvAux(0xEEED, FBF_FREE, 0, NULL);
            break;
        case 5:
            AMS_StopUserVM();
			break;
	}

	return  ELF_APPHide(ev_st, app);
}

UINT32 HandlePowerDown (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st, app);
    if(Util_isKeypadLock() == 1) return RESULT_OK;
    if(JavaAppRunning() == 1) AMS_SuspendUserVM();
    ELF_APPShow(ev_st, app);
    return RESULT_OK;
}

UINT32 Util_MakeItems (LIST_ENTRY_T *plist, LIST_IMAGE_ELEMENT_T *ilist, UINT32 num)
{
    UINT32 i;

    if(plist == NULL || ilist == NULL)
        return RESULT_FAIL;

	for(i=0; i<num; i++)
	{
		plist[i].editable = FALSE;
		plist[i].content.static_entry.formatting = 1;

        WCHAR tmp_uri[FS_MAX_URI_NAME_LENGTH + 1];
        u_strcpy(tmp_uri, uri);
        u_strcat(tmp_uri, icons[i]);

        DL_FsGetIDFromURI(tmp_uri, &ilist[i].image.file_id);
        ilist[i].image_index = i + 1;
        ilist[i].image_type = LIST_IMAGE_DL_FS_MID_T;

		switch(i)
		{
			case 0:
				UIS_MakeContentFromString("j0s1", &(plist[i].content.static_entry.text), &ilist[i], 0x010017F7); //0x010017F7 | Рестарт
				break;
			case 1:
				UIS_MakeContentFromString("j0s1", &(plist[i].content.static_entry.text), &ilist[i], 0x01002257); //0x01002257 | Выключить
				break;
			case 2:
				UIS_MakeContentFromString("j0s1", &(plist[i].content.static_entry.text), &ilist[i], 0x010000BE); //0x010000BE - Заблокировать
				break;
		    case 3:
                UIS_MakeContentFromString("j0s1", &(plist[i].content.static_entry.text), &ilist[i], LANG_APPLICATIONS);
                break;
            case 4:
                UIS_MakeContentFromString("j0s1 q2", &(plist[i].content.static_entry.text), &ilist[i], 0x01001204, L"Java");
				break;
		}
	}

	return RESULT_OK;
}

BOOL Util_isKeypadLock (void)
{
	UINT8 keypad_statate;
	DL_DbFeatureGetCurrentState(DB_FEATURE_KEYPAD_STATE, &keypad_statate);
	return keypad_statate;
}
