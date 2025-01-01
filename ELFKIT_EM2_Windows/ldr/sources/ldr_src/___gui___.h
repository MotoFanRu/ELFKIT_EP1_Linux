#ifndef LDR_GUI_H
#define LDR_GUI_H

#include <apps.h>
#include <filesystem.h>
#include <dl_keypad.h>
#include <hw_keypad.h>
#include <resources.h>
#include <utilities.h>
#include <uis.h>

#define EVENT_RUN    0xABCD
#define EVENT_DEL    EVENT_RUN + 1
#define EVENT_LIB    EVENT_RUN + 2
#define EVENT_PM     EVENT_RUN + 3

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char app_name[APP_NAME_LEN] = "ElfLoader";

//Состояния приложения
typedef enum
{
    APP_STATE_ANY,
    APP_STATE_INIT,

    APP_STATE_EM,
    APP_STATE_PM,
    APP_STATE_PI,

    APP_STATE_MAX
} APP_STATES_T;

//Ресурсы приложения
typedef enum
{
    APP_RES_STRING_PM = 0,
    APP_RES_STRING_RUN,
    APP_RES_STRING_DEL,
    APP_RES_STRING_LIB,
    APP_RES_STRING_END,

    APP_RES_ACTION_PM,
    APP_RES_ACTION_RUN,
    APP_RES_ACTION_DEL,
    APP_RES_ACTION_LIB,
    APP_RES_ACTION_END,

    APP_RES_MAX
} APP_RES_T;
RESOURCE_ID APPResources[APP_RES_MAX];

//Структура описывающая приложение
typedef struct
{ 
    APPLICATION_T apt;
} APP_T;

UINT32 APPStart    (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 APPExit     (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 StateExit   (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);
UINT32 HandleUITokenGranted    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 APPInitActions          (ACTIONS_T *action_list, UINT8 action_type);
UINT32 APPInitResources        ();
UINT32 APPFreeResources        ();


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    WCHAR  *u_name;
    WCHAR  u_fullname[128];
    UINT8  in_autorun;
} FILEINFO;

UINT32 EMEntriesCnt = 0;
UINT32 EMSelectedID = 0;

FILEINFO *filelist = NULL;

//EM State
UINT32 EMStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 EMHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 EMHandleAction   (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 EMHandleKeyPress (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 EMHandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 EMHandleListReq  (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 EMSendListItems  (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num);
UINT32 EMUpdate         (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem, BOOL UpdateFileList);

UINT32 EMGetElfList();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    char *a_name;
    WCHAR u_name[APP_NAME_LEN + 1];
} ELF_LIST_NAMES_T;


UINT32 PMEntriesCnt = 0;
UINT32 PMSelectedID = 0;

ELF_LIST_NAMES_T *Elfs = NULL;  //Список имён запущенных эльфов
ELF_LIST_NAMES_T SelectedElf;   //Выбранный эльф

UINT32 PIStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 PIHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 PMStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 PMHandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 PMHandleSelect    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 PMHandleEndElf    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 PMHandleNavigate  (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 PMElfListChanged  (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 PMHandleListReq   (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 PMSendListItems   (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num);
UINT32 PMUpdate          (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem, BOOL UpdateFileList);

UINT32 PMGetElfList();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
