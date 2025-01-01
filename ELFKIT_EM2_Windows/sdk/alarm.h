#ifndef ALARM_H
#define ALARM_H

#include <typedefs.h>
#include <time_date.h>

#define ALARM_RECORD_NAME_LEN 24

typedef struct
{
    CLK_TIME_T  time;
    CLK_DATE_T  date;
    UINT32      unk;
} DL_ALARM_DATA_T;

typedef struct
{
    UINT8       index;
    UINT8       unk;
    WCHAR       name[ALARM_RECORD_NAME_LEN + 1];
    CLK_TIME_T  time;
    UINT64      alert_tone;
    BOOL        is_alarm_enabled;
    UINT8       alarm_id;
    UINT8       ringer_volume;
    BOOL        is_wake_up_alarm;
} ALMCLK_RECORD_T;

// создать запись
UINT32 DL_DbAlmclkCreateRecord (ALMCLK_RECORD_T * record);

// Изменить запись
UINT32 DL_DbAlmclkModifyRecord (ALMCLK_RECORD_T * record);

// получить кол-во записей
UINT32 DL_DbAlmclkGetNumberOfRecords (UINT8 unk, UINT8 * count); //unk = 0

// получить запись по её индексу
UINT32 DL_DbAlmclkGetRecordByIndex (UINT8 index, ALMCLK_RECORD_T * record);

// удалить запись
UINT32 DL_DbAlmclkDeleteRecordByIndex (UINT8 index);

// удалить все будильники
UINT32 DL_DbAlmclkDeleteAllRecords (void);

// установить Alarm
BOOL DL_ClkStoreIndividualEvent (IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data);

// удалить Alarm
BOOL DL_ClkDeleteIndividualEvent (IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data, UINT8 mask); // 1 - time, 2- date....  63 - полное удаление?

#endif
