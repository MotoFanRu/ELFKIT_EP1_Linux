
#include "app.h"


UINT32 appStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 appExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

#ifndef WIN32

	#undef dbgf
	#define dbgf( format, ... ) 
	#undef dbg
	#define dbg( format ) 
	
#endif

/* Название приложения. Длина строки именно такая и никакая иначе */
const char			app_name[APP_NAME_LEN] = "UGH!"; 

UINT32				evcode_base;

ldrElf				elf;

UINT32				gGlobalState;
APPLICATION_T		*gApplication;


EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{
	{ EV_REVOKE_TOKEN,				APP_HandleUITokenRevoked },
	{ STATE_HANDLERS_END,			NULL },
};

EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
	{ EV_GRANT_TOKEN,				HandleUITokenGranted },
	{ STATE_HANDLERS_END,			NULL }
};

EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
	{ STATE_HANDLERS_END,			NULL },
};

const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
	{
		APP_STATE_ANY,
		NULL,
		NULL,
		any_state_handlers
	},
	
	{
		APP_STATE_INIT,
		NULL,
		NULL,
		init_state_handlers
	},

	{
		APP_STATE_MAIN,
		MainStateEnter,
		MainStateExit,
		main_state_handlers
	}

};


ldrElf* _start( WCHAR *uri, WCHAR *params )
{
	UINT32					status = RESULT_OK;
	
	if ( ldrIsLoaded( (char*)app_name ) )
	{
		cprint("Dummy: Already loaded!\n");
		return NULL;
	}
	
	evcode_base = ldrRequestEventBase( );
	evcode_base = ldrInitEventHandlersTbl( any_state_handlers, evcode_base );
	evcode_base = ldrInitEventHandlersTbl( init_state_handlers, evcode_base );
	evcode_base = ldrInitEventHandlersTbl( main_state_handlers, evcode_base );
	
	gGlobalState = GLOBAL_STATE_INIT;

	resInitHomeDir(uri);

	status = APP_Register(  &evcode_base, 1,
							state_handling_table,
							APP_STATE_MAX,
							(void*)appStart );

	ldrSendEvent( evcode_base );

	elf.name = (char*)app_name;

	return &elf;
}


UINT32 appStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
	UINT32					status = RESULT_OK;
	
	gApplication = APP_InitAppData( (void *)APP_HandleEvent,
									sizeof(APPLICATION_T),
									reg_id,
									0, 0,
									1,
									AFW_APP_CENTRICITY_PRIMARY,
									AFW_FOCUS,
									AFW_POSITION_TOP );

	status = APP_Start( ev_st,
						gApplication,
						APP_STATE_INIT,
						state_handling_table,
						appExit,
						app_name, 0 );

	elf.app = gApplication;

	return status;
}


UINT32 appExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
	UINT32					status;

	cprint("Dummy: Exit\n");
	
	status = APP_ExitStateAndApp( ev_st, app, 0 );
		
	ldrUnloadElf(); 

	return status;
}


UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
	UIS_DIALOG_T			dialog;
	SU_PORT_T				port = app->port;

	if ( type != ENTER_STATE_ENTER )
		return RESULT_OK;

	#ifdef SETCANVASCONTROL
		setCanvasControl__13StatusManagerFScPUs( &theStatusManager, TRUE, NULL );
	#endif


	dialog = UIS_CreateNullDialog( &port );
	
	if ( dialog == NULL ) 
		return RESULT_FAIL;
	
	app->dialog = dialog;
	
	// gGlobalState управляет состоянием таска
	gGlobalState = GLOBAL_STATE_MAIN;
	
	// Создать главный таск
	mainTaskCreate();
	
	return RESULT_OK;
}


UINT32 MainStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type )
{
	if (type != EXIT_STATE_EXIT)
		return RESULT_OK;
		
	gGlobalState = GLOBAL_STATE_EXIT;
	
	if ( app->dialog != NULL ) 
		APP_UtilUISDialogDelete( &app->dialog );
	
	return RESULT_OK;
}


UINT32 HandleUITokenGranted( EVENT_STACK_T * ev_st,  APPLICATION_T * app )
{
	UINT32					status;

	status = APP_HandleUITokenGranted( ev_st, app );
	
	if( (status == RESULT_OK) && (app->token_status == 2) ) 
		status = APP_UtilChangeState( APP_STATE_MAIN, ev_st, app );

	return status;
}

UINT32 appTerminate()
{
	gApplication->exit_status = TRUE;
	
	return 0;
}
