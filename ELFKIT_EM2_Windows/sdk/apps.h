#ifndef APPS_H
#define APPS_H

#include <mem.h>
#include <trace.h>
#include <events.h>
#include <typedefs.h>
#include <shortcuts.h>

#define APP_NAME_LEN       13
#define APP_STATE_NAME_LEN 13

#define STATE_HANDLERS_END         ((UINT32)(-1))
#define STATE_HANDLERS_RESERVED    ((UINT32)(-2))
#define STATE_HANDLERS_PM_API_EXIT ((UINT32)(-3))

enum
{
    ENTER_STATE_ENTER,
    ENTER_STATE_RESUME
};
typedef UINT8 ENTER_STATE_TYPE_T;


enum
{
    EXIT_STATE_EXIT,
    EXIT_STATE_SUSPEND
};
typedef UINT8 EXIT_STATE_TYPE_T;


typedef UINT32 EVENT_HANDLER_T    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
typedef UINT32 ENTER_STATE_FUNC_T (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
typedef UINT32 EXIT_STATE_FUNC_T  (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);


typedef struct
{
    EVENT_CODE_T    code;
    EVENT_HANDLER_T *hfunc;
} EVENT_HANDLER_ENTRY_T;


typedef struct
{
    UINT8                       state;
    ENTER_STATE_FUNC_T          *enter_fn;
    EXIT_STATE_FUNC_T           *exit_fn;
    const EVENT_HANDLER_ENTRY_T *htable;
} STATE_HANDLERS_ENTRY_T;


typedef struct
{
    UINT8  state;
    char   name[APP_STATE_NAME_LEN];
} APP_STATE_NAME_TABLE_T;


// Структура приложения
struct APPLICATION_T
{
    UINT32                       val_dword1;    //+00 0x00    (в прошиве val_dword1 сравнивается с EVENT_STACK_T::code)
    UINT8                        val_byte1;     //+04 0x04
    UINT8                        val_byte2[3];  //+05 0x05
    UINT32                       val_dword2;    //+08 0x08
    UINT8                        val_byte3[8];  //+12 0x0C

    LOG_ID_T                     log_id;        //+20 0x14
    AFW_ID_T                     afw_id;        //+24 0x18
    UIS_DIALOG_T                 dialog;        //+28 0x1c
    UIS_DIALOG_T                 dialog_notice; //+32 0x20
    SU_PORT_T                    port;          //+36 0x24
    void                         *history;      //+40 0x28
    EVENT_HANDLER_T              *exit_fn;      //+44 0x2c
    STATE_HANDLERS_ENTRY_T       *handlers_tbl; //+48 0x30
    const APP_STATE_NAME_TABLE_T *state_names;  //+52 0x34

    SEEM_0002_T                  *sc_record;    //+56 0x38
    const char                   *app_name;     //+60 0x3c

    APP_ID_T                     app_id;        //+64 0x40
    REG_ID_T                     reg_id;        //+66 0x42
    UINT32                       timer_handle;  //+68 0x44
    UINT8                        token_status;  //+72 0x48
    UINT8                        exit_status;   //+73 0x49
    BOOL                         focused;       //+74 0x4a
    UINT8                        state;         //+75 0x4b
    UINT8                        substate;      //+76 0x4c
    UINT8                        security;      //+77 0x4d
    UINT8                        pad2[2];       //+78 0x4e
}; //size = 80 0x50

typedef struct
{
    EVENT_CODE_T    ev_code;
    void            *start_fn;
    UINT32          unk1;
    UINT32          unk2;
    REG_ID_T        reg_id;
    UINT8           unk3;
} REG_INFO_T;


//взято с SDK EP2 LTE
#define AFW_PREPROCESSING           0
#define AFW_FOCUS                   1
#define AFW_POSTPROCESSING          2
typedef UINT8 AFW_APP_RSTACK_TYPE_T;

#define AFW_POSITION_TOP            0
#define AFW_POSITION_P0             (AFW_POSITION_TOP + 1)
#define AFW_POSITION_P1             (AFW_POSITION_P0  + 1)
#define AFW_POSITION_P2             (AFW_POSITION_P0  + 2)
#define AFW_POSITION_P3             (AFW_POSITION_P0  + 3)
#define AFW_POSITION_P4             (AFW_POSITION_P0  + 4)
#define AFW_POSITION_P5             (AFW_POSITION_P0  + 5)
#define AFW_POSITION_BOTTOM         (AFW_POSITION_P5  + 1)
typedef UINT8 AFW_APP_RSTACK_POS_T;

typedef UINT32 AFW_APP_PRIORITY_T;

enum
{
    AFW_APP_CENTRICITY_PRIMARY = 0,
    AFW_APP_CENTRICITY_SECONDARY,
    AFW_APP_CENTRICITY_NONE
};
typedef UINT8 AFW_APP_CENTRICNESS_T;


// Зарегистрировать APPLICATION
UINT32 APP_Register(const EVENT_CODE_T *reg_table, UINT8 reg_count, const STATE_HANDLERS_ENTRY_T *state_handlers, UINT8 state_count, void *start_fn);

// Искать приложение по reg_id
UINT32 AFW_InquireRoutingStackByRegId (REG_ID_T reg_id);

// Инициализировать данные приложения
APPLICATION_T* APP_InitAppData( void      *main_event_handler,
                                UINT32    sizeof_app,
                                REG_ID_T  reg_id,
                                UINT32    history_size,        // size of history stack element. 0 if not use history
                                UINT16    max_state_levels,    
                                UINT32    token_priority,      //AFW_APP_PRIORITY_T
                                UINT16    centricity,          //AFW_APP_CENTRICNESS_T
                                UINT8     routing_stack,       //AFW_APP_RSTACK_TYPE_T
                                UINT8     stack_priority );    //AFW_APP_RSTACK_POS_T

// Стандартная функция управления ивентами для нефоновых приложений
void APP_HandleEvent(EVENT_STACK_T *ev_st, APPLICATION_T *app, APP_ID_T app_id, REG_ID_T reg_id);

// Стандартная функция управления ивентами для фоновых приложений
void APP_HandleEventPrepost(EVENT_STACK_T *ev_st, APPLICATION_T *app, APP_ID_T app_id, REG_ID_T reg_id);

// смена обработчика (переход между фоновым и нефоновым режимом)
UINT32 APP_ChangeRoutingStack( APPLICATION_T         *app,
                               EVENT_STACK_T         *ev_st,
                               void                  *main_event_handler,
                               AFW_APP_RSTACK_TYPE_T routing_stack,
                               AFW_APP_RSTACK_POS_T  stack_priority,
                               AFW_APP_PRIORITY_T    token_priority,
                               AFW_APP_CENTRICNESS_T centricity );
// старт приложения
UINT32 APP_Start( EVENT_STACK_T                 *ev_st,
                  APPLICATION_T                 *app,
                  UINT8                         start_state,
                  const STATE_HANDLERS_ENTRY_T  *state_handlers,
                  EVENT_HANDLER_T               *exit_fn,
                  const char                    *app_name,
                  const APP_STATE_NAME_TABLE_T  *state_names );

// чистка данных и выход из приложения
UINT32 APP_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 param2); //param2 = 0;
UINT32 APP_StateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T action);
UINT32 APP_ExitStateAndApp (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 param2); //param2 = 0;

