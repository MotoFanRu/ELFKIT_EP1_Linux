#include "Tunes.h"
#include "Graphics.h"
#include "LNGparser.h"
#include "Config.h"
#include "Utils.h"

static SETTING_T Setting;


static BOOL KEY_PRESS = false;
static BOOL KEY_RELEASE = false;

static UINT8   blink = 0; // 1бит - боковые, 2бит - фонарик, 3бит - дисплей и клава

static UINT64 *tunes_playlist=NULL; // треки плейлиста
static UINT64  *tunes_list=NULL; // база треков
static INT16 id=0; // id трека в базе 
static UINT16 countTunes =0; // кол-во треков


UIS_DIALOG_T        dialog;

static  TAGS_T tag;

WCHAR TunesU[128]; // папка  Tunes.elf
static  WCHAR TunesPLU[128]; // имя проигр-го плейлиста
static  WCHAR TunesPLnew[128];
static  WCHAR nameCfgFileU[128];
static  WCHAR TunesTrackU[272]; // имя проигрываемого трека
  
static UINT32 timePosition=0; //  текущая позиция
static UINT32 timeDuraction=0; // общее время трека
static INT32  timeRew=1; // шаг перемотки
            // -1 = назад
            //  1 = вперед


static UINT8 State_Player = 0; // состояние плеера 

static UINT32 ENTRIES_NUM=4;		// кол-во пунктов

static FILEINFO  *file_list=NULL; // файлы

static WCHAR  cur_folder[256]; // текущая папка

static DRAWING_BUFFER_T		bufd; // буфер для рисования


ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "Tunes"; 


// SHELL
static EVENT_HANDLER_ENTRY_T shell_any_state_handlers[] =
{
    { STATE_HANDLERS_PM_API_EXIT,       shell_destroyApp  },
    { NULL,                                   DialFinish  },

    { EV_REVOKE_TOKEN,           APP_HandleUITokenRevoked },
    { EV_TIMER_EXPIRED,       	             player_Timer },

    { STATE_HANDLERS_RESERVED,                    appShow },
    { STATE_HANDLERS_RESERVED,     	   		Delete_Action },
    { STATE_HANDLERS_RESERVED,          AddInQueue_Action },

    { EV_MME_OPEN_SUCCESS,				            Play  },
    { EV_MME_OPEN_ERROR,                            Next  },
	{ EV_MME_PLAY_COMPLETE,		            PlayComplete  },
	{ EV_MME_SEEK_SUCCESS,			            SeekPlay  },
    { EV_DEVICE_DETACH,                        HandsFree  },
    { EV_DEVICE_ATTACH,                        HandsFree  },

    { STATE_HANDLERS_END,          					NULL  },
};

