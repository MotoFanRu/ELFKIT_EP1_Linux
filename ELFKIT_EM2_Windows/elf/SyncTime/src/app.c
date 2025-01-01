// Daytime Protocol (RFC-867)
// by om2804
// port by tim apple
#include "app.h"
#include "iniparser.h"

// ���� �������� ������ �������
socket_f *				socket = NULL;
socket_connect_f *		socket_connect = NULL;
socket_write_f *		socket_write = NULL;
socket_read_f *			socket_read = NULL;
socket_bind_f *			socket_bind = NULL;
socket_close_f *		socket_close = NULL;
socket_delete_f *		socket_delete = NULL;

INT8 UTC;
HSOCK sock;
INT32 port = 13;
UINT8 msg_status;
char Server[64]; //time.nist.gov. see others at http://tf.nist.gov/timefreq/service/time-servers.html

ldrElf *elf = NULL;
const char app_name[APP_NAME_LEN] = "SyncTime";

// � ����� http ������� 2 ������� �������� ������, ����� ��� ���� ������ �������,
//  �� � �������� GET ��� ������, ������� ������ ����� �������
// ��� POST ���� ������ � ������ ���� ��������� "Content-Length", ������� ��������� ���������� ������
//char sock_write_data[128];


static EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
	{ STATE_HANDLERS_PM_API_EXIT,	HandleExit			},
	{ STATE_HANDLERS_END,		    NULL				},
};

static EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
	{ STATE_HANDLERS_RESERVED,      HandleAPIAnsw		},
	{ STATE_HANDLERS_RESERVED,      HandleSockAnsw		},
	{ STATE_HANDLERS_END,		    NULL				},
};

static EVENT_HANDLER_ENTRY_T msg_state_handlers[] =
{
	{ EV_DIALOG_DONE,			    HandleExit			},
	{ STATE_HANDLERS_END,		    NULL				},
};

const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
	{ HW_STATE_ANY,			// State
	  NULL,					// ���������� ����� � state
	  NULL,					// ���������� ������ �� state
	  any_state_handlers	// ������ ������������ �������
	},

	{ HW_STATE_MAIN,
	  MainStateEnter,
	  StateExit,
	  main_state_handlers
	},

	{ HW_STATE_MSG,
	  MsgStateEnter,
	  StateExit,
	  msg_state_handlers
	}
};

UINT32 ELF_Entry (ldrElf *ela, WCHAR * params)
{
	UINT32			status = RESULT_OK;
	UINT32			evcode_base;

    elf = ela;
    elf->name = (char *)app_name;

	if(ldrIsLoaded((char*)app_name))
	{
		dbg("Already loaded");
		return RESULT_FAIL;
	}

	evcode_base = elf->evbase + 1;
	evcode_base = ldrInitEventHandlersTbl( (EVENT_HANDLER_ENTRY_T *)main_state_handlers, evcode_base );
	evcode_base = ldrInitEventHandlersTbl( (EVENT_HANDLER_ENTRY_T *)any_state_handlers, evcode_base );
	evcode_base = ldrInitEventHandlersTbl( (EVENT_HANDLER_ENTRY_T *)msg_state_handlers, evcode_base );

	status = APP_Register(&elf->evbase,	1, state_handling_table, HW_STATE_MAX, (void*)ELF_Start);
    if(status == RESULT_OK)
    {
        ldrSendEvent(elf->evbase);
    }

	return status;
}


UINT32 ELF_Start( EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info )
{
	APP_T *app = NULL;
	UINT32 status = RESULT_OK;

	if ( AFW_InquireRoutingStackByRegId( reg_id ) == RESULT_OK )
	{
	    PFprintf("%s: Already loaded!\n", app_name);
		return RESULT_OK; // ��� �� ����� ������ ���������
	}

	app = (APP_T *)APP_InitAppData((void *)APP_HandleEvent, sizeof(APP_T), reg_id, 0, 1, 1, 1, 1, 0 );
	if(!app)
	{
	    PFprintf("%s: Initialization failed\n", app_name);
	    ldrUnloadElf(elf);
	    return RESULT_OK;
	}
    elf->app = &app->apt;

    sock = HSOCK_INVALID;
	ReadConfig(&elf->id);

	status = APP_Start(ev_st, &app->apt, HW_STATE_MAIN, state_handling_table, ELF_Exit, app_name, 0 );
	if(status == RESULT_OK)
	{
		PFprintf("%s: Started\n", app_name);
	}
	else
	{
		PFprintf("%s: Start failed\n", app_name);
		APP_HandleFailedAppStart(ev_st, (APPLICATION_T*)app, 0);
		ldrUnloadElf(elf);
		return RESULT_FAIL;
	}

	return RESULT_OK;
}

