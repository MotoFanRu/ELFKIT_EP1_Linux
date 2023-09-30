#define DEBUG

#ifndef SOCK_API_H
#define SOCK_API_H

#include <apps.h>
#include <dsma.h>
#include <synsock_api.h>
#include <time_date.h>
#include <utilities.h>

#define getTime() (suPalTicksToMsec(suPalReadTime()))

extern const char app_name[];

// SOCKET API
HSOCK	socket( int type, UINT32 ev_callback );
int		socket_connect( HSOCK hs, const char * host, int port );
UINT32	socket_write( HSOCK hs, const void * buf, UINT32 size );
UINT32	socket_read( HSOCK hs, void * buf, UINT32 size );
int		socket_bind( HSOCK hs, int loc_port );
int		socket_close(HSOCK hs);
int		socket_delete( HSOCK hs );

#endif
