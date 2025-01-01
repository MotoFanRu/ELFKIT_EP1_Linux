#ifndef BATTERY_H
#define BATTERY_H

#include <typedefs.h>

// ����� ������� �������
enum
{
    CHARGING_MODE_NONE = 0,
    CHARGING_MODE_IN_PROGRESS = 1,
    CHARGING_MODE_CHARGING_COMPLETE
};
typedef UINT8 CHARGING_MODE;
CHARGING_MODE DL_PwrGetChargingMode (void);

// ��������� ������� � ���������
UINT8 DL_PwrGetActiveBatteryPercent (void);

//???
UINT8 DL_PwrGetActiveBattery (void);

//������ ������� ������ ������� (1 - ��, 0 - ���)
UINT8 DL_PwrGetLowBatteryStatus (void);

//???
UINT8 DL_PwrGetAttachedBattery (void);

//???
UINT8 DL_PwrGetMainBatteryType (void);

//???
UINT8 DL_PwrGetAuxBatteryType (void);

//���������� �� ������� ������� (1 - ��, 0 - ���)
UINT8 DL_AccIsExternalPowerActive (void);

//��� �������???
UINT8 DL_AccGetChargerType (void);

//������� �������. �������� � �����������
UINT32 RTIME_MAINT_get_battery_voltage(void);
#endif
