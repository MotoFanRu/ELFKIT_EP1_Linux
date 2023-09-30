#ifndef INFO_H
#define INFO_H

#include <apps.h>
#include <uis.h>
#include <dl.h>
#include <loader.h>

typedef struct
{ 
    APPLICATION_T           apt; 
} APP_INFO_T;

typedef enum
{
    HW_STATE_ANY,
    HW_STATE_MAIN,
   
    HW_STATE_MAX
} HW_STATES_T;

typedef struct
{
    GRAPHIC_METRIC_T img_size;
} IMG_FONT_T;

UINT32 InfoStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 InfoExit( EVENT_STACK_T *ev_st,  void *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app );

UINT32 Timer( EVENT_STACK_T *ev_st,  void *app );

UINT32 FlipOpen( EVENT_STACK_T *ev_st,  void *app );
UINT32 FlipClose( EVENT_STACK_T *ev_st,  void *app );
UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  void * app );
UINT32 KeyRelease( EVENT_STACK_T *ev_st,  void *app );

UINT32 DrawStrWT( char * str, UINT16 x, UINT16 y, BOOL );

UINT32 LoadFont( );
void UpdateInfo( );


#endif