static EVENT_HANDLER_ENTRY_T shell_init_state_handlers[] =
{
    { EV_GRANT_TOKEN,           shell_HandleUITokenGranted },
    { STATE_HANDLERS_END,         		  			  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_main_state_handlers[] =
{
    
    { EV_REQUEST_LIST_ITEMS,       	  shell_HandleListReq  },
	{ EV_SELECT, 				     shell_SelectItem_Main },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { EV_DONE,                     	               appHide },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_playlists_state_handlers[] =
{

    { EV_REQUEST_LIST_ITEMS,       	  shell_HandleListReq  },
	{ EV_SELECT, 				shell_SelectItem_Playlists },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { STATE_HANDLERS_RESERVED,     	   		NewList_Action },
	
    { STATE_HANDLERS_RESERVED,     	      DeleteAll_Action },
    { STATE_HANDLERS_RESERVED,         AutoCreatePL_Action },
    { STATE_HANDLERS_RESERVED,         SelectItemPL_Action },
    
    { EV_DONE,                     	             PrevState },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_playlist_state_handlers[] =
{

    { EV_REQUEST_LIST_ITEMS,       	  shell_HandleListReq  },
	{ EV_SELECT, 				 shell_SelectItem_Playlist },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { EV_DONE,                     	             PrevState },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_fbrowserplayer_state_handlers[] =
{

    { EV_REQUEST_LIST_ITEMS,       	  shell_HandleListReq  },
	{ EV_SELECT, 		   shell_SelectItem_Fbrowserplayer },
    { STATE_HANDLERS_RESERVED,     	   		NewList_Action },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { STATE_HANDLERS_RESERVED,         SelectItemFB_Action },

    { STATE_HANDLERS_RESERVED,            Bluetooth_Action },
    { EV_DONE,                     	             PrevState },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_fbrowserplaylist_state_handlers[] =
{
    
    { EV_REQUEST_LIST_ITEMS,       	  shell_HandleListReq  },
	{ EV_SELECT, 		 shell_SelectItem_Fbrowserplaylist },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { STATE_HANDLERS_RESERVED,         SelectItemFB_Action },
    { EV_DONE,                     	   		   StateMinus2 },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_skins_state_handlers[] =
{
 
    { EV_REQUEST_LIST_ITEMS,       	   shell_HandleListReq },
	{ EV_SELECT, 					shell_SelectItem_Skins },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { EV_DONE,                     	   		     PrevState },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_setting_state_handlers[] =
{

    { EV_REQUEST_LIST_ITEMS,       	   shell_HandleListReq },
	{ EV_SELECT, 				  shell_SelectItem_Setting },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { EV_DONE,                     	             PrevState },
    { STATE_HANDLERS_END,           		      	  NULL },
};


static EVENT_HANDLER_ENTRY_T shell_edit_state_handlers[] =
{
    { EV_DATA,                               	 shell_EditData  },
    { EV_DONE,                       		       shell_EditOk  },
	{ EV_CANCEL,                 				      PrevState  },
    { STATE_HANDLERS_END,         			     	        NULL },
};

static EVENT_HANDLER_ENTRY_T shell_canvas_state_handlers[] =
{
    { NULL,                                              repaint },
    { NULL,                                        ActiveDisplay },

    { EV_GAIN_FOCUS,                                     focus },
	{ EV_LOSE_FOCUS,                                     nofocus },

    { EV_VOL_CHANGE_REQUEST,                       APP_ConsumeEv },

    { EV_INK_KEY_PRESS,                    canvas_HandleKeypress },
    { EV_INK_KEY_RELEASE,                canvas_HandleKeyrelease },
    { STATE_HANDLERS_END,         			     	        NULL },
};

static EVENT_HANDLER_ENTRY_T shell_info_state_handlers[] =
{
	{ EV_DONE,                 				          PrevState  },
    { STATE_HANDLERS_END,         			     	        NULL },
};

static EVENT_HANDLER_ENTRY_T shell_queue_state_handlers[] =
{
 
    { EV_REQUEST_LIST_ITEMS,       	   shell_HandleListReq },
	{ EV_SELECT, 					shell_SelectItem_Queue },
    { STATE_HANDLERS_RESERVED,            Randomize_Action },
    { STATE_HANDLERS_RESERVED,     	   		NewList_Action },
	{ EV_LIST_NAVIGATE,							  Navigate },
    { EV_DONE,                     	   		     PrevState },
    { STATE_HANDLERS_END,           		      	  NULL },
};

static EVENT_HANDLER_ENTRY_T shell_background_state_handlers[] =
{
    { STATE_HANDLERS_RESERVED,                           appinit },
	{ EV_KEY_PRESS,                        player_HandleKeypress },
	{ EV_KEY_RELEASE,                    player_HandleKeyrelease },

    { STATE_HANDLERS_END,         			     	        NULL },
};

static const STATE_HANDLERS_ENTRY_T shell_state_handling_table[] =
{
    { SHELL_STATE_ANY,
      NULL,
      NULL,
      shell_any_state_handlers
    },

    { SHELL_STATE_INIT,
      NULL,
      NULL,
      shell_init_state_handlers
    },
    
    { SHELL_STATE_MAIN,
      shell_MainStateEnter,
      shell_StateExit,
      shell_main_state_handlers
    },

    { SHELL_STATE_PLAYLISTS,
      shell_PlaylistsStateEnter,
      shell_PlaylistsStateExit,
      shell_playlists_state_handlers
    },

    { SHELL_STATE_PLAYLIST,
      shell_PlaylistStateEnter,
      shell_PlaylistStateExit,
      shell_playlist_state_handlers
    },

    { SHELL_STATE_FBROWSERPLAYER,
      shell_FbrowserplayerStateEnter,
      shell_FbrowserplayerStateExit,
      shell_fbrowserplayer_state_handlers
    },

    { SHELL_STATE_FBROWSERPLAYLIST,
      shell_FbrowserplaylistStateEnter,
      shell_FbrowserplaylistStateExit,
      shell_fbrowserplaylist_state_handlers
    },

    { SHELL_STATE_SKINS,
      shell_SkinsStateEnter,
      shell_SkinsStateExit,
      shell_skins_state_handlers
    },

    { SHELL_STATE_SETTING,
      shell_SettingStateEnter,
      shell_StateExit,
      shell_setting_state_handlers
    },
	
	{ SHELL_STATE_EDIT,
      shell_EditStateEnter,
      shell_StateExit,
      shell_edit_state_handlers
    },

    { SHELL_STATE_CANVAS,
      shell_CanvasStateEnter,
      shell_CanvasStateExit,
      shell_canvas_state_handlers
    },

    { SHELL_STATE_INFO,
      shell_InfoStateEnter,
      shell_StateExit,
      shell_info_state_handlers
    },

    { SHELL_STATE_QUEUE,
      shell_QueueStateEnter,
      shell_StateExit,
      shell_queue_state_handlers
    },

    { SHELL_STATE_BACKGROUND,
      NULL,
      NULL,
      shell_background_state_handlers
    }

};

// сохранить файл очереди
void save_file_queue(void)
{
    dbgf("save_file_queue begin...");
    WCHAR   uri[256];
    FILE_HANDLE_T    f;
    UINT32  wr;
    UINT32  magic=0x71756575;

    u_strcpy(uri, TunesU);
    u_strcat(uri, L"queue");

    DL_FsDeleteFile(uri, 0);
    f = DL_FsOpenFile(uri, FILE_WRITE_MODE, 0);
    DL_FsWriteFile(&magic, 4, 1, f, &wr);
    DL_FsWriteFile(&id, 4, 1, f, &wr);
    DL_FsWriteFile(tunes_list, countTunes*sizeof(UINT64), 1, f, &wr );
    DL_FsCloseFile(f);

    dbgf("save_file_queue end...");
}

// загрузить файл очереди
void load_file_queue(void)
{
    dbgf("load_file_queue begin...");
    WCHAR           uri[256];
    FILE_HANDLE_T            f;
	UINT32          fSize;
    UINT32          rd;
    UINT32          magic;


    u_strcpy(uri, TunesU);
    u_strcat(uri, L"queue");

    f = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID) return;

        fSize = DL_FsGetFileSize(f);
        DL_FsReadFile( &magic, 4, 1, f, &rd ); // magic
        if (magic != 0x71756575) return;

        DL_FsReadFile( &id, 4, 1, f, &rd ); // id трека

        free(tunes_list);
        tunes_list = malloc(fSize-8);
        if (tunes_list == NULL) return;

        DL_FsReadFile( tunes_list, fSize-8, 1, f, &rd ); // на ID файла отводится 8 байт
        countTunes = (fSize-8)/sizeof(UINT64);

        DL_FsCloseFile(f);

    dbgf("load_file_queue end...");
}

// обновить id трека в файле очереди
void upd_id_file_queue()
{
    dbgf("upd_id_file_queue begin...");
    WCHAR           uri[256];
    FILE_HANDLE_T            f;
    UINT32          wr;

    u_strcpy(uri, TunesU);
    u_strcat(uri, L"queue");

    f = DL_FsOpenFile(uri, FILE_READ_PLUS_MODE, 0);
    DL_FsFSeekFile(f,4,SEEK_WHENCE_SET);
    DL_FsWriteFile(&id,4,1,f,&wr);
    DL_FsCloseFile(f);

    dbgf("upd_id_file_queue end...");
}



// добавить треки в tunes_list
UINT32 add_in_tunes_list(UINT64 *tunes, UINT32 new_size, UINT8 type) 
{
    dbgf("add_in_tunes_list begin...");
    UINT64  *temp_tunes_list=NULL;
    
    if (Setting.Play_rnd != 0) rands(tunes, new_size);

    switch (type) {
    case ADD_NONE:
        id=0;
        free(tunes_list);
        tunes_list = malloc(new_size*sizeof(UINT64));
        if (tunes_list == NULL) return RESULT_FAIL;

        memcpy(tunes_list, tunes, new_size*sizeof(UINT64));
        dbgf("id tunes_list = %x%x", tunes_list[id]);
        countTunes = new_size;

        
        break;

    case ADD_QUEUE:
    case ADD_ONE_TRACK:
        temp_tunes_list = malloc(countTunes*sizeof(UINT64));
        if (temp_tunes_list == NULL) return RESULT_FAIL;

        memcpy(temp_tunes_list, tunes_list, countTunes*sizeof(UINT64));
        free(tunes_list);
        tunes_list = malloc((countTunes+new_size)*sizeof(UINT64));
        if (tunes_list == NULL) return RESULT_FAIL;

        memcpy(tunes_list, temp_tunes_list, countTunes*sizeof(UINT64));
        free(temp_tunes_list);
        memcpy((UINT64*)(tunes_list+countTunes), tunes, new_size*sizeof(UINT64));
        countTunes += new_size;
        break;
    }

    if (Setting.Queue_save > 1) save_file_queue();

    dbgf("add_in_tunes_list end...");
    return RESULT_OK;
}

// удаление трека из очереди
UINT32 del_item_tunes_list(UINT32 item) 
{
    dbgf("del_item_tunes_list begin...");
    UINT64 *temp_tunes_list=NULL;


    temp_tunes_list = malloc((countTunes-1)*sizeof(UINT64));
    if (temp_tunes_list == NULL) return RESULT_FAIL;

    memcpy(temp_tunes_list, (UINT64*)(tunes_list), item*sizeof(UINT64));
    memcpy((UINT64*)(temp_tunes_list+item), (UINT64*)(tunes_list+item+1), (countTunes-(item+1))*sizeof(UINT64));

    free(tunes_list);
    tunes_list = malloc((countTunes-1)*sizeof(UINT64));
    if(tunes_list == NULL) return RESULT_FAIL;

    memcpy(tunes_list, temp_tunes_list, (countTunes-1)*sizeof(UINT64));
    free(temp_tunes_list);

    countTunes --;

    if (Setting.Queue_save > 1 ) save_file_queue();

    dbgf("del_item_tunes_list end...");
    return RESULT_OK;
}

UINT32 rands(UINT64 *tunes, UINT32 new_size) // random
{
    dbgf("rands begin...");
    UINT16 i;
    UINT16 rnd;
    UINT64 i_buf;

    if (new_size < 2) return RESULT_FAIL;

    randomize();

    for (i=0; i < new_size; i++) {
       rnd = random(new_size-1);

       i_buf = tunes[i];
       tunes[i] = tunes[rnd];
       tunes[rnd] = i_buf;
    }

    if (Setting.Queue_save > 1 ) save_file_queue();

    dbgf("rands end...");
	return RESULT_OK; 
}

/*****************/

//!!!!!!!!!!!!!!!!!! найти этот ивент!!!!!!!!!!!!!!!!!!!!
#define EV_CALLS_TERMINATED 0x40404040

UINT32 ELF_Entry(ldrElf *ela, WCHAR *param)
{
    UINT32 i, reserve, status = RESULT_OK;
    WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];
    WCHAR SkinFolder[FS_MAX_URI_NAME_LENGTH + 1];

    elf = ela;
    elf->name = (char *)app_name;

    //Check if application is already loaded
    if(ldrIsLoaded((char *)app_name))
    {
        PFprintf("%s: Application already loaded!\n", app_name);
        return RESULT_FAIL;
    }

    //определяем имя конфига
    u_strcpy(nameCfgFileU, L"file:/");
    DL_FsGetURIFromID(&elf->id, (nameCfgFileU + 6));
    nameCfgFileU[u_strlen(nameCfgFileU)-3] = 0;
    u_strcat(nameCfgFileU, L"cfg");

    // читаем настройки
    ReadSetting();

    dbgf("ev_code = 0x%x", Setting.ev_code);
    dbgf("EV_CALLS_TERMINATED = 0x%x", EV_CALLS_TERMINATED);

    shell_any_state_handlers[1].code = EV_CALLS_TERMINATED;
    shell_canvas_state_handlers[0].code = EV_DISPLAY_ACTIVE;
    shell_canvas_state_handlers[1].code = EV_DISPLAY_NO_ACTIVE;
    
    // Дозаполнем наши таблицы обработчиков, где есть STATE_HANDLERS_RESERVED
    reserve = elf->evbase + 1;
    ldrInitEventHandlersTbl( shell_any_state_handlers, reserve);
    ldrInitEventHandlersTbl( shell_playlists_state_handlers, reserve);
    ldrInitEventHandlersTbl( shell_fbrowserplayer_state_handlers, reserve);
    ldrInitEventHandlersTbl( shell_fbrowserplaylist_state_handlers, reserve);
    ldrInitEventHandlersTbl( shell_queue_state_handlers, reserve);
    ldrInitEventHandlersTbl( shell_background_state_handlers, reserve);

    /* Регистрируем приложение Shell */
    status = APP_Register( Setting.ev_code?&Setting.ev_code:&elf->evbase,   // На какое событие должно запускаться приложение
                               1,                       // Кол-во событий, на которое должно запускаться приложение
                               shell_state_handling_table,    // Таблица состояний
                               SHELL_STATE_MAX,            // Количество состояний
                               (void*)shell_startApp );   // Функция, вызываемая при запуске
    
    // определяем путь к Tunes.elf
    // определяем путь к Tunes.elf
    u_strcpy(TunesU, L"file:/");
    DL_FsGetURIFromID(&elf->id, (TunesU + 6));
    for(i=u_strlen(TunesU)-1; i > 0 ; i--)
    {
        if (TunesU[i] == '/') break;
        TunesU[i] = 0;
    }

    // плейлист по умолчанию
    u_strcpy(TunesPLU, TunesU);
    u_strcat(TunesPLU, L"Playlists/All.tpl");

    // папка скина
    u_strcpy(SkinFolder, TunesU);
    u_strcat(SkinFolder, L"Skins/");
    u_strcat(SkinFolder, Setting.Skin_name);
    u_strcat(SkinFolder, L"/");

    ParseConfig(&cfg_skin, SkinFolder);  // читаем конфиг скина

    if (Setting.Queue_save > 1) load_file_queue();

    if (!Setting.ev_code ) 
    {
        if (u_strlen(param)) 
        {
            dbgf("Start Shell (param)");

            param2uri(param, uri);
            ldrSendEvent(elf->evbase); 

              /*
             u_strcpy(ext, SplitPath(uri, L"."));
             u_strmakelower(ext);
             if (!u_strcmp(ext, L"mp3") || !u_strcmp(ext, L"m4a") || !u_strcmp(ext, L"wav") || !u_strcmp(ext, L"amr") || !u_strcmp(ext, L"wav")) {
                 PlayFile(uri, ADD_NONE);
             } else if (!u_strcmp(ext, L"tpl")) {
                 PlayTPL(uri, ADD_NONE);
             } else if (!u_strcmp(ext, L"mvpl")) {
                 PlayMVPL(uri, ADD_NONE);
             }
             
            */
        }
        else
        {
            dbgf("Start Shell", NULL);
            ldrSendEvent(elf->evbase);  // Запустить немедленно Shell 
        }
    }

    return RESULT_OK;   // Пока не имеет значения, что возвращать
}


void My_APP_HandleEvent( EVENT_STACK_T *ev_st, APPLICATION_T *app, APP_ID_T appid, REG_ID_T regid )
{
	APP_SHELL_T    	*papp = ( APP_SHELL_T * ) app;
    
	if(papp->apt.focused) 
    {
        if(papp->apt.state != SHELL_STATE_CANVAS && papp->apt.state != SHELL_STATE_BACKGROUND && papp->apt.state != SHELL_STATE_EDIT)
        {
            EVENT_T *event = AFW_GetEv(ev_st);

            if(event->code == EV_KEY_PRESS) 
            {  
                // подмена джой влево - назад, джой вправо - джой центр
                switch ( event->data.key_pressed ) 
                {
                    case KEY_LEFT:
                        if (papp->apt.state != SHELL_STATE_MAIN)
                        {
                            APP_ConsumeEv(ev_st, app);
                            AFW_AddEvNoD(ev_st, EV_DONE); // назад
                        }
                        break;

                    case KEY_RIGHT:
                        event->data.key_pressed = KEY_CAM;
                        break;

                    case KEY_SMART: // в листах смарт-клавиша - пауза/воспроизведение
                        if (papp->startPlayer)
                        {
                            if (State_Player == PLAY) Pause(ev_st, app);
                            else if (State_Player == PAUSE || State_Player == STOP) Play(ev_st, app);
                            APP_ConsumeEv(ev_st, app);
                        }
                        break;
                }

                // в плей-листах
                if(papp->apt.state == SHELL_STATE_PLAYLISTS)
                {
                    if(event->data.key_pressed == KEY_0)
                    {  // отметить файл

                        APP_ConsumeEv(ev_st, app);

                        if (file_list[papp->item-1].selected) file_list[papp->item-1].selected=false;
                        else file_list[papp->item-1].selected=true;
                        dbgf("sel = %d", file_list[papp->item-1].selected);

                        UpdateList(ev_st,app,papp->item); // обновление пункта
                    }
                }
            }
        }
    }

    APP_HandleEvent(ev_st, app, appid, regid);
}


UINT32 Show( EVENT_STACK_T *ev_st,  APPLICATION_T *app, BOOL show )
{
    UINT32 status;
    APP_SHELL_T *papp = (APP_SHELL_T *)app;

    if(papp->isShow != show)
    {
        papp->isShow = show;
        
        status=APP_ChangeRoutingStack( app, ev_st,
                                (void *)(papp->isShow ? My_APP_HandleEvent : APP_HandleEventPrepost),
                                (papp->isShow ? 1 : 0),
                                (papp->isShow ? 0 : 1), 1, (papp->isShow ? 1 : 2));    
    }

    return status;
}


UINT32 appHide( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    dbgf("appHide begin...");
    APP_SHELL_T     *papp=(APP_SHELL_T*)app;
    
    if (papp->startPlayer) 
    { // плеер был запущен
        APP_UtilChangeState( SHELL_STATE_BACKGROUND, ev_st, app );
        return Show(ev_st, app, false); // в фон
    } 
    else
    {
        return shell_destroyApp(ev_st,app); // выход 
    }
}

UINT32 appShow( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    dbgf("appShow begin...");

    APP_ConsumeEv(ev_st,app);
    APP_UtilChangeState( SHELL_STATE_CANVAS, ev_st, app );
    return Show(ev_st, app, true);
}

/* Функция вызываемая при старте плеера */
UINT32 player_start( EVENT_STACK_T *ev_st, APPLICATION_T *app )
{
    dbgf("player_start begin...");

    APP_SHELL_T     *papp=(APP_SHELL_T*)app;
    WCHAR            SkinFolder[256];
    STATUS_VALUE_T   value;

    /* Проверяем, а вдруг уже запущено? */
    if(!papp->startPlayer) 
    {
        papp->startPlayer = true;

        u_strcpy(SkinFolder, TunesU);
        u_strcat(SkinFolder, L"Skins/");
        u_strcat(SkinFolder, Setting.Skin_name);
        u_strcat(SkinFolder, L"/");
        LoadSkin(SkinFolder); // грузим скин

        repaint(ev_st, app);

        // читаем оператора
    	UIS_GetCurrentStatusValue(NETWORK_STATUS,  &value);	
    	u_strcpy(papp->Operator, value.quoted_string);
    }

    Create(ev_st, app);
    return RESULT_OK;
}


/* Функция выкл. плеера */
UINT32 player_stop( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32 status = RESULT_OK;
    APP_SHELL_T *papp = (APP_SHELL_T *)app;

    dbgf( "Exit player");

    StopAnimationId();

    StopTimer( 0, app );
    StopTimer( 7, app );
    StopTimer( 8, app );
    StopTimer( 9, app );
    StopTimer( 10, app );
    StopTimer( 11, app );
    StopTimer( 12, app );
    Blink(0, false);
    Blink(1, false);
    blink=0;

    if(!Setting.Queue_save) 
    {
        free(tunes_list);
        countTunes = 0;
    }

    FreeMemSkin();
    if (papp->mme_media_file != 0) Delete(ev_st, app);
        
    ShowStr(papp->Operator, NETWORK_STATUS); // оператора в поле оператора
    ShowStr(NULL, DATE_STATUS);              // выводим дату в поле даты
    JavaStopMarquee(jsapi_marquee_text);     // Удаляем тект в яве
		
    papp->startPlayer = false;
    DL_AudPlayTone(1, 0xFF);                 // подаём тон при закытии плеера

    return status;
}


/* Обработчик события, EV_KEY_PRESS в данном случае */
UINT32 player_HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    EVENT_T *event = AFW_GetEv(ev_st);
    APP_SHELL_T *papp = (APP_SHELL_T*) app;

    dbgf("key_pressed = %d", event->data.key_pressed);

    if(event->data.key_pressed == KEY_HEADSET || event->data.key_pressed == KEY_VOICE)
    {
        if (!APP_MMC_Util_IsVoiceCall())
        {
            StartTimer( 1000,  1, 1,  app );
            APP_ConsumeEv( ev_st, app );
        }
    }

    if(Setting.Lock_kb && KeypadLock())
        return RESULT_OK;


	if(WorkingTable()) 
    {
		switch(event->data.key_pressed)
		{
            case KEY_RED: // выход
				return shell_destroyApp(ev_st, app);
    			break;
			
			case KEY_RIGHT: // следующая
				StartTimer(1000, 2, 1, app);
				timeRew = 1;
				APP_ConsumeEv(ev_st, app);
	       		break;
			
			case KEY_LEFT: // предидущая
				StartTimer(1000, 2, 1, app);
				timeRew = -1;
			    APP_ConsumeEv(ev_st, app);
			    break;

 
            case KEY_UP: // пауза/воспроизведение/следующая
                if (!APP_MMC_Util_IsVoiceCall())
                {
                    StartTimer(1000, 1, 1, app);
                    APP_ConsumeEv(ev_st, app);
                }
			    break; 

            case KEY_DOWN: // Shell
                if (!KeypadLock()) 
                {
                    APP_ConsumeEv(ev_st, app);
                    return appShow(ev_st, app);
                }
			    break;
			
            case KEY_VOL_UP: // громкость +
               	APP_ConsumeEv(ev_st, app);
                StartTimer(1000, 1, 1, app);
			    break; 
			
            case KEY_VOL_DOWN: // громкость -
                APP_ConsumeEv(ev_st, app);
                StartTimer(1000, 5, 1, app);
			    break;
		}
	}
	else // не на рабочем столе
	{
		switch(event->data.key_pressed)
		{
            case KEY_SMART: // смарт  - пауза/плей 
                APP_ConsumeEv(ev_st, app);

				switch(State_Player) 
                {
                    case PLAY :
						Pause(ev_st, app);
					break;

                    case STOP:
                    case PAUSE:
						Play(ev_st, app);
					break;
									
					case DELETE:
						Create(ev_st, app);
					break;
				}
				
                break;

        case KEY_VOL_UP: // громкость +
            if(JavaApp()) 
            {
                setVolumeMode(app, VOLUME_INCREMENT);
                papp->volume = getVolume();
                
                APP_ConsumeEv(ev_st, app);
            }
            else 
            {
                StartTimer(1000, 1, 1, app);
            }
			break; 
			
        case KEY_VOL_DOWN: // громкость -
            if (JavaApp()) 
            {
                setVolumeMode(app, VOLUME_DECREMENT);
                papp->volume = getVolume();
                
                APP_ConsumeEv(ev_st, app);
            }
            else
            {
                StartTimer(1000, 5, 1, app);
            }
			break;
		}
	}
    
    return RESULT_OK;
}

UINT32 player_HandleKeyrelease( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	EVENT_T       *event = AFW_GetEv(ev_st);
    APP_SHELL_T   *papp = (APP_SHELL_T*) app;

    dbgf("key_release = %d", event->data.key_pressed);

    if (event->data.key_pressed == KEY_HEADSET || event->data.key_pressed == KEY_VOICE)
    {
        StopTimer( 1, app );
                if (!APP_MMC_Util_IsVoiceCall())
                {
                    if (!papp->LongPress)
                    {
					   switch (State_Player)
                        {
                            case PLAY :
                                Pause(ev_st, app);
                            break;
                                    
                            case PAUSE:
                            case STOP:
                                Play(ev_st, app);
                            break;
                                    
                            case DELETE:
                                Create(ev_st, app);
                            break;
                        }
                    }
                            
                    papp->LongPress = false;
                                
                   APP_ConsumeEv( ev_st, app );	
                }
    }


    if (Setting.Lock_kb && KeypadLock())  return RESULT_OK;

	
	if (WorkingTable() )
	{
		switch (event->data.key_pressed) 
		{		
        case KEY_UP :
            StopTimer( 1, app );
                if (!APP_MMC_Util_IsVoiceCall())
                {
                        if (!papp->LongPress)
                        {
   
                            switch (State_Player)
                            {
                                case PLAY:
                                    Pause(ev_st, app);
                                break;
    
                                case STOP:
                                case PAUSE:
                                    Play(ev_st, app);
                                break;
                                
                                case DELETE:
                                    Create(ev_st, app);
                                break;
                            }
                        }
                        
                        papp->LongPress = FALSE;
    
                    APP_ConsumeEv( ev_st, app );	
                }

			break; 
			
			
            case KEY_RIGHT :
                StopTimer( 2, app );
                StopTimer( 6, app );
					if (!papp->LongPress) {
						Next(ev_st, app);
					} else {
                        State_Player = PLAY;
						Seek(app, timePosition*1000);
                        timeRew = 0;
					}
					
					papp->LongPress = false;
			break;
			
            case KEY_LEFT :
                StopTimer( 2, app );
                StopTimer( 6, app );
                    if (!papp->LongPress) {
						Back(ev_st, app);
					} else {
                        State_Player = PLAY;
						Seek(app, timePosition*1000);
                        timeRew = 0;
					}

					papp->LongPress = false;
			break;

            case KEY_VOL_UP: // громкость +
                StopTimer( 1, app );
                if (papp->LongPress) {
                    papp->LongPress = false;
                } else {
                    setVolumeMode(app, VOLUME_INCREMENT);
                    papp->volume = getVolume();

                    ShowVolume();
                    papp->showVolume = 0;

                    paint_clear_time(ev_st, app);
                    paint_volumebar(ev_st, app);

					APP_ConsumeEv( ev_st, app );
                }
			break;
			
            case KEY_VOL_DOWN: // громкость -
                StopTimer( 5, app );
				if (papp->LongPress) {
                    papp->LongPress = false;
                } else {
                    setVolumeMode(app, VOLUME_DECREMENT);
                    papp->volume = getVolume();

                    ShowVolume();
                    papp->showVolume = 0;

                    paint_clear_time(ev_st, app);
                    paint_volumebar(ev_st, app);

					APP_ConsumeEv( ev_st, app );
                }
			break;
		}
	}
	else
	{
		switch (event->data.key_pressed)
		{
            case KEY_SMART:
                APP_ConsumeEv( ev_st, app );
            break;

            case KEY_VOL_UP: // громкость +
                StopTimer( 1, app );
				papp->LongPress = false;
			break;
			
            case KEY_VOL_DOWN: // громкость -
                StopTimer( 5, app );
				papp->LongPress = false;
			break;

		}	
	}
    

	return RESULT_OK;
}

UINT32 StartTimer(UINT32 period, UINT32 id, UINT8 type, APPLICATION_T *app)
{
    dbgf("StartTimer begin (%d)...", id);
    UINT32 status;
    APP_SHELL_T *papp = (APP_SHELL_T*)app;
    IFACE_DATA_T  iface;

    StopTimer(id, app);
    iface.port = app->port;

    if(type == 0) status = DL_ClkStartTimer(&iface, period, id);
    else status = DL_ClkStartCyclicalTimer(&iface, period, id);

    papp->timer_handle[id] = iface.handle;

    dbgf("StartTimer end (%d)...", id);
    return status;
}

UINT32 StopTimer(UINT32 id, APPLICATION_T *app)
{
    dbgf("StopTimer begin (%d)...", id);
    UINT32 status;
    APP_SHELL_T *papp = (APP_SHELL_T *)app;
    IFACE_DATA_T  iface;

    if(papp->timer_handle[id] != NULL)
    {
        iface.port = app->port;
        iface.handle = papp->timer_handle[id];

        status = DL_ClkStopTimer(&iface);
        papp->timer_handle[id] = NULL;
    }

    dbgf("StopTimer end (%d)...", id);
    return status;
}

UINT32 player_Timer (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 t_id = GET_TIMER_ID(ev_st);
    UINT32 t_handle = AFW_GetEvSeqn(ev_st);
	APP_SHELL_T *papp = (APP_SHELL_T *)app;

    //если id таймера больше кол-ва таймеров и его хэдл не равен хандлу запущенного нами таймера, то не обрабатываем его
    if (t_id >= COUNT_TIMERS || t_handle != papp->timer_handle[t_id]) 
        return RESULT_OK;

    dbgf("player_Timer (%d) begin...", t_id);
    APP_ConsumeEv(ev_st, app);
	switch (t_id) 
	{
        case 0: // подсчёт времени проигрывания
            timePosition+=1;
            if (Setting.Groth_vol) 
            {
                // нарастание громкости
                if (getVolume() < papp->volume && timePosition < 9)
                {
                    dbgf("grothing volume = %d", getVolume());
                    setVolumeMode(app, VOLUME_INCREMENT);
                }
            }
            
            if (Setting.Fading_vol) 
            {
                // затухание громкости
                if (getVolume() > 1 && timeDuraction-timePosition < papp->volume+1) 
                {
                    dbgf("fading volume = %d", getVolume());
                    setVolumeMode(app, VOLUME_DECREMENT);
                }
            }
            
            if (papp->showVolume == 3 ) 
            {
                repaint(ev_st,app);
                papp->showVolume++;
            
            } 
            else if (papp->showVolume > 3) 
            {
                paint_clear_time(ev_st, app);
                paint_progressbar(ev_st, app);
                
                ShowTime();
            } 
            else 
            {
                papp->showVolume++;
            }
            
            ShowName();
            break;	
		
        case 1: // при длинном зажатии вверх, гарнитура долго, громкость вверх долго - след. трек
            StopTimer( 1, app );
			papp->LongPress = true;
			Next(ev_st, app);
            break;
		
		case 2: // при длинном зажатии вправо и влево - перемотка
			StopTimer( 2, app );
			papp->LongPress = true;
			Pause(ev_st, app);
            State_Player = REWIND;
            paint_stateicons(ev_st, app);
			StartTimer( 100, 6, 1,  app );
		    break;
		
		case 5: // громкость вниз - предидущий. трек
			StopTimer( 5, app );
			papp->LongPress = true;
			Back(ev_st, app);
		    break;

        case 6: // перемотка
            StopTimer( 0, app );
            if (timePosition%10 == 0) 
            {
                if (timeRew>0) 
                {
                    timeRew+=1; 
                } 
                else if (timeRew<0) 
                {
                    timeRew-=1;
                }
            }

            if ((INT32)(timePosition+timeRew) < 0) 
            {
                timePosition = 0;
            } 
            else if ((INT32)(timePosition+timeRew) > timeDuraction)
            {
                timePosition = timeDuraction;
            } 
            else 
            {
                timePosition += timeRew;
            }
            
            if ((timePosition % 5 == 0)) ShowTime();
            paint_clear_time(ev_st, app);
            paint_progressbar(ev_st, app);
            break;

        case 7: // время вспышки - боковые
            Blink(0, true);
            break;

        case 8: // время задержки
            Blink(0, false);
            break;

        case 9: // время вспышки - фонарик
            Blink(1, true);
            break;

        case 10: // время задержки
            Blink(1, false);
            break;

        case 11: // время вспышки - клава и дисплей
            Blink(2, true);
            break;

        case 12: // время задержки
            Blink(2, false);
            break;

        case 13:
            JavaStopMarquee(jsapi_marquee_text);
            StopTimer(13, app);
            break;

	}

    dbgf("player_Timer (%d) end...", t_id);
    return RESULT_OK;
}

UINT32 Blink(UINT8 type, BOOL on)
{
    dbgf("Blink begin...");
    if (on) 
    {
        DL_KeyUpdateKeypadBacklight( 1 );
        UIS_SetBacklightWithIntensity(255, random(5)+1);
    } 
    else 
    {
        DL_KeyUpdateKeypadBacklight(0);
        UIS_SetBacklightWithIntensity(255, 0);
        DAL_EnableDisplay(0);
    }

    dbgf("Blink end...");
    return RESULT_OK;
}

UINT32 Create( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    APP_SHELL_T     *papp = (APP_SHELL_T*)app;
    IFACE_DATA_T    if_data;

    dbgf("id = %d, tunes_list[id] = %x%x", id, tunes_list[id]);

    TunesTrackU[0] = 0;
    DL_FsGetURIFromID(tunes_list+id, TunesTrackU); // находим путь трека по индексу в базе
    if(TunesTrackU[0] == 0 && id < countTunes-1) 
    {
        DL_AudPlayTone(0, 0xFF); // сигналим о том, что нет трека
        return Next(ev_st,app);
    }

    if (Setting.Queue_save > 1) upd_id_file_queue(); 


    udbg("track name = ", TunesTrackU);

    if (papp->mme_media_file != 0) Delete(ev_st,app); // уже есть? тогда удаляем
    if_data.port = app->port;

    papp->mme_media_file = (VOID *)MME_GC_playback_create(&if_data, TunesTrackU, NULL, 0, NULL, 0, 0, NULL, NULL); // открываем файл
    dbgf("mme_media_file = 0x%x", papp->mme_media_file);
        
    // если всё хорошо, меняем состояние
    if (papp->mme_media_file != 0) 
    {
        State_Player = STOP;
        timePosition = 0;
    }
    
	return RESULT_OK;
}

UINT32 Delete(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    dbgf("Delete begin...");
	UINT32 Error = 0;
    APP_SHELL_T *papp = (APP_SHELL_T *)app;

	if (papp->mme_media_file != 0) 
	{
        Error |= MME_GC_playback_stop(papp->mme_media_file);    
        Error |= MME_GC_playback_close(papp->mme_media_file);
		papp->mme_media_file = (MME_GC_MEDIA_FILE *)NULL; // обнуляем id
		dbgf("Error_Delete = %d", Error);
	}
	
	// если всё хорошо, меняем состояние
	if (Error == 0) 
    {
         State_Player = DELETE;
         StopTimer(0, app);  // останов подсчёта времени 
         ShowiTunesState();
    }

    dbgf("Delete end...");
	return RESULT_OK;
}



UINT32 Play (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
	UINT32 Error=1;
    EVENT_T *event = AFW_GetEv(ev_st);
    APP_SHELL_T *papp = (APP_SHELL_T *)app;

    dbgf("Play begin...");
    if (event->code == EV_MME_OPEN_SUCCESS)
    {
        if (papp->mme_media_file == ((MME_OPEN_SUCCESS_T*)(event->attachment))->media_handle && papp->mme_media_file != NULL)
        {
            //нарастание громкости
            if (Setting.Groth_vol) 
                if (getVolume() > 0)
                    setVolume(1);

            Error = MME_GC_playback_start(papp->mme_media_file, 0);   // воспроизводим
            dbgf("Error_Play = %d", Error);

            if (JavaApp()) 
            {
                JavaStartMarquee((WCHAR *)SplitPath(TunesTrackU, L"/"));
                StartTimer(3000,13,0,app);
            }
        }
    } 
    else 
    {
        if(papp->mme_media_file != NULL) 
        {
            Error = MME_GC_playback_start(papp->mme_media_file, 0);   // воспроизводим
            dbgf( "Error_Play = %d", Error );
        }
    }

	// если всё хорошо, меняем состояние  и запускаем таймер 
	if (Error == 0) 
	{

        // старт анимации
        if (State_Player == PAUSE || State_Player == STOP) 
        {
           StartAnimationId( cfg_skin.pictures[Cfg_Animation].x, cfg_skin.pictures[Cfg_Animation].y,  cfg_skin.pictures[Cfg_Animation].anchor);
        }

        MME_GC_playback_get_attribute(papp->mme_media_file, DURATION, &timeDuraction);
		MME_GC_playback_get_attribute(papp->mme_media_file, POSITION, &timePosition);
        dbgf("timeDuration: %d, timePosition: %d", timeDuraction, timePosition);

        ReadTags(papp->mme_media_file); // прочитаем теги
		StartTimer(1000, 0, 1, app);

		State_Player = PLAY;

        ShowiTunesState();
        ShowName();
        ShowTime();

        repaint(ev_st, app);
	}

    dbgf("Play end...");
	return RESULT_OK;
}

UINT32 ReadTags(MME_GC_MEDIA_FILE  mme_media_file)
{
    MEDIA_FILE_INFO_T   file_info;
    UINT16              i=0, j=0;
    UINT8               SEPARATOR = false;
    WCHAR               *file_name;

    memset (&file_info, 0, sizeof(MEDIA_FILE_INFO_T));
      
    file_info.author.str_size = TAG_SIZE*sizeof(WCHAR);
    file_info.author.str = malloc(file_info.author.str_size);
    if (file_info.author.str == NULL) return RESULT_FAIL;
    file_info.author.str[0] = 0;
    
    file_info.title.str_size = TAG_SIZE*sizeof(WCHAR);
    file_info.title.str = malloc(file_info.title.str_size);
    if (file_info.title.str == NULL) return RESULT_FAIL;
    file_info.title.str[0] = 0;
    
    file_info.album.str_size = TAG_SIZE*sizeof(WCHAR);
    file_info.album.str = malloc(file_info.album.str_size);
    if (file_info.album.str == NULL) return RESULT_FAIL;
    file_info.album.str[0] = 0;

    if (Setting.Read_tags)
    MME_GC_playback_get_attribute(mme_media_file, FILE_INFO, &file_info);

    memset (&tag, 0, sizeof(TAGS_T));

    file_name = (WCHAR *)SplitPath(TunesTrackU, L"/");

        if (file_info.author.str[0] == 0 || file_info.title.str[0] == 0) {
    
            SEPARATOR = 0;
            for (i=0; i < u_strlen(file_name)-4; i++) {
                if (SEPARATOR == 0 && (file_name[i] < '0'+10 || file_name[i] == '-' || file_name[i] == ' ' || file_name[i] == '.' || file_name[i] == '_')) {
                    continue;
                } else if (SEPARATOR == 1 ) {
                    if (file_name[i] != '-') {
                        if (j+1 < TAG_SIZE){
                            tag.author[j] = file_name[i];
                            tag.author[j+1] = 0;
                            j++;
                        }
    
                    } else {
                        SEPARATOR = 2;
                        j = 0;
                    }
                } else if (SEPARATOR == 2 ) {
                    if (j+1 < TAG_SIZE){
                            tag.title[j] = file_name[i];
                            tag.title[j+1] = 0;
                            j++;
                    }
    
                } else {
                    SEPARATOR = 1;
                    tag.author[0] = file_name[i];
                    j = 1;
                }
            }

        } 

        if (file_info.author.str[0] != 0) u_strcpy(tag.author, file_info.author.str);
        if (file_info.title.str[0] != 0) u_strcpy(tag.title, file_info.title.str);
        u_strcpy(tag.album, file_info.album.str);
          
            free(file_info.author.str);
            free(file_info.title.str);
            free(file_info.album.str);

    return RESULT_OK;
}

UINT32 Stop(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    dbgf("Stop begin...");
	UINT32 Error=1;
    APP_SHELL_T *papp = (APP_SHELL_T *) app;

	if (papp->mme_media_file != 0) 
	{
		Error = MME_GC_playback_stop(papp->mme_media_file); // стоп
		dbgf( "Error_Stop = %d", Error );
	}
	
	// если всё хорошо, меняем состояние
	if (Error == 0)
    {
        State_Player = STOP;
        StopTimer(0, app);  // останов подсчёта времени 
        ShowiTunesState();
        JavaStopMarquee(jsapi_marquee_text);
        timePosition = 0;

        StopAnimationId();
        paint_stateicons(ev_st, app);
        paint_clear_time(ev_st,app);
        paint_progressbar(ev_st, app);
    }
	
    dbgf("Stop end...");
	return RESULT_OK;
}

UINT32 Pause(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    dbgf("Pause begin...");
	UINT32 Error=1;
    APP_SHELL_T *papp = ( APP_SHELL_T* ) app;

	if (papp->mme_media_file != 0) 
    {
		Error = MME_GC_playback_pause(papp->mme_media_file); // пауза
		dbgf( "Error_Pause = %d", Error );
	}
	
	// если всё хорошо, меняем состояние
	if (Error == 0) 
    {
        State_Player = PAUSE;
        StopTimer(0, app);  // останов подсчёта времени 
        ShowiTunesState();
        JavaStopMarquee(jsapi_marquee_text);

        StopAnimationId(); // останавливаем анимацию
        paint_stateicons(ev_st, app);
        paint_clear_time(ev_st,app);
        paint_progressbar(ev_st, app);
    }

    dbgf("Pause end...");
	return RESULT_OK;
}

UINT32 PlayComplete(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    APP_SHELL_T *papp = (APP_SHELL_T *)app;
    MME_PLAY_COMPLETE_T *attach = AFW_GetEv(ev_st)->attachment;

    dbgf("PlayComplete begin...");
    dbgf("Status = %d", attach->status);

    if(papp->mme_media_file == attach->media_handle && papp->mme_media_file != NULL) 
    {
        if(attach->status == 8)
        {
            // трек закончился
            if (Setting.Play_rep == 1)
            {
                Create(ev_st, app);
            }
            else
            {
                if (id < countTunes-1 ) 
                {
                    if (Setting.Auto_next != 0)
                        Next(ev_st, app);
                } 
                else 
                {
                    if(Setting.Play_rep == 0) 
                    {
                        Stop(ev_st, app);
                    } 
                    else 
                    {
                        if (Setting.Auto_next != 0)
                            Next(ev_st, app);
                    }
                }
            }
        } 
        else 
        {
            StopTimer(0, app);
           
            if(APP_MMC_Util_IsVoiceCall()) // нам звонят, ну или мы звоним
            {
                Pause(ev_st,app);          // ставим на паузу
                ShowiTunesState();
            }
            else
            {
                Seek(app, timePosition*1000);
            }
        }
    }

    dbgf("PlayComplete end...");
    return RESULT_OK;
}

UINT32 Seek (APPLICATION_T *app, UINT32 seektime)
{
	UINT32              Error=1;
    APP_SHELL_T         *papp = (APP_SHELL_T*)app;

	dbgf("Seek time = %d ms", seektime);
	Error = MME_GC_playback_seek(papp->mme_media_file, seektime);
	dbgf("Error_Seek = %d", Error);
	return RESULT_OK;
}

UINT32 SeekPlay(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
	UINT32              Error=1;
    APP_SHELL_T         *papp = ( APP_SHELL_T* ) app;

    dbgf("SeekPlay begin...");
	StopTimer(0, app);
	
	if (papp->mme_media_file != NULL && State_Player == PLAY) 
	{	
		Error = MME_GC_playback_start(papp->mme_media_file, 0);   // воспроизводим
		dbgf("Error_SeekPlay = %d", Error);
    }
	
	if (Error == 0) 
    {
        // старт анимации
         StartAnimationId( cfg_skin.pictures[Cfg_Animation].x, cfg_skin.pictures[Cfg_Animation].y,  cfg_skin.pictures[Cfg_Animation].anchor);
         StartTimer( 1000, 0, 1, app );
         ShowiTunesState();
         paint_stateicons(ev_st, app);
    }

    dbgf("SeekPlay end...");
	return RESULT_OK;
}

UINT32 Next( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    dbgf("Next track");

    if (id < countTunes-1) id++;
    else id = 0;
    
    Create(ev_st, app);  
	return RESULT_OK;
}


UINT32 Back( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    dbgf("Back track");

	if (id > 0) id--;
    else  id = countTunes - 1;

	Create(ev_st, app);
	return RESULT_OK;
}


//  воспроизведение файла
UINT32 PlayFile(WCHAR* filename, UINT8 type)
{
    dbgf("PlayFile begin...");
    UINT64          tunes;

    DL_FsGetIDFromURI(filename, &tunes);
    if(tunes == 0) return RESULT_FAIL;
    add_in_tunes_list(&tunes, 1, type);

    dbgf("PlayFile end...");	
	return RESULT_OK;
}

// воспроизвести папку
UINT32 PlayFolder(WCHAR* foldername, UINT8 type)
{
    FS_SEARCH_PARAMS_T		sp;
	FS_SEARCH_RESULT_T		hResult;
	FS_SEARCH_HANDLE_T		hSearch;

	UINT16		            count_files, count_res=1;
	UINT16			        i;

    WCHAR                   search_string[256];
    UINT64                  *tunes;

    if(u_strncmp(foldername, L"file:/", 6))	{
        u_strcpy(search_string, L"file:/");
        u_strcat(search_string, foldername);
    } else {
        u_strcpy(search_string, foldername);
    }
    	
	// определяем нужен ли сплеш
	if ( foldername[u_strlen(foldername)-1] != '/' ) {
		u_strcat(search_string, L"/");
	}
    u_strcat(search_string, L"\xFFFE*.mp3\xFFFE*.m4a");

	sp.flags = 0xD; 
	sp.attrib = 0;
	sp.mask = 0;

	DL_FsSSearch( sp, search_string, &hSearch, &count_files, 0 );
    dbgf("count files = %d", count_files);
    if (count_files == 0 ) return 0;

    tunes = malloc(count_files*sizeof(UINT64));
    if (tunes == NULL) return RESULT_FAIL;

  
	for ( i=0; i<count_files; i++ ) {
		DL_FsSearchResults( hSearch, i, &count_res, &hResult );
        DL_FsGetIDFromURI(hResult.name, tunes+i);
    }
	DL_FsSearchClose( hSearch );

    add_in_tunes_list(tunes, count_files, type);
    free(tunes);

	return RESULT_OK;
}


UINT32 PlayTPL(WCHAR* tplname, INT32 index, UINT8 type)
{
    FILE_HANDLE_T   f;
	UINT32          fSize;
    UINT64          *tunes=NULL;
    UINT32          readen;
    UINT32          i;
    UINT64          tune;

    f = DL_FsOpenFile(tplname, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID) return RESULT_FAIL;

    if (type == ADD_ONE_TRACK) { // прочитать только один трек
        DL_FsFSeekFile(f, sizeof(UINT64)*index, SEEK_WHENCE_SET);
        DL_FsReadFile(&tune, sizeof(UINT64), 1, f, &readen); // на ID файла отводится 8 байт
        DL_FsCloseFile(f);
        add_in_tunes_list(&tune, 1, type);
    } else {
        fSize = DL_FsGetFileSize(f);
        tunes = (UINT64*)malloc(fSize);
        if (tunes == NULL) return RESULT_FAIL;
     
        DL_FsReadFile( tunes, fSize, 1, f, &readen ); // на ID файла отводится 8 байт
        DL_FsCloseFile(f);
    
        if (index < 0) {
            add_in_tunes_list(tunes, fSize/sizeof(UINT64), type);
        }else {
        
            tune = tunes[index];
        
            add_in_tunes_list(tunes, fSize/sizeof(UINT64), type);
        
            if (type == ADD_NONE) {
                    for (i=0; i < countTunes; i++) {
                        if (tunes_list[i] == tune) break;
                    }
            
                    if (i >= countTunes) {
                        id=0;
                    } else {
                        id = i;
                    }
            }
        }
    
        free(tunes);
    }
            
    dbgf("countTunes = %d\n", countTunes);

    return RESULT_OK;
}



UINT32 PlayMVPL(WCHAR* tplname, INT32 index, UINT8 type)
{
    FILE_HANDLE_T   f;
	UINT32          fSize;
    UINT64          *tunes=NULL;
    UINT64          tune;
    UINT32          readen;
    UINT32          i, j=0, n=0;
    char            *bufR=NULL;
    WCHAR           uri[512];

    f = DL_FsOpenFile(tplname, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID) return RESULT_FAIL;

    fSize = DL_FsGetFileSize(f);
    bufR = malloc(fSize);
    if (bufR == NULL) return RESULT_FAIL;
 
    DL_FsReadFile( bufR, fSize, 1, f, &readen ); 
    DL_FsCloseFile(f);

    for (i=0; i < fSize; i++) if (bufR[i] == 0x0A) n++;
    tunes = malloc(n*sizeof(UINT64));
    if (tunes == NULL) 
    {
        free(bufR);
        return RESULT_FAIL;
    }
    n=0;

    for (i=0; i < fSize; i++){
        if (bufR[i] == 0x0A) {
            bufR[i] = 0;
            u_atou(bufR+j, uri);
            udbg("uri = ", uri);
            DL_FsGetIDFromURI(uri,  tunes+n);
            n++;
            j = i+1;
        }
    }


    free(bufR);

    if (index < 0) {
        add_in_tunes_list(tunes, n, type);
    }else {
    
        tune = tunes[index];
    
        add_in_tunes_list(tunes, n, type);
    
        if (type == ADD_NONE) {
                for (i=0; i < countTunes; i++) {
                    if (tunes_list[i] == tune) break;
                }
        
                if (i >= countTunes) {
                    id=0;
                } else {
                    id = i;
                }
        }
    }

    free(tunes);
        	
	dbgf("countTunes = %d\n", countTunes);

    return RESULT_OK;
}

UINT32 ReadPlayList() 
{
	UINT32          readen;
	FILE_HANDLE_T   f;
    UINT32          fSize;

	udbg("Tunes PLU = ", TunesPLU);

		f = DL_FsOpenFile(TunesPLU, FILE_READ_MODE, 0);
        if (f == FILE_HANDLE_INVALID ) {
            ENTRIES_NUM = 0;
            return RESULT_FAIL;
        }
        fSize = DL_FsGetFileSize(f);

        ENTRIES_NUM = fSize/sizeof(UINT64);

        free(tunes_playlist);
        tunes_playlist = malloc(fSize);
        if (tunes_playlist == NULL) return RESULT_FAIL;

        DL_FsReadFile( tunes_playlist, fSize, 1, f, &readen );
        DL_FsCloseFile(f);

    return RESULT_OK;
}

UINT32 DelItemPlayList(UINT32 item) // удаленние пункта из плейлиста
{
    UINT32          r, w;
	FILE_HANDLE_T   f;
	UINT32          fSize;

    UINT64          *tunes=NULL;
    UINT32          num;

    f = DL_FsOpenFile(TunesPLU, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID )  return RESULT_FAIL;

	fSize = DL_FsGetFileSize(f);
    num = fSize/sizeof(UINT64);

    tunes = malloc(fSize);
    if (tunes == NULL) return RESULT_FAIL;

    DL_FsReadFile( tunes, fSize, 1, f, &r );
    DL_FsCloseFile(f);

    DL_FsDeleteFile( TunesPLU,  0 );

    memcpy((UINT64*)(tunes+item), (UINT64*)(tunes+item+1), (num-(item+1))*sizeof(UINT64));

    f = DL_FsOpenFile(TunesPLU, FILE_WRITE_MODE, 0);
    if (f == FILE_HANDLE_INVALID )  return RESULT_FAIL;

    DL_FsWriteFile( tunes, (num-1)*sizeof(UINT64), 1, f, &w);
    DL_FsCloseFile(f);
    free(tunes);


    return RESULT_OK;
}




// читаем данные из конфига
UINT32 ReadSetting(void) 
{
	UINT32 readen;
	FILE_HANDLE_T f;
	
	// читаем данные из конфига
	if (DL_FsFFileExist(nameCfgFileU)) // проверяем есть ли файл
	{
		f = DL_FsOpenFile(nameCfgFileU, FILE_READ_MODE, 0);
		DL_FsReadFile( (void*)&Setting, sizeof(SETTING_T), 1, f, &readen );
        DL_FsCloseFile(f);

        dbgf("Read Setting", NULL);
    }
	
	return RESULT_OK;
}

// записываем в конфиг
UINT32 SaveSetting(void)
{
	UINT32 written;
	FILE_HANDLE_T f;

	
	// запись в файл
	f = DL_FsOpenFile(nameCfgFileU, FILE_WRITE_MODE, 0);
	DL_FsWriteFile( (void*)&Setting, sizeof(SETTING_T), 1, f, &written );
	DL_FsCloseFile(f);
    dbgf("Save Setting", NULL);
	return RESULT_OK;
}

// подключение/отключение гарнитуры
UINT32 HandsFree( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    static      BOOL hf_disconn=false;
    EVENT_T     *event = AFW_GetEv(ev_st );
    UINT8       id_dev = *((UINT8*)event->attachment);

    dbgf("device id = %d", id_dev);
    if (id_dev == 4 || id_dev == 5 || id_dev == 6 || id_dev == 28) { // определим id устройства
        if (event->code == EV_DEVICE_DETACH) {
            if (State_Player == PLAY) {
                Pause(ev_st, app);
                hf_disconn=true;
            }
        } else if (event->code == EV_DEVICE_ATTACH) {
            if (State_Player == PAUSE && hf_disconn) {
                Play(ev_st, app);
                hf_disconn=false;
            }
        }
    }
    return RESULT_OK;
}

// завершение звонка. снять с паузы
UINT32 DialFinish( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    if (State_Player == PAUSE && Setting.Play_call != 0) 
    {
        dbgf("Dial finish begin...");
        State_Player = PLAY;
        Seek(app, timePosition*1000);
        dbgf("Dial finish end...");
    }
    return RESULT_OK;
}
 
/* Функция вызываемая при старте приложения */
UINT32 shell_startApp( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_SHELL_T *app = NULL;
    UINT32      i, status = RESULT_OK;
    WCHAR       uri[256];

    if(AFW_InquireRoutingStackByRegId( reg_id ) == RESULT_OK ) 
    {
        AFW_CreateInternalQueuedEvAux(ldrFindEventHandlerTbl(shell_any_state_handlers, appShow), 100, NULL, NULL); 
        return RESULT_OK;
    }
    
    app = (APP_SHELL_T*)APP_InitAppData(My_APP_HandleEvent, sizeof(APP_SHELL_T), reg_id, 0, 1, 1, 1, 1, 0);
    if(!app)
	{
	    PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = &app->apt;

    u_strcpy(uri, TunesU);
    u_strcat(uri, L"Tunes.lng");
    status = ReadLang(uri);     // читаем языковые ресурсы
    status = InitResources();   // cоздаём ресурсы 
    
    u_strcpy(uri, TunesU);
    u_strcat(uri, L"Skins/");
    u_strcat(uri, Setting.Skin_name);
    u_strcat(uri, L"/icons/");
    if (!DL_FsDirExist(uri)) 
    {
        u_strcpy(uri, TunesU);
        u_strcat(uri, L"icons/");
    }
    
    dbgf("Load icons...");
    status = LoadIcons(uri);
    
    // инициализация app
    app->isShow = true;
    app->startPlayer = false;
    app->num_state=0;
    for (i=0; i < SHELL_STATE_MAX; i++) 
    {
        app->prev_state[i]=0;
        app->prev_item[i]=1;
    }
    app->item=1;
    memset(app->timer_handle, 0, COUNT_TIMERS * sizeof(UINT32));
    app->mme_media_file = NULL;
    app->showVolume = 3;
    app->volume = getVolume();
    app->vol_mute = 0;
    app->LongPress = false;
    memset(app->Operator, 0, 32*sizeof(WCHAR));
    
    status = APP_Start(ev_st, &app->apt, SHELL_STATE_INIT, shell_state_handling_table, appHide, app_name, 0);
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


UINT32 shell_destroyApp (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    APP_SHELL_T *papp = (APP_SHELL_T *)app;

    if(papp->startPlayer)
        player_stop(ev_st, app);

	RemoveResources();  // удалим ресурсы
    FreeMemIcons();     // выгрузим иконки
    CloseLang();        // закроем lang
	
	free(file_list);
    free(tunes_playlist);

    APP_UtilUISDialogDelete(&app->dialog);                // удалем диалог 
    UINT32 status = APP_ExitStateAndApp(ev_st, app, 0);   // Завершаем работу приложения
    
    if(Setting.ev_code == 0) ldrUnloadElf(elf);
    return status;
}

UINT32  ChangeState( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT8 new_state)
{
    UINT32          status=0;
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;

    papp->prev_state[papp->num_state] = papp->apt.state;
    papp->prev_item[papp->num_state] = papp->item; 

    dbgf("num_state = %d", papp->num_state);
    dbgf("prev_item = %d", papp->prev_item[papp->num_state]);
    dbgf("prev_state = %d", papp->prev_state[papp->num_state]);

    if (papp->prev_state[papp->num_state-1] != papp->prev_state[papp->num_state]) {
        papp->num_state++;
    }

    papp->item = 1;

    status = APP_UtilChangeState( new_state, ev_st, app );

    return status;
}

UINT32 StateMinus2( EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32          status=0;
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;


    if (papp->num_state > 0){
        papp->num_state-=2;
        papp->item = papp->prev_item[papp->num_state];

            dbgf("num_state = %d", papp->num_state);
            dbgf("prev_item = %d", papp->prev_item[papp->num_state]);
            dbgf("prev_state = %d", papp->prev_state[papp->num_state]);

        status = APP_UtilChangeState( papp->prev_state[papp->num_state] , ev_st, app );
    } else {
        papp->item=1;
        status = APP_UtilChangeState( SHELL_STATE_MAIN , ev_st, app );
    }

    return status;
}

UINT32 PrevState (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT32 status;
    APP_SHELL_T *papp = (APP_SHELL_T *)app;

    if (papp->num_state > 0)
    {
        papp->num_state--;
        papp->item = papp->prev_item[papp->num_state];

        dbgf("num_state = %d", papp->num_state);
        dbgf("prev_item = %d", papp->prev_item[papp->num_state]);
        dbgf("prev_state = %d", papp->prev_state[papp->num_state]);

        status = APP_UtilChangeState(papp->prev_state[papp->num_state], ev_st, app);
    }
    else 
    {
        papp->item = 1;
        status = APP_UtilChangeState(SHELL_STATE_MAIN , ev_st, app);
    }

    return status;
}

UINT32 UpdateList( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 sItem )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;

	if (sItem > 0 && ENTRIES_NUM > 0 ) { // обновляем лист
        dbgf("Update List", NULL);
            APP_UtilAddEvUISListChange( ev_st,
                                    app,
                                    0, // = 0
                                    sItem,
                                    ENTRIES_NUM,
                                    TRUE,
                                    2,          // = 2
                                    FBF_FREE,
                                    NULL,
                                    NULL );	

            UIS_HandleEvent( papp->dialog,  ev_st );
    } else { // перезаходим в стейт
        APP_UtilChangeState( papp->state,  ev_st,  app );
    }
			
	
	return RESULT_OK;
}

UINT32 ChangeListPosition( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 sItem )
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;

	if (sItem > 1) {
        APP_UtilAddEvChangeListPosition(ev_st, app, sItem, FBF_FREE, NULL, NULL); 
        UIS_HandleEvent( papp->apt.dialog,  ev_st );
    }
	
	
	return RESULT_OK;
}



/* Обработчик входа в main state */
UINT32 shell_MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
    ACTIONS_T				action_list; //Список действий
    WCHAR                   main_caption[32];
    

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;


    dbgf("MainStateEnter", NULL);


    u_strcpy(main_caption, L"Tunes v");
    u_ltou(VER_MAJOR, main_caption+u_strlen(main_caption));
    u_strcat(main_caption, L".");
    u_ltou(VER_MINOR, main_caption+u_strlen(main_caption));
    #ifdef DEBUG
    u_strcat(main_caption, L" debug");
    #endif
    u_strcat(main_caption, L" by om2804");

    
    DRM_SetResource( Resources[RES_LIST_CAPTION], main_caption, (u_strlen(main_caption)+1)*sizeof(WCHAR) );
    action_list.count = 0;
    ActionUpd(&action_list, EV_DONE, Resources[RES_ACTION4]);


        ENTRIES_NUM = 4;
        if (State_Player != 0 || tunes_list != NULL) ENTRIES_NUM++;
    
    	dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 &action_list,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
        papp->apt.dialog = dialog;


    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
        shell_SendListItems_Main(ev_st, app, 1, starting_num);
        
        ChangeListPosition(ev_st,app,papp->item);
        
    return RESULT_OK;
}


UINT32 shell_PlaylistsStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T             *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
	ACTIONS_T				action_list; //Список действий


	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

    u_strcpy(cur_folder, TunesU);
    u_strcat(cur_folder, L"Playlists/");
    
	if (!DL_FsDirExist( cur_folder ))
		DL_FsMkDir( cur_folder,  0 ); // создаем папку плейлистов
					
    FindPlaylists(cur_folder); // ищим плейлисты

       
    DRM_SetResource( Resources[RES_LIST_CAPTION], Lang.str[LNG_PLAYLISTS], (u_strlen(Lang.str[LNG_PLAYLISTS])+1)*sizeof(WCHAR) );
    action_list.count = 0;
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_any_state_handlers, Delete_Action ),Resources[RES_ACTION2]); 
	ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_playlists_state_handlers, NewList_Action ), Resources[RES_ACTION1]);
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_playlists_state_handlers, AutoCreatePL_Action ), Resources[RES_ACTION6]);
    ActionUpd(&action_list, EV_SELECT, Resources[RES_ACTION5]);
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_any_state_handlers, AddInQueue_Action ),Resources[RES_ACTION0]);
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_playlists_state_handlers, DeleteAll_Action ), Resources[RES_ACTION7]);


        

    	dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 &action_list,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
    
        papp->apt.dialog = dialog;

        UIS_SetCenterSelectAction (dialog, ldrFindEventHandlerTbl( shell_playlists_state_handlers, SelectItemPL_Action ));


    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	shell_SendListItems_Playlists(ev_st, app, 1, starting_num);
        ChangeListPosition(ev_st,app,papp->item);
    return RESULT_OK;
}

