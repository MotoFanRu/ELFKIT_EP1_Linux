#define DEBUG

#ifndef SOCK_UTILS_H
#define SOCK_UTILS_H

#include <dl.h>
#include <mem.h>
#include <dsma.h>
#include <trace.h>
#include <events.h>
#include <synsock_api.h>
#include <websessions.h>



#define SocketAnswer(ev_code,add_data) \
        AFW_CreateInternalQueuedEvAuxD(ev_code,&add_data,FBF_LEAVE,0,NULL)



typedef struct
{
    //
    WS_RECORD_T         ws_rec;

	UINT32				timeout; // время существования сокета
	char				apn[WS_RECORD_APN_LEN + 1];
	char				login[WS_RECORD_LOGIN_LEN + 1];
	char				password[WS_RECORD_PASS_LEN + 1];
} WEB_SESSION_INFO_T;

enum
{
	SOCK_STATE_NULL = 0,
	SOCK_STATE_CREATE,
	SOCK_STATE_SET_ATTR,
	SOCK_STATE_ACTIVATE,
	SOCK_STATE_READY,
	SOCK_STATE_ERROR,

	// tim apple mod
	SOCK_STATE_NOWRITE,
	SOCK_STATE_CLOSED
};
typedef UINT8 SOCK_STATE_T;

typedef struct
{
	HSOCK			hsock;
    UINT32			ev_callback;
	SOCK_STATE_T	state;

	CMD_NUM			cmd;
	UINT32			proxy_id;
    char			host[MAX_HOST_LEN + 1];
	UINT32			tra_prot;

    //Следующие поля одинаковы для всех.
	UINT32          net_prot;               //сетевой протокол. одинаков для всех. PROTOCOL_IP
	UINT32          link_prot;              //???. одинаков для всех. LINK_GPRS
	UINT32          phys_prot;              //физический протокол. одинаков для всех. USE_GPRS

	int				loc_port;
	int				rem_port;

	UINT32			last_activity;
	UINT32			activate_counts;

} SOCKET_T;


extern const char app_name[];

extern SOCKET_T				socket_tbl[MAX_SOCKETS];
extern DSMA_ATTRIBUTE_T		attrs[MAX_SOCKETS][16];
extern HSOCK				next_hsock;
extern SU_PORT_T			port;
extern WEB_SESSION_INFO_T   ws_info;

int util_find_sock( HSOCK hsock );
int util_find_sock_by_proxy( UINT32 proxy_id );
int util_find_sock_by_cmd( UINT32 cmd );
int util_delete_sock( int idx );
int util_find_free_sock( void );
int util_init_attr( int idx );
int delete_and_notify( int i );

UINT32 WSRead( void );
UINT32 WSAccept( void );

#endif /* __SOCK_UTILS_H__ */
