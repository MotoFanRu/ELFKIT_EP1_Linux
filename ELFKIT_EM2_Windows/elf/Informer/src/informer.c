#include "informer.h"
#include "lngparser.h"
#include "xmlparser.h"
#include "iniparser.h"

///INFO*
INFO_DATE_T     info_date;
INFO_BATTERY_T  info_battery;
INFO_SIGNAL_T   info_signal;
INFO_CELLS_T    info_cells;
INFO_DISKS_T    info_disks;
INFO_WEATHER_T  info_weather;
INFO_BALANCE_T  info_balance;
INFO_ALARM_T    info_alarm;
INFO_DATEBOOK_T info_datebook;
INFO_CALLTIME_T info_calltime;

///
SU_PORT_T su_port;
UIS_DIALOG_T dialog;
DRAWING_BUFFER_T bufd;
UINT16 current_skin_index = 0xFFFF;

///
BOOL net = FALSE;
BOOL exit = FALSE;

///
UINT32 timer_handle[COUNT_TIMERS];              //хэндлы таймеров
WCHAR ELF_folder[FS_MAX_URI_NAME_LENGTH + 1];   //папка эльфа

//2 предидущие нажатые кнопки
INT16 key1 = -1;
INT16 key2 = -1;


/// Weather
// сюда запомним адреса функций
socket_f *              socket = NULL;
socket_connect_f *		socket_connect = NULL;
socket_write_f *		socket_write = NULL;
socket_read_f *			socket_read = NULL;
socket_bind_f *			socket_bind = NULL;
socket_close_f *		socket_close = NULL;
socket_delete_f *		socket_delete = NULL;

HSOCK g_socket = HSOCK_INVALID;
FILE_HANDLE_T fh = FILE_HANDLE_INVALID;


INT32   rlen=0; // прочитанно байт
UINT32  content_len = SIZE_BUF; // размер загружаемого файла

/// Balnce
USSD_SS_ID_T ss_id = 0xFFFF;

/// DateBook
UINT32 month_day = 0xFFFFFFFF;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "Informer";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT, HandleReqExit         },
    { EV_IDLE_FOCUS,            IdleStart               },
    { EV_DISPLAY_ACTIVE,        RepaintStart            },

	{ EV_REG_NETWORK,			RegNetwork				},
    { EV_KEY_PRESS,             HandleKeypress          },

    { EV_USSD_NOTIFY,           USSDHandleNotify        },
    { EV_USSD_REQUEST,          USSDHandleNotify        },
    { EV_USSD_COMPLETE,         USSDHandleNotify        },
    { EV_USSD_OPERATION,        USSDHandleNotify        },

    { EV_DBK_DATABASE_QUERY,    DBKHandleQuery          },
    { EV_DBK_DATABASE_ALTERED,  DBKHandleSendRequest    },
    { EV_DBK_MONTH_VIEW,		DBKHandleView           },
    { EV_DBK_WEEK_VIEW,         DBKHandleView           },

    { EV_TIMER_EXPIRED,       	TimerHandle             },
    { EV_ALARMCLK_EXPIRED,      AlarmHandle             },

    //{ EV_ALMCLK_CHANGE,         ALMCLKHandleNotify      },


    // Список всегда должен заканчиватся такой записью
    { STATE_HANDLERS_END,       NULL                    },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { STATE_HANDLERS_RESERVED,	HandleAPIAnsw		},	// Здесь будет ответ на запрос socket api. (evcode_base + 0)
	{ STATE_HANDLERS_RESERVED,	HandleSockAnsw		},	// Здесь будет ответ от сокетов уже. (evcode_base + 1)

    //{ EV_REG_NETWORK,           NetworkReg          },


    { STATE_HANDLERS_END,           NULL            },
};

/* Таблица соответствий обработчиков, состояний и функций входа-выхода из состояния.
   Порядок состояний такой же, как в enum-e */
