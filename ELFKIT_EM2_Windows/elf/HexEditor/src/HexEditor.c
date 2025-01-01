#include "HexEditor.h"
#include "Graphics.h"

WCHAR       file_open[256];
WCHAR       CFGFile[256];

INT32           offset=0;
UINT32          file_size=0;
UINT8           *buffer = NULL;
FILE_HANDLE_T   f=FILE_HANDLE_INVALID;

VIEW_MODES_T    view_mode=HEX_MODE;

UIS_DIALOG_T        dialog;
DRAWING_BUFFER_T	bufd; // буфер для рисования
GRAPHICAL_CONSTS	*g_consts = NULL;

UINT32          ENTRIES_NUM=0;		// кол-во пунктов
FILEINFO        *file_list=NULL; // файлы
WCHAR           cur_folder[256];  // текущая папка

WCHAR           find_text[256];

ldrElf *eapp = NULL; 

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked },
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { EV_GRANT_TOKEN,               HandleUITokenGranted },
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_INK_KEY_PRESS,             HandleKeypress },
    { STATE_HANDLERS_END,                     NULL },
};

const EVENT_HANDLER_ENTRY_T edit_state_handlers[] =
{
    { EV_DATA,                               	 EditData  },
    { EV_DONE,                       		       EditOk  },
	{ EV_CANCEL,                 				     Back  },
    { STATE_HANDLERS_END,                             NULL },
};

const EVENT_HANDLER_ENTRY_T menu_state_handlers[] =
{
    { EV_DIALOG_DONE,             	            destroyApp },
    { EV_REQUEST_LIST_ITEMS,       	        HandleListReq  },
	{ EV_SELECT, 		                       mSelectItem },

    { EV_DONE,                     	                  Back },
    { STATE_HANDLERS_END,           		      	  NULL },
};

const EVENT_HANDLER_ENTRY_T fbrowser_state_handlers[] =
{
    { EV_DIALOG_DONE,             	            destroyApp },
    { EV_REQUEST_LIST_ITEMS,       	        HandleListReq  },
	{ EV_SELECT, 		                      fbSelectItem },

    { EV_DONE,                     	                  Back },
    { STATE_HANDLERS_END,           		      	  NULL },
};

const EVENT_HANDLER_ENTRY_T find_state_handlers[] =
{
    { EV_DATA,                               	 FindData  },
    { EV_DONE,                       		       EditOk  },
	{ EV_CANCEL,                 				     Back  },
    { STATE_HANDLERS_END,                             NULL },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { HE_STATE_ANY,               // State
      NULL,                       // Обработчик входа в state
      NULL,                       // Обработчик выхода из state
      any_state_handlers          // Список обработчиков событий
    },

    { HE_STATE_INIT,
      NULL,
      NULL,
      init_state_handlers
    },
    
    { HE_STATE_MAIN,
      MainStateEnter,
      StateExit,
      main_state_handlers
    },

    { HE_STATE_EDIT,
      EditStateEnter,
      StateExit,
      edit_state_handlers
    },

    { HE_STATE_MENU,
      MenuStateEnter,
      StateExit,
      menu_state_handlers
    },

    { HE_STATE_FBROWSER,
      FbrowserStateEnter,
      FbrowserStateExit,
      fbrowser_state_handlers
    },

    { HE_STATE_FIND,
      FindStateEnter,
      StateExit,
      find_state_handlers
    }
};

UINT32 ELF_Entry (ldrElf *ela, WCHAR *Params)
{
	UINT32 status = RESULT_OK;

	eapp = ela;
	eapp->name = (char *)app_name;

	if(ldrIsLoaded((char *)app_name))
	{
        	PFprintf("%s: Application already loaded!\n", app_name);
        	return RESULT_FAIL;
	}

	u_strcpy(CFGFile, L"file:/\0");
    	DL_FsGetURIFromID(&eapp->id, (CFGFile + 6));
	WCHAR *ptr = CFGFile + u_strlen(CFGFile);
	while(*ptr != L'/') ptr--;
	u_strcpy(ptr + 1, L"hexeditor.cfg\0");

	ReadSetting(CFGFile);	
	u_strcpy(file_open, L"/");
	
	OpenFile(file_open);
	LoadBuffer(0);
    

	status = APP_Register(&eapp->evbase, 1, state_handling_table, HE_STATE_MAX, (void*)startApp);
	if(status == RESULT_OK)
	{
        	PFprintf("%s: Application has been registered successfully!\n", app_name);

        	//Run application immediatly
        	ldrSendEvent(eapp->evbase);
	}
	else
	{
		PFprintf("%s: Can't register application!\n", app_name);
	}


	return status;
}


UINT32 startApp( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )//void *reg_hdl )
{
	APP_HEXEDITOR_T *app = NULL;
	UINT32 status = RESULT_OK;

	if(AFW_InquireRoutingStackByRegId(reg_id) == RESULT_OK)
	{
		PFprintf("%s: Application already started!\n", app_name);
		return RESULT_OK;
	}

	app = (APP_HEXEDITOR_T*)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_HEXEDITOR_T), reg_id, 0, 1, 1, 1, 1, 0);
	if(!app)
	{
	   	PFprintf("%s: Can't initialize application data\n", app_name);
		ldrUnloadElf(eapp);
		return RESULT_OK;
	}
	eapp->app = &app->apt;


	find_text[0] = 0;
	InitResources();


	status = APP_Start(ev_st, &app->apt, HE_STATE_INIT, state_handling_table, destroyApp, app_name, 0);
	if(status == RESULT_OK)
	{
		PFprintf("%s: Application has been started successfully!\n", app_name);
	}
	else
	{
		PFprintf("%s: Can't start application!\n", app_name);
		RemoveResources();
		APP_HandleFailedAppStart(ev_st, (APPLICATION_T*)app, 0);
		ldrUnloadElf(eapp);
	}

	return RESULT_OK;
}


