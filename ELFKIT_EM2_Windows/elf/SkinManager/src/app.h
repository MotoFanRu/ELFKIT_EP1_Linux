#define DEBUG

#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <fw_apps.h>
#include <dl_keypad.h>
#include <filesystem.h>
#include <skins.h>
#include <utilities.h>
#include <uis.h>

#define THEME_MAIN_REGISTER 0x3F0
#define SKIN_PREVIEW_FILE_NAME L"preview.gif"
#define SKIN_WALLPAPER_FILE_EXT L".jpg"
#define DEFAUL_MMA_UCP_URI L"file://a/mobile/system/mma_ucp"

typedef enum
{
    HW_STATE_ANY,
    HW_STATE_INIT,
    HW_STATE_MAIN,
    HW_STATE_PREV,
    HW_STATE_CONF,
    HW_STATE_INFO,
    HW_STATE_MAX
} HW_STATES_T;

typedef enum
{
    RES_LIST_CAPTION = 0,
    RES_LIST_LABEL0,
    RES_LIST_LABEL1,
    RES_LIST_ACTION0,
    RES_LIST_ACTION1,
    RES_MAX
} HW_RESOURCE;

typedef struct
{
    BOOL isCurrent;
    WCHAR *SkinName;
} SKIN_INFO_T;

//use thisapp(app) to access to struct APP_T
typedef struct
{
    APPLICATION_T apt;
    BOOL isShow;
    WCHAR *SkinFolder;

    UINT32 EntriesNum;
    UINT32 SelectedId;

    SKIN_INFO_T *sInfo;
    RESOURCE_ID *Resources;
} APP_T;
#define thisapp(app) ((APP_T *)(app))

//Enry, Start, End functions
UINT32 ELF_Entry    (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Change background/focus modes
UINT32 ELF_ChangeRStack (EVENT_STACK_T *ev_st, APPLICATION_T *app, BOOL show);
UINT32 ELF_APPShow (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 ELF_APPHide (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Main state handlers
UINT32 MainStateEnter	(EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 MainStateExit	(EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 MainHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainHandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainHandleInfo (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainHandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainHandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainHandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainHandleUpdate (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem);
UINT32 MainHandleSendListItems (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num);

//Preview state handlers
UINT32 PrevStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 PrevStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 PrevHandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 PrevHandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Confirmation state handlers
UINT32 ConfStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 ConfStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 ConfHandleYesNo (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Information state handlers
UINT32 InfoStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 InfoStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 InfoHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//utils
UINT32 Util_InitResources (APPLICATION_T *app);
UINT32 Util_RemoveResources (APPLICATION_T *app);
UINT32 Util_InitDialogActions (ACTIONS_T *actions, APPLICATION_T *app);
UINT32 Util_SetSkin (UINT32 idx, APPLICATION_T *app);       //применяет выбранный скин, mma_ucp из скина и обоину
UINT32 Util_GetSkins (APPLICATION_T *app);                  //получает список установленных скинов.

#endif
