#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <dl.h>
#include <dl_keypad.h>
#include <filesystem.h>
#include <phonebook.h>
#include <utilities.h>

#define BLACKLIST_CONFIG_FILE_URI L"file://a/mobile/blacklist.cfg"
#define BLACKLIST_LOG_FILE_URI L"file://a/mobile/blacklist.log"

enum
{
    BLACKLIST_TYPE_NONE = 0,    //Не блокировать
    BLACKLIST_TYPE_ALL,         //Блокировать все
    BLACKLIST_TYPE_CATEGORY     //Блокировать категорию
};

typedef UINT8 BLACKLIST_TYPE_T;
typedef UINT8 BLACKLIST_GROUP_T;
typedef DL_SIG_CALL_CAUSE_T BLACKLIST_CALL_CAUSE_T;

//Config
typedef struct
{
    BLACKLIST_TYPE_T type;
    BLACKLIST_GROUP_T group;
    BLACKLIST_CALL_CAUSE_T cause;
} BLACKLIST_CONFIG;

const BLACKLIST_CONFIG default_config = {0,0,0};

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

UINT32 HandleReqExit    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleCallReceived (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandlePBKReadRecord (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Utils
UINT32 Util_LoadConfig ();
UINT32 Util_BlackListLogger (WCHAR *number);

#endif
