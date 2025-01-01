#ifndef BATTERY_H
#define BATTERY_H

#include <typedefs.h>

// Режим зарядки батареи
enum
{
    CHARGING_MODE_NONE = 0,
    CHARGING_MODE_IN_PROGRESS = 1,
    CHARGING_MODE_CHARGING_COMPLETE
};
typedef UINT8 CHARGING_MODE;
CHARGING_MODE DL_PwrGetChargingMode (void);

// показания батареи в процентах
UINT8 DL_PwrGetActiveBatteryPercent (void);

//???
UINT8 DL_PwrGetActiveBattery (void);

//Низкий уровень зарядя батареи (1 - да, 0 - нет)
UINT8 DL_PwrGetLowBatteryStatus (void);

//???
UINT8 DL_PwrGetAttachedBattery (void);

//???
UINT8 DL_PwrGetMainBatteryType (void);

//???
UINT8 DL_PwrGetAuxBatteryType (void);

//подключено ли внешнее питание (1 - да, 0 - нет)
UINT8 DL_AccIsExternalPowerActive (void);

//Тип зарядки???
UINT8 DL_AccGetChargerType (void);

//Вольтаж батареи. Значение в миливольтах
UINT32 RTIME_MAINT_get_battery_voltage(void);
#endif
