#include "sock_utils.h"

int util_find_sock( HSOCK hsock )
{
	int i;

	if ( hsock == HSOCK_INVALID )
		return -1;
	for ( i=0; i<MAX_SOCKETS; i++ ) {
		if ( hsock == socket_tbl[i].hsock )
			return i;
	}

	return -1;
}


int util_find_sock_by_proxy( UINT32 proxy_id )
{
	int i;

	if ( proxy_id == PROXY_NULL )
		return -1;
	for ( i=0; i<MAX_SOCKETS; i++ ) {
		if ( proxy_id == socket_tbl[i].proxy_id )
			return i;
	}

	return -1;
}


int util_find_sock_by_cmd( UINT32 cmd )
{
	int i;

	for ( i=0; i<MAX_SOCKETS; i++ ) {
		if ( cmd == socket_tbl[i].cmd )
			return i;
	}

	return -1;
}


int util_delete_sock( int idx )
{
	if ( idx < 0 || idx >= MAX_SOCKETS )
		return -1;

	if ( socket_tbl[idx].proxy_id != PROXY_NULL )
		DL_DSMA_ProxyDestroy(socket_tbl[idx].proxy_id);

	socket_tbl[idx].proxy_id = PROXY_NULL;
	socket_tbl[idx].hsock = HSOCK_INVALID;
	socket_tbl[idx].state = 0;
	socket_tbl[idx].activate_counts = 0;

	return 0;
}


int util_find_free_sock( void )
{
	int i;

	for ( i=0; i<MAX_SOCKETS; i++ ) {
		if ( socket_tbl[i].hsock == HSOCK_INVALID )
			return i;
	}

	return -1;
}


int util_init_attr( int idx )
{
	int		i = 0;


	memclr(&attrs[idx], sizeof(DSMA_ATTRIBUTE_T)*16);
	// TCP / UDP
	attrs[idx][i].attr = DSMA_ATTR_TRANS_PROT;
	attrs[idx][i].value = &socket_tbl[idx].tra_prot;
	attrs[idx][i++].size = sizeof(UINT32);
	// IP
	attrs[idx][i].attr = DSMA_ATTR_NET_PROT;
	attrs[idx][i].value = &socket_tbl[idx].net_prot;//net_protocol;
	attrs[idx][i++].size = sizeof(UINT32);
	// ?
	attrs[idx][i].attr = DSMA_ATTR_LINK_PROT;
	attrs[idx][i].value = &socket_tbl[idx].link_prot;
	attrs[idx][i++].size = sizeof(UINT32);
	// GPRS, другие не поддерживаем
	attrs[idx][i].attr = DSMA_ATTR_PHYS_PROT;
	attrs[idx][i].value = &socket_tbl[idx].phys_prot;
	attrs[idx][i++].size = sizeof(UINT32);
	// точка доступа
	attrs[idx][i].attr = DSMA_ATTR_APN;
	attrs[idx][i].value = ws_info.apn;
	attrs[idx][i++].size = strlen(ws_info.apn);
	// login
	attrs[idx][i].attr = DSMA_ATTR_USER_NAME;
	attrs[idx][i].value = ws_info.login;
	attrs[idx][i++].size = strlen(ws_info.login);
	// password
	attrs[idx][i].attr = DSMA_ATTR_PW;
	attrs[idx][i].value = ws_info.password;
	attrs[idx][i++].size = strlen(ws_info.password);
	// к кому коннект
	attrs[idx][i].attr = DSMA_ATTR_REMOTE_IP;
	attrs[idx][i].value = socket_tbl[idx].host;
	attrs[idx][i++].size = strlen(socket_tbl[idx].host);
	// на какой порт стучимся
	attrs[idx][i].attr = DSMA_ATTR_REMOTE_PORT;
	attrs[idx][i].value = &socket_tbl[idx].rem_port;
	attrs[idx][i++].size = sizeof(UINT32);
	// на каком порте ловим входящие
	if ( socket_tbl[idx].loc_port != 0 ) {
		attrs[idx][i].attr = DSMA_ATTR_LOCAL_PORT;
		attrs[idx][i].value = &socket_tbl[idx].loc_port;
		attrs[idx][i++].size = sizeof(UINT32);
	}

	return i;
}


// убить сокет и оповестить клиента
int delete_and_notify( int i )
{
	ADD_EVENT_DATA_T add_data;
	add_data.data_tag = 0;

	if ( socket_tbl[i].hsock != HSOCK_INVALID )
	{
		add_data.data.params[0] = SOCK_ANSW_DELETE;
		add_data.data.params[1] = socket_tbl[i].hsock;
		SocketAnswer(socket_tbl[i].ev_callback, add_data);
		util_delete_sock(i);
	}

	return 0;
}


UINT32 WSRead( void )
{
	UINT8 			index;
	IFACE_DATA_T	iface_data;
	UINT32			status;

	iface_data.port = port;

	if(ws_info.ws_rec.gprs_record)
        mfree(ws_info.ws_rec.gprs_record);

	__rt_memset(&ws_info, 0, sizeof(WEB_SESSION_INFO_T));

	ws_info.ws_rec.gprs_record = (void*)malloc( sizeof(WS_GPRS_RECORD_T) );
	if ( !ws_info.ws_rec.gprs_record ) return RESULT_FAIL;

	// Получение индекса текущей Web-сессии
	status = DL_DbWebSessionsGetDefaultSessionIndex( &index );
	dbgf("DefaultSessionIndex, status = %d, index = %d\n", status, index);
	if ( status != WS_RESULT_OK ) return RESULT_FAIL;

	// Получение записи Web-сессии по индексу
	// функция асинхронная. ответ придёт по ивенту EV_WEBSESSION_OPERATION,  в аттаче будет WS_EVENT_T
	status = DL_DbWebSessionsGetSessionByIndex( &iface_data, index, &ws_info.ws_rec);
	dbgf("GetSessionByIndex, status = %d\n", status);

	if ( status == WS_RESULT_OK ) // вдруг сразу прочитал
		return WSAccept();

	if ( status >= WS_RESULT_FAILED ) // >= ошибка
		return RESULT_FAIL;

	// WS_RESULT_IN_PROGRESS
	// иначе ждем ивент EV_WEBSESSION_OPERATION

	return RESULT_OK;;
}


UINT32 WSAccept( void )
{
	switch ( ws_info.ws_rec.timeout )
	{
		case WS_TIMEOUT_NULL:
			ws_info.timeout = 0;
		break;

		case WS_TIMEOUT_1_MIN:
			ws_info.timeout = 1*60000;
		break;

		case WS_TIMEOUT_2_MIN:
			ws_info.timeout = 2*60000;
		break;

		case WS_TIMEOUT_5_MIN:
			ws_info.timeout = 5*60000;
		break;

		case WS_TIMEOUT_10_MIN:
			ws_info.timeout = 10*60000;
		break;

		case WS_TIMEOUT_15_MIN:
			ws_info.timeout = 15*60000;
		break;
	}

	u_utoa( ws_info.ws_rec.gprs_record->apn, ws_info.apn); // apn
	u_utoa( ws_info.ws_rec.gprs_record->login, ws_info.login); // login
	u_utoa( ws_info.ws_rec.gprs_record->password, ws_info.password); // pass

	dbgf("timeout: %d\napn: %s\nlogin:%s\npassword: %s\n", ws_info.timeout, ws_info.apn, ws_info.login, ws_info.password);

	mfree(ws_info.ws_rec.gprs_record);
	ws_info.ws_rec.gprs_record = NULL;

	return RESULT_OK;
}

/* EOF */
