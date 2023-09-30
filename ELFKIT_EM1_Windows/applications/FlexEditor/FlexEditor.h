#ifndef FLEXEDITOR_H
#define FLEXEDITOR_H

#include <apps.h>
#include <uis.h>
#include <dl.h>
#include <loader.h>
#include <lang_strings.h>
#include <drm_icons.h>

typedef struct
{ 
    APPLICATION_T           apt; 
} APP_FLEXEDITOR_T;

typedef enum
{
    HW_STATE_ANY,

    HW_STATE_INIT,
    HW_STATE_MAIN,
    
    HW_STATE_MAX
} HW_STATES_T;

typedef enum
{
	RES_LIST_CAPTION=0,
	RES_SOFTKEY_LABEL,
	RES_ACTION1,
	RES_ICON_UN,
	RES_ICON_CK,
	RES_MAX
} HW_RESOURCE;

RESOURCE_ID * Resources = NULL;

UINT32 FlexEditorStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 FlexEditorExit( EVENT_STACK_T *ev_st,  void *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );
UINT32 StateExit( EVENT_STACK_T *ev_st,  void *app,  EXIT_STATE_TYPE_T type );

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app );
UINT32 NoticeStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );
UINT32 NoticeExit( EVENT_STACK_T *ev_st,  void *app );
UINT32 SendListItems( EVENT_STACK_T *ev_st,  void *app, UINT32 start, UINT32 num);

UINT32 InitResources( );
UINT32 InitDlgActions( ACTIONS_T *action_list );
UINT32 RemoveResources( );

UINT32 Select( EVENT_STACK_T *ev_st,  void *app);


#endif