/* Функция выхода из приложения */
UINT32 destroyApp( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    //APP_UtilStopTimer( app );

    // закроем файл, если был открыт

    DL_FsCloseFile( f);
    free(buffer);
    RemoveResources();
    APP_UtilUISDialogDelete(  &papp->dialog ); // удалем диалог

    /* Завершаем работу приложения */
    status = APP_Exit( ev_st, app, 0 );

    /* Выгружаем эльф */
    ldrUnloadElf(eapp);

    return status;
}

// читаем данные из конфига
UINT32 ReadSetting(WCHAR *cfg_file) 
{
	UINT32 readen;
	FILE_HANDLE_T fh;

	
	// читаем данные из конфига
	fh = DL_FsOpenFile(cfg_file, FILE_READ_MODE, 0);
    if (fh == FILE_HANDLE_INVALID) return RESULT_FAIL;
    DL_FsReadFile( file_open, 256*sizeof(WCHAR), 1, fh, &readen );
    DL_FsCloseFile(fh);

	return RESULT_OK;
}

// записываем в конфиг
UINT32 SaveSetting(WCHAR *cfg_file)
{
	UINT32 written;
	FILE_HANDLE_T fh;

	// запись в файл
	fh = DL_FsOpenFile(cfg_file, FILE_WRITE_MODE, 0);
    if (fh == FILE_HANDLE_INVALID) return RESULT_FAIL;
    DL_FsWriteFile( file_open, 256*sizeof(WCHAR), 1, fh, &written );
	DL_FsCloseFile(fh);

	return RESULT_OK;
}

/* В этой функции мы будем создавать все необходимые нам ресурсы */
UINT32 InitResources( )
{
    UINT32						status;

	const WCHAR list_caption[] = L"/";
    const WCHAR edit_caption[] = L"HEX";


	/* Ресурс для заголовка списка */
    status = DRM_CreateResource( &Resources[RES_LIST_CAPTION], RES_TYPE_STRING, (void*)list_caption, (u_strlen((WCHAR*)list_caption)+1)*sizeof(WCHAR) );
	if( status!=RESULT_OK ) return status;

	status |= DRM_CreateResource( &Resources[RES_EDIT_CAPTION], RES_TYPE_STRING, (void*)edit_caption, (u_strlen((WCHAR*)edit_caption)+1)*sizeof(WCHAR) );
    if( status!=RESULT_OK ) return status;
	

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



/* Обработчик входа в main state */
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;

    
	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

	//Размер дисплея
	GRAPHIC_POINT_T disp_size;
	UIS_CanvasGetDisplaySize(&disp_size);

	bufd.w = disp_size.x + 1;
	bufd.h = disp_size.y + 1;
	bufd.buf = NULL;

	if(bufd.w == 176 && bufd.h == 220) g_consts = (GRAPHICAL_CONSTS *)&consts_176_220;
	else g_consts = (GRAPHICAL_CONSTS *)&consts_240_320;

	dialog = UIS_CreateColorCanvas ( &port, &bufd, TRUE ); // создаём канвас

    if(dialog == 0) return RESULT_FAIL;

    papp->dialog = dialog;

	
	paint();



    return RESULT_OK;
}

UINT32 StateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	APPLICATION_T           *papp = (APPLICATION_T*) app;

	APP_UtilUISDialogDelete( &papp->dialog );

	return RESULT_OK;
}

UINT32 EditStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    UIS_DIALOG_T            dialog = NULL;
    WCHAR                   buf[8];
    WCHAR                   buf_offset[64];
    UINT32                  num_chars=2;
    UINT8                   edit_type=1;
   

    if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

    switch(view_mode) {
    case HEX_MODE:
        num_chars = 2;
        edit_type = 1;
        u_strcpy(buf_offset, L"HEX, 0x");
        dec2hex(buffer[RelativeOffset(offset)], buf, 2);
        break;

    case DEC_MODE:
        num_chars = 3;
        edit_type = 32; 
        u_strcpy(buf_offset, L"DEC, 0d");
        u_ltou(buffer[RelativeOffset(offset)], buf);
        break;

    case TEXT_MODE:
        num_chars = 1;
        edit_type = 1;
        u_strcpy(buf_offset, L"TEXT, 0x");
        buf[0] = buffer[RelativeOffset(offset)];
        buf[1] = 0;
        break;
    }

    // заголовок 
    dec2hex(offset, buf_offset+u_strlen(buf_offset), 8);
    DRM_SetResource( Resources[RES_EDIT_CAPTION], (void*)buf_offset, (u_strlen(buf_offset)+1)*sizeof(WCHAR));


        dialog = UIS_CreateCharacterEditor( &papp->port,
                                            buf,
                                            edit_type,
                                            num_chars,
                                            FALSE,
                                            NULL,            
                                            Resources[RES_EDIT_CAPTION] );


        if(dialog == NULL) return RESULT_FAIL;

        papp->dialog = dialog;


        return RESULT_OK;
}

