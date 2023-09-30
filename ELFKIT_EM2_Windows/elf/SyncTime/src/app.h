#define DEBUG

#ifndef APP_H
#define APP_H

#include <apps.h>
#include <loader.h>
#include <filesystem.h>
#include <synsock_api.h>
#include <resources.h>
#include <time_date.h>
#include <utilities.h>
#include <uis.h>

typedef struct
{
	APPLICATION_T apt;
} APP_T;

typedef enum
{
	HW_STATE_ANY,
	HW_STATE_MAIN,
    HW_STATE_MSG,
	HW_STATE_MAX
} HW_STATES_T;

UINT32 ELF_Entry (ldrElf *ela, WCHAR * params);
UINT32 ELF_Start( EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info );
UINT32 ELF_Exit( EVENT_STACK_T *ev_st,  APPLICATION_T * app );
UINT32 HandleExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T * app,  ENTER_STATE_TYPE_T type );
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T * app,  ENTER_STATE_TYPE_T type );
UINT32 MsgStateEnter( EVENT_STACK_T * ev_st, APPLICATION_T * app, ENTER_STATE_TYPE_T type );
UINT32 StateExit( EVENT_STACK_T *ev_st,  APPLICATION_T * app,  EXIT_STATE_TYPE_T type );

UINT32 HandleAPIAnsw( EVENT_STACK_T *ev_st, APPLICATION_T * app );
UINT32 HandleSockAnsw( EVENT_STACK_T *ev_st, APPLICATION_T * app );

UINT32 ReadConfig (DL_FS_MID_T *id);
UINT32 SyncTime( char *buf );
#endif
