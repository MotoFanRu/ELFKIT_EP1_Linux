#include "sock_api.h"
#include "sock_utils.h"

// MY Socket API ----------------------------------------------

HSOCK socket( int type, UINT32 ev_callback )
{
	int		idx;

	HSOCK	hsock;
	dbgf("create socket, type = %d, ev_cb = 0x%x\n", type, ev_callback);

	if ( type != SOCK_TCP && type != SOCK_UDP )
		return HSOCK_INVALID;

	if ( ev_callback == 0 || ev_callback == STATE_HANDLERS_END )
		return HSOCK_INVALID;

	idx = util_find_free_sock();
	if ( idx == -1 )
		return HSOCK_INVALID;

	hsock = next_hsock++;
	// еще раз почистим
	memclr(&socket_tbl[idx], sizeof(SOCKET_T));
	socket_tbl[idx].hsock = hsock;
	socket_tbl[idx].ev_callback = ev_callback;
	socket_tbl[idx].tra_prot = (type == SOCK_UDP ? PROTOCOL_UDP : PROTOCOL_TCP);

    //эти параметры постоянны
	socket_tbl[idx].net_prot = PROTOCOL_IP;
	socket_tbl[idx].link_prot = LINK_GPRS;
	socket_tbl[idx].phys_prot = USE_GPRS;

	socket_tbl[idx].loc_port = 0;
	socket_tbl[idx].cmd = DL_DSMA_ProxyCreate( port );
	// следующий ивент о создании прокси
	socket_tbl[idx].state = SOCK_STATE_CREATE;
	socket_tbl[idx].last_activity = getTime();
	socket_tbl[idx].activate_counts = 0;

	dbgf("create socket. cmd = %x", socket_tbl[idx].cmd);
	return hsock;
}


int socket_connect( HSOCK hsock, const char * host, int port )
{
	int		idx;
	UINT32	len;
	CMD_NUM	cmd;

	idx = util_find_sock(hsock);
	dbgf("connect socket %d, idx:%d to %s:%d\n", hsock, idx, host, port);
	if ( idx == -1 || socket_tbl[idx].proxy_id == PROXY_NULL )
		return -1;

	len = strlen((char*)host);
	if ( len > MAX_HOST_LEN )
		return -1;

	strcpy(socket_tbl[idx].host, (char*)host);
	socket_tbl[idx].rem_port = port;
	len = util_init_attr(idx);
	cmd = DL_DSMA_ProxyAttributeSet( socket_tbl[idx].proxy_id, len, &attrs[idx][0] );
	socket_tbl[idx].cmd = cmd;
	// следующий ивент о параметрах прокси
	socket_tbl[idx].state = SOCK_STATE_SET_ATTR;
	socket_tbl[idx].last_activity = getTime();
	socket_tbl[idx].activate_counts = 0;

	return 0;
}


int socket_bind( HSOCK hsock, int loc_port )
{
	int idx = util_find_sock(hsock);
	dbgf("bind socket %d, idx:%d to port %d\n", hsock, idx, loc_port);
	if ( idx == -1 )
		return -1;
	// я так понял, что мы должны привязаться к порту
	socket_tbl[idx].loc_port = loc_port;
	socket_tbl[idx].last_activity = getTime();
	return 0;
}


int socket_close( HSOCK hsock )
{
	int idx = util_find_sock(hsock);

	dbgf("close socket %d, idx:%d\n", hsock, idx);
	if ( idx == -1 || socket_tbl[idx].proxy_id == PROXY_NULL )
		return -1;

	// не закрывать gprs сессию, но разорвать соединение с удаленным хостом
	socket_tbl[idx].cmd = DL_DSMA_ProxyDeactivateSocket( socket_tbl[idx].proxy_id );
	socket_tbl[idx].last_activity = getTime();
	socket_tbl[idx].activate_counts = 0;

	return 0;
}


int socket_delete( HSOCK hsock )
{
	int idx = util_find_sock(hsock);

	dbgf("delete socket %d, idx:%d\n", hsock, idx);
	if ( idx == -1 )
		return idx;

	util_delete_sock(idx);

	return 0;
}


UINT32 socket_write( HSOCK hsock, const void * buf, UINT32 size )
{
	int			idx = util_find_sock(hsock);
	UINT32		wr;

	dbgf("write to socket %d, idx:%d\n", hsock, idx);
	if ( idx == -1 || socket_tbl[idx].proxy_id == PROXY_NULL )
		return -1;

	if ( socket_tbl[idx].state == SOCK_STATE_READY )
		wr = DL_DSMA_ProxyWrite( socket_tbl[idx].proxy_id, (BYTE *)buf, size );
	else
		wr = -1;

	socket_tbl[idx].last_activity = getTime();

	return wr;
}


UINT32 socket_read( HSOCK hsock, void * buf, UINT32 size )
{
	int			idx = util_find_sock(hsock);
	UINT32		rd;

	dbgf("read from socket %d, idx:%d\n", hsock, idx);
	if ( idx == -1 || socket_tbl[idx].proxy_id == PROXY_NULL )
		return -1;

	if ( socket_tbl[idx].state == SOCK_STATE_READY
			|| socket_tbl[idx].state == SOCK_STATE_NOWRITE )
		rd = DL_DSMA_ProxyRead( socket_tbl[idx].proxy_id, buf, size );
	else
		rd = -1;

	socket_tbl[idx].last_activity = getTime();

	return rd;
}