UINT32 shell_PlaylistsStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	APP_SHELL_T           *papp = (APP_SHELL_T*)app;

    if ( type == EXIT_STATE_SUSPEND ) return RESULT_OK;

    free(file_list);
	APP_UtilUISDialogDelete( &papp->apt.dialog );

	return RESULT_OK;
}

UINT32 shell_PlaylistStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T             *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
    WCHAR                   buf_str[64];
    ACTIONS_T				action_list; //Список действий


	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

	
     ReadPlayList();

     u_strcpy(buf_str, (WCHAR *)SplitPath( TunesPLU, L"/" ));
     buf_str[u_strlen(buf_str)-4] = 0;
     u_strcat(buf_str, L" / ");
     u_ltou(ENTRIES_NUM, buf_str+u_strlen(buf_str)); // кол-во треков в плейлисте

     DRM_SetResource( Resources[RES_LIST_CAPTION], buf_str, (u_strlen(buf_str)+1)*sizeof(WCHAR) );
     action_list.count = 0;
     ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_any_state_handlers, Delete_Action ), Resources[RES_ACTION2]);
     ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_any_state_handlers, AddInQueue_Action ), Resources[RES_ACTION0]);

        dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 &action_list,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
    
        papp->apt.dialog = dialog;


    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	shell_SendListItems_Playlist(ev_st, app, 1, starting_num);
        ChangeListPosition(ev_st,app,papp->item);
    return RESULT_OK;
}

