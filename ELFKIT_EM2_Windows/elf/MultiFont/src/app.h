#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <codegroups.h>
#include <fonts.h>
#include <filesystem.h>
#include <resources.h>
#include <utilities.h>
#include <uis.h>

#define FONT_TABLE_RAM_ADDR 0x096C3C00	//for v3x
#define FONT_CONFIG_URI L"file://a/mobile/system/fonttype.bin"

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_INIT,
    APP_STATE_MAIN,
    APP_STATE_MAX
} APP_STATES_T;

typedef struct
{
    APPLICATION_T apt;

    UINT32 entries_num;     //кол-во шрифтов
    RESOURCE_ID caption;    //заголовок меню
    FONT_ENTRY_T **fonts;   //список шрифтов
    UINT32 current_font_id; //текущий шрифт
} APP_T;

UINT32 ELF_Entry    (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 MainStateEnter	(EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 MainStateExit	(EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleReqExit        (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleListReq        (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleSelect         (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 SendListItems        (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num);
UINT32 Update               (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem);

//Utils
UINT32 InitApp   (APPLICATION_T *app);
UINT32 SetFont   (UINT32 *font_id);
UINT32 ReadCfg   (UINT32 *font_id);
UINT32 WriteCfg  (UINT32 *font_id);
#endif
