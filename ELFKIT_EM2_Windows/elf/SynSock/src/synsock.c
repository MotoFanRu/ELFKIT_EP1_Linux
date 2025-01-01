// автор: G-XaD, mod: om2804
#include "synsock.h"
#include "sock_api.h"
#include "sock_utils.h"

// возможно что всегда нужно выставлять входящий порт
//#define LOCAL_PORT_START  100
HSOCK				next_hsock;                   // идентификатор следующего сокета
SU_PORT_T			port;                         // порт приложения

SOCKET_T			socket_tbl[MAX_SOCKETS];      // инфа о сокетах
DSMA_ATTRIBUTE_T	attrs[MAX_SOCKETS][16];       // атрибуты для DSMA

WEB_SESSION_INFO_T  ws_info;
ADD_EVENT_DATA_T	add_data;

ldrElf				*elf = NULL;
const char			app_name[APP_NAME_LEN] = "SynSock";

// В принципе и этого стэйта хватит ...
// какая разница в каком стэйте ловить ивенты
static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
	{ STATE_HANDLERS_PM_API_EXIT,   HandleExit			},	// PM API: cmd EXIT
	{ SOCKET_MGR_MAIN_REGISTER,	    HandleIfaceReq		},
	{ EV_TIMER_EXPIRED,			    HandleTimerExpiried	},

	{ EV_WEBSESSION_OPERATION,	    HandleWSOperation	},
//	{ EV_WEBSESSION_CHANGE,		    HandleWSChange		},

	{ EV_DSMA_PROXY_CREATE,		    NET_HandleProxyEvent },
	{ EV_DSMA_PROXY_STATE,		    NET_HandleProxyEvent },
	{ EV_DSMA_BUFF_STATE,		    NET_HandleProxyEvent },
	{ EV_DSMA_PROXY_ATTR,		    NET_HandleProxyEvent },

	// Список всегда должен заканчиватся такой записью
	{ STATE_HANDLERS_END,		    NULL				},
};


/* Таблица соответствий обработчиков, состояний и функций входа-выхода из состояния.
	Порядок состояний такой же, как в enum-e */
static const STATE_HANDLERS_ENTRY_T state_handlintable[] =
{
	{ HW_STATE_ANY,			// State
	  NULL,					// Обработчик входа в state
	  NULL,					// Обработчик выхода из state
	  any_state_handlers	// Список обработчиков событий
	}
};

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params)
{
	UINT32		status = RESULT_OK;
	UINT32		reserve;

    elf = ela;
    elf->name = (char *)app_name;

	/* Проверяем, а вдруг уже запущено? */
	if(ldrIsLoaded( (char*)app_name ))
	{
		dbg("Application already loaded");
		return RESULT_FAIL;
	}

	reserve = elf->evbase + 1;
	reserve = ldrInitEventHandlersTbl( any_state_handlers, reserve );

	/* Регистрируем приложение */
	status = APP_Register(&elf->evbase,	1, state_handlintable, HW_STATE_MAX, (void*)ELF_Start);
	dbgf("Register: APP_Register DONE, status = %d\n", status);

	if(status == RESULT_OK)
        ldrSendEvent( elf->evbase );	// Запустить немедленно

	dbg("Register: END\n");
	return RESULT_OK;
}


/* Функция вызываемая при старте приложения */
UINT32 ELF_Start( EVENT_STACK_T * ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info )
{
	APPLICATION_T *		app = NULL;
	UINT32				status = RESULT_OK;

	dbg("ELF_Start: Enter\n");
	if( AFW_InquireRoutingStackByRegId( reg_id ) == RESULT_OK )
	{
		return RESULT_OK; // Нам не нужен второй экземпляр
	}

	app = (APPLICATION_T *)APP_InitAppData(
						(void*)APP_HandleEventPrepost,	// Обработчик для фоновых приложений
						sizeof(APP_ELF_T),			// Размер структуры приложения
						reg_id,
						0, 0,
						1,
						2,
						0,
						3 );

	if ( !app )
	{
		ldrUnloadElf(elf);
		return RESULT_OK;
	}
    elf->app = app;
    ELF_Init(app);

	status = APP_Start( ev_st, app, HW_STATE_ANY, state_handlintable, ELF_Exit, app_name, 0);
	if ( status == RESULT_OK )
	{
		// запустить таймер, который будет убивать простаивающие сокеты
		APP_UtilStartCyclicalTimer( SOCKETS_ACTIVITY_TIMER_DELAY, SOCKETS_ACTIVITY_TIMER_ID, app );
		dbg("Started");
	}
	else
	{
		dbg("Start failed");
		APP_HandleFailedAppStart( ev_st, (APPLICATION_T*)app, 0 );
		ldrUnloadElf(elf);
		return RESULT_OK;
	}

	dbg("ELF_Start: END\n");

	return RESULT_OK;
}