UINT32 shell_PlaylistStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	APPLICATION_T           *papp = (APPLICATION_T*) app;

    if ( type == EXIT_STATE_SUSPEND )  return RESULT_OK;

    free(tunes_playlist);
    APP_UtilUISDialogDelete( &papp->dialog );

	return RESULT_OK;
}

UINT32 shell_FbrowserplayerStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
	ACTIONS_T				action_list; //Список действий


	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

        dbgf("Fbrowser for player", NULL);
            // текущая папка для файлбраузера
            u_strcpy(cur_folder, L"/b/mobile/audio/");
            if (!DL_FsDirExist(L"file://b/mobile/audio/")) {
                cur_folder[1] = 'c';
            } 
		FindFile( cur_folder , L"*");

        dbgf("Set Resource", NULL);

        DRM_SetResource( Resources[RES_LIST_CAPTION],  cur_folder , (u_strlen(cur_folder)+1)*sizeof(WCHAR) );
        action_list.count = 0;
        ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_fbrowserplayer_state_handlers, NewList_Action ), Resources[RES_ACTION1]);
        ActionUpd(&action_list, EV_SELECT, Resources[RES_ACTION5]);
        ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_any_state_handlers, AddInQueue_Action ), Resources[RES_ACTION0]);
        ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_fbrowserplayer_state_handlers, Bluetooth_Action ),  Resources[RES_ACTION8]);
        
        
        dbgf("Create dialog", NULL);

    	dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 &action_list,
    							 Resources[RES_LIST_CAPTION] );
      
        dbgf("Dialog = %d", dialog);
        if(dialog == NULL) return RESULT_FAIL;
    
        papp->apt.dialog = dialog;

        dbgf("Set Center Action", NULL);
        UIS_SetCenterSelectAction (dialog, ldrFindEventHandlerTbl( shell_fbrowserplayer_state_handlers, SelectItemFB_Action ));

    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	shell_SendListItems_Fbrowser(ev_st, app, 1, starting_num);
        dbgf("Change List Pos", NULL);
        ChangeListPosition(ev_st,app,papp->item);
        dbgf("Fbrowser ok!", NULL);

    return RESULT_OK;
}

UINT32 shell_FbrowserplayerStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	APPLICATION_T           *papp = (APPLICATION_T*) app;

    if ( type == EXIT_STATE_SUSPEND ) return RESULT_OK;

    free(file_list);
	APP_UtilUISDialogDelete( &papp->dialog );

	return RESULT_OK;
}

UINT32 shell_FbrowserplaylistStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
	ACTIONS_T				action_list; //Список действий


	if(type!=ENTER_STATE_ENTER) return RESULT_OK;
		

            // текущая папка для файлбраузера
            u_strcpy(cur_folder, L"/b/mobile/audio/");
            if (!DL_FsDirExist(L"file://b/mobile/audio/")) {
                cur_folder[1] = 'c';
            } 
		FindFile( cur_folder , L"*" );

        DRM_SetResource( Resources[RES_LIST_CAPTION],  cur_folder , (u_strlen(cur_folder)+1)*sizeof(WCHAR) );
        action_list.count = 0;
        ActionUpd(&action_list, EV_SELECT, Resources[RES_ACTION3]);


    	dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 &action_list,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
    
        papp->apt.dialog = dialog;

        UIS_SetCenterSelectAction (dialog, ldrFindEventHandlerTbl( shell_fbrowserplaylist_state_handlers, SelectItemFB_Action ));

    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	shell_SendListItems_Fbrowser(ev_st, app, 1, starting_num);
        //ChangeListPosition(ev_st,app,papp->item);
    return RESULT_OK;
}

UINT32 shell_FbrowserplaylistStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	APPLICATION_T           *papp = (APPLICATION_T*) app;

    if ( type == EXIT_STATE_SUSPEND ) return RESULT_OK;
      
    free(file_list);
	APP_UtilUISDialogDelete( &papp->dialog );

	return RESULT_OK;
}


UINT32 shell_SkinsStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num, i;
	

	if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;


    u_strcpy(cur_folder, TunesU);
    u_strcat(cur_folder, L"Skins/");

	if (!DL_FsDirExist( cur_folder ))
		DL_FsMkDir( cur_folder,  0 ); // создаем папку
					


	FindFile( cur_folder, L"*");

    for (i=1; i < ENTRIES_NUM; i++) {
        if (!u_strcmp(file_list[i].name, Setting.Skin_name)) {
            file_list[i].selected = true;
            break;
        }
    }

    DRM_SetResource( Resources[RES_LIST_CAPTION], Lang.str[LNG_SKINS], (u_strlen(Lang.str[LNG_SKINS])+1)*sizeof(WCHAR) );

        ENTRIES_NUM --;


    	dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 NULL,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
        papp->apt.dialog = dialog;

        
    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	shell_SendListItems_Skins(ev_st, app, 1, starting_num);
        //ChangeListPosition(ev_st,app,papp->item);
    return RESULT_OK;
}

UINT32 shell_SkinsStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
    if(type == EXIT_STATE_SUSPEND) return RESULT_OK;

    free(file_list);
	APP_UtilUISDialogDelete(&app->dialog);

	return RESULT_OK;
}

UINT32 shell_SettingStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    UINT32 starting_num;

	if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

    DRM_SetResource( Resources[RES_LIST_CAPTION],  Lang.str[LNG_SETTING] , (u_strlen(Lang.str[LNG_SETTING])+1)*sizeof(WCHAR) );
    ENTRIES_NUM = SETTING_COUNT_ITEMS;

    dialog = UIS_CreateList( &app->port,
    						 0,
    						 SETTING_COUNT_ITEMS,
    						 0,
    						 &starting_num,
    						 0,
    						 2,
    						 NULL,
    						 Resources[RES_LIST_CAPTION] );
      
    if(dialog == NULL) return RESULT_FAIL;    
    app->dialog = dialog;

    // Чтобы список заработал, ему нужно сразу передать starting_num элементов
    shell_SendListItems_Setting(ev_st, app, 1, starting_num);
    //ChangeListPosition(ev_st,app,sItem_num);
    return RESULT_OK;
}

UINT32 shell_EditStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    RESOURCE_ID             CaptionEditor;

    if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

    dbgf("EditStateEnter", NULL);

		CaptionEditor = Resources[RES_EDIT_CAPTION];

        dialog = UIS_CreateCharacterEditor( &papp->port,
                                            NULL,
                                            1,
                                            32,
                                            FALSE,
                                            NULL,            //&action_list,
                                            CaptionEditor
                                            );

        if(dialog == NULL) return RESULT_FAIL;

        papp->dialog = dialog;

        return RESULT_OK;
}


UINT32 shell_EditData( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32       status = 0;
    EVENT_T      *event = AFW_GetEv(ev_st);
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;

	
    if (event->attachment != 0)
    {
		u_strcpy(TunesPLnew, TunesU);
		u_strcat(TunesPLnew, L"Playlists/");
		u_strcat(TunesPLnew, event->attachment);
		u_strcat(TunesPLnew, L".tpl");
	}

    APP_ConsumeEv(ev_st, app);


    // откуда был вызван edit ?
    if (papp->prev_state[papp->num_state-1] == SHELL_STATE_QUEUE ) {
        AddQueuePL();
        status = PrevState( ev_st, app );
    } else {
        status = ChangeState( ev_st, app, SHELL_STATE_FBROWSERPLAYLIST );
    }

    
    return status;

}

UINT32 shell_EditOk( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
     UINT32                  status = 0;
     ADD_EVENT_DATA_T        *ev_data;
     APPLICATION_T           *papp = (APPLICATION_T*) app;

     AFW_AddEvEvD(ev_st, EV_REQUEST_DATA,ev_data);
	 UIS_HandleEvent( papp->dialog,  ev_st );

    return status;
}


UINT32 shell_CanvasStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    GRAPHIC_POINT_T         display_size;   

	if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

    dbgf("CanvasStateEnter");

    UIS_CanvasGetDisplaySize(&display_size);
    bufd.w = display_size.x + 1;
    bufd.h = display_size.y + 1;
    bufd.buf = NULL;
		
	dialog = UIS_CreateColorCanvas (&port, &bufd, !cfg_skin.setting.s); // создаём канвас
    if(dialog == NULL) return RESULT_FAIL;
    papp->dialog = dialog;

    // старт анимации
    if (State_Player == PLAY)
    {
        StartAnimationId(cfg_skin.pictures[Cfg_Animation].x, cfg_skin.pictures[Cfg_Animation].y,  cfg_skin.pictures[Cfg_Animation].anchor);
    }
    
    repaint(ev_st, app);
    return RESULT_OK;
}

UINT32 shell_CanvasStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
    dbgf("CanvasStateExit", NULL);

    if ( type != EXIT_STATE_EXIT )
        return RESULT_OK;

    StopAnimationId();
	APP_UtilUISDialogDelete(&app->dialog);
	
	return RESULT_OK;
}

UINT32 focus (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    dbgf("focus begin...");
    if(app->state == SHELL_STATE_CANVAS)
    {
        repaint(ev_st, app);
        StartAnimationId(cfg_skin.pictures[Cfg_Animation].x, cfg_skin.pictures[Cfg_Animation].y,  cfg_skin.pictures[Cfg_Animation].anchor);
    }

    dbgf("focus end...");    
	return RESULT_OK;
}

UINT32 nofocus(EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    StopAnimationId();
	return RESULT_OK;
}

UINT32 shell_InfoStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type)
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    SU_PORT_T             port = papp->apt.port;
    CONTENT_T             contents;
    CONTENT_T             con_name;
    CONTENT_T             con_file_size;
    CONTENT_T             con_length;
    CONTENT_T             con_bitrate;
    CONTENT_T             con_type;

    UINT32                file_size;
    AUDIO_FORMAT_T        audio_format;
    WCHAR                 time[8];


	if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

    dbgf("InfoStateEnter");

    file_size = DL_FsSGetFileSize(TunesTrackU, 0);
    MME_GC_playback_get_attribute(papp->mme_media_file, AUDIO_FORMAT, &audio_format);

    // имя 
    UIS_MakeContentFromString( "s0 q1N",  &con_name, LANG_NAME, TunesTrackU );
    // размер файла
    UIS_MakeContentFromString( "s0 i1 q2N",  &con_file_size, LANG_FILE_SIZE, file_size/1024, L"Kb" );
    // продолжительность
    sec2min(timeDuraction, time);
    UIS_MakeContentFromString( "s0 i1 q2q3 q4N",  &con_length, LANG_FILE_LENGTH, timeDuraction, L"sec [", time, L"min]" );
    // битрейт
    if (audio_format.audio_codec == WAV_CODEC) {
        UIS_MakeContentFromString( "s0 i1 q2 i3 q4 q5N",  &con_bitrate, LANG_BITRATE, audio_format.wav_bit_rate?8*audio_format.wav_bit_rate:4, L"Kbit/s", audio_format.sampling_freq/1000, L"KHz", audio_format.audio_mode?L"Stereo":L"Mono" );
    } else {
        UIS_MakeContentFromString( "s0 i1 q2 i3 q4 q5N",  &con_bitrate, LANG_BITRATE, audio_format.bit_rate, L"Kbit/s", audio_format.sampling_freq/1000, L"KHz", audio_format.audio_mode?L"Stereo":L"Mono" );
    }

    // тип
    if (audio_format.audio_codec == MP3_CODEC) {
        UIS_MakeContentFromString( "s0 q1N",  &con_type, LANG_TYPE2, L"MP3");
    } else if (audio_format.audio_codec == AAC_CODEC) {
        UIS_MakeContentFromString( "s0 g1N",  &con_type, LANG_TYPE2, L"AAC");
    } else if (audio_format.audio_codec == MIDI_CODEC) {
        UIS_MakeContentFromString( "s0 q1N",  &con_type, LANG_TYPE2, L"MIDI");
    } else if (audio_format.audio_codec == WAV_CODEC) {
        UIS_MakeContentFromString( "s0 q1N",  &con_type, LANG_TYPE2, L"WAV");
    } else if (audio_format.audio_codec == AMR_CODEC) {
        UIS_MakeContentFromString( "s0 q1N",  &con_type, LANG_TYPE2, L"AMR");
    } else {
        UIS_MakeContentFromString( "s0 q1N",  &con_type, LANG_TYPE2, L"None");
    }

    UIS_MakeContentFromString( "q0Nr1r2r3r4r5", &contents, L"Info", &con_name, &con_file_size, &con_length, &con_bitrate, &con_type);
    dialog = UIS_CreateViewer(&port, &contents, NULL);

    if(dialog == NULL) return RESULT_FAIL;
    app->dialog = dialog;

    return RESULT_OK;
}