static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { IF_STATE_ANY,               // State
      NULL,                       // Обработчик входа в state
      NULL,                       // Обработчик выхода из state
      any_state_handlers          // Список обработчиков событий
    },

    { IF_STATE_MAIN,
      MainStateEnter,
      NULL,
      main_state_handlers
    }

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Entry (ldrElf *ela, WCHAR *params)
{
	UINT32 i, reserve, status = RESULT_OK;
    WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];

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
    reserve = ldrInitEventHandlersTbl(main_state_handlers, reserve);

    //Register application
    status = APP_Register(&elf->evbase, 1, state_handling_table, IF_STATE_MAX, (void*)ELF_Start);
    if(status == RESULT_OK)
    {
        PFprintf("%s: Application has been registered successfully!\n", app_name);

        //Getting elf uri.
        u_strcpy(ELF_folder, L"file:/");
        DL_FsGetURIFromID(&elf->id, (ELF_folder + 6));

        //Getting elf folder
        WCHAR *ptr = ELF_folder + u_strlen(ELF_folder);
        while(*ptr != L'/')
        {
            *ptr = 0;
            ptr --;
        }
        udbg("ELF_folder = ", ELF_folder);

        u_strcpy(uri, ELF_folder);
        u_strcat(uri, L"Informer.lng");
        ReadLang(uri);

		// читаем конфиг
		// из папки эльфа
        u_strcpy(uri, ELF_folder);
        u_strcat(uri, L"Informer.ini");
        ReadIniSettings(uri);

        // обновляем секции из папки скина
        current_skin_index = UIS_GetCurrentSkinIndex();

        u_strcpy(uri, L"file:/");
        u_strcat(uri, UIS_GetSkinStorageFolder(1));
        u_strcat(uri, UIS_GetSkinName(current_skin_index));
        u_strcat(uri, L"/Informer.ini");

        udbg("SKIN = ", uri);
		ReadIniSettings(uri);

        //Информация о вышках.
        ReadCells(ELF_folder);

        //Run application immediatly
        ldrSendEvent(elf->evbase);
    }
    else
    {
        PFprintf("%s: Can't register application!\n", app_name);
    }

    return status;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info)
{
    APP_INFORMER_T *app = NULL;
    UINT32 status = RESULT_OK, i;

    //Check if app is initialized
	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
		return RESULT_OK;
	}

	//Initialize application
    app = (APP_INFORMER_T*)APP_InitAppData((void *)APP_HandleEventPrepost, sizeof(APP_INFORMER_T), reg_id, 0, 1, 1, 2, 0, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //Starting application
    status = APP_Start(ev_st, &app->apt, IF_STATE_MAIN, state_handling_table, ELF_Exit, app_name, 0);
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ELF_Exit(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 i, status;

    if(exit == FALSE)
        return RESULT_OK;

    dbgf("EXIT ELF", NULL);

    // Останавливаем таймеры
    for (i=0; i<COUNT_TIMERS; i++)
        StopTimer(i, app);

    // cellid
    if (info_cells.Enabled)
    {
        free(info_cells.cells);
    }

    // Погода
    if (info_weather.Enabled)
    {
        if(info_weather.Show_Picture)
        {
            DRM_ClearResource(info_weather.PicRes);
            free(info_weather.Picture);
        }

        if(g_socket != HSOCK_INVALID)
        {
                // разрываем соединение
                status = socket_close(g_socket);
                dbgf("socket_close DONE, status = %d", status);
                status = socket_delete(g_socket);
                dbgf("socket_delete DONE, status = %d", status);
        }
        DL_FsCloseFile(fh);
    }

    // Баланс
    if (info_balance.Enabled)
    {
        DL_SigCallSSAbortUSSDReq(ss_id);
        free(info_balance.Words);
    }

    // будильник
    //AlarmUnregister(ev_st, app);

    // календарь
    if (info_datebook.Enabled)
        DbkUnregister(ev_st, app);

    ///CLOSE LANG
    CloseLang();

    ///DELETE DIALOG AND EXIT APP
    APP_UtilUISDialogDelete(&app->dialog);
    APP_ExitStateAndApp(ev_st, app, 0);

    ///UNLOAD ELF
    ldrUnloadElf(elf);
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    exit = TRUE;
    APP_ConsumeEv(ev_st, app);
    app->exit_status = TRUE;
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    GRAPHIC_POINT_T display_size;
    ADD_EVENT_DATA_T add_data;

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

    dbgf("port=0x%x", app->port);
    su_port = app->port;

    UIS_CanvasGetDisplaySize(&display_size);

    bufd.w = display_size.x + 1;
    bufd.h = display_size.y + 1;
    bufd.buf = NULL;

    dialog = UIS_CreateColorCanvasWithWallpaper(&app->port, &bufd, 0, 0);// create canvas
    if(dialog == 0) return RESULT_FAIL;
    app->dialog = dialog;

    if (info_weather.Enabled)
    {
        // Запрос API к SynSock
        add_data.data_tag = 0;
        add_data.data.params[0] = ldrFindEventHandlerTbl(main_state_handlers, HandleAPIAnsw);
        SockReqAPI(add_data);
    }


	// загрузка Погоды
	if(info_weather.Enabled)
        LoadFile();

    if (info_balance.Enabled)
        USSDHandleSendRequest(ev_st, app);

    // регистрация для будильников
    //AlarmRegister(ev_st,app);

    if (info_datebook.Enabled)
        DbkRegister(ev_st,app);

    // обновление данных
    Repaint(app);
    return RESULT_OK;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 IdleStart(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    dbgf("IdleStart", NULL);

    UpdateData();
    Paint();

    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 RepaintStart(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    dbgf("RepaintStart. Display state = %d", DAL_GetDisplayState());
    
    Repaint(app);
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 ReadIniSettings(WCHAR *IniFile)
{
    char    chr[2];
    char    value[16];
    char    *off_set=NULL;
    UINT32  i, n=0, l;

    //Open ini
    if(INI_Open(IniFile) != INI_RESULT_OK)
        return RESULT_FAIL;

    INI_SetSeparator('=');

    //"DATE"
    INI_ReadParameter("DATE", "Enabled", "yes", value, 4);
    info_date.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("DATE","Show_Date", "yes", value, 4);
    info_date.Show_Date = !strncmp(value, "yes", 3);

    INI_ReadParameter("DATE","Show_WeekDay", "yes", value, 4);
    info_date.Show_WeekDay = !strncmp(value, "yes", 3);

    ReadIniStyle("DATE", &info_date.info.style);


    //"BATTERY"
    INI_ReadParameter("BATTERY", "Enabled", "yes", value, 4);
    info_battery.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("BATTERY", "Show_Percent", "yes", value, 4);
    info_battery.Show_Percent = !strncmp(value, "yes", 3);

    INI_ReadParameter("BATTERY", "Show_Voltage", "yes", value, 4);
    info_battery.Show_Voltage = !strncmp(value, "yes", 3);

    INI_ReadParameter("BATTERY", "Max_Value", "0xB0", value, 4);
    info_battery.Max_Value = str2ul(value);

    INI_ReadParameter("BATTERY", "Min_Value", "0x76", value, 4);
    info_battery.Min_Value = str2ul(value);

    ReadIniStyle("BATTERY", &info_battery.info.style);


    //"SIGNAL"
    INI_ReadParameter("SIGNAL", "Enabled", "yes", value, 4);
    info_signal.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("SIGNAL", "Show_Percent", "yes", value, 4);
    info_signal.Show_Percent = !strncmp(value, "yes", 3);

    INI_ReadParameter("SIGNAL", "Show_Dbm", "yes", value, 4);
    info_signal.Show_Dbm = !strncmp(value, "yes", 3);

    ReadIniStyle("SIGNAL", &info_signal.info.style);


    //"DISKS"
    INI_ReadParameter("DISKS", "Enabled", "yes", value, 4);
    info_disks.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("DISKS", "Show_AllBytes", "yes", value, 4);
    info_disks.Show_AllBytes = !strncmp(value, "yes", 3);

    INI_ReadParameter("DISKS", "Show_FreeBytes", "yes", value, 4);
    info_disks.Show_FreeBytes = !strncmp(value, "yes", 3);

    ReadIniStyle("DISKS", &info_disks.info.style);


    //"CELLS"
    INI_ReadParameter("CELLS", "Enabled", "yes", value, 4);
    info_cells.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("CELLS", "Show_CellId", "yes", value, 4);
    info_cells.Show_CellId = !strncmp(value, "yes", 3);

    INI_ReadParameter("CELLS", "Show_Location", "yes", value, 4);
    info_cells.Show_Location = !strncmp(value, "yes", 3);

    ReadIniStyle("CELLS", &info_cells.info.style);


    //"WEATHER"
    INI_ReadParameter("WEATHER", "Enabled", "yes", value, 4);
    info_weather.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "AutoUpdate", "0", value, 4);
    info_weather.AutoUpdate = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "Key1_Update", "0", value, 4);
    info_weather.keyupdate1 = str2ul(value);

    INI_ReadParameter("WEATHER", "Key2_Update", "0", value, 4);
    info_weather.keyupdate2 = str2ul(value);

    INI_ReadParameter("WEATHER", "Key3_Update", "0", value, 4);
    info_weather.keyupdate3 = str2ul(value);

    INI_ReadParameter("WEATHER", "ID", "26850", info_weather.ID, 8);

    INI_ReadParameter("WEATHER", "Show_Date", "yes", value, 4);
    info_weather.Show_Date = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "Show_Temperature", "yes", value, 4);
    info_weather.Show_Temperature = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "Show_Wind", "yes", value, 4);
    info_weather.Show_Wind = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "Show_Phenomena", "yes", value, 4);
    info_weather.Show_Phenomena = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "Show_Picture", "yes", value, 4);
    info_weather.Show_Picture = !strncmp(value, "yes", 3);

    INI_ReadParameter("WEATHER", "Picture_X", "0", value, 4);
    info_weather.Picture_X = str2ul(value);

    INI_ReadParameter("WEATHER", "Picture_Y", "0", value, 4);
    info_weather.Picture_Y = str2ul(value);

    ReadIniStyle("WEATHER", &info_weather.info.style);


    //"BALANCE"
    INI_ReadParameter("BALANCE", "Enabled", "yes", value, 4);
    info_balance.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("BALANCE", "USSD", "*100#", value, 16);
    u_atou(value, info_balance.USSD);

    INI_ReadParameter("BALANCE", "Words", "0", value, 16);
    if(value[0] != '0')
    {
        l = strlen(value);
        for (i=0; i < l; i++) if (value[i] == ',') {n++; value[i]=0;}
        info_balance.countWords = n+1;
        info_balance.Words = malloc((n+1)*sizeof(UINT32));
        off_set = value;
        for(i=0; i < n+1; i++)
        {
            info_balance.Words[i] = str2ul(off_set);
            dbgf("Word #%d", info_balance.Words[i]);
            off_set += strlen(off_set)+1;
        }
    }
    else
    {
        info_balance.Words = NULL;
    }


    INI_ReadParameter("BALANCE", "Time_Update", "60000", value, 16);
    info_balance.Time_Update = str2ul(value);

    INI_ReadParameter("BALANCE", "Key1_Update", "0", value, 4);
    info_balance.keyupdate1 = str2ul(value);

    INI_ReadParameter("BALANCE", "Key2_Update", "0", value, 4);
    info_balance.keyupdate2 = str2ul(value);

    INI_ReadParameter("BALANCE", "Key3_Update", "0", value, 4);
    info_balance.keyupdate3 = str2ul(value);

    ReadIniStyle("BALANCE", &info_balance.info.style);


    //"ALARM"
    INI_ReadParameter("ALARM", "Enabled", "yes", value, 4);
    info_alarm.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("ALARM", "Count", "4", value, 8);
    info_alarm.Count = str2ul(value);

    ReadIniStyle("ALARM", &info_alarm.info.style);


    //"DATEBOOK"
    INI_ReadParameter("DATEBOOK", "Enabled", "yes", value, 4);
    info_datebook.Enabled = !strncmp(value, "yes", 3);

    INI_ReadParameter("DATEBOOK", "Show_Date", "yes", value, 4);
    info_datebook.Show_Date = !strncmp(value, "yes", 3);

    INI_ReadParameter("DATEBOOK", "Show_Title", "yes", value, 4);
    info_datebook.Show_Title = !strncmp(value, "yes", 3);

    ReadIniStyle("DATEBOOK", &info_datebook.info.style);


    //"CALLTIME"
    INI_ReadParameter("CALLTIME", "Enabled", "yes", value, 4);
    info_calltime.Enabled = !strncmp(value, "yes", 3);

    ReadIniStyle("CALLTIME", &info_calltime.info.style);

    //Close ini
    INI_Close();
    return RESULT_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ReadIniStyle(const char *section_name, INFO_STYLE_T *style)
{
    char value[16];

    INI_ReadParameter(section_name, "X", "0", value, 4);
    style->x = str2ul(value);

    INI_ReadParameter(section_name, "Y", "0", value, 4);
    style->y = str2ul(value);

    INI_ReadParameter(section_name, "Anchor", "20", value, 4);
    {
        if (value[0] == 'r' || value[0] == 'R') style->anchor = ANCHOR_TOP | ANCHOR_RIGHT;
        else if (value[0] == 'l' || value[0] == 'L') style->anchor = ANCHOR_TOP | ANCHOR_LEFT;
        else if (value[0] == 'c' || value[0] == 'C') style->anchor = ANCHOR_TOP | ANCHOR_HCENTER;
        else style->anchor = str2ul(value);
    }

    INI_ReadParameter(section_name, "Between", "18", value, 4);
    style->between = str2ul(value);

    INI_ReadParameter(section_name, "Font", "1", value, 4);
    style->font = str2ul(value);

    INI_ReadParameter(section_name, "Style", "0", value, 4);
    style->font_style = str2ul(value);

    INI_ReadParameter(section_name, "Size", "6", value, 4);
    style->font_size = str2ul(value);

    INI_ReadParameter(section_name, "Color", "0xFFFFFF00", value, 16);
    style->color = UINT32toCOLOR_T(str2ul(value));
}


