#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <seems.h>
#include <canvas.h>
#include <dl_keypad.h>
#include <filesystem.h>
#include <utilities.h>
#include <uis.h>

typedef enum
{
    POSITION_BIT_0 = 0,
    POSITION_BIT_1,
    POSITION_BIT_2,
    POSITION_BIT_3,
    POSITION_BIT_4,
    POSITION_BIT_5,
    POSITION_BIT_6,
    POSITION_BIT_7,
    POSITION_VALUE,
    POSITION_SEEM

} POSITION_T;

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_INIT,
    APP_STATE_MAIN,
    APP_STATE_EDIT,
    APP_STATE_MAX

} APP_STATES_T;

//use thisapp(app) to access to struct APP_T
typedef struct
{
    APPLICATION_T apt;

    RESOURCE_ID SeemCaption;
    RESOURCE_ID ValueCaption;

    DRAWING_BUFFER_T dBuf;
    POSITION_T cursorPosition;

    SEEM_ELEMENT_DATA_CONTROL_T seemDescription;
    UINT8 seemValue;
    BOOL seemLoaded;

} APP_T;
#define thisapp(app) ((APP_T *)(app))

UINT32 ELF_Entry	(ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 HandleReqExit        (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 APPInitResources (APPLICATION_T *app);
UINT32 APPFreeResources (APPLICATION_T *app);

///main state handlers
UINT32 MainStateEnter	(EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 MainStateExit	(EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);
UINT32 HandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app);

///edit state handlers
UINT32 EditStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 EditStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);
UINT32 HandleEditOk (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleEditExit (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleEditData (EVENT_STACK_T *ev_st, APPLICATION_T *app);

///Utils
typedef enum
{
    BIT_CLR = 0,
    BIT_GET = 1,
    BIT_SET = 2
} BIT_OPERATION_TYPE;

UINT8 BitOperation(VOID *src, UINT8 pos, BIT_OPERATION_TYPE type);
VOID  DrawGUI (APPLICATION_T *app);
#endif