UINT32 shell_QueueStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_SHELL_T             *papp = (APP_SHELL_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
    ACTIONS_T				action_list; //Список действий
    WCHAR                   buf_str[64];
	

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;
    action_list.count = 0;
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_queue_state_handlers, Randomize_Action ), Resources[RES_ACTION9]);
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_any_state_handlers, Delete_Action ), Resources[RES_ACTION2]);
    ActionAdd(&action_list, ldrFindEventHandlerTbl( shell_queue_state_handlers, NewList_Action ), Resources[RES_ACTION1]);


    ENTRIES_NUM = countTunes;

     u_strcpy(buf_str, Lang.str[LNG_QUEUE]);
     u_strcat(buf_str, L" / ");
     u_ltou(ENTRIES_NUM, buf_str+u_strlen(buf_str)); // кол-во треков в очереди

    DRM_SetResource( Resources[RES_LIST_CAPTION], buf_str, (u_strlen(buf_str)+1)*sizeof(WCHAR) );
    

    	dialog = UIS_CreateList( &port,
    							 0,
    							 ENTRIES_NUM,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 &action_list,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
        papp->apt.dialog = dialog;

        
    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	shell_SendListItems_Queue(ev_st, app, 1, starting_num);
        ChangeListPosition(ev_st,app, id+1);
    return RESULT_OK;
}



UINT32 shell_StateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
    if(type == EXIT_STATE_SUSPEND)
        return RESULT_OK;

    dbgf("StateExit");
	APP_UtilUISDialogDelete(&app->dialog);

	return RESULT_OK;
}

UINT32 shell_HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    APP_SHELL_T             *papp=(APP_SHELL_T*)app;
    UINT32                  status;

    // Вызываем дефолтный обработчик события
    status = APP_HandleUITokenGranted( ev_st, app );

    // Если всё хорошо, меняем текущий state
    if( (status == RESULT_OK) && (papp->apt.token_status == 2) )
    {
        if (papp->startPlayer)
        {
            status = APP_UtilChangeState( SHELL_STATE_CANVAS , ev_st, app );
        }
        else
        {
            APP_UtilChangeState( SHELL_STATE_BACKGROUND, ev_st, app );
            Show(ev_st, app, false); // в фон
            AFW_CreateInternalQueuedEvAux( ldrFindEventHandlerTbl(shell_background_state_handlers, appinit), 100, NULL, NULL ); 
            //status = APP_UtilChangeState( SHELL_STATE_MAIN , ev_st, app );
            //Show(ev_st, app, true);
        }
    }

    return status;
}

// вход в main
UINT32 appinit( EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    APP_ConsumeEv(ev_st,app);
    APP_UtilChangeState(SHELL_STATE_MAIN, ev_st, app);
	return Show(ev_st, app, true);
}

UINT32 FindFile( WCHAR *folder, WCHAR *filtr ) // поиск файлов
{
     FS_SEARCH_PARAMS_T		sp;
     FS_SEARCH_RESULT_T		hResult;
     FS_SEARCH_HANDLE_T		hSearch;
    
    
     UINT16		count_files=0, count_res=1;
     UINT32		status = RESULT_OK;
     INT32		i=0, j=0;
     UINT8       mode=0;
     WCHAR       volumes[12];
    
     WCHAR       search_string[272];


     if ( folder==NULL || !u_strcmp(folder, L"/")) {
             u_strcpy(search_string, L"/");
     } else {
         // определяем нужен ли file://
         if(u_strncmp(folder, L"file:/", 6))
         {
             u_strcpy(search_string, L"file:/");
             u_strcat(search_string, folder);
         } else {
             u_strcpy(search_string, folder);
         }
    
         // определяем нужен ли сплеш
         if ( folder[u_strlen(folder)-1] != '/' ) 
             u_strcat(search_string, L"/");
    
         u_strcat(search_string, L"\xFFFE");
         u_strcat(search_string, filtr);
     }

 ///// Особенности /////
 // 1) в корне диска `a` в hResult.name относительный путь!
 // 2) в списке может быть имя текущей папки. При этом в конце есть слэш. Последствия патча "удаление ненужных папок с тела"
 //////////////////////

    dbgf("Find files...", NULL);


    if (!u_strcmp(search_string, L"/")) { // диски

        DL_FsVolumeEnum(volumes);

        free(file_list);
        file_list = malloc(sizeof(FILEINFO)*4);
        if (file_list == NULL) return RESULT_FAIL;

        cur_folder[0] = '/';

        ENTRIES_NUM = 0;

        for (i=0; i < 4; i++) {
            file_list[i].name[0] = volumes[i*3+1];//'a' + i;
            file_list[i].name[1] = 0;
            file_list[i].ext = NULL;
            file_list[i].attrib = FS_ATTR_VOLUME;
            file_list[i].selected = false;

            ENTRIES_NUM++;

            if (volumes[i*3+2] == NULL)  break;
        }

        return RESULT_OK;
    }

	
	sp.flags = 0x1C; 
	sp.attrib = 0;
	sp.mask = 0;
	
	status = DL_FsSSearch( sp, search_string, &hSearch, &count_files, 0 );
    dbgf("Search status = %d", status);
	if ( status != RESULT_OK ) {
        DL_FsSearchClose( hSearch );
        return RESULT_FAIL;
    }

    dbgf("count_files = %d", count_files);
	
	ENTRIES_NUM = count_files+1;

	dbgf("Memory Fbrowser !", NULL);
	free(file_list);
    dbgf("Free file_list !", NULL);
    file_list = malloc(sizeof(FILEINFO)*(count_files+1));
    if (file_list == NULL) return RESULT_FAIL;
	

	u_strcpy(file_list[0].name, L"..");
	file_list[0].ext = NULL;
	file_list[0].attrib = FS_ATTR_DIRECTORY;
    file_list[0].selected = false;



    for ( i=0, j=1; j<=count_files; i++ ) {

            if (i >= count_files){
                if (mode > 0) {
                    break;
                } else {
                    i=-1;
                    mode=1;
                    continue;
                }
            }

		status = DL_FsSearchResults( hSearch, i, &count_res, &hResult );
		if ( status == RESULT_OK ) 
		{
            if ((mode==0) && !(hResult.attrib&FS_ATTR_DIRECTORY)) continue;
            if ((mode==1) && (hResult.attrib&FS_ATTR_DIRECTORY)) continue;

            // фиксим последствия  патча "удаление ненужных папок с тела"
            if(!u_strcmp(hResult.name+u_strlen(hResult.name)-1, L"/")) continue;
            	
			u_strcpy(file_list[j].name, (WCHAR *)SplitPath(hResult.name, L"/"));
			file_list[j].attrib = hResult.attrib;
            file_list[j].ext = (WCHAR *)SplitPath(file_list[j].name, L".");
            if (!u_strcmp(file_list[j].ext, file_list[j].name)) file_list[j].ext = NULL;
			file_list[j].selected = false;
		} 
        j++;
	}

	DL_FsSearchClose( hSearch ); 


    dbgf("Find files end", NULL);

	
	return RESULT_OK;
}


UINT32 FindPlaylists( WCHAR *folder ) // поиск плейлистов
{
  
     FS_SEARCH_PARAMS_T		sp;
     FS_SEARCH_RESULT_T		hResult;
     FS_SEARCH_HANDLE_T		hSearch;
    
    
     UINT16		count_files=0, count_res=1;
     UINT32		status = RESULT_OK;
     INT32		i=0, j=0;
    
     WCHAR       search_string[272];
     WCHAR       allTPL[272]; // полный список, включает все треки

     u_strcpy(allTPL, folder);

         // определяем нужен ли file://
         if(u_strncmp(folder, L"file:/", 6))
         {
             u_strcpy(search_string, L"file:/");
             u_strcat(search_string, folder);
             u_strcpy(allTPL, L"file:/");
             u_strcat(allTPL, folder);
         } else {
             u_strcpy(search_string, folder);
             u_strcpy(allTPL, folder);
         }
    
         // определяем нужен ли сплеш
         if ( folder[u_strlen(folder)-1] != '/' ) {
                 u_strcat(search_string, L"/");
                 u_strcat(allTPL, L"/");
         }

         u_strcat(allTPL, L"All.tpl");
         u_strcat(search_string, L"\xFFFE*.tpl");
         
         udbg("search_string = ", search_string);


 ///// Особенности /////
 // 1) в корне диска `a` в hResult.name относительный путь!
 // 2) в списке может быть имя текущей папки. При этом в конце есть слэш. Последствия патча "удаление ненужных папок с тела"
 //////////////////////

    dbgf("Find playlists...", NULL);
   
	sp.flags = 0x1C; 
	sp.attrib = 0;
	sp.mask = 0;
	
	status = DL_FsSSearch( sp, search_string, &hSearch, &count_files, 0 );
    dbgf("Search status = %d", status);
	if ( status != RESULT_OK || count_files == 0 ) {
        ENTRIES_NUM = 0;
        DL_FsSearchClose( hSearch );
        return RESULT_FAIL;
    }

    dbgf("count playlists = %d", count_files);
	dbgf("Memory Fbrowser !");

	free(file_list);
    file_list = malloc(sizeof(FILEINFO)*(count_files));
    if (file_list == NULL) 
    {
        ENTRIES_NUM = 0;
        return RESULT_FAIL;
    }

    
    if (DL_FsFFileExist(allTPL)) { // общий плейлист всегда вначале списка
        u_strcpy(file_list[0].name, L"All.tpl");
        file_list[0].ext = (WCHAR *)SplitPath(file_list[0].name, L".");;
        file_list[0].attrib = FS_ATTR_DEFAULT;
        file_list[0].selected = false;  
        j=1;
    } else {
        j=0;
    }
    

    for ( i=0; i < count_files; i++ ) {
		status = DL_FsSearchResults( hSearch, i, &count_res, &hResult );
		if ( status == RESULT_OK ) 
		{
            // фиксим последствия  патча "удаление ненужных папок с тела"
            if(!u_strcmp(hResult.name+u_strlen(hResult.name)-1, L"/")) continue;
            	
			u_strcpy(file_list[j].name, (WCHAR *)SplitPath(hResult.name, L"/"));
            
            // общий плейлист пропускаем, он будет первым
            if (!u_strcmp(L"All.tpl", file_list[j].name)) continue;
			file_list[j].attrib = hResult.attrib;
            file_list[j].ext = (WCHAR *)SplitPath(file_list[j].name, L".");
            if (!u_strcmp(file_list[j].ext, file_list[j].name)) file_list[j].ext = NULL;
			file_list[j].selected = false;
            j++;
		} 
        
	}
    ENTRIES_NUM = j;
    

	DL_FsSearchClose( hSearch ); 


    dbgf("Find playlists end", NULL);

	
	return RESULT_OK;
}

UINT32 shell_HandleListReq( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	APP_SHELL_T			*papp = (APP_SHELL_T*) app;
	EVENT_T				*event;
	UINT32				start, num;

	// Если у нашего приложения нет "фокуса", значит сообщение пришло не для нас
	if( !papp->apt.focused ) return RESULT_OK;

	// Извлекаем из ивента нужные нам данные
	event = AFW_GetEv( ev_st );

	start = event->data.list_items_req.begin_idx;
	num = event->data.list_items_req.count;

	// Мы поймали ивент, который предназначается только нам - значит, мы должны его убрать из списка
	APP_ConsumeEv( ev_st, app );
	switch (papp->apt.state) {
        case SHELL_STATE_MAIN:
            return shell_SendListItems_Main( ev_st, app, start, num );
            break;

        case SHELL_STATE_PLAYLISTS:
            return shell_SendListItems_Playlists( ev_st, app, start, num );
            break;

        case SHELL_STATE_PLAYLIST:
            return shell_SendListItems_Playlist( ev_st, app, start, num );
            break;

        case SHELL_STATE_FBROWSERPLAYLIST:
        case SHELL_STATE_FBROWSERPLAYER:
            return shell_SendListItems_Fbrowser( ev_st, app, start, num );
            break;

        case SHELL_STATE_SKINS:
            return shell_SendListItems_Skins( ev_st, app, start, num );
            break;

        case SHELL_STATE_SETTING:
            return shell_SendListItems_Setting( ev_st, app, start, num );
            break;

        case SHELL_STATE_QUEUE:
            return shell_SendListItems_Queue( ev_st, app, start, num );
            break;
     }

    return RESULT_OK;
}

UINT32 Navigate (EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    EVENT_T               *event = AFW_GetEv(ev_st);

	papp->item = event->data.index;
	return RESULT_OK;
}


UINT32 NewList_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) // создать плейлист
{
    APP_ConsumeEv( ev_st, app );

    ChangeState( ev_st, app, SHELL_STATE_EDIT );

	return RESULT_OK;
}

UINT32 Delete_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) // удалить плейлист
{
    APP_SHELL_T           *papp = (APP_SHELL_T*)app;
    UINT32                i;
	WCHAR                 TunesPL[128];


	APP_ConsumeEv( ev_st, app );

    switch (papp->apt.state ) {

    case SHELL_STATE_PLAYLISTS:
   
        u_strcpy(TunesPL, TunesU);
        u_strcat(TunesPL, L"Playlists/");
        u_strcpy(cur_folder, TunesPL);

        // проверка выделенных файлов
        for (i=0; i < ENTRIES_NUM; i++) 
            if (file_list[i].selected) break;

        if (i >= ENTRIES_NUM ) { // нет выделенных файлов
            u_strcat(TunesPL, file_list[papp->item-1].name);
            DL_FsDeleteFile( TunesPL,  0 ); // удаляем плейлист
        } else {
            // удаляем выделенные
            for (i=0; i < ENTRIES_NUM; i++) {
                if (file_list[i].selected) {
                    u_strcat(TunesPL, file_list[i].name);
                    DL_FsDeleteFile( TunesPL,  0 ); // удаляем плейлист
                    u_strcpy(TunesPL, cur_folder);
                }
            }
        }

        FindPlaylists( cur_folder );
       
        break;

    case SHELL_STATE_PLAYLIST:

        DelItemPlayList(papp->item-1);

        ReadPlayList(); // здесь можно прочитать только часть плейлиста
        break;

    case SHELL_STATE_QUEUE:
        del_item_tunes_list(papp->item-1);
        ENTRIES_NUM--;
        break;
    }

    UpdateList(ev_st, app, papp->item-1);


    
    return RESULT_OK;
}


UINT32 DeleteAll_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) // удалить все плейлисты
{

	WCHAR                   TunesPL[128];
    UINT16                  i;


	APP_ConsumeEv( ev_st, app );

        u_strcpy(TunesPL, TunesU);
        u_strcat(TunesPL, L"Playlists/");
        u_strcpy(cur_folder, TunesPL);


    for (i=0; i < ENTRIES_NUM; i++) {
        u_strcat(TunesPL, file_list[i].name);
        DL_FsDeleteFile( TunesPL,  0 ); // удаляем плейлист
        u_strcpy(TunesPL, cur_folder);
    }
    

    UpdateList(ev_st, app, 0);

	return RESULT_OK;
}


UINT32 AutoCreatePL_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    APP_SHELL_T             *papp = (APP_SHELL_T*)app;
    int i=0, n=0;

    UINT64 tunes;

	UINT32 written;
	FILE_HANDLE_T f;

    FS_SEARCH_RESULT_T *fs_result_folder=NULL;
    UINT16 res_count_folder=0;
	
	FS_SEARCH_PARAMS_T fs_param;
	FS_SEARCH_RESULT_T fs_result;    
	FS_SEARCH_HANDLE_T fs_handle;

	UINT16 res_count=0, count_f=1; 

    WCHAR PlayListU[128];

    // создание плейлистов по папкам

    // ищем папки
    WCHAR search_string[272] = L"file://b/mobile/audio/\xFFFE*";

    APP_ConsumeEv( ev_st, app );

    fs_param.flags = 0x1C; 
    fs_param.attrib = 0x0010;
    fs_param.mask = 0x0010; 

    DL_FsSSearch( fs_param, search_string, &fs_handle, &res_count_folder, 0x0);

    dbgf("res_count_folder = %d\n", res_count_folder);

    fs_result_folder = malloc(res_count_folder*sizeof(FS_SEARCH_RESULT_T));
    if (fs_result_folder == NULL) return RESULT_FAIL;

    DL_FsSearchResults(fs_handle, 0, &res_count_folder, fs_result_folder);

    for (n=0; n < res_count_folder; n++) {

        //поиск файлов в каждой папке

        u_strcpy(search_string, L"file:/");
        u_strcat(search_string, fs_result_folder[n].name);
        u_strcat(search_string, L"/\xFFFE*.mp3\xFFFE*.m4a");


        fs_param.flags = FS_SEARCH_RECURSIVE_AND_SORT_BY_NAME;
        fs_param.attrib = 0x0000;
        fs_param.mask = 0x0000; 

        DL_FsSSearch( fs_param, search_string, &fs_handle, &res_count, 0x0);   

        dbgf("res_count = %d\n", res_count);

        if (res_count == 0) continue;

        u_strcpy(PlayListU, TunesU); 
        u_strcat(PlayListU, L"Playlists/");
        DL_FsMkDir( PlayListU,  0 ); // создаем папку
        u_strcat(PlayListU, (WCHAR *)SplitPath(fs_result_folder[n].name, L"/"));
        u_strcat(PlayListU, L".tpl");

        udbg("PlayListU = ", PlayListU);

        f = DL_FsOpenFile(PlayListU, FILE_WRITE_MODE, 0);

        for (i=0; i < res_count; i++) {		
            DL_FsSearchResults(fs_handle, i, &count_f, &fs_result);

            DL_FsGetIDFromURI(fs_result.name, &tunes); // получение id
            DL_FsWriteFile(&tunes, sizeof(UINT64), 1, f, &written ); // запись в файл
        }

        DL_FsCloseFile(f);
        DL_FsSearchClose(fs_handle);
    }

    free(fs_result_folder);

   // создание общего плейлиста Tunes.tpl

        fs_param.flags = FS_SEARCH_RECURSIVE_AND_SORT_BY_TIME; 
        fs_param.attrib = 0x0000;
        fs_param.mask = 0x0000; 

        DL_FsSSearch( fs_param, L"file://b/mobile/audio/\xFFFE*.mp3\xFFFE*.m4a", &fs_handle, &res_count, 0x0);   

        dbgf("res_count = %d\n", res_count);

        u_strcpy(PlayListU, TunesU); 
        u_strcat(PlayListU, L"Playlists/All.tpl");

        udbg("PlayListU = ", PlayListU);

        f = DL_FsOpenFile(PlayListU, FILE_WRITE_MODE, 0);

        for (i=0; i < res_count; i++) {		
            DL_FsSearchResults(fs_handle, i, &count_f, &fs_result);

            DL_FsGetIDFromURI(fs_result.name, &tunes); // получение id
            DL_FsWriteFile(&tunes, sizeof(UINT64), 1, f, &written ); // запись в файл
        }

        DL_FsCloseFile(f);
        DL_FsSearchClose(fs_handle);

         FindPlaylists( cur_folder );

   
        if (papp->item > 1) {
            UpdateList(ev_st, app, papp->item-1);
        } else {
            UpdateList(ev_st, app, 1);
        }


    return RESULT_OK;
}