UINT32 EditData( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32       status = 0;
    EVENT_T      *event = AFW_GetEv(ev_st);
    char         buf[4];
    UINT8        value;


    switch(view_mode) {
    case HEX_MODE:
        u_utoa( event->attachment, buf);
        value = strtoul( buf, NULL, 16 );// строка в  long
        break;

    case DEC_MODE:
        value  = u_atol( event->attachment);
        break;

    case TEXT_MODE:
        UCS2toCP1251(event->attachment, buf);
        value = buf[0];
        break;
    }

    SaveOffset(offset, value);
   
    status = APP_UtilChangeState( HE_STATE_MAIN, ev_st, app );
    
    return status;
}

UINT32 EditOk( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
     UINT32                  status = 0;
     ADD_EVENT_DATA_T        ev_data;
     APPLICATION_T           *papp = (APPLICATION_T*) app;

     AFW_AddEvEvD(ev_st, EV_REQUEST_DATA, &ev_data);
	 UIS_HandleEvent( papp->dialog,  ev_st );

     return status;
}

UINT32 Back( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32                	    status = 0;
    APPLICATION_T               *papp = (APPLICATION_T*)app;

    switch (papp->state) {
    case HE_STATE_MENU:
    case HE_STATE_EDIT:
        status = APP_UtilChangeState(HE_STATE_MAIN, ev_st, app );
        break;

    case HE_STATE_FBROWSER:
        status = APP_UtilChangeState(HE_STATE_MENU, ev_st, app );
        break;

    case HE_STATE_FIND:
        status = APP_UtilChangeState(HE_STATE_MENU, ev_st, app );
        break;
    }
    
    return status;
}


UINT32 MenuStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_HEXEDITOR_T           *papp = (APP_HEXEDITOR_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;


	if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

	UINT32 Size;
	DRM_GetResourceLength(0x0100048D, &Size);

	WCHAR* ResData = (WCHAR*)malloc((Size + 1) * sizeof(WCHAR));
	DRM_GetResource(0x0100048D, (void*)ResData, Size);
	DRM_SetResource( Resources[RES_LIST_CAPTION], (void*)ResData, Size);
	free(ResData);

    	dialog = UIS_CreateList( &port,
    							 0,
    							 MENU_MAX,
    							 0,
    							 &starting_num,
    							 0,
    							 2,
    							 NULL,
    							 Resources[RES_LIST_CAPTION] );
      
    
        if(dialog == NULL) return RESULT_FAIL;
    
        papp->apt.dialog = dialog;

        
    	// Чтобы список заработал, ему нужно сразу передать starting_num элементов
    	mSendListItems(ev_st, app, 1, starting_num);

    return RESULT_OK;
}



UINT32 FbrowserStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APP_HEXEDITOR_T           *papp = (APP_HEXEDITOR_T*)app;
    SU_PORT_T               port = papp->apt.port;
	UINT32					starting_num;
    INT16                  i;
	
	char tmp[256];


	if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;
	u_utoa(file_open, tmp);
	dbgf("file_open %s size %d", tmp, u_strlen(file_open));
	
	u_strcpy(cur_folder, file_open);
	WCHAR *ptr = cur_folder + u_strlen(cur_folder);
	while(*ptr != L'/') ptr--;
	*(ptr + 1) = 0;
    
	FindFile(cur_folder, L"*"); // поиск файлов

	dbgf("removing file:/");
	if(!u_strncmp(cur_folder, L"file:/", 6))
	{
		dbgf("file:/ found. removing...");
		WCHAR *p = cur_folder;
		p += 6;
		
		u_strcpy(cur_folder, p);
	}

	u_utoa(cur_folder, tmp);
	dbgf("cur_folder %s size %d", tmp, u_strlen(cur_folder));
    
	DRM_SetResource( Resources[RES_LIST_CAPTION], (void*)cur_folder, u_strlen(cur_folder)*sizeof(WCHAR));

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
    	fbSendListItems(ev_st, app, 1, starting_num);

    return RESULT_OK;
}


UINT32 FbrowserStateExit (EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type)
{
    if(file_list != NULL) free(file_list);
	APP_UtilUISDialogDelete(&app->dialog);

	return RESULT_OK;
}

/* Функция-обработчик события EV_REQUEST_LIST_ITEMS - диалоги типа List с помощью этого ивента
    запрашивают очередную порцию данных от приложения */
UINT32 HandleListReq( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	APP_HEXEDITOR_T			*papp = (APP_HEXEDITOR_T*) app;
	EVENT_T					*event;
	UINT32					start, num;

	// Если у нашего приложения нет "фокуса", значит сообщение пришло не для нас
	if( !papp->apt.focused ) return RESULT_OK;

	// Извлекаем из ивента нужные нам данные
	event = AFW_GetEv( ev_st );

	start = event->data.list_items_req.begin_idx;
	num = event->data.list_items_req.count;

	// Мы поймали ивент, который предназначается только нам - значит, мы должны его убрать из списка
	APP_ConsumeEv( ev_st, app );

    switch (papp->apt.state) {
    case HE_STATE_MENU:
        mSendListItems( ev_st, app, start, num );
        break;
     case HE_STATE_FBROWSER:
        fbSendListItems( ev_st, app, start, num );
        break;
    }

    return RESULT_OK;
}

