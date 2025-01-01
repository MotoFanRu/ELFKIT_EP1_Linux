#ifndef APP_H
#define APP_H

#include <apps.h>
#include <loader.h>
#include <filesystem.h>
#include <dl_keypad.h>
#include <utilities.h>
#include <uis.h>

typedef struct
{
    APPLICATION_T apt;
} APP_T;

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_MAX
} APP_STATES_T;

typedef struct
{
    WCHAR image_uri[FS_MAX_URI_NAME_LENGTH + 1];
} FILEINFO;

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit  (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 HandleReqExit    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleTimer      (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 ListBkgs         (APPLICATION_T *app);
UINT32 ReadConfig       (DL_FS_MID_T *id);

#endif