/* Обработчик события, EV_KEY_PRESS в данном случае */
UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT8 keypress = GET_KEY(ev_st);

    // обновление информации
    if (keypress == info_weather.keyupdate1 || keypress == info_balance.keyupdate1)
    {
        if(key1 == -1)
            key1 = keypress;
    }
    else if (keypress == info_weather.keyupdate2 || keypress == info_balance.keyupdate2)
    {
        if(key2 == -1 && key1 != -1)
            key2 = keypress;
    }
    else if (keypress == info_weather.keyupdate3 || keypress == info_balance.keyupdate3)
    {
        if(key2 != -1 && key1 != -1)
        {
            // обновление погоды
            if (info_weather.Enabled)
            {
                 if (info_weather.keyupdate1 != 0 || info_weather.keyupdate2 != 0 || info_weather.keyupdate3 != 0)
                 {
                     if (key1 == info_weather.keyupdate1 && key2 == info_weather.keyupdate2 && keypress == info_weather.keyupdate3)
                     {
                        key1 = -1; key2 = -1;
                        dbgf("Update weather", NULL);
                        Update();
                    }
                 }
            }

             // обновление баланса
            if (info_balance.Enabled)
            {
                 if (info_balance.keyupdate1 != 0 || info_balance.keyupdate2 != 0 || info_balance.keyupdate3 != 0)
                 {
                     if (key1 == info_balance.keyupdate1 && key2 == info_balance.keyupdate2 && keypress == info_balance.keyupdate3)
                     {
                        key1 = -1; key2 = -1;
                        dbgf("Update balance", NULL);
                        USSDHandleSendRequest(ev_st, app);
                    }
                 }
            }
        }
    }
    else
    {
        key1 = -1;
        key2 = -1;
    }

    if(keypress == KEY_RED)
         Repaint(app);

    return RESULT_OK;
}


UINT32 RegNetwork( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	SIGNAL_STRENGTH_T signal_strength;
	DL_SigRegQuerySignalStrength(&signal_strength);

    if (signal_strength.percent > 0)
    {
        if (!net)
        {
            net = TRUE;

            // автообновление баланса
            if (info_balance.Time_Update && info_balance.Enabled)
            {
                USSDHandleSendRequest(ev_st,app);
                StartTimer(info_balance.Time_Update * 1000, 3, 1, app);
            }

            // загрузка Погоды
            if (info_weather.Enabled)
                LoadFile();
        }
    }
    return RESULT_OK;
}


UINT32 TimerHandle (EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    //Check timer
    if(isOurTimer(AFW_GetEvSeqn(ev_st)) == RESULT_FAIL)
        return RESULT_OK;

    //Get timer ID
    UINT32 ID = GET_TIMER_ID(ev_st);
    if(ID < COUNT_TIMERS) dbgf("Timer %d", ID);

	if (ID == 1)
	{
        Repaint(app);
	}
	else if (ID == 2)
	{
        Paint();
        StartTimer(TIME_UPDATE_DATE, 1, 0, app);
    }
    else if (ID == 3) // обновление USSD
    {
        USSDHandleSendRequest(ev_st,app);
    }
    else if (ID == 4) // повтор чтения из календаря
    {
        DBKHandleView(ev_st,app);
    }
    else if (ID == 5) // повтор чтения из календаря
    {
        DBKHandleSendRequest(ev_st,app);
    }
    else if (ID == 6) // повтор регистрации календаря
    {
        DbkRegister(ev_st,app);
    }

	return RESULT_OK;
}

UINT32 AlarmHandle( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    DL_ALARM_DATA_T *data = (DL_ALARM_DATA_T*)(AFW_GetEv(ev_st)->attachment);

    dbgf("Alarm %d", data->unk);

    if (data->unk == 1 /*&& (data->time.hour == 0 || data->time.hour % 6 == 0  )*/)
    {

        Weather(info_weather.info.str);
        Repaint(app);
    }

    return RESULT_OK;
}


void Repaint(APPLICATION_T *app)
{
    if (WorkingTable())
    {
        //Обработка закрытого флипа для v3x
        if(FLIP_STATE != 0 && flip_state == 0)
            return;

        UpdateData();
        UIS_ForceRefresh();

        StartTimer(TIME_REFRESH, 2, 0, app);
    }
}

UINT32 UpdateData(void)
{
    UINT16 skid;
    WCHAR  uri[256];

    // проверка смены скина
    skid = UIS_GetCurrentSkinIndex();
    if (current_skin_index != skid)
    {
		// из папки эльфа
		u_strcpy(uri, ELF_folder);
        u_strcat(uri, L"Informer.ini");
        ReadIniSettings(uri);

	    // из папки скина
        u_strcpy(uri, UIS_GetSkinStorageFolder(1));
        u_strcat(uri, UIS_GetSkinName(skid));
        u_strcat(uri, L"/Informer.ini");
        udbg("SKIN = ",uri);
        ReadIniSettings(uri);

        current_skin_index = skid;
    }

    // обновление данных
    if (info_date.Enabled) Date(info_date.info.str);
    if (info_battery.Enabled) Battery(info_battery.info.str);
    if (info_signal.Enabled) Signal(info_signal.info.str);
    if (info_cells.Enabled) Cells(info_cells.info.str);
    if (info_disks.Enabled) Disks(info_disks.info.str);
    if (info_alarm.Enabled) Alarm(info_alarm.info.str);
	if (info_calltime.Enabled) CallTime(info_calltime.info.str);


    dbgf("Update Data!", NULL);

    return RESULT_OK;
}


void Paint(void)
{

    if (!WorkingTable())
        return; // рисуем только на рабочем столе

    //Обработка закрытого флипа для v3x
    if(FLIP_STATE != 0 && flip_state == 0)
        return;

    dbgf("Paint", NULL);

    if (info_date.Enabled) DrawString(&info_date.info);
    if (info_battery.Enabled) DrawString(&info_battery.info);
    if (info_signal.Enabled) DrawString(&info_signal.info);
    if (info_cells.Enabled) DrawString(&info_cells.info);
    if (info_disks.Enabled) DrawString(&info_disks.info);

    if (info_weather.Enabled)
    {
        DrawString(&info_weather.info);
        if (info_weather.Show_Picture)
        {
            DrawImage(info_weather.Picture, info_weather.PicRes, info_weather.Picture_X, info_weather.Picture_Y);
        }
    }
    if (info_balance.Enabled) DrawString(&info_balance.info);
    if (info_alarm.Enabled) DrawString(&info_alarm.info);
    if (info_datebook.Enabled) DrawString(&info_datebook.info);
	if (info_calltime.Enabled) DrawString(&info_calltime.info);
}


UINT32 DrawString(INFO_STRING_T *info)
{
    GRAPHIC_POINT_T anchor_point;
    COLOR_T         color = {0x00, 0x00, 0x00, 0xFF};
    UINT16          i, len, offset = 0;

    anchor_point.x = info->style.x;
    anchor_point.y = info->style.y;

    UIS_CanvasSetFont(info->style.font, dialog);
    UIS_CanvasSetFontSize(info->style.font_size, dialog);
    UIS_CanvasSetFontStyle(info->style.font_style, dialog);

    UIS_CanvasSetForegroundColor(info->style.color);
    UIS_CanvasSetBackgroundColor(color);

    len = u_strlen(info->str);
    if(len == 0) return RESULT_OK;

    for (i=0; i < len; i++)
    {
        if (info->str[i] == '\n')
        {
            UIS_CanvasDrawColorText (info->str, offset, i - offset,
							 anchor_point,
							 info->style.anchor,
							 dialog	);
            offset = i+1;
            anchor_point.y += info->style.between;
        }
    }

    UIS_CanvasDrawColorText(info->str, offset, i - offset, anchor_point, info->style.anchor, dialog);
    UIS_CanvasSetFontStyle(FONT_STYLE_PLAIN, dialog);

    return RESULT_OK;
}

UINT32 DrawImage( BYTE *picture, RESOURCE_ID picture_res, INT16 x, INT16 y)
{
    GRAPHIC_REGION_T region;
    UINT16 height, width;

    utility_get_image_size(picture_res, &width, &height);

    region.ulc.x=x;
    region.ulc.y=y;
    region.lrc.x=x+width+1;
    region.lrc.y=y+height+1;

    // рисует картинку на рабочем столе. Gif или bitmap
    utility_draw_image( picture_res, region, NULL, NULL, 0);
    return RESULT_OK;
}