UINT32 Bluetooth_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) // передать по Bluetooth
{
	WCHAR uri[FS_MAX_URI_NAME_LENGTH + 1];
    APP_SHELL_T *papp = (APP_SHELL_T*)app;

	APP_ConsumeEv(ev_st, app);

    if (!(file_list[papp->item-1].attrib & FS_ATTR_DIRECTORY) && papp->item > 1) 
    {
        u_strcpy(uri, cur_folder);
        u_strcat(uri, file_list[papp->item-1].name);
        SendViaBT(uri);
    }

	return RESULT_OK;
}

UINT32 AddInQueue_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) 
{
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;
    WCHAR           uri[256];
    UINT32          status;

    APP_ConsumeEv( ev_st, app );

    switch (papp->apt.state) {
    case SHELL_STATE_PLAYLISTS:
                u_strcpy(uri, cur_folder);
                u_strcat(uri, file_list[papp->item-1].name);
                status=PlayTPL(uri, -1, ADD_QUEUE);
                break;

    case SHELL_STATE_PLAYLIST:
                status=PlayTPL(TunesPLU, papp->item-1, ADD_ONE_TRACK);
                break;


        case SHELL_STATE_FBROWSERPLAYER:
            // воспроизвести
            if (papp->item != 1)
            {
                     u_strcpy(uri, cur_folder);
                     u_strcat(uri, file_list[papp->item-1].name);
        
                     udbg("uri = ", uri);
        
                 if (file_list[papp->item-1].attrib & FS_ATTR_DIRECTORY) {
                     status=PlayFolder(uri, ADD_QUEUE);
                 } else {
                     if ( isAudio(file_list[papp->item-1].ext))
                     {
                         status=PlayFile(uri, ADD_QUEUE);
                     } else if ( CmpStr(file_list[papp->item-1].ext, L"tpl")) {
                         status=PlayTPL(uri, -1, ADD_QUEUE);
                     } else if ( CmpStr(file_list[papp->item-1].ext, L"mvpl")) {
                         status=PlayMVPL(uri, -1, ADD_QUEUE);
                     }
                 }
            }
            break;
    }
    
    
    if( !papp->startPlayer ) player_start(ev_st, app);   // Запустить немедленно Player


    return RESULT_OK;
}

UINT32 Randomize_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    rands(tunes_list, countTunes);


    UpdateList(ev_st,app, id+1);

    return RESULT_OK;
}

UINT32 SelectItemFB_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) 
{
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;
    
    UINT32          i;


	APP_ConsumeEv( ev_st, app );

    dbgf("Select item in FB", NULL);
    

    	    if (papp->item == 1 && u_strcmp(cur_folder, L"/"))
			{
                 dbgf("Level Up", NULL);
				// назад
				if (cur_folder[u_strlen(cur_folder)-1] == '/') cur_folder[u_strlen(cur_folder)-1] = 0;
					for (i=u_strlen(cur_folder)-1; i > 0 ; i--)
					{
						if (cur_folder[i] == '/'){
							break;
						} else {
							cur_folder[i] = 0;
						}
					}

					FindFile( cur_folder, L"*" );
	

                    DRM_SetResource( Resources[RES_LIST_CAPTION],  cur_folder , (u_strlen(cur_folder)+1)*sizeof(WCHAR) );
					UpdateList(ev_st, app, papp->prev_item[SHELL_STATE_MAX-1]);
                    papp->prev_item[SHELL_STATE_MAX-1] = 1;
                    UIS_Refresh();
			}
			else
				{
					if (file_list[papp->item-1].attrib & FS_ATTR_DIRECTORY || file_list[papp->item-1].attrib & FS_ATTR_VOLUME)
					{
                        dbgf("Open folder", NULL);
                        //  открыть папку
                        papp->prev_item[SHELL_STATE_MAX-1] = papp->item;
						u_strcat(cur_folder, file_list[papp->item-1].name);
						u_strcat(cur_folder, L"/");
                        udbg("cur_folder = ", cur_folder);
                        
						FindFile( cur_folder, L"*" );

                        DRM_SetResource( Resources[RES_LIST_CAPTION],  cur_folder , (u_strlen(cur_folder)+1)*sizeof(WCHAR) );
                        UpdateList(ev_st, app, 1);
                        UIS_Refresh();
					}
					else
					{
                        dbgf("Select file", NULL);
                        if (papp->apt.state == SHELL_STATE_FBROWSERPLAYER ) {

                            shell_SelectItem_Fbrowserplayer(ev_st, app);


                        } else {
    
                        // добавить файл
                        
                            if (!file_list[papp->item-1].selected)
                            {
                                    file_list[papp->item-1].selected = true;
                                    AddFilePL(file_list[papp->item-1].name);
                                
    
                                if (papp->item < ENTRIES_NUM)
                                {
                                    UpdateList(ev_st, app, papp->item+1);
                                }
                                else
                                {
                                    UpdateList(ev_st, app, 1);
                                }
    
                            }
                        }
                     
					}
				}

	
	return RESULT_OK;
}

UINT32 SelectItemPL_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) // открыть плейлист
{
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;

    APP_ConsumeEv( ev_st, app );

    u_strcpy(TunesPLU, cur_folder);
	u_strcat(TunesPLU, file_list[papp->item-1].name);

    ChangeState( ev_st, app, SHELL_STATE_PLAYLIST );

    return RESULT_OK;
}

UINT32 shell_SelectItem_Main(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 index = (AFW_GetEv(ev_st))->data.index;
    APP_SHELL_T     *papp=(APP_SHELL_T*)app;

    //APP_ConsumeEv( ev_st, app );

    switch (index)
	{
        case 1: //плейлисты
            ChangeState( ev_st, app, SHELL_STATE_PLAYLISTS );
        break;

        case 2: // файлбраузер
            ChangeState( ev_st,  app,  SHELL_STATE_FBROWSERPLAYER );
		break;

        case 3: // скины
            ChangeState( ev_st,  app,  SHELL_STATE_SKINS );
		break;

        case 4: // настройки
            ChangeState( ev_st,  app,  SHELL_STATE_SETTING );
		break;

        case 5: // текущая
            if (!papp->startPlayer) player_start(ev_st, app);
            ChangeState( ev_st, app, SHELL_STATE_CANVAS );
        break;
	}

    return RESULT_OK;
}

UINT32 shell_SelectItem_Playlists(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32  index = (AFW_GetEv(ev_st))->data.index;
    UINT32  status;

    u_strcpy(TunesPLU, cur_folder);
	u_strcat(TunesPLU, file_list[index-1].name);

    status=PlayTPL(TunesPLU, -1, ADD_NONE);
    if (status == RESULT_OK) {
        player_start(ev_st, app);   // Запустить немедленно Player
        //ChangeState( ev_st, app, SHELL_STATE_CANVAS );
    }

    return RESULT_OK;
}

UINT32 shell_SelectItem_Playlist(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32  index = (AFW_GetEv(ev_st))->data.index;
    UINT32  status;

    status=PlayTPL(TunesPLU, index-1, ADD_NONE);
    if (status == RESULT_OK) {
        player_start(ev_st, app);    // Запустить немедленно Player
        //ChangeState( ev_st, app, SHELL_STATE_CANVAS );
    }
 
    return RESULT_OK;
}

UINT32 shell_SelectItem_Fbrowserplayer(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32          status=RESULT_FAIL;
    APP_SHELL_T     *papp = (APP_SHELL_T*) app;
    UINT32          index = papp->item;
    WCHAR           uri[256];


    // воспроизвести
    if (index != 1)
    {
             u_strcpy(uri, cur_folder);
             u_strcat(uri, file_list[index-1].name);

             udbg("uri = ", uri);

         if (file_list[index-1].attrib & FS_ATTR_DIRECTORY || file_list[index-1].attrib & FS_ATTR_VOLUME) {
             status=PlayFolder(uri, ADD_NONE);
         } else {
             if ( isAudio(file_list[index-1].ext))
             {
                 status=PlayFile(uri, ADD_NONE);
             } else if (CmpStr(file_list[index-1].ext, L"tpl")) {
                 status=PlayTPL(uri, -1, ADD_NONE);
             } else if (CmpStr(file_list[index-1].ext, L"mvpl")) {
                 status=PlayMVPL(uri, -1, ADD_NONE);
             }
         }
    }
    
    if (status == RESULT_OK) {
         player_start(ev_st, app);   // Запустить немедленно Player
    }

    //ChangeState(ev_st,app,SHELL_STATE_CANVAS);
    
    return RESULT_OK;
}

UINT32 shell_SelectItem_Fbrowserplaylist(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 index = (AFW_GetEv(ev_st))->data.index;

    //APP_ConsumeEv( ev_st, app );

     if (index != 1)
     {

         if (file_list[index-1].attrib & FS_ATTR_DIRECTORY || file_list[index-1].attrib & FS_ATTR_VOLUME) {
             AddFolderPL(file_list[index-1].name);
         } else {
             AddFilePL(file_list[index-1].name);
         }
            if (!file_list[index-1].selected)
                file_list[index-1].selected = true;
    }

     UpdateList(ev_st,app,index);

    return RESULT_OK;
}

UINT32 shell_SelectItem_Skins(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 index = (AFW_GetEv(ev_st))->data.index;
    WCHAR      SkinFolder[128]; 
    UINT32     i;

    //APP_ConsumeEv( ev_st, app );

    // перезагрузка скина ;)
    u_strcpy(SkinFolder, TunesU);
    u_strcat(SkinFolder, L"Skins/");
    u_strcat(SkinFolder, file_list[index].name);
    u_strcat(SkinFolder, L"/");

    FreeMemSkin();
    ParseConfig(&cfg_skin, SkinFolder);
    LoadSkin(SkinFolder);

    u_strcpy(Setting.Skin_name, file_list[index].name);

    SaveSetting();

    for (i=1; i <= ENTRIES_NUM; i++ ) {
        file_list[i].selected = false;
    }
    file_list[index].selected = true;

    UpdateList(ev_st,app,index);

    return RESULT_OK;
}

UINT32 shell_SelectItem_Setting(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 index = (AFW_GetEv(ev_st))->data.index;

    //APP_ConsumeEv( ev_st, app );

    switch (index)
	{      
        case SETTING_AUTO_NEXT:
            Setting.Auto_next = !Setting.Auto_next;
        break;

        case SETTING_PLAY_CALL:
            Setting.Play_call = !Setting.Play_call;      
        break;

        case SETTING_PLAY_RND:
			Setting.Play_rnd = !Setting.Play_rnd;      
		break;

        case SETTING_PLAY_REP:
            if (Setting.Play_rep < 2){
                Setting.Play_rep++;      
            } else {
                Setting.Play_rep = 0;
            }
		break;

        case SETTING_SHOW_NAME:
			Setting.Show_name = !Setting.Show_name;      
		break;

        case SETTING_SHOW_TIME:
			Setting.Show_time = !Setting.Show_time;      
		break;

        case SETTING_LOCK_KB:
			Setting.Lock_kb = !Setting.Lock_kb;      
		break;

        case SETTING_READ_TAGS:
			Setting.Read_tags = !Setting.Read_tags;      
		break;

        case SETTING_FADING_VOL:
			Setting.Fading_vol = !Setting.Fading_vol;
		break;

        case SETTING_GROTH_VOL:
			Setting.Groth_vol = !Setting.Groth_vol;
		break;

        case SETTING_QUEUE_SAVE:
            if (Setting.Queue_save < 2){
                Setting.Queue_save++;      
            } else {
                Setting.Queue_save = 0;
            }
		break;

        case SETTING_ACTIVE_DISPLAY:
			Setting.Active_display = !Setting.Active_display;
		break;
	}

    SaveSetting();

    UpdateList(ev_st,app,index);

    return RESULT_OK;
}

UINT32 shell_SelectItem_Queue(EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT32 index = (AFW_GetEv(ev_st))->data.index;

    Delete(ev_st,app);
    id = index-1;
    Create(ev_st,app);

    PrevState(ev_st,app);
    //ChangeState(ev_st,app, SHELL_STATE_CANVAS); 

    return RESULT_OK;
}

UINT32 shell_SendListItems_Main( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, index, status=RESULT_OK;
    UINT32					img_res;

    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);


    for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM); i++, index++)
    {
        plist[index].editable = FALSE;			// Поле нередактируемо
        plist[index].content.static_entry.formatting = 1;
    }

    if (ICO_RES[ICO_NEXT]) img_res=ICO_RES[ICO_NEXT];
    else img_res=DRMRES_NEXT;

    UIS_MakeContentFromString( "q0Sp1", &(plist[0].content.static_entry.text), Lang.str[LNG_PLAYLISTS], img_res); // Плейлисты
    UIS_MakeContentFromString( "q0Sp1", &(plist[1].content.static_entry.text), Lang.str[LNG_FBROWSER], img_res); // Файлбраузер
    UIS_MakeContentFromString( "q0Sp1", &(plist[2].content.static_entry.text), Lang.str[LNG_SKINS], img_res); // Скины
    UIS_MakeContentFromString( "q0Sp1", &(plist[3].content.static_entry.text), Lang.str[LNG_SETTING], img_res); // Настройки

    if (State_Player != 0 || tunes_list != NULL) {
            UIS_MakeContentFromString( "q0Sp1",&(plist[4].content.static_entry.text), Lang.str[LNG_CURRENT_TRACK], img_res); // Текущая
    }


    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
    status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}
    

	free(plist);

	return status;
}

UINT32 shell_SendListItems_Playlists( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, index, img_res, img_select=NULL, status=RESULT_OK;
    
    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);

    if (ICO_RES[ICO_NEXT]) img_res=ICO_RES[ICO_NEXT];
    else img_res=DRMRES_NEXT;

            for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++)
			{
				plist[index].editable = FALSE;			// Поле нередактируемо
				plist[index].content.static_entry.formatting = 1;
                file_list[i-1].name[u_strlen(file_list[i-1].name)-4] = 0; // показываем без расширения

                if (file_list[i-1].selected){ // метка на файле
                        if (ICO_RES[ICO_SELECTED]) img_select = ICO_RES[ICO_SELECTED];
                        else img_select = DRMRES_SELECTED;
                } else {
                    img_select = NULL;
                }

                UIS_MakeContentFromString( "p2q0Sp1", &(plist[index].content.static_entry.text), file_list[i-1].name, img_res, img_select);
                
            }

    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	free(plist);

    // добавим расширение обратно
    for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++) {
        u_strcat(file_list[i-1].name, L".tpl");
    }

	return status;
}

UINT32 shell_SendListItems_Playlist( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, index, status=RESULT_OK;

    URI_LIST_T name[LIST_ITEMS_COUNT];

        
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
 	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);

            for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++)
			{
				plist[index].editable = FALSE;			// Поле нередактируемо
				plist[index].content.static_entry.formatting = 1;

                DL_FsGetURIFromID(tunes_playlist+i-1, name[index].uri);

                UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), SplitPath(name[index].uri, L"/"));
			}


    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	free(plist);

	return status;
}


UINT32 shell_SendListItems_Fbrowser( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, index, status=RESULT_OK;
	UINT32					img_res=NULL, img_select=NULL;
    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);

    for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++)
	{
				plist[index].editable = FALSE;			// Поле нередактируемо
				plist[index].content.static_entry.formatting = 1;

                img_select=NULL;
                img_res=NULL;

                   
                    if (file_list[i-1].attrib & FS_ATTR_VOLUME) 
                    {
                        if (ICO_RES[ICO_DISK]) img_res = ICO_RES[ICO_DISK];
                        else img_res = DRMRES_FOLDER;
                    }
                    else if (i==1) {
                        img_res = ICO_RES[ICO_UPWARDS];
                    }
                    else if (file_list[i-1].attrib & FS_ATTR_DIRECTORY) 
                    {
                        if (ICO_RES[ICO_DIR]) img_res = ICO_RES[ICO_DIR];
                        else img_res = DRMRES_FOLDER;
                    }
                    else if ( isAudio(file_list[i-1].ext) )
					{
                        if (ICO_RES[ICO_AUDIO]) img_res = ICO_RES[ICO_AUDIO];
                        else img_res = DRMRES_FILE_AUDIO;
					}
                    else 
                    {
                        img_res = ICO_RES[ICO_NONE];
                    }


                    if (file_list[i-1].selected){
                        if (ICO_RES[ICO_SELECTED]) img_select = ICO_RES[ICO_SELECTED];
                        else img_select = DRMRES_SELECTED;
                    }

                    UIS_MakeContentFromString( "p1q0Sp2", &(plist[index].content.static_entry.text), file_list[i-1].name, img_res, img_select);
	}


    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	free(plist);

	return status;
}

UINT32 shell_SendListItems_Skins( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, img_res, index, status=RESULT_OK;

    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);

    if (ICO_RES[ICO_SELECTED]) img_res=ICO_RES[ICO_SELECTED];
    else img_res=DRMRES_SELECTED;

            for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++)
			{
				plist[index].editable = FALSE;			// Поле нередактируемо
				plist[index].content.static_entry.formatting = 1;

                if (file_list[i].selected){
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), file_list[i].name, img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), file_list[i].name);
                }
				
			}

    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	free(plist);

	return status;
}

UINT32 shell_SendListItems_Setting( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, img_res, index, status=RESULT_OK;

    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); 
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);

    if (ICO_RES[ICO_SELECTED]) img_res=ICO_RES[ICO_SELECTED];
    else img_res=DRMRES_SELECTED;

    for( index=0, i=start; (i<start+num) && (i<=SETTING_COUNT_ITEMS+1); i++, index++)
	{
				plist[index].editable = FALSE;			// Поле нередактируемо
				plist[index].content.static_entry.formatting = 1;
            
            switch (i) {
            case SETTING_AUTO_NEXT:
                if ( Setting.Auto_next != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_AUTONEXT], img_res);
                } else  {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_AUTONEXT]);
                }
                break;

            case SETTING_PLAY_CALL:
                if ( Setting.Play_call != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_PLAY_AFTER_CALL], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_PLAY_AFTER_CALL]);
                }
                break;

            case SETTING_PLAY_RND:
                if ( Setting.Play_rnd != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_RANDOM], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_RANDOM]);
                }
                break;
    

            case SETTING_PLAY_REP:
                switch ( Setting.Play_rep) {
                    case 0:
                        UIS_MakeContentFromString( "q0Ss1", &(plist[index].content.static_entry.text), Lang.str[LNG_REPEAT], 0x010000E6);
                    break;
    
                    case 1:
                        UIS_MakeContentFromString( "q0Sq1", &(plist[index].content.static_entry.text), Lang.str[LNG_REPEAT], Lang.str[LNG_ONE]);
                    break;
    
                    default:
                        UIS_MakeContentFromString( "q0Ss1", &(plist[index].content.static_entry.text), Lang.str[LNG_REPEAT], 0x0100002B);
                    break;
                }
                break;

            case SETTING_SHOW_NAME:
                if ( Setting.Show_name != 0){
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_SHOW_NAME], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_SHOW_NAME]);
                }
                break;

            case SETTING_SHOW_TIME:
                if ( Setting.Show_time != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_SHOW_TIME], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_SHOW_TIME]);
                }
                break;

            case SETTING_LOCK_KB:
                if ( Setting.Lock_kb != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_LOCK_KB], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_LOCK_KB]);
                }
                break;

            case SETTING_READ_TAGS:
                if ( Setting.Read_tags != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_READ_TAGS], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_READ_TAGS]);
                }
                break;

            case SETTING_FADING_VOL:
                if ( Setting.Fading_vol != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_FADING_VOL], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_FADING_VOL]);
                }
                break;

            case SETTING_GROTH_VOL:
                if ( Setting.Groth_vol != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_GROTH_VOL], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_GROTH_VOL]);
                }
                break;
            

            case SETTING_QUEUE_SAVE:
                if ( Setting.Queue_save == 0) {
                     UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_QUEUE_SAVE]);
                } else if ( Setting.Queue_save == 1) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_QUEUE_SAVE], img_res);
                } else {
                    UIS_MakeContentFromString( "q0Sq1", &(plist[index].content.static_entry.text), Lang.str[LNG_QUEUE_SAVE], Lang.str[LNG_AFTER_REBOOT]);
                }
                break;

            case SETTING_ACTIVE_DISPLAY:
                if ( Setting.Active_display != 0) {
                    UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), Lang.str[LNG_ACTIVE_DISPLAY], img_res);
                } else {
                    UIS_MakeContentFromString( "q0", &(plist[index].content.static_entry.text), Lang.str[LNG_ACTIVE_DISPLAY]);
                }
                break;
         }


    }

    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	free(plist);

	return status;
}


