#ifndef APP__H
#define APP__H

#include <apps.h>
#include <loader.h>
#include <utilities.h>
#include "dbg.h"

#include "main.h"
#include "resources.h"


typedef enum
{
	APP_STATE_ANY,

	APP_STATE_INIT,
	APP_STATE_MAIN,

	APP_STATE_MAX
} APP_STATES_T;


ldrElf* _start( WCHAR *uri, WCHAR *params );

// PRIVATE

UINT32 HandleUITokenGranted( EVENT_STACK_T * ev_st,  APPLICATION_T * app );
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 MainStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );


#endif
