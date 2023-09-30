#include "app.h"
#include "graphics.h"
#include "iniparser.h"

UIS_DIALOG_T dialog;

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "Dictaphone";

static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked },
    { STATE_HANDLERS_END,           NULL                     },
};

static EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { EV_GRANT_TOKEN,               HandleUITokenGranted },
    { STATE_HANDLERS_END,           NULL                 },
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{

    { EV_INK_KEY_PRESS,             HandleKeypress          },
    { EV_TIMER_EXPIRED,       	    HandleTimer             },
    { EV_MME_OPEN_SUCCESS,          HandleStartRecord       },
    { EV_MME_PLAY_COMPLETE,         HandleRecordComplete    },
    { STATE_HANDLERS_END,           NULL                    },
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
    }
};

UINT32 ELF_Entry (ldrElf *ela, WCHAR* Params)
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

	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
		return RESULT_OK;
	}

	app = (APP_T*)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    //читаем конфиг
    ReadConfig(&elf->id, &app->config);

	app->mhandle = NULL;
	app->time = 0;
    app->state = DICTAPHONE_STATE_STOP;

	status = APP_Start(ev_st, &app->apt, APP_STATE_INIT, state_handling_table, ELF_Exit, app_name, 0);
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
	StopRecord(app);

	APP_ExitStateAndApp(ev_st, app, 0);
    ldrUnloadElf(elf);

	return RESULT_OK;
}

UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
	UINT32 status = APP_HandleUITokenGranted(ev_st, app);

	if((status == RESULT_OK) && (app->token_status == 2))
		status = APP_UtilChangeState(APP_STATE_MAIN, ev_st, app);

	return status;
}

UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    APP_T *papp = (APP_T *)app;
	if(type != ENTER_STATE_ENTER) return RESULT_OK;

    GRAPHIC_POINT_T disp_size;
    UIS_CanvasGetDisplaySize(&disp_size);

    papp->bufd.w = disp_size.x + 1;
	papp->bufd.h = disp_size.y + 1;
	papp->bufd.buf = NULL;

	app->dialog = UIS_CreateColorCanvas(&app->port, &papp->bufd, TRUE);
	if(app->dialog == 0) return RESULT_FAIL;

	dialog = app->dialog;
	paint(app);

	return RESULT_OK;
}

UINT32 MainStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type)
{
	APP_UtilUISDialogDelete(&app->dialog);
	return RESULT_OK;
}

UINT32 HandleKeypress (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
    UINT8 key = GET_KEY(ev_st);
    APP_ConsumeEv(ev_st, app);

	switch (key)
	{
		case KEY_SOFT_LEFT:
		case KEY_RED:
            app->exit_status = TRUE;
			return RESULT_OK;
			break;

		case KEY_SOFT_RIGHT:
			if (papp->state == DICTAPHONE_STATE_STOP) CaptureCreate(app);
			else StopRecord(app);
            break;
	}

	paint(app);
	return RESULT_OK;
}

UINT32 HandleTimer (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	APP_T *papp = (APP_T*)app;

	if(AFW_GetEvSeqn(ev_st) == app->timer_handle)
	{
		papp->time++;
		paint(app);
	}

	return RESULT_OK;
}

UINT32 HandleStartRecord(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
	APP_T *papp = (APP_T*)app;
	UINT32 status;

	GetFileName(app);
	status = MME_FW_gc_handle_start(papp->mhandle, papp->uri);

	if (status == RESULT_OK)
	{
        papp->state = DICTAPHONE_STATE_RECORD;
		APP_UtilStartCyclicalTimer(1000, 1, app);
	}

	return status;
}

UINT32 HandleRecordComplete (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    MME_PLAY_COMPLETE_T *attach = AFW_GetEv(ev_st)->attachment;
    APP_T *papp = (APP_T *)app;

    if (papp->mhandle == attach->media_handle && papp->mhandle != NULL)
        StopRecord(app);

    return RESULT_OK;
}

UINT32 CaptureCreate(APPLICATION_T *app)
{
    APP_T *papp = (APP_T*)app;
    IFACE_DATA_T iface;

    MMSS_FILE_FORMAT_T file_format;
    MMSS_MEDIA_FORMAT_T media_format;

    iface.port = app->port;
    file_format = MMSS_AMR;

    memset(&media_format, 0, sizeof(MMSS_MEDIA_FORMAT_T));
    media_format.media_type = MMSS_AUDIO;
    media_format.audio_format.bit_rate = 0; // default
    media_format.audio_format.audio_codec = AMR_CODEC;
    media_format.audio_format.sampling_freq = 0; // default
    media_format.audio_format.audio_mode = 0; // Mono

    papp->mhandle = (MME_GC_MEDIA_FILE)MME_FW_gc_handle_capture_create(&iface, NULL, file_format, &media_format);

    if (papp->mhandle) return RESULT_OK;
    else return RESULT_FAIL;
}