UINT32 shell_SendListItems_Queue( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, img_res, index, status=RESULT_OK;

    URI_LIST_T name[LIST_ITEMS_COUNT];

    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;
    memclr(plist, sizeof(LIST_ENTRY_T)*num);

    if (ICO_RES[ICO_SELECTED]) img_res=ICO_RES[ICO_SELECTED];
    else img_res=DRMRES_SELECTED;

            for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++)
			{
				plist[index].editable = FALSE;			// Поле нередактируемо
				plist[index].content.static_entry.formatting = 1;

                DL_FsGetURIFromID(tunes_list+i-1, name[index].uri);

                UIS_MakeContentFromString( "q0Sp1", &(plist[index].content.static_entry.text), SplitPath(name[index].uri, L"/"), i==id+1?img_res:NULL);
			}

    // Добавляем в текущий список ивентов новый ивент, предназначенный для нашего списка
	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T)*num,
									   plist );
	if( status != RESULT_FAIL )
	{
		// Передаём наш список ивентов непосредственно диалогу для обработки
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	free(plist);

	return status;
}

/* В этой функции мы будем создавать все необходимые нам ресурсы */
UINT32 InitResources( )
{
    RES_ACTION_LIST_ITEM_T		action[10];
    UINT32						status;

	const WCHAR list_caption[] = L"Tunes";

    memclr(action, sizeof(RES_ACTION_LIST_ITEM_T)*10);

	/* Ресурс для заголовка списка */
    status = DRM_CreateResource( &Resources[RES_LIST_CAPTION], RES_TYPE_STRING, (void*)list_caption, (u_strlen((WCHAR*)list_caption)+1)*sizeof(WCHAR) );
	status |= DRM_CreateResource( &Resources[RES_EDIT_CAPTION], RES_TYPE_STRING, (void*)Lang.str[LNG_PLAYLIST_NAME], (u_strlen((WCHAR*)Lang.str[LNG_PLAYLIST_NAME])+1)*sizeof(WCHAR) );
    status |= DRM_CreateResource( &Resources[RES_ACTION6_CAPTION], RES_TYPE_STRING, (void*)Lang.str[LNG_AUTOCREATE_PLAYLISTS], (u_strlen(Lang.str[LNG_AUTOCREATE_PLAYLISTS])+1)*sizeof(WCHAR) );
    status |= DRM_CreateResource( &Resources[RES_ACTION1_CAPTION], RES_TYPE_STRING, (void*)Lang.str[LNG_CREATE_PLAYLIST], (u_strlen(Lang.str[LNG_CREATE_PLAYLIST])+1)*sizeof(WCHAR) );
    status |= DRM_CreateResource( &Resources[RES_ACTION0_CAPTION], RES_TYPE_STRING, (void*)Lang.str[LNG_ADD_IN_QUEUE], (u_strlen(Lang.str[LNG_ADD_IN_QUEUE])+1)*sizeof(WCHAR) );
    status |= DRM_CreateResource( &Resources[RES_ACTION9_CAPTION], RES_TYPE_STRING, (void*)Lang.str[LNG_RANDOMIZE], (u_strlen(Lang.str[LNG_RANDOMIZE])+1)*sizeof(WCHAR) );

    // Все нолики закомменчены для оптимизации

    /* Ресурс для действия */
    //action[0].softkey_label = 0;			
    action[0].list_label =  Resources[RES_ACTION0_CAPTION];    //  "Добавить в очередь"
    //action[0].softkey_priority = 0; 
    action[0].list_priority = 5;			// Приоритет в меню
		
	/* Ресурс для действия */
    //action[1].softkey_label = 0;	
    action[1].list_label =  Resources[RES_ACTION1_CAPTION];		// "Создать плейлист"
    //action[1].softkey_priority = 0; 
    action[1].list_priority = 2;			// Приоритет в меню

	/* Ресурс для действия */
    //action[2].softkey_label = 0;			//  не на софт-кнопке
    action[2].list_label = LANG_DELETE;		// "Удалить"
    //action[2].softkey_priority = 0;
    action[2].list_priority = 4;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

    /* Ресурс для действия */
    action[3].softkey_label = LANG_ADD;			//  на софт-кнопке
    //action[3].list_label = 0;		// "Добавить"
    action[3].softkey_priority = 1;
    //action[3].list_priority = 0;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

    /* Ресурс для действия */
    action[4].softkey_label = LANG_EXIT;			//  на софт-кнопке
    //action[4].list_label = 0;		// "Выход"
    action[4].softkey_priority = -1;
    //action[4].list_priority = 0;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

    /* Ресурс для действия */
    //action[5].softkey_label = LANG_PLAY;
    action[5].list_label = LANG_PLAY;			//  в меню
    //action[5].softkey_priority = 1;           // на софт кнопке
    action[5].list_priority = 6;

    //action[5].list_label = 0;		// "Воспр."
    
    //action[5].list_priority = 0;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

    /* Ресурс для действия */
    //action[6].softkey_label = 0;			
    action[6].list_label = Resources[RES_ACTION6_CAPTION];		// "Автосоздание плейлистов"
    //action[6].softkey_priority = 0;
    action[6].list_priority = 1;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

        /* Ресурс для действия */
    //action[7].softkey_label = 0;			
    action[7].list_label = LANG_DELETE_ALL;		// "Удалить всё"
    //action[7].softkey_priority = 0;
    action[7].list_priority = 3;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

        /* Ресурс для действия */
    //action[8].softkey_label = 0;		
    action[8].list_label = LANG_SEND;		// "Передать по блютуз"
    //action[8].softkey_priority = 0;
    action[8].list_priority = 1;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

        /* Ресурс для действия */
    //action[9].softkey_label = 0;			
    action[9].list_label = Resources[RES_ACTION9_CAPTION];		// "Перемешать"
    //action[9].softkey_priority = 0;
    action[9].list_priority = 5;			// Приоритет в меню. не 0, иначе может попасть на софткнопку

    status |= DRM_CreateResource( &Resources[RES_ACTION0], RES_TYPE_ACTION, (void*)&action[0], sizeof(RES_ACTION_LIST_ITEM_T));
	status |= DRM_CreateResource( &Resources[RES_ACTION1], RES_TYPE_ACTION, (void*)&action[1], sizeof(RES_ACTION_LIST_ITEM_T));
	status |= DRM_CreateResource( &Resources[RES_ACTION2], RES_TYPE_ACTION, (void*)&action[2], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION3], RES_TYPE_ACTION, (void*)&action[3], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION4], RES_TYPE_ACTION, (void*)&action[4], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION5], RES_TYPE_ACTION, (void*)&action[5], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION6], RES_TYPE_ACTION, (void*)&action[6], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION7], RES_TYPE_ACTION, (void*)&action[7], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION8], RES_TYPE_ACTION, (void*)&action[8], sizeof(RES_ACTION_LIST_ITEM_T));
    status |= DRM_CreateResource( &Resources[RES_ACTION9], RES_TYPE_ACTION, (void*)&action[9], sizeof(RES_ACTION_LIST_ITEM_T));

	return status;
}



/* В этой функции мы будем освобождать ресурсы */
UINT32 RemoveResources( )
{
	UINT32				status = RESULT_OK;
	UINT32				i;

	for(i=0;i<RES_MAX;i++)
		status |= DRM_ClearResource( Resources[i] );

	return status;
}

// добавляем очередь в плейлист
UINT32 AddQueuePL(void)
{
	UINT32	        written;
	FILE_HANDLE_T	f;
	
	
	f = DL_FsOpenFile(TunesPLnew, FILE_APPEND_MODE, 0);
	DL_FsWriteFile(tunes_list, countTunes*sizeof(UINT64), 1, f, &written );
	DL_FsCloseFile(f);
	
	return RESULT_OK;
} 

// добавляем файл в плейлист
UINT32 AddFilePL(WCHAR* filename)
{
	WCHAR	        file_name[256];
    UINT64          tunes;
	
	u_strcpy(file_name, cur_folder);
	u_strcat(file_name, filename);
	

    DL_FsGetIDFromURI(file_name, &tunes);
    file_write(TunesPLnew, FILE_APPEND_MODE,&tunes, sizeof(UINT64)); 

	return RESULT_OK;
}

// добавляем папку в плейлист
UINT32 AddFolderPL(WCHAR* foldername)
{
    BOOL                    f_c=false;

	UINT32	                written;
	FILE_HANDLE_T	        f;


    FS_SEARCH_PARAMS_T		sp;
	FS_SEARCH_RESULT_T		hResult;
	FS_SEARCH_HANDLE_T		hSearch;

	UINT16		            count_files, count_res=1;
	UINT16			        i;

    WCHAR                   search_string[256];
    UINT64                  tunes;

    if(u_strncmp(foldername, L"file:/", 6))	{
        u_strcpy(search_string, L"file:/");
        u_strcat(search_string, cur_folder);
    } else {
        u_strcpy(search_string, cur_folder);
    }
    u_strcat(search_string, foldername);
    	
	// определяем нужен ли сплеш
	if ( foldername[u_strlen(foldername)-1] != '/' ) {
		u_strcat(search_string, L"/");
	}
    u_strcat(search_string, L"\xFFFE*.mp3\xFFFE*.m4a");


	sp.flags = 0xD; 
	sp.attrib = 0;
	sp.mask = 0;

	DL_FsSSearch( sp, search_string, &hSearch, &count_files, 0 );
    if (count_files == 0 ) return 0;

    if (TunesPLnew[1] == 'c' || TunesPLnew[7] == 'c') f_c = true;
    else {
        f = DL_FsOpenFile(TunesPLnew, FILE_APPEND_MODE, 0);
        if (f == FILE_HANDLE_INVALID ) return 0;
    }
   
	
	for ( i=0; i<count_files; i++ ) {
		DL_FsSearchResults( hSearch, i, &count_res, &hResult );
        DL_FsGetIDFromURI(hResult.name, &tunes);
        if (f_c) file_write(TunesPLnew, FILE_APPEND_MODE, &tunes, sizeof(UINT64));
        else DL_FsWriteFile(&tunes, sizeof(UINT64), 1, f, &written );
    }
	DL_FsSearchClose( hSearch );
	if (!f_c) DL_FsCloseFile(f);

	return count_files;
}


UINT32 canvas_HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    EVENT_T     *event = AFW_GetEv(ev_st);
    APP_SHELL_T   *papp = (APP_SHELL_T*) app;


    KEY_RELEASE = false;
    if (KEY_PRESS) return RESULT_OK;
    KEY_PRESS = true;

    dbgf("canvas keypres %d", event->data.key_pressed);
  
    
	switch (event->data.key_pressed) {
    case KEY_RED:
        KEY_PRESS = false;
        return shell_destroyApp(ev_st,app);
        break;

    case KEY_SOFT_RIGHT:
        KEY_PRESS = false;
            return appHide( ev_st, app );
        break;

    case  KEY_SOFT_LEFT:
         KEY_PRESS = false;
            PrevState( ev_st, app );
        break;

    case  KEY_MENU:
        KEY_PRESS = false;
             ChangeState( ev_st, app, SHELL_STATE_SETTING );
        break;

    case KEY_5:
    case  KEY_CAM:
        KEY_PRESS = false;
             ChangeState( ev_st, app, SHELL_STATE_QUEUE );
        break;

    case  KEY_8:
        KEY_PRESS = false;
             ChangeState( ev_st, app, SHELL_STATE_INFO );
        break;

    case KEY_GREEN:
        KEY_PRESS = false;
            SendViaBT(TunesTrackU);
            break;

    case KEY_1:
        blink ^= 4;
        if (blink >> 2) {
                StartTimer( 500,  7, 1,  app );
                StartTimer( 400,  8, 1,  app );
        } else {
            Blink(0, false);
            StopTimer( 7, app );
            StopTimer( 8, app );
        }
        
        break;

    case KEY_2:
        blink ^= 2;
        if ((blink >> 1) & 1) {
                StartTimer( 650,  9, 1,  app );
                StartTimer( 300,  10, 1,  app );
        } else {
            Blink(1, false);
            StopTimer( 9, app );
            StopTimer( 10, app );
        }
        
        break;


    case KEY_3:
        blink ^= 1;
        if (blink & 1) {
                StartTimer( 550,  11, 1,  app );
                StartTimer( 450,  12, 1,  app );
        } else {
            Blink(2, true);
            StopTimer( 11, app );
            StopTimer( 12, app );
        }
        
        break;




        case KEY_DOWN:
	        StopTimer( 0, app );
	        Stop(ev_st, app);
        break;

        case KEY_6:
        case KEY_RIGHT:
				StartTimer( 1000,  2, 1, app );
				timeRew = 1;
			break;

            case KEY_4:
			case KEY_LEFT: 
				StartTimer( 1000,  2, 1, app );
				timeRew = -1;
			break;


            case KEY_VOICE:
            case KEY_HEADSET:
            case KEY_SMART:
            case KEY_UP: 
                if (!APP_MMC_Util_IsVoiceCall()) {
                    StartTimer( 1000,  1, 1,  app );
                    //APP_ConsumeEv( ev_st, app );
                }
			break; 

    case KEY_VOL_DOWN:
    case KEY_STAR:
        APP_ConsumeEv( ev_st, app );
         setVolumeMode(app, VOLUME_DECREMENT);
         papp->volume = getVolume();

                    ShowVolume();
                    papp->showVolume = 0;

                    //paint(ev_st,app);
                    //paint_stateicons(ev_st,app);
                    paint_clear_time(ev_st, app);
                    paint_volumebar(ev_st, app);
                    break;

    case KEY_VOL_UP:
    case KEY_POUND:
        APP_ConsumeEv( ev_st, app );
        setVolumeMode(app, VOLUME_INCREMENT);
        papp->volume = getVolume();

                    ShowVolume();
                    papp->showVolume = 0;
                    //paint(ev_st,app);
                    //paint_stateicons(ev_st,app);
                    paint_clear_time(ev_st, app);
                    paint_volumebar(ev_st, app);
                    break;

      case KEY_0:
        setVolumeMode(app, VOLUME_MUTE);
        papp->volume = getVolume();

        papp->showVolume = 0;
        paint(ev_st, app);
        paint_stateicons(ev_st, app);
        //paint_clear_time(ev_st, app);
        paint_volumebar(ev_st, app);

        break;
    }


    return RESULT_OK;
}

UINT32 canvas_HandleKeyrelease( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    EVENT_T       *event = AFW_GetEv(ev_st);
    APP_SHELL_T   *papp = (APP_SHELL_T*) app;

    KEY_PRESS = false;
    if (KEY_RELEASE) return RESULT_OK;
    KEY_RELEASE = true;

    dbgf("canvas keyrelease %d", event->data.key_pressed);
        
	switch (event->data.key_pressed) {
    case KEY_SMART:
    case KEY_UP:
    case KEY_HEADSET:
    case KEY_VOICE:
        StopTimer( 1, app );
                if (!APP_MMC_Util_IsVoiceCall())
                {
                    if (!papp->LongPress)
                    {
                       StopTimer( 0, app );

					   switch (State_Player)
                        {
                            case PLAY :
                                Pause(ev_st, app);
                            break;
                                    
                            case PAUSE:
                            case STOP:
                                Play(ev_st, app);
                            break;
                                    
                            case DELETE:
                                Create(ev_st, app);
                            break;
                        }
                    }
                            
                    papp->LongPress = false;
                }
                break;


            case KEY_6:
            case KEY_RIGHT :
                StopTimer( 2, app );
                StopTimer( 6, app );
					if (!papp->LongPress) {
						Next(ev_st, app);
					} else {
                        State_Player = PLAY;
						Seek(app, timePosition*1000);
                        timeRew = 0;
					}
					
					papp->LongPress = false;
			break;

            case KEY_4:
            case KEY_LEFT :
                StopTimer( 2, app );
                StopTimer( 6, app );
                    if (!papp->LongPress) {
						Back(ev_st, app);
					} else {
                        State_Player = PLAY;
						Seek(app, timePosition*1000);
                        timeRew = 0;
					}

					papp->LongPress = false;
			break;
    }

    return RESULT_OK;
}

UINT8 getVolume(void)
{
    UINT8 volume;
    DL_AudGetVolumeSetting(MULTIMEDIA, &volume);
    return volume;
}

void setVolume(UINT8 volume)
{
    DL_AudSetVolumeSetting(MULTIMEDIA, volume);
    dbgf("VolumeId = %d", MULTIMEDIA);
}

UINT32 setVolumeMode(APPLICATION_T *app, UINT8 type)
{
    APP_SHELL_T *papp = (APP_SHELL_T *)app;
    UINT8 volume;

    switch (type) 
    {

    case VOLUME_DECREMENT:
        volume = getVolume();
        dbgf("Volume = %d", volume);
        if (volume > 0) volume--;
        setVolume(volume);
        break;

    case VOLUME_INCREMENT:
        volume = getVolume();
        dbgf("Volume = %d", volume);
        if (volume < 7 ) volume++;
        setVolume(volume);
        break;

    case VOLUME_MUTE:
        if (papp->vol_mute != 0) {
            setVolume(papp->vol_mute);
            papp->vol_mute = 0;
        } else {
            papp->vol_mute = getVolume();
            setVolume(0);
        }
        break;
    }

    return RESULT_OK;

}


UINT32 paint_stateicons( EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT8 i=0, j=0;

    //if (((APPLICATION_T*)app)->state == SHELL_STATE_CANVAS && ((APPLICATION_T*)app)->focused)
    if (app->state == SHELL_STATE_CANVAS && app->focused)
    {
        dbgf("paint_stateicons begin...");
         // иконки статуса воспроизв.
        for (i=PIC_PREV, j=Cfg_Backward; i<=PIC_NEXT; i++, j++)
        {
            DrawImageId(i, cfg_skin.pictures[j].x, cfg_skin.pictures[j].y,  cfg_skin.pictures[j].anchor);
        }
    
        switch (State_Player)
        {
            case PLAY:
                DrawImageId( PIC_PLAY_USE, cfg_skin.pictures[Cfg_Play].x, cfg_skin.pictures[Cfg_Play].y,  cfg_skin.pictures[Cfg_Play].anchor);
                break;
            case PAUSE:
                DrawImageId( PIC_PAUSE_USE, cfg_skin.pictures[Cfg_Pause].x, cfg_skin.pictures[Cfg_Pause].y, cfg_skin.pictures[Cfg_Pause].anchor);
                break;
            case STOP:
                DrawImageId( PIC_STOP_USE, cfg_skin.pictures[Cfg_Stop].x, cfg_skin.pictures[Cfg_Stop].y,  cfg_skin.pictures[Cfg_Stop].anchor);
                break;
            case REWIND:
                if (timeRew > 0)
                {
                    DrawImageId( PIC_NEXT_USE, cfg_skin.pictures[Cfg_Forward].x, cfg_skin.pictures[Cfg_Forward].y,  cfg_skin.pictures[Cfg_Forward].anchor);
                }
                else if (timeRew < 0)
                {
                    DrawImageId( PIC_PREV_USE, cfg_skin.pictures[Cfg_Backward].x, cfg_skin.pictures[Cfg_Backward].y, cfg_skin.pictures[Cfg_Backward].anchor);
                }
                break;
        }

        dbgf("paint_stateicons end...");
    }

    return RESULT_OK;
}