UINT32 ELF_Exit( EVENT_STACK_T *ev_st, APPLICATION_T *app )
{
	UINT32	status;
	if ( sock != HSOCK_INVALID )
	{
			// ��������� ����������
			status = socket_close( sock );
			dbgf("socket_close DONE, status = %d", status);
			status = socket_delete( sock );
			dbgf("socket_delete DONE, status = %d", status);
	}

	status = APP_ExitStateAndApp( ev_st, app, NULL );
	ldrUnloadElf(elf);

	return status;
}

UINT32 MainStateEnter( EVENT_STACK_T * ev_st, APPLICATION_T * app, ENTER_STATE_TYPE_T type )
{
	APPLICATION_T			*papp = (APPLICATION_T*)app;
	ADD_EVENT_DATA_T		add_data;
	CONTENT_T				content;

	if ( type != ENTER_STATE_ENTER ) return RESULT_OK;

	UIS_MakeContentFromString( "MCs0",  &content,  LANG_SYNC_WITH_SERVER );
	papp->dialog = UIS_CreateTransientNotice( &papp->port, &content, NOTICE_TYPE_WAIT );
	if ( papp->dialog == NULL ) return RESULT_FAIL;

	// ������ API � SynSock
	add_data.data_tag = 0;
	add_data.data.params[0] = ldrFindEventHandlerTbl( main_state_handlers, HandleAPIAnsw );
	SockReqAPI(add_data);

	return RESULT_OK;
}


/* ���������� ����� � state */
UINT32 MsgStateEnter( EVENT_STACK_T * ev_st, APPLICATION_T * app, ENTER_STATE_TYPE_T type )
{
	CONTENT_T content;
	if ( type != ENTER_STATE_ENTER ) return RESULT_OK;

	if ( msg_status == RESULT_FAIL )
	{
		UIS_MakeContentFromString( "MCs0",  &content,  LANG_ERROR );
		app->dialog = UIS_CreateTransientNotice( &app->port,  &content,  NOTICE_TYPE_FAIL );
	}
	else
	{
		UIS_MakeContentFromString( "MCs0",  &content,  LANG_COMPLETE );
		app->dialog = UIS_CreateTransientNotice( &app->port,  &content,  NOTICE_TYPE_OK );
	}

	if ( app->dialog == NULL ) return RESULT_FAIL;

	return RESULT_OK;
}


UINT32 StateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	if(type == EXIT_STATE_SUSPEND)
        return RESULT_OK;

	APP_UtilUISDialogDelete(&app->dialog);
	return RESULT_OK;
}


