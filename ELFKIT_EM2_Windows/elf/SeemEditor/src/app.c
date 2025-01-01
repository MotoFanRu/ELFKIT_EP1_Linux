#include "app.h"
#include "graphics.h"

UIS_DIALOG_T dialog;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "SeemEditor2";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,   HandleReqExit               },
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked    },
	{ STATE_HANDLERS_END,           NULL                        },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
	{ EV_GRANT_TOKEN,		        HandleUITokenGranted        },
	{ STATE_HANDLERS_END,           NULL        			    },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_INK_KEY_PRESS,             HandleKeyPress              },
	{ STATE_HANDLERS_END,	        NULL                        },
};

static EVENT_HANDLER_ENTRY_T edit_state_handlers[] =
{
    { EV_DATA,                      HandleEditData              },
    { EV_DONE,                      HandleEditOk                },
	{ EV_CANCEL,                 	HandleEditExit              },
    { STATE_HANDLERS_END,         	NULL                        },
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
      MainStateExit,
      main_state_handlers
    },

    { APP_STATE_EDIT,
      EditStateEnter,
      EditStateExit,
      edit_state_handlers
    }
};

//+++
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

//+++
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
	app = (APP_T*)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Initialize resources
    APPInitResources(&app->apt);

    //Initialize display
    GRAPHIC_POINT_T display;
    UIS_CanvasGetDisplaySize(&display);

    //Initialize drawing buffer
    app->dBuf.w = display.x + 1;
    app->dBuf.h = display.y + 1;
    app->dBuf.buf = NULL;

    //Set cursor position on "Seem" label
    app->cursorPosition = POSITION_SEEM;

    //Initialize seem description
    memset(&app->seemDescription, 0, sizeof(SEEM_ELEMENT_DATA_CONTROL_T));

    //Initialize seem value
    app->seemValue = 0;

    //Initialize seem status
    app->seemLoaded = FALSE;

    //Starting application
    status = APP_Start(ev_st, &app->apt, APP_STATE_INIT, state_handling_table, ELF_Exit, app_name, 0);
    if(status == RESULT_OK)
    {
        PFprintf("%s: Application has been started successfully!\n", app_name);
    }
    else
    {
        APPFreeResources(&app->apt);
		PFprintf("%s: Can't start application!\n", app_name);
		APP_HandleFailedAppStart(ev_st, (APPLICATION_T*)app, 0);
		ldrUnloadElf(elf);
		return RESULT_OK;
    }

	return RESULT_OK;
}

//+++
UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APPFreeResources(app);
	APP_ExitStateAndApp(ev_st, app, 0);
	ldrUnloadElf(elf);
	return RESULT_OK;
}

//+++
UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    app->exit_status = TRUE;
    return RESULT_OK;
}

//+++
UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT32 status = APP_HandleUITokenGranted(ev_st, app);

	if(status == RESULT_OK && app->token_status == 2)
		status = APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);

	return status;
}

//+++
UINT32 APPInitResources (APPLICATION_T *app)
{
    UINT32 status = RESULT_OK;
    WCHAR SeemString[] = L"Seem Record Offset";
    WCHAR ValueString[] = L"Value";

    status |= DRM_CreateResource(&thisapp(app)->SeemCaption, RES_TYPE_STRING, (VOID *)SeemString, (u_strlen(SeemString) + 1) * sizeof(WCHAR));
    status |= DRM_CreateResource(&thisapp(app)->ValueCaption, RES_TYPE_STRING, (VOID *)ValueString, (u_strlen(ValueString) + 1) * sizeof(WCHAR));

    return status;
}

//+++
UINT32 APPFreeResources (APPLICATION_T *app)
{
    UINT32 i, status = RESULT_OK;

    status |= DRM_ClearResource(thisapp(app)->SeemCaption);
    status |= DRM_ClearResource(thisapp(app)->ValueCaption);

    return status;
}

///main state handlers
//+++
UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    if(type != ENTER_STATE_ENTER)
        return RESULT_OK;

    //Create color canvas
    app->dialog = UIS_CreateColorCanvas(&app->port, &thisapp(app)->dBuf, TRUE);
    if(app->dialog == 0) return RESULT_FAIL;

    //Draw SeemEditor GUI.
    dialog = app->dialog;
    DrawGUI(app);

    //return
	return RESULT_OK;
}

//+++
UINT32 MainStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    APP_UtilUISDialogDelete(&app->dialog);
	return RESULT_OK;
}