UINT32 paint_progressbar (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT16 x;
    WCHAR  buf_str[16];
    UINT8  volume;

    //return RESULT_OK;

    //if (((APPLICATION_T*)app)->state == SHELL_STATE_CANVAS && ((APPLICATION_T*)app)->focused)
    if(app->state == SHELL_STATE_CANVAS && app->focused)
    {
        dbgf("paint_progressbar begin...");
        // очищаем поле прогрессбара
        SetFillColor(cfg_skin.objects[Cfg_Background].color);
        FillRect(cfg_skin.objects[Cfg_ProgressBar].x-1, cfg_skin.objects[Cfg_ProgressBar].y-1, cfg_skin.objects[Cfg_ProgressBar].w, cfg_skin.objects[Cfg_ProgressBar].h);

    
        dbgf("kontur");
        // контур прогрессбара
        SetForegroundColor(0x00000000);
        DrawRect(cfg_skin.objects[Cfg_ProgressBar].x-1, cfg_skin.objects[Cfg_ProgressBar].y-1, cfg_skin.objects[Cfg_ProgressBar].w, cfg_skin.objects[Cfg_ProgressBar].h);
    

        dbgf("progress bar");
        // прогресс бар
        x = ((cfg_skin.objects[Cfg_ProgressBar].w-1) * timePosition)/(timeDuraction);
        SetFillColor(cfg_skin.objects[Cfg_ProgressBar].color);
        FillRect(cfg_skin.objects[Cfg_ProgressBar].x, cfg_skin.objects[Cfg_ProgressBar].y, x, cfg_skin.objects[Cfg_ProgressBar].h-2);


        dbgf("time");
        // время 
        sec2min(timePosition, buf_str);
        udbg("Position in min = ", buf_str);
        SetForegroundColor(cfg_skin.strings[Cfg_TimeCurrent].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_TimeCurrent].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_TimeCurrent].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_TimeCurrent].font_style, dialog);
        DrawText(buf_str, cfg_skin.strings[Cfg_TimeCurrent].x, cfg_skin.strings[Cfg_TimeCurrent].y,  cfg_skin.strings[Cfg_TimeCurrent].anchor);
         
        sec2min(timeDuraction-timePosition, buf_str);
        SetForegroundColor(cfg_skin.strings[Cfg_TimeElapsed].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_TimeElapsed].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_TimeElapsed].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_TimeElapsed].font_style, dialog);
        DrawText(buf_str, cfg_skin.strings[Cfg_TimeElapsed].x, cfg_skin.strings[Cfg_TimeElapsed].y, cfg_skin.strings[Cfg_TimeElapsed].anchor);
    
        // продолжительность трека
        sec2min(timeDuraction, buf_str);
        SetForegroundColor(cfg_skin.strings[Cfg_Time].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_Time].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_Time].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_Time].font_style, dialog);
        DrawText(buf_str, cfg_skin.strings[Cfg_Time].x, cfg_skin.strings[Cfg_Time].y, cfg_skin.strings[Cfg_Time].anchor);

        // нарисуем прогрессбар, если не совпадает с волюмссбаром
        if(cfg_skin.objects[Cfg_VolumeBar].y != cfg_skin.objects[Cfg_ProgressBar].y)
        {
            volume = getVolume();

            // очищаем поле волюмбара
            SetFillColor(cfg_skin.objects[Cfg_Background].color);
            FillRect(cfg_skin.objects[Cfg_VolumeBar].x-1, cfg_skin.objects[Cfg_VolumeBar].y-1, cfg_skin.objects[Cfg_VolumeBar].w, cfg_skin.objects[Cfg_VolumeBar].h);
        
            // контур волюмбара
            SetForegroundColor(0x00000000);
            DrawRect(cfg_skin.objects[Cfg_VolumeBar].x-1, cfg_skin.objects[Cfg_VolumeBar].y-1, cfg_skin.objects[Cfg_VolumeBar].w, cfg_skin.objects[Cfg_VolumeBar].h);
        
            // волюмбар
            x = ((cfg_skin.objects[Cfg_VolumeBar].w-1) * volume)/7;
            SetFillColor(cfg_skin.objects[Cfg_VolumeBar].color);
            FillRect(cfg_skin.objects[Cfg_VolumeBar].x, cfg_skin.objects[Cfg_VolumeBar].y, x, cfg_skin.objects[Cfg_VolumeBar].h-2);
        }

        UIS_CanvasSetFontStyle(FONT_STYLE_PLAIN, dialog);

        dbgf("paint_progressbar end...");
    }

    return RESULT_OK;
}

UINT32 paint_volumebar( EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    UINT16  x;
    WCHAR   buf_str[8];
    UINT8   volume;
 
    //if(((APPLICATION_T*)app)->state == SHELL_STATE_CANVAS && ((APPLICATION_T*)app)->focused)
    if(app->state == SHELL_STATE_CANVAS && app->focused)
    {
        dbgf("paint_volumebar begin...");
        volume = getVolume();

        // очищаем поле волюмбара
        SetFillColor(cfg_skin.objects[Cfg_Background].color);
        FillRect(cfg_skin.objects[Cfg_VolumeBar].x-1, cfg_skin.objects[Cfg_VolumeBar].y-1, cfg_skin.objects[Cfg_VolumeBar].w, cfg_skin.objects[Cfg_VolumeBar].h);
    
        // контур волюмбара
        SetForegroundColor(0x00000000);
        DrawRect(cfg_skin.objects[Cfg_VolumeBar].x-1, cfg_skin.objects[Cfg_VolumeBar].y-1, cfg_skin.objects[Cfg_VolumeBar].w, cfg_skin.objects[Cfg_VolumeBar].h);
    
        // волюмбар
        x = ((cfg_skin.objects[Cfg_VolumeBar].w-1) * volume)/7;
        SetFillColor(cfg_skin.objects[Cfg_VolumeBar].color);
        FillRect(cfg_skin.objects[Cfg_VolumeBar].x, cfg_skin.objects[Cfg_VolumeBar].y, x, cfg_skin.objects[Cfg_VolumeBar].h-2);

        // громкость
        u_strcpy(buf_str, L"");
        u_ltou((volume*100)/7, buf_str+u_strlen(buf_str));
        u_strcat(buf_str, L" %");
        SetForegroundColor(cfg_skin.strings[Cfg_Time].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_Time].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_Time].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_Time].font_style, dialog);
        DrawText(buf_str, cfg_skin.strings[Cfg_Time].x, cfg_skin.strings[Cfg_Time].y, cfg_skin.strings[Cfg_Time].anchor );


        // нарисуем прогрессбар, если не совпадает с волюмссбаром
        if (cfg_skin.objects[Cfg_VolumeBar].y != cfg_skin.objects[Cfg_ProgressBar].y)
        {
            // очищаем поле прогрессбара
            SetFillColor(cfg_skin.objects[Cfg_Background].color);
            FillRect(cfg_skin.objects[Cfg_ProgressBar].x-1, cfg_skin.objects[Cfg_ProgressBar].y-1, cfg_skin.objects[Cfg_ProgressBar].w, cfg_skin.objects[Cfg_ProgressBar].h);
        
            // контур прогрессбара
            SetForegroundColor(0x00000000);
            DrawRect(cfg_skin.objects[Cfg_ProgressBar].x-1, cfg_skin.objects[Cfg_ProgressBar].y-1, cfg_skin.objects[Cfg_ProgressBar].w, cfg_skin.objects[Cfg_ProgressBar].h);
        
        
            // прогресс бар
            x = ((cfg_skin.objects[Cfg_ProgressBar].w-1) * timePosition)/(timeDuraction);
            SetFillColor(cfg_skin.objects[Cfg_ProgressBar].color);
            FillRect(cfg_skin.objects[Cfg_ProgressBar].x, cfg_skin.objects[Cfg_ProgressBar].y, x, cfg_skin.objects[Cfg_ProgressBar].h-2);

        }

        UIS_CanvasSetFontStyle(FONT_STYLE_PLAIN, dialog);

        dbgf("paint_volumebar end...");
    }


    return RESULT_OK;
}

UINT32 paint_clear_time( EVENT_STACK_T *ev_st,  APPLICATION_T *app)
{
    GRAPHIC_METRIC_T    string_size;

    //if (((APPLICATION_T*)app)->state == SHELL_STATE_CANVAS && ((APPLICATION_T*)app)->focused)
    if(app->state == SHELL_STATE_CANVAS && app->focused)
    {
        // очищаем поля времени
        dbgf("paint_clear_time begin...");

        string_size = GetStringSize(L"00:00", cfg_skin.strings[Cfg_Time].font_id, cfg_skin.strings[Cfg_Time].font_style);
        SetFillColor(cfg_skin.strings[Cfg_TimeCurrent].bcolor);
        FillRect(cfg_skin.strings[Cfg_TimeCurrent].x, cfg_skin.strings[Cfg_TimeCurrent].y, string_size.width+1, string_size.height);
        SetFillColor(cfg_skin.strings[Cfg_TimeElapsed].bcolor);
        FillRect(cfg_skin.strings[Cfg_TimeElapsed].x, cfg_skin.strings[Cfg_TimeElapsed].y, string_size.width+1, string_size.height);
        SetFillColor(cfg_skin.strings[Cfg_Time].bcolor);
        FillRect(cfg_skin.strings[Cfg_Time].x, cfg_skin.strings[Cfg_Time].y, string_size.width+1, string_size.height);

        dbgf("paint_clear_time end...");
    }
    return RESULT_OK;
}

UINT32 paint (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    UINT8               volume=0;
    UINT16              x;
    WCHAR               buf_str[64];
    GRAPHIC_POINT_T     img_size;
    GRAPHIC_METRIC_T    string_size;

    if (app->state == SHELL_STATE_CANVAS && app->focused == TRUE)
    {
        dbgf("paint begin...");
        // фон
        SetFillColor(cfg_skin.objects[Cfg_Background].color);
        FillRect(cfg_skin.objects[Cfg_Background].x, cfg_skin.objects[Cfg_Background].y, cfg_skin.objects[Cfg_Background].w, cfg_skin.objects[Cfg_Background].h);

        // софт кнопки 
        img_size = GetImageIdSize(PIC_SOFT_LINE);
        DrawImageId( PIC_SOFT_LINE, cfg_skin.pictures[Cfg_SoftPicture].x, cfg_skin.pictures[Cfg_SoftPicture].y,  cfg_skin.pictures[Cfg_SoftPicture].anchor);

        // текст на левой софт кнопке
        SetForegroundColor(cfg_skin.strings[Cfg_SoftLabel].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_SoftLabel].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_SoftLabel].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_SoftLabel].font_style, dialog);
        GetResourceString(buf_str, 0x01001927);
        DrawText(buf_str,  cfg_skin.strings[Cfg_SoftLabel].x, cfg_skin.strings[Cfg_SoftLabel].y, cfg_skin.strings[Cfg_SoftLabel].anchor);

        // текст на правой софт кнопке
        GetResourceString(buf_str, 0x010011AB);
        string_size = GetStringSize( buf_str, cfg_skin.strings[Cfg_SoftLabel].font_id, cfg_skin.strings[Cfg_SoftLabel].font_style );
        DrawText(buf_str, bufd.w - string_size.width - cfg_skin.strings[Cfg_SoftLabel].x, cfg_skin.strings[Cfg_SoftLabel].y, cfg_skin.strings[Cfg_SoftLabel].anchor);

        // титул
        DrawImageId( PIC_TITLE_LINE, cfg_skin.pictures[Cfg_TitlePicture].x, cfg_skin.pictures[Cfg_TitlePicture].y,  cfg_skin.pictures[Cfg_TitlePicture].anchor);
        
        // настройки
        if (Setting.Play_rnd != 0)
            DrawImageId( PIC_RND, cfg_skin.pictures[Cfg_Random].x, cfg_skin.pictures[Cfg_Random].y,  cfg_skin.pictures[Cfg_Random].anchor);

        if (Setting.Play_rep == 1) {
            DrawImageId( PIC_REP_ONE, cfg_skin.pictures[Cfg_RepetitionOne].x, cfg_skin.pictures[Cfg_RepetitionOne].y,  cfg_skin.pictures[Cfg_RepetitionOne].anchor);
        } else if (Setting.Play_rep == 2) {
            DrawImageId( PIC_REP_ALL, cfg_skin.pictures[Cfg_RepetitionAll].x, cfg_skin.pictures[Cfg_RepetitionAll].y,  cfg_skin.pictures[Cfg_RepetitionAll].anchor);
        }

        // mute
        if (getVolume() == 0) 
        {
            DrawImageId( PIC_MUTE, cfg_skin.pictures[Cfg_Mute].x, cfg_skin.pictures[Cfg_Mute].y,  cfg_skin.pictures[Cfg_Mute].anchor);
        }

        // id из countTunes
        u_ltou( id+1, buf_str );
        u_strcat(buf_str, L" ");
        GetResourceString(buf_str+u_strlen(buf_str), 0x010006F9);
        u_strcat(buf_str, L" ");
        u_ltou( countTunes,  buf_str+u_strlen(buf_str) );
        SetForegroundColor(cfg_skin.strings[Cfg_TrackNumber].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_TrackNumber].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_TrackNumber].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_TrackNumber].font_style, dialog);
        DrawText(buf_str, cfg_skin.strings[Cfg_TrackNumber].x, cfg_skin.strings[Cfg_TrackNumber].y, cfg_skin.strings[Cfg_TrackNumber].anchor);

        // имя плейлиста
        u_strcpy(buf_str, (WCHAR *)SplitPath( TunesPLU, L"/" ));
        buf_str[u_strlen(buf_str)-4] = 0;

        SetForegroundColor(cfg_skin.strings[Cfg_Playlist].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_Playlist].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_Playlist].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_Playlist].font_style, dialog);
        DrawText(buf_str, cfg_skin.strings[Cfg_Playlist].x, cfg_skin.strings[Cfg_Playlist].y, cfg_skin.strings[Cfg_Playlist].anchor);
    
        // теги
        SetForegroundColor(cfg_skin.strings[Cfg_Artist].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_Artist].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_Artist].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_Artist].font_style, dialog);
        DrawText(tag.author, cfg_skin.strings[Cfg_Artist].x, cfg_skin.strings[Cfg_Artist].y, cfg_skin.strings[Cfg_Artist].anchor);

        SetForegroundColor(cfg_skin.strings[Cfg_Title].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_Title].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_Title].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_Title].font_style, dialog);
        DrawText(tag.title, cfg_skin.strings[Cfg_Title].x, cfg_skin.strings[Cfg_Title].y, cfg_skin.strings[Cfg_Title].anchor);

        SetForegroundColor(cfg_skin.strings[Cfg_Album].fcolor);
        SetBackgroundColor(cfg_skin.strings[Cfg_Album].bcolor);
        UIS_CanvasSetFont(cfg_skin.strings[Cfg_Album].font_id, dialog);
        UIS_CanvasSetFontStyle(cfg_skin.strings[Cfg_Album].font_style, dialog);
        DrawText(tag.album, cfg_skin.strings[Cfg_Album].x, cfg_skin.strings[Cfg_Album].y, cfg_skin.strings[Cfg_Album].anchor);

        // нарисуем волюмбар, если не совпадает с прогрессбаром
        if (cfg_skin.objects[Cfg_VolumeBar].y != cfg_skin.objects[Cfg_ProgressBar].y)
        {
            volume = getVolume();
            // очищаем поле волюмбара
            SetFillColor(cfg_skin.objects[Cfg_Background].color);
            FillRect(cfg_skin.objects[Cfg_VolumeBar].x-1, cfg_skin.objects[Cfg_VolumeBar].y-1, cfg_skin.objects[Cfg_VolumeBar].w, cfg_skin.objects[Cfg_VolumeBar].h);
        
            // контур волюмбара
            SetForegroundColor(0x00000000);
            DrawRect(cfg_skin.objects[Cfg_VolumeBar].x-1, cfg_skin.objects[Cfg_VolumeBar].y-1, cfg_skin.objects[Cfg_VolumeBar].w, cfg_skin.objects[Cfg_VolumeBar].h);
        
            // волюмбар
            x = ((cfg_skin.objects[Cfg_VolumeBar].w-1) * volume)/7;
            SetFillColor(cfg_skin.objects[Cfg_VolumeBar].color);
            FillRect(cfg_skin.objects[Cfg_VolumeBar].x, cfg_skin.objects[Cfg_VolumeBar].y, x, cfg_skin.objects[Cfg_VolumeBar].h-2);
        }

        UIS_CanvasSetFontStyle(FONT_STYLE_PLAIN, dialog);
        dbgf("paint end...");
    }

    return RESULT_OK;
}


UINT32 repaint( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    dbgf("repaint begin...");
    paint(ev_st, app);
    paint_stateicons(ev_st, app);
    paint_progressbar(ev_st, app);

    dbgf("repaint end...");
    return RESULT_OK;
}


// активность дисплея
UINT32 ActiveDisplay (EVENT_STACK_T *ev_st, APPLICATION_T *app)
{
    if (!Setting.Active_display)  APP_ConsumeEv(ev_st, app);
    return RESULT_OK;
}

// секунды в минуты
UINT32 sec2min(UINT32 seconds, WCHAR *str)
{
    UINT32        t, r;

    for( r='0',t=seconds; t>599; t-=600,r++ ); // Считаем десятки
    str[0] = r;

    for( r='0'; t>59; t-=60,r++ ); // Считаем единицы
    str[1] = r; // Используем факт, что коды цифр идут последовательно
    str[2] = ':';

    for( r='0'; t>9; t-=10,r++ ); // Считаем десятки и остаток 

    // Пользуемся заранее известной длиной строки
    str[3] = r;
    str[4] = '0'+t; // Используем факт, что коды цифр идут последовательно
    str[5] = 0;

    return RESULT_OK;
}

UINT32 ShowiTunesState() // вывод иконки iTunes (воспроизв / пауза)
{
/*
    dbgf("ShowiTunesState begin...");

    STATUS_VALUE_T   status_data;

    if (State_Player == PLAY) 
    {
        status_data.integer = 1;
    } 
    else if (State_Player == PAUSE) 
    {
        status_data.integer = 2;
    } 
    else 
    {
        status_data.integer = 0;
    }

    UIS_SetStatus(ITUNES_STATE_STATUS, status_data);

    dbgf("ShowiTunesState end...");    
*/
	return RESULT_OK;
}

UINT32 ShowStr(WCHAR * string, UINT8 status) // вывод строки в поле даты
{
/*
    dbgf("ShowStr begin...");

	STATUS_VALUE_T   status_data;

    status_data.quoted_string = string;
    UIS_SetStatus(status, status_data);

    dbgf("ShowStr end...");    
*/
	return RESULT_OK;
}

UINT32 ShowName(void)
{
/*
    WCHAR name[TAG_SIZE*3];

    if (Setting.Show_name != 0 && WorkingTable()) 
    {
        dbgf("ShowName begin...");
        u_strcpy(name, tag.author);
        if (tag.title[0] && tag.author[0]) u_strcat(name, L" - ");
        if (tag.title[0]) u_strcat(name, tag.title);

        ShowStr(name, DATE_STATUS); // выводим в поле даты
        dbgf("ShowName end...");
    }
*/
    return RESULT_OK;
}


UINT32 ShowTime(void)
{
/*
    WCHAR bufT[16]; // буфер для строки времени

    if (Setting.Show_time != 0 && WorkingTable()) 
    {
        dbgf("ShowTime begin...");
        sec2min(timePosition, bufT);
        u_strcat(bufT, L" / ");
        sec2min(timeDuraction, bufT+u_strlen(bufT));
    
        ShowStr(bufT, NETWORK_STATUS); //выводим в поле оператора
        dbgf("ShowTime end...");
    }
*/
    return RESULT_OK;
}

UINT32 ShowVolume(void)
{
/*
    WCHAR str_volume[16];
    UINT8 volume;

    if (Setting.Show_time != 0 && WorkingTable()) 
    {
        dbgf("ShowVolume begin...");
        volume = getVolume();
        GetResourceString(str_volume, LANG_VOLUME);
        u_strcat(str_volume, L": ");
        u_ltou( (volume*100)/7 ,  str_volume+u_strlen(str_volume) );
        u_strcat(str_volume, L" %");
    
        ShowStr(str_volume, NETWORK_STATUS); //выводим в поле оператора
        dbgf("ShowVolume end...");
    }
*/
    return RESULT_OK;
}