UINT32 StopRecord (APPLICATION_T *app)
{
    APP_T *papp = (APP_T*)app;
    UINT32 status = RESULT_OK;

    status |= MME_FW_gc_handle_stop(papp->mhandle);
    status |= MME_FW_gc_handle_close(papp->mhandle);

    if (status == RESULT_OK)
        papp->state = DICTAPHONE_STATE_STOP;

    APP_UtilStopTimer(app);
    papp->mhandle=NULL;
    papp->time=0;

    return status;
}

void paint(APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
	WCHAR str[64];

	//фон
	SetFillColor(0xFFFFFF00);
	SetForegroundColor(0x00000000);
	SetBackgroundColor(0xFFFFFFFF);
	FillRect(0, 0, papp->bufd.w, papp->bufd.h);

    //состояние:
	GetResourceString(str, LANG_STATE);
	DrawText(str, papp->config.Indent, papp->config.Indent + papp->config.LineHeight * 0, ANCHOR_LEFT | ANCHOR_TOP);

    //время:
    GetResourceString(str, LANG_TIME);
    DrawText(str, papp->config.Indent, papp->config.Indent + papp->config.LineHeight * 2, ANCHOR_LEFT | ANCHOR_TOP);

    //имя:
    GetResourceString(str, LANG_NAME);
    DrawText(str, papp->config.Indent, papp->config.Indent + papp->config.LineHeight * 4, ANCHOR_LEFT | ANCHOR_TOP);

	switch (papp->state)
	{
		case DICTAPHONE_STATE_STOP:
			SetForegroundColor(0x00B00000);

            GetResourceString(str, LANG_STOPPED);
            DrawText(str, papp->config.Indent + papp->config.TabLenght, papp->config.Indent + papp->config.LineHeight * 1, ANCHOR_LEFT | ANCHOR_TOP);

			GetResourceString(str, LANG_RECORD);
			DrawText(str, papp->bufd.w - papp->config.Indent, papp->bufd.h - papp->config.Indent, ANCHOR_RIGHT | ANCHOR_BOTTOM);
			break;

		case DICTAPHONE_STATE_RECORD:
			SetForegroundColor(0xFF000000);

            GetResourceString(str, LANG_RECORDING);
            DrawText(str, papp->config.Indent + papp->config.TabLenght, papp->config.Indent + papp->config.LineHeight * 1, ANCHOR_LEFT | ANCHOR_TOP);

			DL_FsSGetFileName(papp->uri, str);
			DrawText(str, papp->config.Indent + papp->config.TabLenght, papp->config.Indent + papp->config.LineHeight * 5, ANCHOR_LEFT | ANCHOR_TOP);

            GetResourceString(str, LANG_STOP);
			DrawText(str, papp->bufd.w - papp->config.Indent, papp->bufd.h - papp->config.Indent, ANCHOR_RIGHT | ANCHOR_BOTTOM);
			break;
	}

	sec2hms(papp->time,str);
	DrawText(str, papp->config.Indent + papp->config.TabLenght, papp->config.Indent + papp->config.LineHeight * 3, ANCHOR_LEFT | ANCHOR_TOP);

	//левая-софт !
	SetForegroundColor(0x00000000);
	GetResourceString(str, LANG_EXIT);
	DrawText(str, papp->config.Indent, papp->bufd.h - papp->config.Indent, ANCHOR_LEFT | ANCHOR_BOTTOM);
}

//получить имя записываемого файла
void GetFileName(APPLICATION_T *app)
{
    APP_T *papp = (APP_T *)app;
    WCHAR u_name[30];
    char a_name[30];

    CLK_DATE_T date;
    CLK_TIME_T time;

    DL_ClkGetDate(&date);
    DL_ClkGetTime(&time);

    sprintf(a_name, "%02d.%02d.%04d_%02d-%02d-%02d.amr\0", date.day, date.month, date.year, time.hour, time.minute, time.second);
    u_atou(a_name, u_name);

    memset(papp->uri, 0, sizeof(WCHAR) * (FS_MAX_URI_NAME_LENGTH + 1));
    u_strcpy(papp->uri, papp->config.RecordFolder);
    u_strcat(papp->uri, u_name);
}

//секунды в часы:минуты:секунды
void sec2hms(UINT32 seconds, WCHAR *str)
{
    UINT32        t, r;

    //часы
    for( r='0',t=seconds; t>35999; t-=36000,r++ ); //считаем десятки
    str[0] = r;
    for( r='0'; t>35999; t-=3600,r++ ); //считаем единицы
    str[1] = r; //используем факт, что коды цифр идут последовательно
    str[2] = ':';

    //минуты
    for( r='0',t=seconds; t>599; t-=600,r++ ); //считаем десятки
    str[3] = r;
    for( r='0'; t>59; t-=60,r++ ); //считаем единицы
    str[4] = r; //используем факт, что коды цифр идут последовательно
    str[5] = ':';

    //секуды
    for( r='0'; t>9; t-=10,r++ ); //считаем десятки и остаток
    str[6] = r;
    str[7] = '0'+t; //используем факт, что коды цифр идут последовательно
    str[8] = 0;
}

