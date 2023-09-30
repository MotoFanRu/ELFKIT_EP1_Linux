#ifndef APPS_H
#define APPS_H

#include <loader.h>
#include <time_date.h>
#include <events.h>
#include <dl.h>
#include <filesystem.h>
#include <util.h>
#include <uis.h>
#include <canvas.h>
#include <memory.h>
#include <trace.h>



typedef UINT16      REG_ID_T;

#define APP_NAME_LEN              13
#define STATE_HANDLERS_END        ((UINT32)(-1))
#define STATE_HANDLERS_RESERVED	  ((UINT32)(-2))


typedef enum
{
    ENTER_STATE_ENTER,
    ENTER_STATE_RESUME
} ENTER_STATE_TYPE_T;

typedef enum
{
    EXIT_STATE_EXIT,
    EXIT_STATE_SUSPEND
} EXIT_STATE_TYPE_T;

typedef UINT16    APP_ID_T;

typedef UINT32    EVENT_HANDLER_T( EVENT_STACK_T *ev_st,  void *app );
typedef UINT32    ENTER_STATE_FUNC_T( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );
typedef UINT32    EXIT_STATE_FUNC_T( EVENT_STACK_T *ev_st,  void *app,  EXIT_STATE_TYPE_T type );


typedef struct
{
    EVENT_CODE_T      code;
    EVENT_HANDLER_T   *hfunc;
} EVENT_HANDLER_ENTRY_T;

typedef struct
{
    UINT8                     		  state;
    ENTER_STATE_FUNC_T        		  *enter_fn;
    EXIT_STATE_FUNC_T         		  *exit_fn;
    const EVENT_HANDLER_ENTRY_T     *htable;
} STATE_HANDLERS_ENTRY_T;

typedef struct
{
    UINT8                      unk1[20];
    UINT32                     unk2; //24
    UINT32                     unk3; //28
    UIS_DIALOG_T               dialog; //32
    UINT32                     unk4; //36
    SU_PORT_T                  port; //40
    void                       *unk5; //44
    EVENT_HANDLER_T            *exit_fn; //48
    STATE_HANDLERS_ENTRY_T     *handlers_tbl; //52
    UINT32                     unk6; // always 0 //56
    //56
    UINT8                      pad1[12]; //
    UINT32                     ek;
    //72
    UINT8                      token_status;
	UINT8					   unk7;
	BOOL					   focused;
	UINT8					   state;	
        
    UINT8                      pad2[4];
    //UINT8                      pad[23+1];
} APPLICATION_T; //size = 80


/* Registers the Application */
UINT32 APP_Register( EVENT_CODE_T                   *registry_table,
                     UINT8                          reg_count,
                     const STATE_HANDLERS_ENTRY_T   *state_handlers,
                     UINT8                          state_count,
                     void                           *start_fn );


UINT32 AFW_InquireRoutingStackByRegId( REG_ID_T reg_id ); // не работает

APPLICATION_T* APP_InitAppData( void             *main_event_handler,
                                UINT32            sizeof_app,
                                REG_ID_T          reg_id,
                                UINT32            param3,
                                UINT16            history_size,
	                              UINT32            priority,
    	                          UINT16            param6,
        	                      UINT8             type,
            	                  UINT8             param8 );

/* Стандартная функция управления ивентами для нефоновых приложений */
void APP_HandleEvent( EVENT_STACK_T           *ev_st,
                      APPLICATION_T           *app,
                      APP_ID_T                app_id,
                      REG_ID_T                reg_id );

/* Стандартная функция управления ивентами для фоновых приложений */
void APP_HandleEventPrepost( EVENT_STACK_T           *ev_st,
                             APPLICATION_T           *app,
                             APP_ID_T                app_id,
                             REG_ID_T                reg_id );
                 
UINT32 APP_Start( EVENT_STACK_T                     *ev_st,
                  APPLICATION_T                     *app,
                  UINT8                             start_state,
                  const STATE_HANDLERS_ENTRY_T     *state_handlers,
                  EVENT_HANDLER_T                   *exit_fn,
                  const char                       *app_name,
                  UINT32                            param6 );

UINT32 APP_Exit( EVENT_STACK_T        *ev_st,
                 APPLICATION_T        *app,
                 UINT32               param2 );

/*Change the current state 
    This function will be called exit the current state and the f-I entrance to the new */
UINT32 APP_UtilChangeState( UINT8 new_state,  EVENT_STACK_T *ev_st,  void *app );

/* Change the current state of preservation of the current state of applications (stacked) */
UINT32 APP_UtilHistSaveAndChangeState( void *data,  EVENT_STACK_T *ev_st,  void *app,  UINT8 new_state ); //data - unknown

/* Revert to the previous state, saved APP_UtilHistSaveAndChangeState function */
UINT32 APP_UtilHistGoToPrevState( void *data,  EVENT_STACK_T *ev_st,  void *app );


#endif