RESOURCE_ID LoadPicture(WCHAR *uri, BYTE *pic)
{
    FILE_HANDLE_T f;
    UINT32 r, fSize;
    RESOURCE_ID res;

    f=DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID) return 0;

    fSize = DL_FsGetFileSize(f);
    pic = malloc(fSize);
    if (pic == NULL) return 0;

    DL_FsReadFile(pic, 1, fSize, f, &r);
    DL_FsCloseFile(f);

      // Создать ресурс
    DRM_CreateResource( &res, RES_TYPE_ANIMATION, (void*)(pic), fSize);

    return res;
}

  /*************/
 /*** Date  ***/
/*************/


CLK_DATE_T Date(WCHAR *buf_date)
{
    CLK_DATE_T date;
    UINT32 a, y, m;
    UINT8  res;


    dbgf("update Date", NULL);

    buf_date[0] = 0;


    DL_ClkGetDate( &date);

    a=(14-date.month)/12;
    y=date.year-a;
    m=date.month+12*a-2;

    if (info_date.Show_Date) 
    {
        UIS_FormatDate(date, buf_date);
        u_strcat(buf_date, L"\n");
    }

    if (info_date.Show_WeekDay) 
    {
        switch((date.day+y+y/4-y/100+y/400+(31*m)/12)%7) 
        {

        case 0: // Воскресенье
            res = LNG_SUNDAY;
            break;
        case 1: // Понедельник
            res = LNG_MONDAY;
            break;
        case 2: // Вторник
            res = LNG_TUESDAY;
            break;
        case 3: // Среда
            res = LNG_WEDNESDAY;
            break;
        case 4: // Четверг
            res = LNG_THURSDAY;
            break;
        case 5: // Пятница
            res = LNG_FRIDAY;
            break;
        case 6: // Суббота
            res = LNG_SATURDAY;
            break;
        }

        u_strcat(buf_date, Lang.str[res]);
    }

    return date;
}

  /***************/
 /*** Battery ***/
/***************/


//extern const UINT32 *SBCM_ATOD_vltg;
//#define BAT_MAX		402
//#define BAT_MIN		335

UINT8 Battery(WCHAR *buf_con)
{
    UINT8 percent, voltage;
    
    buf_con[0] = 0;
    percent = DL_PwrGetActiveBatteryPercent();
    voltage = RTIME_MAINT_get_battery_voltage();

    if(info_battery.Show_Percent)
    {
        // проценты
        u_ltou(percent, buf_con + u_strlen(buf_con));
        u_strcat(buf_con, L"% ");
    }
    if (info_battery.Show_Voltage)
    {
        // вольты
        u_ltou(voltage, buf_con + u_strlen(buf_con));
        u_strcat(buf_con, L"V");
    }
   
    return voltage;
}

/*
UINT8 Battery(WCHAR *buf_con)
{
    UINT16  dy,dya,dyb,dyc,bf;
    UINT8   Bat;


    dbgf("update Battery", NULL);

    buf_con[0] = 0;


	//Bat = *((char*)(*SBCM_ATOD_vltg));
	Bat = RTIME_MAINT_get_battery_voltage();

    dy=(Bat) + 231;

    dya=dy/100;             // целые
    dyb=(dy-dya*100)/10;    // десятые
    dyc=dy-100*dya-10*dyb;  // сотые

	if (info_battery.Max_Value < info_battery.Min_Value)
	{
		bf = 0;
    }
    else if(Bat >= info_battery.Max_Value)
    {
        bf=100;
    }
    else if(Bat <= info_battery.Min_Value)
    {
        bf=1;
	}
	else
	{
		bf=((Bat-info_battery.Min_Value)*100)/(info_battery.Max_Value-info_battery.Min_Value);
	}


    if (info_battery.Show_Percent)
    {
        // проценты
        u_ltou(bf, buf_con+u_strlen(buf_con));
        u_strcat(buf_con, L"% ");
    }
    if (info_battery.Show_Voltage)
    {
        // вольты
        u_ltou(dya, buf_con+u_strlen(buf_con));
        u_strcat(buf_con, L".");
        u_ltou(dyb,buf_con+u_strlen(buf_con));
        u_ltou(dyc, buf_con+u_strlen(buf_con));
        u_strcat(buf_con, L"V");
    }

    return Bat;
}
*/



  /***************/
 /*** Signal  ***/
/***************/

SIGNAL_STRENGTH_T Signal(WCHAR *buf_con)
{
    dbgf("update Signal");

    buf_con[0] = 0;

    SIGNAL_STRENGTH_T sig;
    DL_SigRegQuerySignalStrength(&sig);

    if (info_signal.Show_Percent) 
    {
        u_ltou(sig.percent, buf_con+u_strlen(buf_con));
        u_strcat(buf_con, L"% ");
    }
    if (info_signal.Show_Dbm) 
    {
        u_ltou(sig.dbm, buf_con+u_strlen(buf_con));
        u_strcat(buf_con, L"dbm");
    }

    return sig;
}

  /***************/
 /*** Disks   ***/
/***************/

UINT32 Disks(WCHAR *buf_con)
{
    UINT8       i;
    WCHAR       volumes[12];
    WCHAR       volume[3];
    VOLUME_DESCR_T descr;

    dbgf("update Disks", NULL);

    buf_con[0] = 0;


    // получает список дисков
    DL_FsVolumeEnum( volumes ); // result = {0,'/',0,'a',0xff,0xfe,0,'/', ...}


    for (i=0; i < 4; i++) {
       volume[0] = volumes[i*3];
       volume[1] = volumes[i*3+1];
       volume[2] = 0;

       u_strcat(buf_con, volume);
       u_strcat(buf_con, L"/  ");

       DL_FsGetVolumeDescr( volume, &descr );

       if (info_disks.Show_FreeBytes) {
           if (descr.free_size > (1024*1024)) {
               u_ltou(descr.free_size/(1024*1024), buf_con+u_strlen(buf_con));
               if((descr.free_size%(1024*1024))/102400 > 0){
                   u_strcat(buf_con, L",");
                   u_ltou((descr.free_size%(1024*1024))/102400, buf_con+u_strlen(buf_con));
                   u_strcat(buf_con, L" Mb");
               } else {
                   u_strcat(buf_con, L" Mb");
               }
           } else {
               u_ltou(descr.free_size/1024, buf_con+u_strlen(buf_con));
               u_strcat(buf_con, L" Kb");
           }

       }

       if (info_disks.Show_AllBytes) {
           u_strcat(buf_con, L" | ");
           if (descr.vol_size > (1024*1024)) {
               u_ltou(descr.vol_size/(1024*1024), buf_con+u_strlen(buf_con));
               if(descr.vol_size%(1024*1024) > 0){
                   u_strcat(buf_con, L",");
                   u_ltou((descr.vol_size%(1024*1024))/102400, buf_con+u_strlen(buf_con));
                   u_strcat(buf_con, L" Mb\n");
               } else {
                   u_strcat(buf_con, L" Mb\n");
               }
           } else {
               u_ltou(descr.vol_size/1024, buf_con+u_strlen(buf_con));
               u_strcat(buf_con, L" Kb\n");
           }
       } else {
           u_strcat(buf_con, L"\n");
       }

       if (volumes[i*3+2] == NULL)  break;
    }

    return RESULT_OK;
}


  /*************/
 /*** Cells ***/
/*************/

// чтение Cells
UINT32  ReadCells(WCHAR *ELF_folder)
{
    WCHAR           cells_name[256];
    UINT32          readen;
	FILE_HANDLE_T   fh;
	UINT32          fSize;
    char            *bufR;

    int i = 0, j = 0, n = 0;
	char bufVal[16];
	BOOL bValue = true;

    dbgf("ReadCells: START", NULL);
    if (!info_cells.Show_Location || !info_cells.Enabled)
         return RESULT_FAIL;

    u_strcpy(cells_name, ELF_folder);
    u_strcat(cells_name, L"cells.txt");


	fh = DL_FsOpenFile(cells_name, FILE_READ_MODE, 0);
    if (fh == FILE_HANDLE_INVALID) return RESULT_FAIL;

	fSize = DL_FsGetFileSize(fh);
	bufR = (char*)malloc(fSize);

    if(bufR == NULL)
    {
        dbgf("ReadCells: bufR memory alloc fail", NULL);
        return RESULT_FAIL;
    }

    DL_FsReadFile(bufR, fSize, 1, fh, &readen);
    DL_FsCloseFile(fh);

    // подсчитаем кол-во строк
    for (i=0; i < fSize; i++)
		if (bufR[i] == 0x0A)
            info_cells.cells_count++;

    info_cells.cells = (CELLS_T *)malloc(sizeof(CELLS_T) * info_cells.cells_count);
    if(info_cells.cells == NULL)
    {
        dbgf("ReadCells: cells memory alloc fail!", NULL);
        free(bufR);
        return RESULT_FAIL;
    }

    // парсим
    for (i=0; i < fSize; i++)
    {
        if (bufR[i] == '\n')
        { // следующая строка
           info_cells.cells[n].cell_id = strtoul( bufVal,  0,  10 );
           j=0;
           bValue = true;
           n++;
        }
        else if (bufR[i] == ';')
        { // разделитель
              bValue = false;
              j=0;
        }
        else
        {
            if(bufR[i] == '\r') continue; // пропускать
            if (j+1 > 32) continue;


            if (bValue)
            {
                if (bufR[i] == 'x' || bufR[i] == 'X')
                {
                    bufVal[j] = '0';
                    info_cells.cells[n].mask ++;
                }
                else
                {
                    bufVal[j] = bufR[i];
                }
                bufVal[j+1] = 0;
            }
            else
            {
                info_cells.cells[n].name[j] = bufR[i];
                info_cells.cells[n].name[j+1]= 0;
            }

            j++;
        }
    }

    dbgf("ReadCells: END", NULL);

    free(bufR);
    return RESULT_OK;
}

