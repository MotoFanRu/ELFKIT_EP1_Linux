#define DEBUG

#ifndef SYNSOCK_H
#define SYNSOCK_H

#include <apps.h>
#include <loader.h>
#include <dsma.h>
#include <synsock_api.h> // это нужно подключть к другим проектам
#include <websessions.h>
#include <time_date.h>
#include <utilities.h>

#define ACTIVATE_COUNTS_MAX				3       // кол-во попыток создать соединение прежде чем выдать ошибку
#define SOCKETS_ACTIVITY_TIMER_ID		0       // id таймера
#define SOCKETS_ACTIVITY_TIMER_DELAY	10*1000 // время задержки

#define EV_WEBSESSION_OPERATION 0x82143
#define EV_WEBSESSION_CHANGE 0xf05

typedef struct
{
	APPLICATION_T	apt;
} APP_ELF_T;

typedef enum
{
	HW_STATE_ANY,
	HW_STATE_MAX
} HW_STATES_T;

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start( EVENT_STACK_T * ev_st, REG_ID_T reid, REG_INFO_T *reg_info );
UINT32 ELF_Init( APPLICATION_T *app );
UINT32 ELF_Exit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 NET_HandleProxyEvent( EVENT_STACK_T * ev_st, APPLICATION_T * app );
UINT32 HandleIfaceReq( EVENT_STACK_T * ev_st, APPLICATION_T * app );
UINT32 HandleTimerExpiried( EVENT_STACK_T * ev_st, APPLICATION_T * app );
UINT32 HandleWSOperation( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 HandleWSChange( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 HandleExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

#endif
