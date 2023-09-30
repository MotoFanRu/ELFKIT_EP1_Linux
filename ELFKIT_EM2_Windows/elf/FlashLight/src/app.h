#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <dl.h>
#include <dl_keypad.h>
#include <filesystem.h>
#include <utilities.h>

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_MAX
} APP_STATES_T;

//use thisapp(app) to access to struct APP_T
typedef struct
{
    APPLICATION_T apt;

    UINT8 state;
    UINT8 counter;
    UINT8 keycode;
} APP_T;
#define thisapp(app) ((APP_T *)(app))

UINT32 ELF_Entry	(ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 MainStateEnter	(EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 HandleReqExit    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleKeyPress   (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Utils
UINT32 Util_ReadConfig  (DL_FS_MID_T *id, UINT8 *keycode);
#endif