UINT32 ELF_Init(APPLICATION_T *app)
{
	int i;

    // Запомнить порт в глобальной переменной, нужно чтобы это приложение ловило сообщения от DL_DSMA
    next_hsock = 1;
	port = app->port;

	for ( i=0; i<MAX_SOCKETS; i++ )
	{
	    memclr(&socket_tbl[i], sizeof(SOCKET_T));
	    memclr(attrs[i], sizeof(DSMA_ATTRIBUTE_T) * 16);
		util_delete_sock(i);
	}
    memclr(&ws_info, sizeof(WEB_SESSION_INFO_T));
	WSRead(); // читаем данные из web-сесии
}


/* Функция выхода из приложения */
UINT32 ELF_Exit( EVENT_STACK_T *ev_st, APPLICATION_T *app )
{
    int		i;
	UINT32	status;

	dbg("ELF_Exit: Enter\n");

	APP_UtilStopTimer(app);
    mfree(ws_info.ws_rec.gprs_record);

	status = APP_ExitStateAndApp(ev_st, app, NULL);
	dbgf("ELF_Exit: APP_Exit DONE, status = %d\n", status);

	/* выхода не должно происходить, но вдруг... */
	for ( i=0; i<MAX_SOCKETS; i++ )
		delete_and_notify(i);

	dbg("ELF_Exit: END\n"); // именно до!

	return ldrUnloadElf(elf);
}