//формирование информации о Cell
UINT16 Cells(WCHAR *buf_con)
{
    UINT32 i;
    UINT16 cell_id;

    dbgf("Cells: START", NULL);

    buf_con[0] = 0;
    DL_SigRegGetCellID(&cell_id);

    if(info_cells.Show_CellId)
    {
        u_ltou(cell_id, buf_con+u_strlen(buf_con));
        u_strcat(buf_con, L"\n");
    }

    if (info_cells.Show_Location)
    {
        if (info_cells.cells != NULL)
        {
            for (i=0; i < info_cells.cells_count; i++)
            {
                if (info_cells.cells[i].cell_id == cell_id)
                {
                    DL_Char_convCP1251toUCS2String((UINT8*)info_cells.cells[i].name, strlen(info_cells.cells[i].name)+1, buf_con+u_strlen(buf_con), (strlen(info_cells.cells[i].name)+1)*sizeof(WCHAR));
                    //u_atou(cells[i].name, buf_con+u_strlen(buf_con) );
                    break;
                }
            }

            if (i == info_cells.cells_count)
            {
                for (i=0; i < info_cells.cells_count; i++)
                {
                    if (info_cells.cells[i].mask && info_cells.cells[i].cell_id == (cell_id/degree(10,info_cells.cells[i].mask))*degree(10,info_cells.cells[i].mask))
                    {
                         DL_Char_convCP1251toUCS2String((UINT8*)info_cells.cells[i].name, strlen(info_cells.cells[i].name)+1, buf_con+u_strlen(buf_con), (strlen(info_cells.cells[i].name)+1)*sizeof(WCHAR));
                        //u_atou(cells[i].name, buf_con+u_strlen(buf_con) );
                         break;
                    }
                }

                if (i == info_cells.cells_count && !info_cells.Show_CellId)
                {
                    buf_con[0] = 0;
                    u_ltou(cell_id, buf_con+u_strlen(buf_con));
                    u_strcat(buf_con, L"\n");
                }
            }
        }
    }

    dbgf("Cells: END", NULL);
    return cell_id;
}

  /***************/
 /*** WEATHER ***/
/***************/

UINT32 Weather(WCHAR *buf)
{
    UINT32 status;
    INT8 num;
    INT16 tmp;
    WCHAR   uri[256];
    UINT8   cloudiness;
    UINT8   precipitation;
    INT8    temperature;


    dbgf("update Weather", NULL);

    buf[0] = 0;


    num = Weather_getNum();

    dbgf("weater num = %d", num);
    if (num == 0) return RESULT_FAIL;
    if (num < 0) { // прогноз устарел
        num = -num;
        u_strcat(buf, Lang.str[LNG_OLD]);
        u_strcat(buf, L"\n");

        if (info_weather.AutoUpdate) Update(); // автообновление
    } else {
        // установим alarm на следующий прогноз
        CLK_DATE_T date;
        CLK_TIME_T time;

       // if (num < COUNT_MMWEATHER) {
            date.year = info_weather.xml[num-1].forecast.year;
            date.month = info_weather.xml[num-1].forecast.month;
            date.day = info_weather.xml[num-1].forecast.day;

            time.hour = info_weather.xml[num-1].forecast.hour;
            time.minute = 1;
            time.second = 0;

            AlarmStart(1, date, time);
       // }
    }

    num--;

    if (info_weather.xml[num].forecast.month == 0) return RESULT_FAIL;

            if (info_weather.Show_Picture) { // погда картинкой

                u_strcpy(uri, ELF_folder);
                u_strcat(uri, L"graphics/");

                cloudiness = info_weather.xml[num].phenomena.cloudiness;
                precipitation = info_weather.xml[num].phenomena.precipitation;

                if (cloudiness == 0) { // солнце
                    u_strcat(uri, L"clear");
                } else if (cloudiness == 1 && precipitation == 10) { // немного облачно
                    u_strcat(uri, L"little_cloudy");
                } else if (cloudiness == 2 && precipitation == 10) { // облочно
                    u_strcat(uri, L"cloudy");
                } else if (cloudiness == 3 && precipitation == 10) { // пасмурно
                    u_strcat(uri, L"dull");
                } else if ((cloudiness == 1 || cloudiness == 2) && (precipitation == 4 || precipitation == 5)) {
                    u_strcat(uri, L"cloudy_rain");
                } else if ((cloudiness == 1 || cloudiness == 2) && (precipitation == 6 || precipitation == 7)) {
                    u_strcat(uri, L"cloudy_snow");
                } else if (precipitation == 8) {
                    u_strcat(uri, L"thunderstorm");
                } else if (cloudiness == 3 && (precipitation == 4 || precipitation == 5)) { // пасмурно
                    u_strcat(uri, L"dull_rain");
                } else if (cloudiness == 3 && (precipitation == 6 || precipitation == 7)) { // пасмурно
                    u_strcat(uri, L"dull_snow");
                } else {
                    u_strcat(uri, L"error");
                }

                u_strcat(uri, L".gif");
                udbg("Pic uri=%s", uri);
                info_weather.PicRes = LoadPicture(uri, info_weather.Picture);
                dbgf("Pic res=%d", info_weather.PicRes);
            }

        if (info_weather.Show_Date )  // показать дату
        {
            dbgf("Draw Date", NULL);
            // число, месяц
            u_ltou(info_weather.xml[num].forecast.day, buf+u_strlen(buf));
            u_strcat(buf, L"/");
            u_ltou(info_weather.xml[num].forecast.month, buf+u_strlen(buf));
            u_strcat(buf, L" ");

            // утро, день, вечер, ночь
            switch (info_weather.xml[num].forecast.tod) {
            case 0:
                u_strcat(buf, Lang.str[LNG_NIGHT]);
                break;
            case 1:
                u_strcat(buf, Lang.str[LNG_MORNING]);
                break;
            case 2:
                u_strcat(buf, Lang.str[LNG_DAY]);
                break;
            case 3:
                u_strcat(buf, Lang.str[LNG_EVENING]);
                break;
            }

            u_strcat(buf, L"\n");
        }

            dbgf("Draw Weather", NULL);

            if (info_weather.Show_Temperature)
            {
                dbg("Show_Temperature");
                // вычислим среднюю температуру
                temperature = (info_weather.xml[num].temperature.min_val + info_weather.xml[num].temperature.max_val)/2;
				if (temperature > 0) u_strcat(buf, L"+");
                u_ltou(temperature, buf+u_strlen(buf));
                u_strcat(buf, L" xC, ");
                buf[u_strlen(buf) - 4] = 0x00B0;
            }

            if (info_weather.Show_Wind)
            {
                dbg("Show_Wind");

                // вычислим напавление ветра
                tmp = info_weather.xml[num].wind.direction;
                // Север или Юг
                if (tmp==0 || tmp==1 || tmp==7 || tmp==8) {
                    u_strcat(buf, Lang.str[LNG_N]);
                } else if (tmp==3 || tmp==4 || tmp==5) {
                    u_strcat(buf, Lang.str[LNG_S]);
                }

                if (tmp==1 || tmp==3 || tmp==5 || tmp==7) u_strcat(buf, L"-");

                // Запад или Восток
                if (tmp==5 || tmp==6 || tmp==7) {
                    u_strcat(buf, Lang.str[LNG_W]);
                } else if (tmp==1 || tmp==2 || tmp==3) {
                    u_strcat(buf, Lang.str[LNG_O]);
                }

                u_strcat(buf, L", ");
                // скорость ветра
                u_ltou((info_weather.xml[num].wind.min_val + info_weather.xml[num].wind.max_val)/2, buf+u_strlen(buf));
                u_strcat(buf, Lang.str[LNG_MS]);
                u_strcat(buf, L"\n");
            }

            if (info_weather.Show_Phenomena)
            {
                dbg("Show_Phenomena");
                // вычислим облочность
                switch (info_weather.xml[num].phenomena.cloudiness) {
                case 0:
                    u_strcat(buf, Lang.str[LNG_CLEAR]);
                    break;
                case 1:
                    u_strcat(buf, Lang.str[LNG_LITTLECLOUDY]);
                    break;
                case 2:
                    u_strcat(buf, Lang.str[LNG_CLOUDY]);
                    break;
                case 3:
                    u_strcat(buf, Lang.str[LNG_DULL]);
                    break;
                }

                u_strcat(buf, L", ");

                // тип осадков
                if ( info_weather.xml[num].phenomena.rpower == 1 ||  info_weather.xml[num].phenomena.spower == 1) u_strcat(buf, Lang.str[LNG_PERHAPS]);


                switch ( info_weather.xml[num].phenomena.precipitation)
                {
                    dbg("info_weather.xml[num].phenomena.precipitation");
                case 4:
                    u_strcat(buf, Lang.str[LNG_RAIN]);
                    break;
                case 5:
                    u_strcat(buf, Lang.str[LNG_DOWNPOUR]);
                    break;
                case 6:
                case 7:
                    u_strcat(buf, Lang.str[LNG_SNOW]);
                    break;
                case 8:
                    u_strcat(buf, Lang.str[LNG_THUNDERSTORM]);
                    break;
                case 10:
                    u_strcat(buf, Lang.str[LNG_WITHOUTPRECIPITATION]);
                    break;
                }
            }
    return status;
}



