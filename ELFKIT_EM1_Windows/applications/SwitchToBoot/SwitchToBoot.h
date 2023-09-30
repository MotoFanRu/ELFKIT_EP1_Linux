#ifndef SWITCHTOBOOT_H
#define SWITCHTOBOOT_H

#include <apps.h>
#include <uis.h>
#include <dl.h>

typedef struct
{ 
    APPLICATION_T           apt;

} APP_SWITCHTOBOOT_T;

typedef enum
{
    HW_STATE_ANY,

    HW_STATE_INIT,
    HW_STATE_MAIN,

    HW_STATE_MAX
} HW_STATES_T;

UINT32 SwitchToBootStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 SwitchToBootExit( EVENT_STACK_T *ev_st,  void *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app );

#endif