UINT32 HandleAPIAnsw( EVENT_STACK_T * ev_st, APPLICATION_T * app )
{
	EVENT_T *	event = AFW_GetEv(ev_st);
	int			i;


	dbg("APIAnsw: Enter");

	// ��������, ����� ������� ������� ���. ���� ���, �� ����� �� �����
	for ( i=0; i<7; i++ )
	{
		if ( event->data.params[i] == NULL )
		{
			return HandleExit(ev_st, app);
		}
	}

	socket = (socket_f *)(event->data.params[0]);
	socket_connect = (socket_connect_f *)(event->data.params[1]);
	socket_write = (socket_write_f *)(event->data.params[2]);
	socket_read = (socket_read_f *)(event->data.params[3]);
	socket_bind = (socket_bind_f *)(event->data.params[4]);
	socket_close = (socket_close_f *)(event->data.params[5]);
	socket_delete = (socket_delete_f *)(event->data.params[6]);

	APP_ConsumeEv( ev_st, app );


	if ( sock != HSOCK_INVALID ) return RESULT_OK; // ����� ��� ������

	 dbg("create socket...");

	// ������� �����, ���������� �������������, �� ���� ���������� � ������
	// �� ����� ���� ����� ����� ������ �� �����, �� �������� �������� ������ ����� �������� ����� �����
	sock = socket(SOCK_TCP, ldrFindEventHandlerTbl( main_state_handlers, HandleSockAnsw ));

	// ����� ����� ���� ��� �� ������, ���� ��� ��������� ���� ��� �������� �������� ������
	// ������������ ���-�� ��������� ������� ����� ���� 8
	if ( sock == HSOCK_INVALID )
	{
		dbg("can't create socket");
		msg_status=RESULT_FAIL;
		return APP_UtilChangeState( HW_STATE_MSG,  ev_st,  app );
	}

	return RESULT_OK;
}


UINT32 HandleSockAnsw( EVENT_STACK_T *ev_st, APPLICATION_T *app )
{
	//UINT32		status;
	EVENT_T			*event = AFW_GetEv(ev_st);
	UINT32			answ = event->data.params[0];
	UINT32			status = event->data.params[1];
	char			request[128];
	char			*data;
	UINT32			wr;
	INT32			len;

	APP_ConsumeEv( ev_st, app );

	if ( answ == SOCK_ANSW_CREATE )
	{
		// ����� ������
		dbgf("SockAnsw: CREATE, status = %d", status);

		// ��������� ����������
		if ( sock != HSOCK_INVALID )
		{

			if ( socket_connect(sock, Server, port) != -1 )
			{
				dbg("socket_connect OK...");
			}
			else
			{
				dbg("socket_connect ERROR");
				msg_status=RESULT_FAIL;
				APP_UtilChangeState( HW_STATE_MSG,  ev_st,  app );
			}
		}
	}
	else if ( answ == SOCK_ANSW_READY )
	{
		// ����� ����� � ������
		dbg("SockAnsw: READY");

		// ������ � ����� �������
		if ( sock != HSOCK_INVALID )
		{
			strcpy(request, "\r\n");
			data = (char*)request;

			len = strlen(data);		// �������� ��������
			while ( len > 0 )
			{
				wr = socket_write(sock, data, len);
				dbgf("socket_write DONE, %d", wr);
				if ( wr == -1 )
				{
					// ��� ������ ������
					dbgf("socket is not ready %d", sock);
					break;
				}
				else if ( wr == 0 && len > 0 )
				{
					// �� �������...
					dbg("can't write to socket");
					break;
				}
				len -= wr;
				data+= wr;
			}
		}
	}
	else if ( answ == SOCK_ANSW_DATA )
	{
		// �������� ������
		dbg("SockAnsw: DATA");

		if ( sock != HSOCK_INVALID )
		{
			len = 128;
			len = socket_read( sock, request, 128 );
			if (SyncTime(request) == RESULT_OK)
			{
				msg_status=RESULT_OK;
			}
			else
			{
				msg_status=RESULT_FAIL;
			}
			dbgf("socket_read DONE, len = %d\n", len);
			APP_UtilChangeState( HW_STATE_MSG,  ev_st,  app );
		}
	}
	else if ( answ == SOCK_ANSW_DELETE )
	{
		// ����� ������ ��-�� ��������
		dbg("SockAnsw: DELETE");
		sock = HSOCK_INVALID;
		msg_status=RESULT_FAIL;
		APP_UtilChangeState( HW_STATE_MSG,  ev_st,  app );
	}
	else if ( answ == SOCK_ANSW_ERROR )
	{
		// ������
		dbg("SockAnsw: ERROR");
		msg_status=RESULT_FAIL;
		APP_UtilChangeState( HW_STATE_MSG,  ev_st,  app );
	}


	return RESULT_OK;
}


// ���� ���-�� ���� � ������
UINT8 MaxDays(UINT8 month, UINT16 year)
{
	if (month == 1 || month == 3 || month == 5 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
		return 31;
	} else if (month == 2) { // � ������� 28-29 ����
		if (year%4)      return 28;
		else             return 29;
	}

	return 30;
}