UINT32 NET_HandleProxyEvent( EVENT_STACK_T * ev_st, APPLICATION_T * app )
{
	EVENT_T				*event = AFW_GetEv(ev_st);
	UINT32				code = event->code, ev;
	DSMA_DATA_T			*dsma = (DSMA_DATA_T *)event->attachment;
	int					idx, i;


	dbgf("ProxyEvent: code:0x%x, proxy_id:%d, status:%d\n", code, dsma->proxy_id, dsma->status);
	//dbgf("ProxyEvent: ev_code = 0x%x, att_size = %d, att = 0x%p\n", code, event->att_size, event->attachment);
	if ( /*event->att_size != sizeof(DSMA_DATA_T) || */dsma == NULL )
	{
		dbgf("ProxyEvent: ev_code = 0x%x, att_size = %d, att = 0x%p\n", code, event->att_size, event->attachment);
		return RESULT_OK;
	}

	// идентифицируем ответ на "наш" напрос

    idx = util_find_sock_by_proxy(dsma->proxy_id);
	if ( idx == -1 )
	{
	    idx = util_find_sock_by_cmd(dsma->cmd);

	}

	if ( idx == -1 )
	{
		// это наверное не мы создали
		dbgf("ProxyEvent: socket not found. proxy_id:%d\n", dsma->proxy_id);
		return RESULT_OK;
	}

	memclr(&add_data, sizeof(ADD_EVENT_DATA_T));

	if ( code == EV_DSMA_PROXY_CREATE ) ev = 0;
	else if ( code == EV_DSMA_PROXY_STATE ) ev = 1;
	else if ( code == EV_DSMA_BUFF_STATE ) ev = 2;
	else if ( code == EV_DSMA_PROXY_ATTR ) ev = 3;
	else return RESULT_OK;

	switch (ev)
	{
		case 0 : // EV_DSMA_PROXY_CREATE
			// Прокси создано
			dbgf("ProxyEvent: EV_DSMA_PROXY_CREATE, status = %d\n", dsma->status);
			if ( socket_tbl[idx].state != SOCK_STATE_CREATE )
			{
				// эм... мы не ждем создание прокси О_о
				dbgf("ProxyEvent: event = %x, wrong state %d\n", code, socket_tbl[idx].state);
			}
			else if ( socket_tbl[idx].cmd == dsma->cmd )
			{
				// это то, что мы ждем
				// запомнили, по этому обращаемся к DSMA
				socket_tbl[idx].proxy_id = dsma->proxy_id;
				// сообщить хозяину сокета о создании
				add_data.data.params[0] = SOCK_ANSW_CREATE;
				if ( dsma->status == RESULT_OK )
				{
					// Все ОК
					add_data.data.params[1] = RESULT_OK;
					socket_tbl[idx].state = SOCK_STATE_NULL;
				}
				else
				{
					add_data.data.params[1] = RESULT_FAIL;
					dbg("ProxyEvent: status is fail\n");
					socket_tbl[idx].state = SOCK_STATE_NULL;
				}
				// собственно сообщаем
				SocketAnswer(socket_tbl[idx].ev_callback, add_data);
			}
			else
			{
				dbgf("ProxyEvent: wrong cmd %d\n", dsma->cmd);
			}
			break;


		case 3 : // EV_DSMA_PROXY_ATTR
			// конфигурация прошла
			dbgf("ProxyEvent: EV_DSMA_PROXY_ATTR, status = %d\n", dsma->status);
			if ( socket_tbl[idx].state != SOCK_STATE_SET_ATTR )
			{
				// эм... мы не ждем этого
				dbgf("ProxyEvent: event = %x, wrong state %d\n", code, socket_tbl[idx].state);
			}
			else if ( socket_tbl[idx].cmd == dsma->cmd )
			{
				dbgf("ProxyEvent: status = %d\n", dsma->status);
				// ответа не нужно, просто активируем
				if ( dsma->status == RESULT_OK )
				{
					socket_tbl[idx].cmd = DL_DSMA_ProxyActivate( dsma->proxy_id );
					socket_tbl[idx].state = SOCK_STATE_ACTIVATE;
				}
				else
				{
					dbg("ProxyEvent: status is fail\n");
					socket_tbl[idx].state = SOCK_STATE_ERROR;
					add_data.data.params[0] = SOCK_ANSW_ERROR;
					SocketAnswer(socket_tbl[idx].ev_callback, add_data);
				}
			}
			else
			{
				dbgf("ProxyEvent: wrong cmd %d\n", dsma->cmd);
			}
			break;


		case 1: // EV_DSMA_PROXY_STATE
			// Включено, или другие сообщения от прокси
			dbgf("ProxyEvent: EV_DSMA_PROXY_STATE, proxy_state = %d\n", dsma->proxy_state);
			socket_tbl[idx].last_activity = getTime();
			if ( socket_tbl[idx].state == SOCK_STATE_ACTIVATE )
			{
				// Если нужно было активировать прокси
				if ( dsma->proxy_state == PROXY_STATE_ACTIVE )
				{
					// подключились
					dbgf("ProxyEvent: ACTIVATE - sock ready");
					socket_tbl[idx].state = SOCK_STATE_READY;
					// оповещаем
					add_data.data.params[0] = SOCK_ANSW_READY;
					SocketAnswer(socket_tbl[idx].ev_callback, add_data);
				}
				else if ( dsma->proxy_state == PROXY_STATE_FAILED )
				{
					// не подключились
					if ( socket_tbl[idx].activate_counts < ACTIVATE_COUNTS_MAX )
					{
						// пробуем неск раз
						socket_tbl[idx].activate_counts++;
						dbgf("ProxyEvent: ACTIVATE failed - attempt #%d", socket_tbl[idx].activate_counts);
						socket_tbl[idx].cmd = DL_DSMA_ProxyActivate( dsma->proxy_id );
					}
					else
					{
						// безуспешно =(
						socket_tbl[idx].state = SOCK_STATE_ERROR;
						dbgf("ProxyEvent: ACTIVATE - failed, send ERROR");
						add_data.data.params[0] = SOCK_ANSW_ERROR;
						SocketAnswer(socket_tbl[idx].ev_callback, add_data);
						//delete_and_notify( idx );
					}
				}
				else
				{
					// khm
					dbgf("ProxyEvent: event = %x, unexpected proxy_state = %d\n", code, dsma->proxy_state);
				}
			}
			else if ( socket_tbl[idx].state == SOCK_STATE_READY
						|| socket_tbl[idx].state == SOCK_STATE_NOWRITE )
			{
				if ( dsma->proxy_state == PROXY_STATE_INACTIVE )
				{
					// деактивация сокета
					socket_tbl[idx].state = SOCK_STATE_CLOSED;
					dbgf("ProxyEvent: INACTIVE - sock closed\n");
					add_data.data.params[0] = SOCK_ANSW_CLOSED;
					SocketAnswer(socket_tbl[idx].ev_callback, add_data);
					//DL_DSMA_ProxyDeactivateSocket( dsma->proxy_id );
					socket_close( socket_tbl[idx].hsock );
				}
				else
				{
					// khm
					dbgf("ProxyEvent: event = %x, unexpected proxy_state = %d\n", code, dsma->proxy_state);
				}
			}
			else
			{
				// О_о
				dbgf("ProxyEvent: event = %x, unexpected socket state %d\n", code, socket_tbl[idx].state);
			}
			break;


		case 2: // EV_DSMA_BUFF_STATE
			// Входящие данные
			if ( socket_tbl[idx].state == SOCK_STATE_READY
						|| socket_tbl[idx].state == SOCK_STATE_NOWRITE )
			{
				dbgf("ProxyEvent: EV_DSMA_BUFF_STATE, status = %d\n", dsma->status);
				if ( dsma->buf_state == BUFFER_STATE_XON )
				{
					// сообщить приложению, что можно слать данные в сокет
					socket_tbl[idx].state = SOCK_STATE_READY;
					dbg("ProxyEvent: sock ready to write\n");
					add_data.data.params[0] = SOCK_ANSW_READY;
					SocketAnswer(socket_tbl[idx].ev_callback, add_data);
				}
				else if ( dsma->buf_state == BUFFER_STATE_XOFF )
				{
					//сообщить приложению, что сокет не готов для записи
					socket_tbl[idx].state = SOCK_STATE_NOWRITE;
					dbg("ProxyEvent: sock not ready to write\n");
					add_data.data.params[0] = SOCK_ANSW_NOWRITE;
					SocketAnswer(socket_tbl[idx].ev_callback, add_data);
				}
				else if ( dsma->buf_state == BUFFER_STATE_DATA )
				{
					// Если в буфере данные, сообщить приложению
					dbg("ProxyEvent: incoming data\n");
					add_data.data.params[0] = SOCK_ANSW_DATA;
					SocketAnswer(socket_tbl[idx].ev_callback, add_data);
				}
				else {
					// прочие сообщения неизвестны...
					dbgf("ProxyEvent: event = %x, buf_state = %d\n", code, dsma->buf_state);
				}
			}
			else
			{
				// О_о
				dbgf("ProxyEvent: event = %x, unexpected socket state %d\n", code, socket_tbl[idx].state);
			}
	} // switch (code)


	APP_ConsumeEv(ev_st, app);

	return RESULT_OK;
}