//получение строки из ленга
UINT32 GetResourceString(WCHAR *str, RESOURCE_ID res_id)
{
	UINT32 resource_size;
	DRM_GetResourceLength(res_id, &resource_size);
	if(resource_size > 0) DRM_GetResource(res_id, str, resource_size);
	return RESULT_OK;
}

//чтение конфига
UINT32 ReadConfig(DL_FS_MID_T *id, DICTAPHONE_CONFIG_T *config)
{
    WCHAR *ptr = NULL;
    WCHAR ini_uri[FS_MAX_URI_NAME_LENGTH + 1];
    WCHAR rec_uri[FS_MAX_URI_NAME_LENGTH + 1];
    char value[256];

    memset(config, 0, sizeof(DICTAPHONE_CONFIG_T));
    memset(rec_uri, 0, (FS_MAX_URI_NAME_LENGTH + 1) * sizeof(WCHAR));

    //путь к эльфу
    u_strcpy(rec_uri, L"file:/");
    ptr = rec_uri + u_strlen(rec_uri);
    DL_FsGetURIFromID(id, ptr);

    //путь к ini
    u_strcpy(ini_uri, rec_uri);
    ptr = ini_uri + u_strlen(ini_uri);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"dictaphone.ini\0");

    //читаем ini
    if(INI_Open(ini_uri))
    {
        char *end;
        INI_SetSeparator('=');

        //высота строки
        if(INI_ReadParameter(NULL, "LineHeight", "Z", value, 256))
        {
            config->LineHeight = strtoul(value, NULL, 10);
        }
        else
        {
            config->LineHeight = DEF_LINE_HEIGHT;
        }

        //размер табуляции (кол-во пробелов)
        if(INI_ReadParameter(NULL, "TabLenght", "Z", value, 256))
        {
            config->TabLenght = strtoul(value, NULL, 10);
        }
        else
        {
            config->TabLenght = DEF_TAB_LENGHT;
        }

        //отступ от краёв (в пикселях)
        if(INI_ReadParameter(NULL, "Indent", "Z", value, 256))
        {
            config->Indent = strtoul(value, NULL, 10);
        }
        else
        {
            config->Indent = DEF_INDENT;
        }

        //путь к папке с записями
        if(INI_ReadParameter(NULL, "RecordFolder", "/", value, 256) == FALSE)
            strcpy(value, "/");

        if (value[0] == '/' && value[1] == 0 )
        {
            //получаем папку эльфа
            u_strcpy(config->RecordFolder, rec_uri);
            ptr = config->RecordFolder + u_strlen(config->RecordFolder);
            while(*ptr != L'/')
            {
                *ptr = 0;
                ptr--;
            }
        }
        else
        {
            if (value[0] == '/' && value[2] != '/' && value[4] != ':')
            {
                //путь относительно папки эльфа
                //получаем папку эльфа
                u_strcpy(config->RecordFolder, rec_uri);

                ptr = config->RecordFolder + u_strlen(config->RecordFolder);
                while(*ptr != L'/')
                {
                    *ptr = 0;
                    ptr--;
                }
                *ptr = 0;

                u_atou(value, ptr);
            }
            else
            {
                //абсолютный путь
                u_atou(value, config->RecordFolder);
            }
            INI_Close();
        }

        //если указанного в ini пути не существует, то получаем папку эльфа
        if(DL_FsDirExist(config->RecordFolder) == false)
        {
            u_strcpy(config->RecordFolder, rec_uri);
            ptr = config->RecordFolder + u_strlen(config->RecordFolder);
            while(*ptr != L'/')
            {
                *ptr = 0;
                ptr--;
            }
        }
    }
    else
    {
        //получаем папку эльфа
        u_strcpy(config->RecordFolder, rec_uri);
        WCHAR *ptr = config->RecordFolder + u_strlen(config->RecordFolder);
        while(*ptr != L'/')
        {
            *ptr = 0;
            ptr--;
        }

        config->Indent = DEF_INDENT;
        config->TabLenght = DEF_TAB_LENGHT;
        config->LineHeight = DEF_LINE_HEIGHT;
    }

    //если в конце нет слеша, то добавляем его
    ptr = config->RecordFolder + u_strlen(config->RecordFolder) - 1;
    if(*ptr != L'/')
    {
        *(ptr + 1) = L'/';
        *(ptr + 2) = 0;
    }

    return RESULT_OK;
}