UINT32 HandleAPIAnsw( EVENT_STACK_T * ev_st, APPLICATION_T *app )
{
	EVENT_T *	event = AFW_GetEv(ev_st);
	int			i;

	dbgf("APIAnsw: Enter", NULL);

	APP_ConsumeEv( ev_st, app );

	socket = (socket_f *)(event->data.params[0]);
	socket_connect = (socket_connect_f *)(event->data.params[1]);
	socket_write = (socket_write_f *)(event->data.params[2]);
	socket_read = (socket_read_f *)(event->data.params[3]);
	socket_bind = (socket_bind_f *)(event->data.params[4]);
	socket_close = (socket_close_f *)(event->data.params[5]);
	socket_delete = (socket_delete_f *)(event->data.params[6]);

	// Проверка, вдруг какойто функции нет. Если так, то выход из эльфа
	for ( i=0; i<7; i++ ) {
		if ( event->data.params[i] == NULL )
		{
			return HandleReqExit(ev_st, app);
		}
	}

    //
    //LoadFile();

	return RESULT_OK;
}

UINT32 HandleSockAnsw( EVENT_STACK_T *ev_st, APPLICATION_T *app )
{
    UINT32      status;
	EVENT_T *	event = AFW_GetEv(ev_st);
	UINT32		answ = event->data.params[0];
    char        request[128];
	char 		*data;
    UINT32		wr, w;
    INT32       len=0;
    //char        cont_len[16];
    UINT32      i;
    char    buffer[SIZE_BUF];

    if (!info_weather.Enabled) return RESULT_OK;


	if ( answ == SOCK_ANSW_CREATE ) {
		// сокет создан
		dbgf("SockAnsw: CREATE, status = %d", event->data.params[1]);

        // установим соединение
        if ( g_socket != HSOCK_INVALID ) {
				if ( socket_connect(g_socket, HTTP_HOST, 80) != -1 ) {
					dbgf("socket_connect OK...", NULL);
				} else {
					dbgf("socket_connect ERROR", NULL);
				}
		}

	} else if ( answ == SOCK_ANSW_READY ) {
		// сокет готов к записи
		dbgf("SockAnsw: READY", NULL);

        // запись в сокет запроса
        if ( g_socket != HSOCK_INVALID ) {

            rlen=0;
            content_len = SIZE_BUF;

            sprintf(request, "GET /xml/%s_1.xml HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n", info_weather.ID, HTTP_HOST, HTTP_USER_AGENT);
            data = (char*)request;

                len = strlen(data);		// осталось записать
				while ( len > 0 ) {
					wr = socket_write(g_socket, data, len);
					dbgf("socket_write DONE, %d", wr);
					if ( wr == -1 ) {
						// нет такого сокета
						dbgf("socket is not ready %d", g_socket);
						break;
					} else if ( wr == 0 && len > 0 ) {
						// не пишется...
						dbgf("can't write to socket", NULL);
						break;
					}
					len -= wr;
					data += wr;
				}

		}

	} else if ( answ == SOCK_ANSW_DATA ) {
		// входящие данные
		dbgf("SockAnsw: DATA", NULL);
		len = SIZE_BUF;
		if ( g_socket != HSOCK_INVALID ) {

			while ( len > 0 ) {
				len = socket_read( g_socket, buffer+rlen, SIZE_BUF );
                dbgf("socket_read DONE, len = %d\n", len);

                //if (len == 0) break;
                dbgf("rlen = %d", rlen);
                data = buffer+rlen;
                dbgf("data:\n%s\n", data);

                if (strncmp(data, "HTTP/1.1 200 OK", 15)) {
                    if (rlen == -1) break; // сервер сообщил об ошибке

                } else {
                    // откроем файл для записи
                    WCHAR   uri[256];
                    u_strcpy(uri, ELF_folder);
                    u_atou(info_weather.ID, uri+u_strlen(uri));
                    u_strcat(uri, L".xml");
                    fh = DL_FsOpenFile( uri,  FILE_WRITE_MODE,  0 );
                }

				DL_FsWriteFile( data, 1, len, fh, &w);
                dbgf("write = %d", w);

				rlen += len;
                dbgf("rlen = %d", rlen);
			}

			for (i=0; i+3 < rlen; i++) {
                if (buffer[i] == 0x0D && buffer[i+1] == 0x0A && buffer[i+2] == 0x0D && buffer[i+3] == 0x0A) {
					dbgf("%s", buffer+i+4);
					content_len = strtol( buffer+i+4, NULL, 16 );
					dbgf("content_len = %d", content_len);
					content_len += i + 4;
					break;
                }
            }
			dbgf("content_len = %d", content_len);

			/*
			setBuffer(data);
            INI_SetSeparator(':');
            ReadParamINI("Content-Length", cont_len, 16, "0");
            if (cont_len[0] != '0') content_len = strtoul(cont_len, NULL, 10);
            dbgf("content_len = %d", content_len);
			//if (content_len == 0) return RESULT_OK;
			*/
			/*
			for (i=0; i+3 < len; i++) {
                            if (data[i] == 0x0D && data[i+1] == 0x0A && data[i+2] == 0x0D && data[i+3] == 0x0A) {
                                if (len-(i+4) > 0) {
                                    DL_FsWriteFile( data+i+4, 1, len-(i+4), fh, &w);
                                    dbgf("write = %d", w);
                                    rlen += len-(i+4);
                                }
                            }
                        }
			*/

            if (rlen >= content_len || rlen == -1) {

                            DL_FsCloseFile(fh);
                            // разрываем соединение
                            status = socket_close( g_socket );
                			dbgf("socket_close DONE, status = %d", status);
                			status = socket_delete( g_socket );
                			dbgf("socket_delete DONE, status = %d", status);
                            g_socket = HSOCK_INVALID;

                LoadFile();

                Repaint(app);
            }
        }

	} else if ( answ == SOCK_ANSW_DELETE ) {
		// сокет закрыт из-за таймаута
		dbgf("SockAnsw: DELETE", NULL);
		g_socket = HSOCK_INVALID;
        DL_FsCloseFile(fh);

	} else if ( answ == SOCK_ANSW_ERROR ) {
		// ошибка
		dbgf("SockAnsw: ERROR", NULL);
	}

	return RESULT_OK;
}



