#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <dl.h>
#include <dal.h>
#include <dl_keypad.h>
#include <filesystem.h>
#include <time_date.h>
#include <utilities.h>

#define STATE_ON  1
#define STATE_OFF 0

typedef struct
{
    UINT8 magic[3];
    UINT8 version;
} FNL_Header;

typedef struct
{
    UINT8 state;    //must be always 0 in funlight.pat!
    UINT8 events;
    UINT16 period;
} FNL_Record;

enum
{
    FNL_FLASH   = 1,
    FNL_KEYPAD  = 2,
    FNL_DISPLAY = 4,
    FNL_CLI     = 8,
    FNL_LED     = 16,
    FNL_KPTOP   = 32,
    FNL_KPBOT   = 64
};

enum
{
    FUNCTION_KEYPAD = 0,    //0
    FUNCTION_DISPLAY,       //4
    FUNCTION_FLASHLIGHT,    //8
    FUNCTION_CLI,           //C
    FUNCTION_KEYPAD_BOTTOM, //10
    FUNCTION_KEYPAD_TOP,    //14
    FUNCTION_NULL,          //18
    FUNCTION_CHARGING_LED   //1C
};

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

UINT32 HandleReqExit        (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleCallReceived   (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleKeyPress       (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleTimer          (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Utils
UINT32 Util_IsFnLTimer (UINT32 timer_handle);
UINT32 Util_StopTimers (APPLICATION_T *app);
UINT32 Util_ReadConfig (DL_FS_MID_T *id);
void   Util_ResetState (void);

asm
(
"ll_call:                \n"
    "lrw    r7, 0x8000064\n"
    "subi   sp, 8        \n"
    "ixw    r7, r3       \n"
    "ld.w   r7, (r7, 0)  \n"
    "st.w   r15, (sp, 0) \n"
    "jsr    r7           \n"
    "ld.w   r15, (sp, 0) \n"
    "addi   sp, 8        \n"
    "jmp    r15          \n"
);
extern void ll_call (int p1, int function);

#endif