UINT32 mSendListItems( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist=NULL; // Буффер для элементов списка
	UINT32					i, index, status=RESULT_OK;
    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;

    for( index=0, i=start; (i<start+num) && (i<=MENU_MAX+1); i++, index++) {

        plist[index].editable = FALSE;			// Поле нередактируемо
		plist[index].content.static_entry.formatting = 1;
	}

    UIS_MakeContentFromString("s0", &(plist[0].content.static_entry.text), 0x01000DAE );
    UIS_MakeContentFromString("s0", &(plist[1].content.static_entry.text), 0x01000304 );



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


UINT32 fbSendListItems( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num)
{
	APPLICATION_T *papp = (APPLICATION_T *)app;
	LIST_ENTRY_T *plist = NULL;

	UINT32 i, index, status=RESULT_OK;
	UINT32 img_res=NULL, img_select=NULL;
    
    if( num==0 ) return RESULT_FAIL; // Контролируем параметр, на всякий случай
	
	plist = malloc( sizeof(LIST_ENTRY_T)*num ); // malloc
	if( plist==NULL ) return RESULT_FAIL;

    for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM+1); i++, index++) {

        plist[index].editable = FALSE;			// Поле нередактируемо
		plist[index].content.static_entry.formatting = 1;

        img_select=NULL;
        img_res=NULL;


        if (file_list[i-1].attrib & FS_ATTR_DIRECTORY) {
            img_res = DRM_FOLDER;
        } else {
			img_res = NULL;
		}	

        UIS_MakeContentFromString( "p1q0", &(plist[index].content.static_entry.text), file_list[i-1].name, img_res);

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


UINT32 mSelectItem( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) 
{
    UINT32 index = AFW_GetEv(ev_st)->data.index;

    switch (index) {
    case MENU_FILE_OPEN:
        APP_UtilChangeState(HE_STATE_FBROWSER, ev_st, app );
        break;
    case MENU_FIND:
        APP_UtilChangeState(HE_STATE_FIND, ev_st, app );
        break;
    }

	
	return RESULT_OK;
}


UINT32 fbSelectItem( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) 
{

    UINT32 index = AFW_GetEv(ev_st)->data.index;
    
    UINT32          i;
    WCHAR           uri[FS_MAX_URI_NAME_LENGTH+1];
   

    	    if (index == 1 && u_strcmp(cur_folder, L"/"))
			{
                 dbgf("Level Up", NULL);
				// назад
				if (cur_folder[u_strlen(cur_folder)-1] == '/') cur_folder[u_strlen(cur_folder)-1] = 0;
					for (i=u_strlen(cur_folder)-1; i > 0 ; i--)
					{
						if (cur_folder[i] == '/')
						{
							break;
						}
						else
						{
							cur_folder[i] = 0;
						}
					}

					FindFile( cur_folder, L"*");	
					UpdateList(ev_st, app, 1);
					UIS_Refresh();
			}
			else
			{
					if (file_list[index-1].attrib & FS_ATTR_DIRECTORY)
					{
                        dbgf("Open folder", NULL);
                        //  открыть папку
   						u_strcat(cur_folder, file_list[index-1].name);
						u_strcat(cur_folder, L"/");
                        
						FindFile( cur_folder, L"*");

                        UpdateList(ev_st, app, 1);
                        UIS_Refresh();
					}
					else
					{
                        dbgf("Open file", NULL);
                        // открыть файл
                        u_strcpy(uri, L"file:/");
                        u_strcat(uri, cur_folder);
                        u_strcat(uri, file_list[index-1].name);
                        OpenFile(uri);
                        LoadBuffer(0);

                        APP_UtilChangeState( HE_STATE_MAIN, ev_st, app );
					}
			}

	
	return RESULT_OK;
}

UINT32 UpdateList( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 sItem )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;

	if (sItem != 0) {
        dbgf("Update List", NULL);

        DRM_SetResource( Resources[RES_LIST_CAPTION], (void*)cur_folder, (u_strlen(cur_folder)+1)*sizeof(WCHAR));

            APP_UtilAddEvUISListChange( ev_st,
                                    app,
                                    0, // = 0
                                    sItem,
                                    ENTRIES_NUM,
                                    TRUE,
                                    2,          // = 2
                                    FBF_LEAVE,
                                    NULL,
                                    NULL );	

            UIS_HandleEvent( papp->dialog,  ev_st );
    }
			
	
	return RESULT_OK;
}


UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    UINT32                  status;

    // Вызываем дефолтный обработчик события
    status = APP_HandleUITokenGranted( ev_st, app );

    // Если всё хорошо, меняем текущий state
    if( (status == RESULT_OK) && (papp->token_status == 2) )
    {
        status = APP_UtilChangeState( HE_STATE_MAIN, ev_st, app );
    }

    return status;
}