UINT32 LoadFile(void)
{
    UINT32 fSize, r, i;
    WCHAR  uri[FS_MAX_URI_NAME_LENGTH + 1];
    char   buffer[SIZE_BUF];

    tag_t   tag;

    if (!info_weather.Enabled) return RESULT_FAIL;


    u_strcpy(uri, ELF_folder);
    u_atou(info_weather.ID, uri+u_strlen(uri));
    u_strcat(uri, L".xml");

    dbgf("load file", NULL);
    memset(buffer, 0, SIZE_BUF);

    if (DL_FsFFileExist(uri))
    {
        dbgf("exist file", NULL);
        fh = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
        if(fh != FILE_HANDLE_INVALID)
        {
            fSize = DL_FsGetFileSize(fh);
            if (fSize > 0)
            {
                rlen = 0;
                DL_FsReadFile( buffer, 1, fSize, fh, &r );
                dbgf("read = %d b", r);
            }
            DL_FsCloseFile(fh);
        }

        // Парсинг файла
        for (i=0; i < COUNT_MMWEATHER; i++)
        {
            InitTag("FORECAST", &tag);
            if (ReadTag(buffer, &tag, i+1) != NULL) 
            {
                info_weather.xml[i].forecast.day = strtol(GetPTagString(tag, "day"), NULL, 10);
                info_weather.xml[i].forecast.month = strtol(GetPTagString(tag, "month"), NULL, 10);
                info_weather.xml[i].forecast.year = strtol(GetPTagString(tag, "year"), NULL, 10);
                info_weather.xml[i].forecast.hour = strtol(GetPTagString(tag, "hour"), NULL, 10);
                info_weather.xml[i].forecast.tod = strtol(GetPTagString(tag, "tod"), NULL, 10);
            } 
            else
            {
                memset(&info_weather.xml[i].forecast, 0, sizeof(FORECAST));
            }
            CloseTag(&tag);

            InitTag("PHENOMENA", &tag);
            if (ReadTag(buffer, &tag, i+1) != NULL) 
            {
                info_weather.xml[i].phenomena.cloudiness = strtol(GetPTagString(tag, "cloudiness"), NULL, 10);
                info_weather.xml[i].phenomena.precipitation = strtol(GetPTagString(tag, "precipitation"), NULL, 10);
                info_weather.xml[i].phenomena.rpower = strtol(GetPTagString(tag, "rpower"), NULL, 10);
                info_weather.xml[i].phenomena.spower = strtol(GetPTagString(tag, "spower"), NULL, 10);
            } 
            else 
            {
                memset(&info_weather.xml[i].phenomena, 0, sizeof(PHENOMENA));
            }
            CloseTag(&tag);

            InitTag("TEMPERATURE", &tag);
            if (ReadTag(buffer, &tag, i+1) != NULL) 
            {
                info_weather.xml[i].temperature.min_val = strtol(GetPTagString(tag, "min"), NULL, 10);
                info_weather.xml[i].temperature.max_val = strtol(GetPTagString(tag, "max"), NULL, 10);
            }
            else
            {
                memset(&info_weather.xml[i].temperature, 0, sizeof(TEMPERATURE));
            }
            CloseTag(&tag);

            InitTag("WIND", &tag);
            if (ReadTag(buffer, &tag, i+1) != NULL)
            {
                info_weather.xml[i].wind.min_val = strtol(GetPTagString(tag, "min"), NULL, 10);
                info_weather.xml[i].wind.max_val = strtol(GetPTagString(tag, "max"), NULL, 10);
                info_weather.xml[i].wind.direction = strtol(GetPTagString(tag, "direction"), NULL, 10);
            }
            else
            {
                memset(&info_weather.xml[i].wind, 0, sizeof(WIND));
            }
            CloseTag(&tag);
        }

        Weather(info_weather.info.str);
    }

    return RESULT_OK;
}

UINT32 Update()
{
	SIGNAL_STRENGTH_T sig;

    if (!info_weather.Enabled) return RESULT_FAIL;

	DL_SigRegQuerySignalStrength(&sig); // а вдруг нет сети
	if (sig.percent == 0) return RESULT_FAIL;


    dbgf("update", NULL);

    if ( g_socket != HSOCK_INVALID )
            return RESULT_OK; // сокет уже открыт

    dbgf("create socket...", NULL);

                // создаем сокет, запоминаем идентификатор, по нему обращаемся к сокету
                // На самом деле сокет будет создан не сразу, об успешном создании сокета бкдет сообщено через ивент
        g_socket = socket(SOCK_TCP, ldrFindEventHandlerTbl( main_state_handlers, HandleSockAnsw ));

        // Сокет может быть уже не создан, если нет свободных мест или неверные входящие данные
        // максимальное кол-во созданных сокетов может быть 8
        if ( g_socket == HSOCK_INVALID ) {
            dbgf("can't create socket", NULL);
            return RESULT_FAIL;
        }

        return RESULT_OK;
}

// получить номер актуального прогноза, начиная с 1
INT8 Weather_getNum(void)
{
    INT8               num=0;
    INT32              y, m, d, h;

    CLK_DATE_T          date;
    CLK_TIME_T          time;

    DL_ClkGetDate( &date);
    DL_ClkGetTime( &time);


    while (num < COUNT_MMWEATHER) 
    {
    
        if (info_weather.xml[num].forecast.month == 0) { // что? не можем найти дату прогноза???
             return -num; // если 0 это означает только одно - файл повреждён, пора обновлять погоду!
        }
    
        y=info_weather.xml[num].forecast.year;
        m=info_weather.xml[num].forecast.month;
        d=info_weather.xml[num].forecast.day;
        h=info_weather.xml[num].forecast.hour;
    
        dbgf("h =%d", h);
        dbgf("time.hour =%d", time.hour);
    
        if (y > date.year) return num+1;
        else if (y >= date.year && m > date.month) return num+1;
        else if (y >= date.year && m >= date.month && d > date.day) return num+1;
        else if (y >= date.year && m >= date.month && d >= date.day && h > time.hour) return num+1;
        else  num++; // смотрим дату следующего прогноза
    }
    
    return -num;
}



  /***************/
 /*** BALANCE ***/
/***************/

// отправка запроса
UINT32 USSDHandleSendRequest( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    IFACE_DATA_T        ifd;
	USSD_REQ_STRING_T   ss;
	SIGNAL_STRENGTH_T   sig;

	DL_SigRegQuerySignalStrength(&sig);
	if(sig.percent == 0) return RESULT_OK; // а вдруг нет сети

    ifd.port = app->port;

	// заполняем ss
	u_strcpy(ss.ss_string, info_balance.USSD);
	ss.ton = 1;
	ss.npi = 1;

	// пошел запрос =)
	ss_id = DL_SigCallSSOperationReq(&ifd, &ss, 0);
	return RESULT_OK;
}

// пришло оповещение от уссд-службы
UINT32 USSDHandleNotify(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    WCHAR str[200];
    UINT32 i, j, b=0, n=0;
    EVENT_T *event = AFW_GetEv(ev_st);

    memset(str, 0, 200 * sizeof(WCHAR));
    memset(info_balance.info.str, 0, INFO_STRING_SIZE * sizeof(WCHAR));

    if (USSD_INVALID_SS_ID == ss_id) return RESULT_FAIL;
    if (*((USSD_SS_ID_T*)(event->attachment)) != ss_id) return RESULT_OK;

    dbgf("ev_code = 0x%x", event->code);
    if (event->code == EV_USSD_COMPLETE)
    {
        if ((( USSD_COMPLETE_T*)(event->attachment))->result == 0)
        {
            u_strncpy(str, (( USSD_COMPLETE_T*)(event->attachment))->ussd_string.str, 200);
        }
    }
    else if (event->code == EV_USSD_OPERATION)
    {
        if ((( USSD_OPERATION_T*)(event->attachment))->result == 0 && (( USSD_OPERATION_T*)(event->attachment))->return_data_type == 7)
        {
            u_strncpy(str, (( USSD_OPERATION_T*)(event->attachment))->return_data.str, 200);
        }
    }
    else
    {
        u_strncpy(str, (( USSD_REQUEST_NOTYFY_T*)(event->attachment))->ussd_string.str, 200);
    }

    APP_ConsumeEv(ev_st,app);
    udbg("req_ussd = %s",str);

    if (info_balance.Words[0] != '0')
    {
        for (i=0; i < INFO_STRING_SIZE; i++)
        {
            if ((str[i] == L' ' && str[i+1] != L' ') || str[i] == L'\n' || str[i] == 0)
            {
                n++;
                for (j=0; j < info_balance.countWords; j++)
                {
                    if (info_balance.Words[j] == n)
                    {
                        u_strncat(info_balance.info.str, str+b, i-b);
                        u_strncat(info_balance.info.str, L" \0", 2);
                        break;
                    }
                }
                b=i+1;
            }
        }
    }
    else
    {
         u_strncpy(info_balance.info.str, str, INFO_STRING_SIZE);
    }

    Repaint(app);
    return RESULT_OK;
}

  /*************/
 /*** Alarm ***/
/*************/

/*
// регистрирует приложение на изменение инфы о будильниках
UINT32 AlarmRegister(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    SU_PORT_T port = ((APPLICATION_T*)app)->port;
    UINT32    status;

    status = DL_DbAlmclkRegisterApplication(port);
    if (status < 2) {
        StartTimer(1000, 7, 0, app);
        return status;
    }

    return ALMCLKHandleNotify(ev_st,app);
}

UINT32 AlarmUnregister(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    return DL_DbAlmclkUnregisterApplication(app->port);
}
*/
// обновление  будильников
UINT32 Alarm(WCHAR *buf)
{
    ALMCLK_RECORD_T alarm;
    UINT32 status, i, j=0;
    UINT8 cnt = 0;
    UINT8 val = 0;
    BOOL am;

    dbgf("update Alarm");

    buf[0] = 0;
    DL_DbAlmclkGetNumberOfRecords(0, &cnt);

    for (i=1; i<=cnt; i++)
    {
        status = DL_DbAlmclkGetRecordByIndex(i, &alarm);
        if (status == 0)
        {
            if (alarm.is_alarm_enabled)
            {
                if (j < info_alarm.Count) j++;
                else break;

                UIS_FormatTime(alarm.time, buf + u_strlen(buf));
                u_strcat(buf, L" ");
            }
        }
    }

    return RESULT_OK;
}

  /****************/
 /*** DateBook ***/