// вызывается из APP_Exit после чистки
UINT32 APP_Done (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 APP_HandleFailedAppStart (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 param2); // param2 = 0

// Сменить текущий state. При этом будет вызвана функция выхода из текущего state и ф-я входа в новый
UINT32 APP_UtilChangeState (UINT8 new_state, EVENT_STACK_T *ev_st, APPLICATION_T *app);

// То же, что APP_UtilChangeState, но при этом ещё будет извлечён(consumed) event
UINT32 APP_UtilConsumeEvChangeState (UINT8 new_state, EVENT_STACK_T *ev_st, APPLICATION_T *app);

// Чистка данных и выход из приложения. При этом будет извлечён(consumed) event
UINT32 APP_UtilConsumeEvAndExit (EVENT_STACK_T *ev_st, APPLICATION_T *app);

// Сменить текущий state с сохранением текущего состояния приложения (в стек)
UINT32 APP_UtilHistSaveAndChangeState (void *data, EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT8 new_state); //data - unknown

// Вернуть предыдущий state, сохранённый функцией APP_UtilHistSaveAndChangeState
UINT32 APP_UtilHistGoToPrevState (void *data, EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Удаляет диалог и извлекает ивент.
UINT32 APP_HandleDialogDeleteAndConsumeEv (EVENT_STACK_T *ev_st, APPLICATION_T *app);

// Token Status
typedef enum
{
    APP_TOKEN_NONE,
    APP_TOKEN_REQUESTED,
    APP_TOKEN_GRANTED,
    APP_TOKEN_REVOKED
} APP_TOKEN_STATUS_T;

// Стандартный обработчик EV_GRANT_TOKEN
UINT32 APP_HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app);

// Стандартный обработчик EV_REVOKE_TOKEN
UINT32 APP_HandleUITokenRevoked (EVENT_STACK_T *ev_st, APPLICATION_T *app);

// Имя приложения с app_id
UINT32 APP_UtilGetAppName (UINT16 app_id, char *app_name);

#endif