UINT32 FindStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    UIS_DIALOG_T            dialog = NULL;
    //WCHAR                   buf[255];
    WCHAR                   buf_offset[] = L"Find ";
    UINT8                   edit_type=1;
   

    if(type!=ENTER_STATE_ENTER) 
		return RESULT_OK;

    switch(view_mode) {
    case HEX_MODE:
        edit_type = 1;
        u_strcat(buf_offset, L"HEX");
        break;

    case DEC_MODE:
        edit_type = 32; 
        u_strcat(buf_offset, L"DEC");
        break;

    case TEXT_MODE:
        edit_type = 1;
        u_strcat(buf_offset, L"TEXT");
        break;
    }

    // заголовок 
    DRM_SetResource( Resources[RES_EDIT_CAPTION], (void*)buf_offset, (u_strlen(buf_offset)+1)*sizeof(WCHAR));


        dialog = UIS_CreateCharacterEditor( &papp->port,
                                            find_text,
                                            edit_type,
                                            255, // кол-во возможных символов
                                            FALSE,
                                            NULL,            
                                            Resources[RES_EDIT_CAPTION] );


        if(dialog == NULL) return RESULT_FAIL;

        papp->dialog = dialog;


        return RESULT_OK;
}

UINT32 FindData( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32       status = 0;
    EVENT_T      *event = AFW_GetEv(ev_st);
    char         buf[255];


    u_strcpy(find_text, event->attachment);

    Data2Pattern(event->attachment, buf);

    FindPattern(buf, offset);
    status = APP_UtilChangeState( HE_STATE_MAIN, ev_st, app );
    
    return status;
}

UINT32 Data2Pattern(WCHAR *data, char *pattern)
{
    char         hex[4];
    UINT16       i=0, j=0;

    switch(view_mode) {
    case DEC_MODE:
    case HEX_MODE:
        u_utoa( data, pattern);

        for (i=0; i < strlen(pattern); i+=2) {
            strncpy(hex, pattern+i, 2);
            if (hex[0] == '?' || hex[1] == '?') {
                pattern[j] = '?';
            } else {
                pattern[j] = (char)strtoul( hex, NULL, 16 ); // строка в  long
            }
            j++;
        }
        pattern[j] = 0;
        break;


    case TEXT_MODE:
        UCS2toCP1251(data, pattern);
        break;
    }

    return RESULT_OK;
}

UINT32 FindFile( WCHAR *folder, WCHAR *filtr ) // поиск файлов
{
    INT32                   err=RESULT_OK;
	FS_SEARCH_PARAMS_T		sp;
	FS_SEARCH_RESULT_T		hResult;
	FS_SEARCH_HANDLE_T		hSearch;
	char tmp[256];


	UINT16		count_files=0, count_res=1;
	UINT32		status = RESULT_OK;
	INT32		i=0, j=0;
    UINT8       mode=0;
    WCHAR       volumes[12];

    WCHAR       search_string[256];

    dbgf("**FindFile** \n", NULL);	
	if ( folder==NULL || !u_strcmp(folder, L"/")) {
            u_strcpy(search_string, L"/");
    } else {
        // определяем нужен ли file://
		dbgf("**FindFile** Checking file://...\n", NULL);
        if(u_strncmp(folder, L"file:/", 6))
        {
            u_strcpy(search_string, L"file:/");
            u_strcat(search_string, folder);
        } else {
            u_strcpy(search_string, folder);
        }
            
        // определяем нужен ли сплеш
		dbgf("**FindFile** Checking slash\n", NULL);
        if ( folder[u_strlen(folder)-1] != '/' ) 
            u_strcat(search_string, L"/");
    
        u_strcat(search_string, L"\xFFFE");
        u_strcat(search_string, filtr);
		
		u_utoa(search_string,tmp);
		dbgf("**FindFile** search_string = %s\n", tmp);
    }

    ///// Особенности /////
    // 1) в корне диска `a` в hResult.name относительный путь!
    // 2) в списке может быть имя текущей папки. При этом в конце есть слэш. Последствия патча "удаление ненужных папок с тела"
    //////////////////////

    if (!u_strcmp(search_string, L"/")) // диски
    {
	dbgf("**FindFile** Volumes\n", NULL);

        DL_FsVolumeEnum(volumes);

        free(file_list);
        file_list = malloc(sizeof(FILEINFO)*4);
        if (file_list == NULL) return RESULT_FAIL;

        cur_folder[0] = '/';

        ENTRIES_NUM = 0;
		dbgf("**FindFile** Volumes before for\n", NULL);
        for (i=0; i < 4; i++) {
            file_list[i].name[0] = volumes[i*3+1];//'a' + i;
            file_list[i].name[1] = 0;
            file_list[i].attrib = FS_ATTR_DIRECTORY;

            ENTRIES_NUM++;

            if (volumes[i*3+2] == NULL)  break;
        }

        return RESULT_OK;
    }
	
	sp.flags = 0x1C; 
	sp.attrib = 0;
	sp.mask = 0;
	
	status = DL_FsSSearch( sp, search_string, &hSearch, &count_files, 0 );

	if ( status != RESULT_OK ) {
		dbgf("**FindFile** unsuccess for\n", NULL);
        DL_FsSearchClose( hSearch );
        return RESULT_FAIL;
    }

    dbgf("FB: count_files = %d", count_files);
	
	ENTRIES_NUM = count_files+1;

	if(file_list != NULL) free(file_list);
    file_list = malloc(sizeof(FILEINFO)*(count_files+1));
    if (file_list == NULL) return RESULT_FAIL;
	
	u_strcpy(file_list[0].name, L"..");
	file_list[0].attrib = FS_ATTR_DIRECTORY;


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
			u_utoa(hResult.name,tmp);
			dbgf("hResult.name = %s\n",tmp);			
			
            if ((mode==0) && !(hResult.attrib&FS_ATTR_DIRECTORY)) continue;
            if ((mode==1) && (hResult.attrib&FS_ATTR_DIRECTORY)) continue;

            // фиксим последствия  патча "удаление ненужных папок с тела"
            if(!u_strcmp(hResult.name+u_strlen(hResult.name)-1, L"/")) continue;
            	
			u_strcpy(file_list[j].name, SplitPath(hResult.name, L"/"));
			//dbgf("file_list[%d].name = %s\n",j,u_utoa(file_list[j].name,tmp));
			
			/*u_utoa(file_list[j].name,tmp);
		    dbgf("file_list[%d].name = %s\n",j,tmp);*/
			
			file_list[j].attrib = hResult.attrib;
			j++;
		} 
        
	}

	DL_FsSearchClose( hSearch ); 
	
	return RESULT_OK;
}