/****************/

// регистрирует приложение на изменение инфы
UINT32 DbkRegister(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 status = DL_DBK_RegisterApplication(app->port);
    if (status != 0)
    {
        // пробуем снова, но через секунду
        StartTimer(1000, 6, 0, app);
        return status;
    }
    status |= DBKHandleSendRequest(ev_st,app);
    return status;
}

UINT32 DbkUnregister(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    return DL_DBK_UnregisterApplication(app->port);
}

// загрузка недели
UINT32 DBKHandleSendRequest( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32 status;
    CLK_DATE_T start_date;
    CLK_DATE_T end_date;
    IFACE_DATA_T  iface_data;

    dbgf("DBKHandleSendRequest", NULL);

    iface_data.port = ((APPLICATION_T*)app)->port;


    DL_ClkGetDate(&start_date);
    memcpy(&end_date, &start_date, sizeof(CLK_DATE_T));
    end_date.day = MaxDays(start_date.month, start_date.year);


    // ищем события календаря в пределах недели
    status = DL_DBK_GetMonthView( &iface_data, start_date, end_date );
    dbgf("DL_DBK_Get*View status = %d", status);
    if (status > 1) { // пробуем снова, но через секунду
            StartTimer(1000, 5, 0, app);
    }

    return RESULT_OK;
}

// поиск первого события
UINT32 DBKHandleView( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    EVENT_T			        *event = AFW_GetEv(ev_st);

    CLK_DATE_T              start_date;
	IFACE_DATA_T            iface_data;
    UINT32                  status;
    UINT32                  evcode = event->code;

    UINT32		month_schedule;

   dbgf("DBKHandleView", NULL);

	DL_ClkGetDate(&start_date);

    if (evcode == EV_DBK_WEEK_VIEW || evcode == EV_DBK_MONTH_VIEW) 
    {

        if ( !event->attachment ) return RESULT_OK;

        month_schedule = *(UINT32*)event->attachment;
        dbgf("month_schedule = %d", month_schedule);

        APP_ConsumeEv(ev_st, app);


        // поиск первого дня с событиями
    	month_day = 1;
    	while ( month_schedule )
    	{
    		if ( month_day >= start_date.day && month_schedule & 0x01 == 1 )
    			break;

    		month_schedule >>= 1;
    		month_day++;
    	}

    	// действительно нашли ?
    	if ( !month_schedule )
    	{
    		dbgf("DBKHandleMonthView: no events at month", NULL);
        	return RESULT_OK;
    	}
    }

        iface_data.port = ((APPLICATION_T*)app)->port;

        start_date.day = month_day;
	    status = DL_DBK_GetNextEventForDayView(&iface_data, true, start_date);
        if (status > 1) { // пробуем снова, но через секунду
            StartTimer(1000, 4, 0, app);
        }
        dbgf("DL_DBK_Get*Event* status = %d", status);


    return RESULT_OK;
}


UINT32 DBKHandleQuery( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    DBK_DATABASE_RESULT_T  *res=(DBK_DATABASE_RESULT_T*)AFW_GetEv(ev_st)->attachment;

    APP_ConsumeEv(ev_st, app);

    dbgf("Query result = %d", res->result);

    info_datebook.info.str[0] = 0;

    if (res->result < 2) 
    {
        if (info_datebook.Show_Date)
        {
            u_ltou(res->event_instance.event_details.start_date.day, info_datebook.info.str);
            u_strcat(info_datebook.info.str, L".");
            u_ltou(res->event_instance.event_details.start_date.month, info_datebook.info.str+u_strlen(info_datebook.info.str));
            u_strcat(info_datebook.info.str, L".");
            u_ltou(res->event_instance.event_details.start_date.year, info_datebook.info.str+u_strlen(info_datebook.info.str));
            u_strcat(info_datebook.info.str, L" ");
        }

        if (info_datebook.Show_Title) 
        {
            udbg("title = %s",res->event_instance.event_details.title);
            u_strcat(info_datebook.info.str, res->event_instance.event_details.title);
            Repaint(app);
        }

    }

    return RESULT_OK;
}

 /******************/
/****  CallTime ***/
/*****************/

UINT32 CallTime(WCHAR *buf)
{
	CALL_TIME_T	time;
    buf[0] = 0;

    //Время последнего разговора
	DL_DbFeatureGetBlock(ID_RC_LAST_CALL_TIME, &time);

	if (time.hour < 10) u_strcat(buf, L"0");
	u_ltou(time.hour, buf+u_strlen(buf));

	u_strcat(buf, L":");
	if (time.minute < 10) u_strcat(buf, L"0");
	u_ltou(time.minute, buf+u_strlen(buf));

	u_strcat(buf, L":");
	if (time.second < 10) u_strcat(buf, L"0");
	u_ltou(time.second, buf+u_strlen(buf));


    return RESULT_OK;
}


// Util

// макс кол-во дней в месяце
UINT8 MaxDays(UINT8 month, UINT16 year)
{
    const UINT8 month_endday[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if (month == 2 && year%4) return month_endday[month-1]+1;
    else return month_endday[month-1];
}

// возведение в степень
UINT32 degree(UINT32 x, UINT32 y)
{
    UINT32 result = x;
    if(y == 0) return 1;

    while(y > 1)
    {
        result *= x;
        y--;
    }

    return result;
}

 // состояние: TRUE - на рабочем столе, FALSE  - не нарабочем столе
BOOL WorkingTable(void)
{
	UINT8 res;
	UIS_GetActiveDialogType(&res);
    return (res == DialogType_Idle ? TRUE : FALSE);
}

UINT32 str2ul (char *tmp_buf)
{
    return strtoul(tmp_buf, NULL, (tmp_buf[0] == '0' && (tmp_buf[1] == 'x' || tmp_buf[1] == 'X'))?16:10);
}

COLOR_T UINT32toCOLOR_T(UINT32 n)
{
    COLOR_T c;

    c.red = n >> 24;
    c.green = (n >> 16)&0x00FF;
    c.blue = (n >> 8)&0x0000FF;
    c.transparent = n&0x000000FF;

    return c;
}

UINT32 StartTimer( UINT32 period, UINT32 id, UINT8 type, APPLICATION_T *app )
{
    UINT32 status;
    IFACE_DATA_T  iface;

    StopTimer(id, app);
    dbgf("Start timer %d", id);

    iface.port = app->port;

    if (type == 0)  status = DL_ClkStartTimer(&iface, period, id);
    else  status = DL_ClkStartCyclicalTimer(&iface, period, id);

    timer_handle[id] = iface.handle;
    return status;
}

UINT32 StopTimer (UINT32 id, APPLICATION_T *app)
{
    UINT32 status = RESULT_OK;
    IFACE_DATA_T  iface;

    dbgf("Stop timer %d", id);

    if (timer_handle[id] != 0 && timer_handle[id] != 0xffffffff)
    {
        iface.port = app->port;
        iface.handle = timer_handle[id];

        status = DL_ClkStopTimer(&iface);
        timer_handle[id] = 0;
    }

    return status;
}

UINT32 isOurTimer(UINT32 THandle)
{
    UINT32 i;
    for(i=0; i<COUNT_TIMERS;i++)
        if(THandle == timer_handle[i])
            return RESULT_OK;

    return RESULT_FAIL;
}

UINT32 AlarmStart(UINT32 id, CLK_DATE_T date, CLK_TIME_T time)
{
    IFACE_DATA_T  iface;
    DL_ALARM_DATA_T alarm_data;

    dbgf("Start alarm %d", id);
    dbgf("  hour %d", time.hour);

    iface.port = su_port;

    memcpy(&alarm_data.date, &date, sizeof(CLK_DATE_T));
    memcpy(&alarm_data.time, &time, sizeof(CLK_TIME_T));
    alarm_data.unk = id;

    return DL_ClkStoreIndividualEvent( &iface, alarm_data );
}

UINT32 AlarmStop( UINT32 id, CLK_DATE_T date, CLK_TIME_T time)
{
    IFACE_DATA_T  iface;
    DL_ALARM_DATA_T alarm_data;

    dbgf("Stop alarm %d", id);

    iface.port = su_port;

    memcpy(&alarm_data.date, &date, sizeof(CLK_DATE_T));
    memcpy(&alarm_data.time, &time, sizeof(CLK_TIME_T));
    alarm_data.unk = id;

    return DL_ClkDeleteIndividualEvent( &iface, alarm_data, 55 );
}
