#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <dl.h>
#include <java.h>
#include <seems.h>
#include <resources.h>
#include <filesystem.h>
#include <utilities.h>
#include <usb.h>
#include <uis.h>

#define USB_CFG_PATH L"file://a/mobile/system/usbmode_cfg.bin"
#define USB_MODE_SJAL 0xAA

typedef struct
{
    BOOL CableReaction;
    UINT8 CableID;
    UINT8 MCARDID;
    UINT8 ModemID;
    UINT8 P2KID;
} USBConfig;
const USBConfig default_config = {TRUE, 0x14, USB_MODE_MCARD, USB_MODE_MODEM, USB_MODE_P2K};

typedef struct
{
    RESOURCE_ID drm_id;
	RESOURCE_ID str_id;
	USB_MODE_T  value;
} LINE;

typedef enum
{
	APP_RES_LIST_CAPTION=0,
	APP_RES_ACTION,
	RES_MAX
} APP_RESOURCE;

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_BKG,
    APP_STATE_GUI,
    APP_STATE_MAX
} APP_STATES_T;

typedef struct
{
    APPLICATION_T apt;

    BOOL isShow;
    BOOL USBstatus;

    UINT32 entries_num;
    LINE *item;

    USBConfig cfg;          //Файл настроек
    USB_MODE_T curr_mode;   //текущий режим
} APP_T;

//Enry, Start, End functions
UINT32 ELF_Entry    (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Change background/focus modes
UINT32 ELF_ChangeRStack (EVENT_STACK_T *ev_st, APPLICATION_T *app, BOOL show);
UINT32 ELF_APPShow (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 ELF_APPHide (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 GuiStateEnter   (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 GuiStateExit    (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 HandleDone    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleSelect  (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleConnectDevice    (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleDisconnectDevice (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleSendListItems    (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 start, UINT32 num);


//USB Utils
enum
{
	USB_MODE_ACTION_GET = 0,
	USB_MODE_ACTION_SET
};
typedef UINT8 USB_MODE_ACTION_T;

UINT32 USBReadConfig (USBConfig *cfg);
UINT32 USBDefaulMode (USB_MODE_T *mode, USB_MODE_ACTION_T Action);
#endif
