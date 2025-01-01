#ifndef USB_UTILS_H
#define USB_UTILS_H

#include <dl.h>

//Ивент запуска стандартного меню настроек USB
#define USB_SETTINGS_EV 0x3FE

//USB_CABLE_ID - ID кабеля. 0x14 for RAZR V3x R252211LD_U_85.9B.E6P
#define USB_CABLE_ATTACHED(USB_CABLE_ID) DL_AccGetConnectionStatus(USB_CABLE_ID)

//Аттач иветна (DEVICE ID)
#define EV_GET_DEV_ID(ev) (*((UINT8*)ev->attachment))

//Ивент присоединения/отсоединения кабеля.
#define IS_USB_CABLE_EVENT(ev, USB_CABLE_ID) (ev->attachment && EV_GET_DEV_ID(ev) == USB_CABLE_ID)

#endif