// Обработчик ивента запроса функций сокетов
UINT32 HandleIfaceReq( EVENT_STACK_T * ev_st, APPLICATION_T * app )
{
	EVENT_T				*event = AFW_GetEv(ev_st);
	UINT32				ev_cb = event->data.params[0];

	dbgf("HandleIfaceReq, ev_cb = 0x%x", ev_cb);
	APP_ConsumeEv(ev_st, app);

	if ( ev_cb != 0 )
	{
		memclr( &add_data, sizeof(ADD_EVENT_DATA_T) );
		add_data.data.params[0] = (UINT32)socket;
		add_data.data.params[1] = (UINT32)socket_connect;
		add_data.data.params[2] = (UINT32)socket_write;
		add_data.data.params[3] = (UINT32)socket_read;
		add_data.data.params[4] = (UINT32)socket_bind;
		add_data.data.params[5] = (UINT32)socket_close;
		add_data.data.params[6] = (UINT32)socket_delete;
		SocketAnswer(ev_cb, add_data);
	}

	return RESULT_OK;
}


// тут будем убивать простаивающие сокеты
UINT32 HandleTimerExpiried( EVENT_STACK_T * ev_st, APPLICATION_T * app )
{
	EVENT_T				*event = AFW_GetEv(ev_st);
	UINT32				time;
	int					i;


	if ( ((DL_TIMER_DATA_T *)(event->attachment))->ID != SOCKETS_ACTIVITY_TIMER_ID )
	{
		// это не тот таймер О_о
		return RESULT_OK;
	}

	APP_ConsumeEv(ev_st, app);

	time = getTime();

	for ( i=0; i<MAX_SOCKETS; i++ )
	{
		if ( socket_tbl[i].hsock != HSOCK_INVALID )
		{
			if ( time - socket_tbl[i].last_activity > ws_info.timeout )
			{
				delete_and_notify( i );
			}
		}
	}


	if ( !ws_info.apn[0] )
		WSRead();

	return RESULT_OK;
}


UINT32 HandleWSOperation( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	WS_EVENT_T		*ws = AFW_GetEv(ev_st)->attachment;

	//dbgf("HandleWSOperation: att_size = %d\n", AFW_GetEv(ev_st)->att_size);
	dbgf("HandleWSOperation: result = %d\n", ws->result);
	APP_ConsumeEv(ev_st, app);

	if ( ws->result == RESULT_OK )
	{
		WSAccept();
	}

	return RESULT_OK;
}


UINT32 HandleWSChange( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	//WS_EVENT_T		*ws = AFW_GetEv(ev_st)->attachment;

	dbg("HandleWSChange");
	APP_ConsumeEv(ev_st, app);

	WSRead();

	return RESULT_OK;
}


UINT32 HandleExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	dbg("HandleExit\n");
	//cprint("ScrShot: HandleExit\n");

	app->exit_status = TRUE;

	APP_ConsumeEv(ev_st, app);

	return RESULT_OK;
}


/* EOF */