UINT32 Test_search()
{
	/*INT32                   err=RESULT_OK;
	FS_SEARCH_PARAMS_T		sp;
	FS_SEARCH_RESULT_T		hResult;
	FS_SEARCH_HANDLE_T		hSearch;
	char tmp[256];


	UINT16		count_files=0, count_res=1;
	UINT32		status = RESULT_OK;
	INT32		i=0, j=0;
    UINT8       mode=0;
    WCHAR       volumes[12];

    WCHAR       search_string[] = L"file://a/";
	
	u_strcat(search_string, L"\xFFFE");
    u_strcat(search_string, L"*");

    u_utoa(search_string,tmp);
	dbgf("**FindFile** search_string = %s\n", tmp);
	
	sp.flags = 0x1C; 
	sp.attrib = 0;
	sp.mask = 0;
	
	status = DL_FsSSearch( sp, search_string, &hSearch, &count_files, 0 );

	if ( status != RESULT_OK ) {
		dbgf("**FindFile** unsuccess for\n", NULL);
        DL_FsSearchClose( hSearch );
        return RESULT_FAIL;
    }

    dbgf("FB: count_files = %d", count_files);
	
	ENTRIES_NUM = count_files+1;

	if(file_list != NULL) free(file_list);
    file_list = malloc(sizeof(FILEINFO)*(count_files+1));
    if (file_list == NULL) return RESULT_FAIL;
	
	u_strcpy(file_list[0].name, L"..");
	file_list[0].attrib = FS_ATTR_DIRECTORY;


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
			u_utoa(hResult.name,tmp);
			dbgf("hResult.name = %s\n",tmp);			
			
            if ((mode==0) && !(hResult.attrib&FS_ATTR_DIRECTORY)) continue;
            if ((mode==1) && (hResult.attrib&FS_ATTR_DIRECTORY)) continue;

            // фиксим последствия  патча "удаление ненужных папок с тела"
            if(!u_strcmp(hResult.name+u_strlen(hResult.name)-1, L"/")) continue;
            	
			u_strcpy(file_list[j].name, SplitPath(hResult.name, L"/"));
			//dbgf("file_list[%d].name = %s\n",j,u_utoa(file_list[j].name,tmp));
			
	
			
			file_list[j].attrib = hResult.attrib;
			j++;
		} 
        
	}

	DL_FsSearchClose( hSearch ); */
	
	GRAPHIC_REGION_T region;

    region . ulc . x = 0;
    region . ulc . y = 0;
    region . lrc . x = 20;
    region . lrc . y = 20;

}


UINT32 OpenFile(WCHAR *uri)
{
    dbgf("Open file", NULL);

    DL_FsCloseFile( f);
    f = DL_FsOpenFile( uri,  FILE_READ_PLUS_MODE,  0 );
    if (f == FILE_HANDLE_INVALID  ) return RESULT_FAIL;
    u_strcpy(file_open, uri);
    SaveSetting(CFGFile);

    offset = 0;

    file_size = DL_FsGetFileSize( f );
    dbgf("file_size = %d", file_size);

    free(buffer);
    buffer = malloc(g_consts->CELLS_COUNT);
    memset(buffer, 0, g_consts->CELLS_COUNT); 

    return RESULT_OK;
}

UINT32 LoadBuffer(INT32 off_set)
{
    UINT32 r;


    if (f == FILE_HANDLE_INVALID) return RESULT_FAIL;

    DL_FsFSeekFile( f, (off_set/g_consts->CELLS_COUNT)*g_consts->CELLS_COUNT, SEEK_WHENCE_SET );
    if (file_size < g_consts->CELLS_COUNT) { 
        DL_FsReadFile( buffer,  1,  file_size, f, &r );
    } else {
        DL_FsReadFile( buffer,  1,  g_consts->CELLS_COUNT, f, &r );
    }
    dbgf("Read %d b from %d b", r, g_consts->CELLS_COUNT);

    return r;
}

UINT32 SaveOffset(INT32 off_set, UINT8 value)
{
    UINT32 w;

    dbgf("Save offset 0x%x, new value = 0x%x", off_set, value);

    buffer[RelativeOffset(offset)] = value;
    DL_FsFSeekFile( f,  off_set, SEEK_WHENCE_SET );
    DL_FsWriteFile( &value, 1, 1, f, &w);

    return RESULT_OK;
}