UINT32 SyncTime(char *buf)
{
	UINT32			t0;
	UINT32			dy,dm,dd, th,tm,ts;
	UINT32			tt,l,h;
	CLK_DATE_T		date;
	CLK_TIME_T		time;
	int				num;
	INT8			hour;


	dbgf("%s", buf);

	// Daytime Protocol (RFC-867) http://tf.nist.gov/service/its.htm
	//format ACTS : JJJJJ YR-MO-DA HH:MM:SS TT L H msADV UTC(NIST) OTM
	//example of ans: 54462 07-12-28 18:55:44 00 0 0 744.7 UTC(NIST) *
	num = sscanf (buf,"%d %d-%d-%d %d:%d:%d %d %d %d",&t0,&dy,&dm,&dd,&th,&tm,&ts,&tt,&l,&h);
	dbgf("num = %d", num);
	if (h > 2 || num < 7) return RESULT_FAIL; // �������� ������� ��������� ������ ������� :( ����������� ����� 5 ���


	dbgf("%d", t0);
	dbgf("%d", dy);
	dbgf("%d", dm);
	dbgf("%d", dd);
	dbgf("%d", th);
	dbgf("%d", tm);
	dbgf("%d", ts);
	dbgf("%d", tt); // ������, ������ �����


	//date.year = 2000 + dy;
	//date.month = dm;
	//date.day = dd;

	hour = (INT8)th + UTC; // � ���������� �������� �����
	// ��������� ��� �������, ������� �������
	if (tt >= 50) hour++;

	if (th < 24 && hour >= 24) { // ������� ���� �� ���� ���� ������
		if (dd+1 > MaxDays(dm, 2000 + dy)) {
			// ������������ ����� � ���
			if (dm + 1 > 12) {
			   date.year = 2000 + dy + 1; // ����� ���
			   date.month = 1;
			   date.day = 1;
			} else {
			   date.year = 2000 + dy;
			   date.month = dm + 1;
			   date.day = 1;
			}
		} else {
			date.year = 2000 + dy;
			date.month = dm;
			date.day = dd+1;
		}
	}

	if (hour < 0) {
		time.hour = 24 + hour;
	} else if (hour > 23) {
		time.hour = hour - 24;
	} else {
		time.hour = hour;
	}

	time.minute = tm;
	time.second = ts;

	dbgf("date %d-%d-%d", date.day, date.month, date.year);
	dbgf("time %d:%d:%d", time.hour, time.minute, time.second);

	DL_ClkSetDate( date );
	DL_ClkSetTime( time );

	return RESULT_OK;
}


UINT32 HandleExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	APP_ConsumeEv( ev_st, app );
	app->exit_status = TRUE;

	return RESULT_OK;
}

UINT32 ReadConfig (DL_FS_MID_T *id)
{
    WCHAR *ptr, ini_uri[FS_MAX_URI_NAME_LENGTH + 1];
    char utc_string[5];

    memclr(Server, 64 * sizeof(char));
    memclr(utc_string, 5 * sizeof(char));
    memclr(ini_uri, (FS_MAX_URI_NAME_LENGTH + 1) * sizeof(WCHAR));

    u_strcpy(ini_uri, L"file:/");
    DL_FsGetURIFromID(&elf->id, (ini_uri + 6));

    ptr = ini_uri + u_strlen(ini_uri);
    while(*ptr != L'/') ptr--;
    u_strcpy(ptr + 1, L"synctime.ini\0");

	if(INI_Open(ini_uri) == INI_RESULT_OK)
	{
		char *end;
		INI_SetSeparator('=');
		INI_ReadParameter(NULL, "server", "192.43.244.18", Server, 64);
        INI_ReadParameter(NULL, "utc", "0" , utc_string, 4);
		UTC = (INT8)strtol( utc_string, &end,  10 );
		dbgf("Server = %s", Server);
		dbgf("UTC = %s", utc_string);
		INI_Close();
	}

    return RESULT_OK;
}
