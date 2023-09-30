#ifndef MULTIBKG_H
#define MULTIBKG_H

#include <apps.h>
#include <uis.h>
#include <dl.h>
#include <loader.h>

typedef struct
{ 
    APPLICATION_T           apt; 
} APP_MULTIBKG_T;

typedef enum
{
    HW_STATE_ANY,
    HW_STATE_MAIN,
   
    HW_STATE_MAX
} HW_STATES_T;

UINT32 MultiBkgStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 MultiBkgExit( EVENT_STACK_T *ev_st,  void *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app );

UINT32 Timer( EVENT_STACK_T *ev_st,  void *app );

UINT32 FlipOpen( EVENT_STACK_T *ev_st,  void *app );
UINT32 FlipClose( EVENT_STACK_T *ev_st,  void *app );

typedef struct
{ 
    WCHAR       u_fullname[256];
} FILEINFO;


#endif