UINT32 GoTo(INT32 off_set) 
{
    dbgf("GoTo 0x%x", off_set);
    offset = off_set;
    LoadBuffer(off_set);

    return RESULT_OK;
}

INT32 FindPattern(char *pattern, INT32 off_set)
{
    INT32 cur_offset = off_set;
    INT32 begin_offset = RelativeOffset(off_set);  
    INT32 offset_pattern = -1;
    UINT32 i=0, j=0;

    dbgf("Find pattern = '%s'", pattern);

    while (cur_offset+i < file_size+g_consts->CELLS_COUNT) {

        LoadBuffer(cur_offset);

        for (i=begin_offset; i < g_consts->CELLS_COUNT && cur_offset+i < file_size; i++) {
            if (buffer[i] == pattern[j] || pattern[j] == '?') {
                if (j < strlen(pattern)-1) {
                    dbgf("Symbol %d found", pattern[j]);
                    j++;
                    if (offset_pattern == -1) offset_pattern = (cur_offset/g_consts->CELLS_COUNT)*g_consts->CELLS_COUNT+i;
                } else {
                    dbgf("Pattern found. offset = 0x%x", offset_pattern);
                    GoTo(offset_pattern); // паттерна найдена!
                    return offset_pattern;
                }
            } else {
                j=0;
                offset_pattern = -1;
            }
        }

        begin_offset = 0;
        cur_offset += g_consts->CELLS_COUNT;
    }

    LoadBuffer(off_set);

    return offset_pattern;
}

INT32 RelativeOffset(INT32 off_set)
{
   INT32 I;

    if (off_set >= g_consts->CELLS_COUNT) {
        I = off_set % (g_consts->CELLS_COUNT);
    } else {
        I = off_set;
    }

    return I;
}


UINT32 paint(void)
{
    GRAPHIC_POINT_T  item;
    UINT16  x, y;
    WCHAR   hex[16];
    WCHAR   info[64];
    UINT32  adr_major=0;
    UINT16  adr_minor=0;
    UINT32  addres=0;
    UINT32  i;

    adr_major = (offset/g_consts->CELLS_COUNT)*g_consts->CELLS_COUNT;

    addres = adr_major;
	//<+chik>
	UIS_CanvasSetFont( 0, dialog);
	//</+chik>
    // фон
	SetFillColor(0xFFFFFF00);
    FillRect(0, 0, bufd.w, bufd.h);

    // сетка
    SetForegroundColor(0x0000FF00);
    for (y=g_consts->GRID_Y; y < bufd.h; y += g_consts->LINE_H) {
        DrawLine(0, y, bufd.w, y);
    }

    for (x=g_consts->GRID_X; x < bufd.w; x += g_consts->COLUMN_W) {
        DrawLine(x, 0, x, y-g_consts->LINE_H);
    }

    // информация
    SetForegroundColor(0x0000FF00);
    SetBackgroundColor(0xFFFFFFFF);
        // размер файла
    u_strcpy(info, L"fs = ");
    u_ltou(file_size, info+u_strlen(info));
        // страница
    u_strcat(info, L" b; page: ");
    u_ltou(adr_major/g_consts->CELLS_COUNT+1, info+u_strlen(info));
    u_strcat(info, L"/");
    if (file_size%g_consts->CELLS_COUNT > 0) {
        u_ltou(file_size/g_consts->CELLS_COUNT+1, info+u_strlen(info));
    } else {
        u_ltou(file_size/g_consts->CELLS_COUNT, info+u_strlen(info));
    }

    switch(view_mode) {
    case HEX_MODE:
        u_strcat(info, L"; HEX");
        break;

    case DEC_MODE:
        u_strcat(info, L"; DEC");
        break;

    case TEXT_MODE:
        u_strcat(info, L"; TEXT");
        break;
    }

    DrawText(info, 1 , y-g_consts->LINE_H+1, ANCHOR_LEFT | ANCHOR_TOP);

    // адреса
    SetForegroundColor(0x00000000);
    SetBackgroundColor(0xFFFFFFFF);
        // слева 
    for (y=0; y < g_consts->LINES_COUNT; y++ ) {
        dec2hex( adr_major, hex, 8 );
        DrawText(hex, 1, g_consts->GRID_Y + (g_consts->LINE_H*y)+1, ANCHOR_LEFT | ANCHOR_TOP);
        adr_major += g_consts->COLUMNS_COUNT;
    }
        // сверху
    for (x=0;  x < g_consts->COLUMNS_COUNT; x ++) {
        dec2hex( adr_minor, hex, 2 );
        DrawText(hex, g_consts->GRID_X + (g_consts->COLUMN_W*x)+1, 1, ANCHOR_LEFT | ANCHOR_TOP);
        adr_minor++;
    }

        // текущий оффсет
    SetForegroundColor(0xFF000000);
    dec2hex( offset, hex, 8 );
    DrawText(hex, 1 , 1, ANCHOR_LEFT | ANCHOR_TOP);

        // выделенная ячейка
    SetFillColor(0xBBBBBB00);
    item = XY(offset);
    FillRect(item.x+1, item.y+1, g_consts->COLUMN_W-2, g_consts->LINE_H-2);

    // заполняем данными
    if (buffer != NULL) {
        SetForegroundColor(0xFF000000);
        SetBackgroundColor(0xFFFFFFFF);
        x = 0;
        y = 0;
        for (i=0; i < g_consts->CELLS_COUNT; i++) {
            if (addres+i < file_size) {
                switch (view_mode) {
                case HEX_MODE:
                    dec2hex( (UINT32)buffer[i], hex, 2 );
                    break;

                case DEC_MODE:
                    u_ltou(buffer[i], hex);
                    break;

                case TEXT_MODE:
                    hex[0] = buffer[i];
                    hex[1] = 0;
                    break;
                }
                
                DrawText(hex, g_consts->GRID_X+(g_consts->COLUMN_W*x)+1 , g_consts->GRID_Y + (g_consts->LINE_H*y)+1, ANCHOR_LEFT | ANCHOR_TOP);
        
                if (x < g_consts->COLUMNS_COUNT-1) {
                    x++;
                } else {
                    x = 0;
                    y++;
                }
            }
        }
    }

    

    return RESULT_OK;
}