//+++
UINT32 HandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT8 bit = 0;
    UINT8 key = GET_KEY(ev_st);
    APP_ConsumeEv(ev_st, app);

    switch(key)
    {
        //move up cursor position +++
        case KEY_2:
        case KEY_UP:
            if(thisapp(app)->cursorPosition != POSITION_SEEM)
                thisapp(app)->cursorPosition += 1;
            break;

        //move down cursor position +++
        case KEY_8:
        case KEY_DOWN:
            if(thisapp(app)->cursorPosition != POSITION_BIT_0)
                thisapp(app)->cursorPosition -= 1;
            break;

        //change application state or change bit value +++
        case KEY_5:
        case KEY_CAM:
            if(thisapp(app)->cursorPosition == POSITION_SEEM || thisapp(app)->cursorPosition == POSITION_VALUE)
            {
                return APP_UtilChangeState(APP_STATE_EDIT, ev_st, app);
            }
            else
            {
                bit = BitOperation(&thisapp(app)->seemValue, thisapp(app)->cursorPosition, BIT_GET);
                thisapp(app)->seemValue = BitOperation(&thisapp(app)->seemValue, thisapp(app)->cursorPosition, (bit == 1) ? BIT_CLR : BIT_SET);
            }
            break;

        //load seem value +++
        case KEY_SOFT_LEFT:
        case KEY_STAR:
            thisapp(app)->seemDescription.seem_size = 1;
            if(SEEM_ELEMENT_DATA_read(&thisapp(app)->seemDescription, &thisapp(app)->seemValue, TRUE) == 0)
                thisapp(app)->seemLoaded = TRUE;
            else
                thisapp(app)->seemLoaded = FALSE;
            break;

        //save seem value +++
        case KEY_SOFT_RIGHT:
        case KEY_POUND:
            if(thisapp(app)->seemLoaded == TRUE)
                SEEM_ELEMENT_DATA_write(&thisapp(app)->seemDescription, &thisapp(app)->seemValue);
            return RESULT_OK;
            break;

        //close app +++
        case KEY_RED:
            app->exit_status = TRUE;
            return RESULT_OK;

        default:
            return RESULT_OK;
    }

    //draw SeemEditor gui
    DrawGUI(app);

    //return result
    return RESULT_OK;
}

///edit state handlers
//+++
UINT32 EditStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    char a_input[60];
    WCHAR u_input[60];

    switch(thisapp(app)->cursorPosition)
    {
        case POSITION_SEEM: //seem
            //value
            sprintf(a_input, "%04X %04X %04X", thisapp(app)->seemDescription.seem_element_id, thisapp(app)->seemDescription.seem_record_number, thisapp(app)->seemDescription.seem_offset);
            u_atou(a_input, u_input);
            //dialog
            app->dialog = UIS_CreateCharacterEditor(&app->port, u_input, 5, 14, FALSE, NULL, thisapp(app)->SeemCaption);
            break;

        case POSITION_VALUE: //value
            //value
            sprintf(a_input, "%X", thisapp(app)->seemValue);
            u_atou(a_input, u_input);
            //dialog
            app->dialog = UIS_CreateCharacterEditor(&app->port, u_input, 5, 2, FALSE, NULL, thisapp(app)->ValueCaption);
            break;
    }

    return (app->dialog == 0) ? RESULT_FAIL : RESULT_OK;
}

//+++
UINT32 EditStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
    APP_UtilUISDialogDelete(&app->dialog);
    app->dialog = NULL;

	return RESULT_OK;
}

//+++
UINT32 HandleEditOk (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status = 0;
    ADD_EVENT_DATA_T ev_data;

    AFW_AddEvEvD(ev_st, EV_REQUEST_DATA, &ev_data);
    UIS_HandleEvent(app->dialog, ev_st);

    return status;
}

//+++
UINT32 HandleEditExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);
}

//+++
UINT32 HandleEditData (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    char buf[60];
    char *values[3] = {NULL, NULL, NULL};
    UINT32 i, j, len;

    if(AFW_GetEv(ev_st)->attachment != 0)
    {
		u_utoa(AFW_GetEv(ev_st)->attachment, buf);
		switch(thisapp(app)->cursorPosition)
		{
			case POSITION_SEEM: //seem
                //parse input string
                len = strlen(buf);
                trimSpaces(buf, (char *)(buf + len));
                values[0] = &buf[0];

                for(i = 0, j = 1; i < len && j < 3; i += 1)
                {
                    if(buf[i] == ' ')
                    {
                        buf[i] = 0;
                        if(buf[i+1] != ' ') values[j++] = &buf[i+1];
                    }
                }

                //save results in seemDescription
                thisapp(app)->seemDescription.seem_element_id = (values[0] != NULL) ? strtoul(values[0], NULL, 16) : 0;
                thisapp(app)->seemDescription.seem_record_number = (values[1] != NULL) ? strtoul(values[1], NULL, 16) : 0;
                thisapp(app)->seemDescription.seem_offset = (values[2] != NULL) ? strtoul(values[2], NULL, 16) : 0;

                break;

			case POSITION_VALUE: //value
                thisapp(app)->seemValue = strtoul(buf, NULL, 16);
                break;
        }
    }

    return APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);
}


///Utils
//+++
UINT8 BitOperation(VOID *src, UINT8 pos, BIT_OPERATION_TYPE type)
{
    if(src == NULL || pos > 7)
        return 2;

    UINT8 value = *((UINT8 *)src);

    switch(type)
    {
        case BIT_CLR:
            value = value & (~(1 << pos));
            break;

        case BIT_GET:
            value = (value & (1 << pos)) >> pos;
            break;

        case BIT_SET:
            value = value | (1 << pos);
            break;

        default:
            value = 2;
            break;
    }

    return value;
}

