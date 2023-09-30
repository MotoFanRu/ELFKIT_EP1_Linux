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
#include <websessions.h>

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_MAX
} APP_STATES_T;

//use thisapp(app) to access to struct APP_T
typedef struct
{
    APPLICATION_T apt;
} APP_T;
#define thisapp(app) ((APP_T *)(app))

UINT32 ELF_Entry	(ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

//any state handlers
UINT32 HandleReqExit        (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleTimer          (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleNetworkStatus  (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//utils
UINT32 Util_SetWebSession   (APPLICATION_T *app);
#endif