GRAPHIC_POINT_T  XY(UINT32 sItem)
{
    GRAPHIC_POINT_T  item;
    UINT32 I=0;
    UINT32 n_line, n_column;


    I = RelativeOffset(offset);


    n_line = I/g_consts->COLUMNS_COUNT;
    n_column = I - g_consts->COLUMNS_COUNT*n_line;
    
    item.y = g_consts->GRID_Y + n_line*g_consts->LINE_H;
    item.x = g_consts->GRID_X + n_column*g_consts->COLUMN_W;


    return item;
}


UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    EVENT_T     *event = AFW_GetEv(ev_st);
    INT32       off_set=offset;
    char        buf[256];


    APP_ConsumeEv( ev_st, app );

	switch (event->data.key_pressed)
	{
        case KEY_RED: // выход
            destroyApp( ev_st,  app );
			break;

        case KEY_3:
            Data2Pattern(find_text, buf);
            FindPattern(buf, offset+u_strlen(find_text));
            break;

        case KEY_0:
            if (view_mode < MAX_MODE) {
                view_mode++;
            } else {
                view_mode = 0;
            }
            break;

        case KEY_POUND:
            GoTo((offset+36<file_size)?(offset+36):0);
            break;
    
        case KEY_STAR:
            GoTo((offset-36>0)?(offset-36):(file_size-1));
            break;

        case KEY_6:
        case KEY_RIGHT:
            if(offset+1 < file_size) offset ++;
            break;

		case KEY_4:
        case KEY_LEFT:
            if (offset > 0) offset --;
            break;	

        case KEY_8:
        case KEY_DOWN:
                if(offset+g_consts->COLUMNS_COUNT < file_size) offset += g_consts->COLUMNS_COUNT;
            break;	

         case KEY_2:
         case KEY_UP:
            if (offset-g_consts->COLUMNS_COUNT >= 0) {
                offset -= g_consts->COLUMNS_COUNT;
            }
            break;

        case KEY_5:
        case KEY_CAM:
		if(DL_FsFFileExist(file_open))
			APP_UtilChangeState(HE_STATE_EDIT, ev_st, app);
		break;

        case 0xe:
            APP_UtilChangeState(HE_STATE_MENU, ev_st, app );
            break;
			
		case KEY_9:
			Test_search();
			break;
    }

    if ((off_set/g_consts->CELLS_COUNT) != (offset/g_consts->CELLS_COUNT)) 
    {
        LoadBuffer(offset);
    }

    paint();

    return RESULT_OK;
		
}

WCHAR* dec2hex( UINT32 dec, WCHAR *hex, UINT16 size )
{

	UINT16 hex_buf[4]; // остаток	
	UINT16 len=0; // кол-во элементов hex_buf
	INT16 i, j; // счётчик 


	if(dec==0) {
        for (j=0; j < size; j++ ) {
            hex[j] = '0';
        }
		hex[j]= 0;
	} else {
    
        while (dec != 0) {
            hex_buf[len] = dec%16;
            dec = dec/16;
            len ++;
        }

        for (j=0; j < size - len; j++ ) {
            hex[j] = '0';
        }
        
        len--;

        for (i=len; i >= 0; i--) {
            if(hex_buf[i]<10) hex[j] = '0'+hex_buf[i];
            else hex[j] = 'A'+hex_buf[i]-10;
            j++;
        }   
        hex[j] = 0;
    }

	return hex;
}

// взять часть строки от spliter до конца
WCHAR* SplitPath( WCHAR* path, WCHAR* spliter ) 
{
	int j = u_strlen(path)-2;
	while( j > 0 ) 
	{
		if ( !u_strncmp(path+j, spliter, 1) ) 
		{
			return path+j+1;
		}
		j--;
	}
	return path;
}


UINT32 UCS2toCP1251(WCHAR *src, char *trg)
{
    UINT16  i;

    for(i=0; i <= u_strlen(src); i++) {
        if (src[i] < 0xA0) { // до 128
            trg[i] = src[i];
        } else if (src[i] == 0x451){ // ё
            trg[i] = 0xB8;
        } else if (src[i] == 0x401){ // Ё
            trg[i] = 0xA8;
        } else if (src[i] > 0x40F) { // кирилица
            trg[i] = src[i] - 0x350;
        } else {
            trg[i] = 0x3F; // ? - нет символа
        }
    }

    return RESULT_OK;
}