#define RECTANGLE_COUNT          8
#define RECTANGLE_REGION        20
#define RECTANGLE_SIZE_BIG      16
#define RECTANGLE_SIZE_SMALL    10

//+++
VOID DrawGUI (APPLICATION_T *app)
{
    UINT32 i, x_pos;
    char a_text[60];
    WCHAR u_text[60];
    GRAPHIC_POINT_T point;
    UINT8 cursorPosition = thisapp(app)->cursorPosition;

    //background
    UIS_CanvasSetFont(0, dialog);
	SetFillColor(0xFFFFFF00);
	SetForegroundColor(0x00000000);
	SetBackgroundColor(0xFFFFFFFF);
    FillRect(0, 0, thisapp(app)->dBuf.w, thisapp(app)->dBuf.h);

    //seem label
    SetBackgroundColor(0xFFFFFFFF);
    if(cursorPosition == POSITION_SEEM) SetForegroundColor(0xFF000000);
    else SetForegroundColor(0x00000000);

    u_strcpy(u_text, L"Seem Record Offset:");
    DrawText(u_text, thisapp(app)->dBuf.w/2, 10, ANCHOR_HCENTER | ANCHOR_VCENTER);

    //seem description
    sprintf(a_text, "%04X %04X %04X", thisapp(app)->seemDescription.seem_element_id, thisapp(app)->seemDescription.seem_record_number, thisapp(app)->seemDescription.seem_offset);
    u_atou(a_text, u_text);
    DrawText(u_text, thisapp(app)->dBuf.w/2, 30, ANCHOR_HCENTER | ANCHOR_VCENTER);


    //value label
    SetBackgroundColor(0xFFFFFFFF);
    if(cursorPosition == POSITION_VALUE) SetForegroundColor(0xFF000000);
    else SetForegroundColor(0x00000000);

    u_strcpy(u_text, L"Value:");
    DrawText(u_text, thisapp(app)->dBuf.w/2, 50, ANCHOR_HCENTER | ANCHOR_VCENTER);

    //value description
    sprintf(a_text, "%X", thisapp(app)->seemValue);
    u_atou(a_text, u_text);
    DrawText(u_text, thisapp(app)->dBuf.w/2, 70, ANCHOR_HCENTER | ANCHOR_VCENTER);


    //bits label
    SetBackgroundColor(0xFFFFFFFF);
    SetForegroundColor(0x00000000);

    u_strcpy(u_text, L"Bits:");
    DrawText(u_text, thisapp(app)->dBuf.w/2, 90, ANCHOR_HCENTER | ANCHOR_VCENTER);

    //bits description
    x_pos = (thisapp(app)->dBuf.w - RECTANGLE_COUNT*RECTANGLE_REGION)/2;
    for(i = 0; i < RECTANGLE_COUNT; i += 1)
    {
        if(cursorPosition == RECTANGLE_COUNT-i-1)
        {
            SetFillColor(0xFF000000);
            SetForegroundColor(0xFF000000);
        }
        else
        {
            SetFillColor(0x00000000);
            SetForegroundColor(0x00000000);
        }

        DrawRect(x_pos + i*RECTANGLE_REGION, 120, RECTANGLE_SIZE_BIG, RECTANGLE_SIZE_BIG);
        if(BitOperation((void *)&thisapp(app)->seemValue, 7-i, BIT_GET)) FillRect(x_pos + i*RECTANGLE_REGION + (RECTANGLE_SIZE_BIG - RECTANGLE_SIZE_SMALL)/2, 120 + (RECTANGLE_SIZE_BIG - RECTANGLE_SIZE_SMALL)/2, RECTANGLE_SIZE_SMALL, RECTANGLE_SIZE_SMALL);

        u_ultou(RECTANGLE_COUNT-i-1, u_text);
        DrawText(u_text, x_pos + i*RECTANGLE_REGION + RECTANGLE_SIZE_BIG/2, 130, ANCHOR_HCENTER | ANCHOR_VCENTER);
    }

    //draw softkeys labels & status
    SetBackgroundColor(0xFFFFFFFF);
    SetForegroundColor(0x00000000);

    //status
    u_strcpy(u_text, L"Status:");
    DrawText(u_text, thisapp(app)->dBuf.w/2, 150, ANCHOR_HCENTER | ANCHOR_VCENTER);

    u_strcpy(u_text, (thisapp(app)->seemLoaded == TRUE) ? L"seem is loaded" : L"seem is not loaded");
    DrawText(u_text, thisapp(app)->dBuf.w/2, 170, ANCHOR_HCENTER | ANCHOR_VCENTER);

    //left softkey
    u_strcpy(u_text, L"Load");
    DrawText(u_text, 5, thisapp(app)->dBuf.h - 5, ANCHOR_LEFT | ANCHOR_BOTTOM);

    //right softkey
    u_strcpy(u_text, L"Save");
    DrawText(u_text, thisapp(app)->dBuf.w - 5, thisapp(app)->dBuf.h - 5, ANCHOR_RIGHT | ANCHOR_BOTTOM);
}
