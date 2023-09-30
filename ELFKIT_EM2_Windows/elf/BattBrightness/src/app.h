#ifndef APP_H
#define APP_H

//Must be always included!
#include <apps.h>
#include <loader.h>

//Another includes...
#include <dl.h>
#include <dal.h>
#include <battery.h>
#include <filesystem.h>
#include <utilities.h>

typedef struct
{
    UINT8 low_brightness_value;      //«начение €ркости диспле€ при разр€женной батарее (1..6)
    UINT8 high_brightness_value;     //«начение €ркости диспле€ при зар€женной батарее (1..6)
    UINT8 use_high_value_from_phone; //»спользовать значение €ркости диспле€ при зар€женной батарее из телефона (1 - да, 0 - нет)
} CONFIG_T;
const CONFIG_T default_cfg = {1, 6, 0};

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_MAX
} APP_STATES_T;

//use thisapp(app) to access to struct APP_T
typedef struct
{
    APPLICATION_T apt;
    CONFIG_T cfg;
} APP_T;
#define thisapp(app) ((APP_T *)(app))

UINT32 ELF_Entry	(ldrElf *ela, WCHAR *params);
UINT32 ELF_Start	(EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit		(EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Any state handlers
UINT32 HandleReqExit            (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleBatteryLow         (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleBatteryPowerChange (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//Utils
UINT32 Util_ReadConfig (DL_FS_MID_T *id, CONFIG_T *cfg);
UINT32 Util_GetDispBrightness (UINT8 *Bval);
UINT32 Util_SetDispBrightness (UINT8 Bval);

#endif
