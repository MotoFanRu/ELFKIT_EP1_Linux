#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#include <typedefs.h>
// непроверено !

#define SC_TYPE_LIST_ITEM  0x00     //???. Метка на пункт меню и метка на Java определяется так.
#define SC_TYPE_URL        0x05
#define SC_TYPE_APP        0x06     //+++

#define SC_TEXT_LEN        0x10     //+++
#define SC_MAX_NUMBER      0x64
#define SC_ALL_RECORD      0xFE
#define SC_INVALID_RECORD  0xFF

typedef UINT8 SC_TYPE_T;

typedef struct
{
    UINT8   seem_rec;               // +++. seem_rec-1
    UINT8   type;                   // +++. тип метки 
    UINT8   index;                  // +++. номер метки в списке меток. отсчет от 1.
    UINT8   unk1;                   // +++. 0xFE
    UINT32  ev_code;                // +++. код ивента
    UINT32  list_index;             // +++. id выделенного пункта списка (для Java это номер приложения в списке. отсчёт от 1)
    UINT32  param1;                 // +++. ???
    UINT32  list2_index;            // +++. ??? тоже какой-то id пункта
    UINT32  param2;                 // +++. ???
    UINT32  data;                   // +++. используется для указателя на данные, например указатель на URL-адрес (для Java возможно это JAVA_APP_ID)
    UINT32  lang_text;              // +++. RESOURCE_ID 01 XX XX XX
    WCHAR   text[SC_TEXT_LEN];      // +++. имя метки
    UINT8   unk2[4];                // +++. ??? XX XX XX (00 or 01 or FF).
} SEEM_0002_T;                      // +++. размер = 68 байт


// создание метки                                    
UINT8 DL_DbShortcutCreateRecord(IFACE_DATA_T *data, SEEM_0002_T *record); //10726BE4

// обновление метки
UINT8 DL_DbShortcutUpdateRecord(IFACE_DATA_T *data, SEEM_0002_T *record); //10726B4A

// удаление метки
UINT8 DL_DbShortcutDeleteRecord(IFACE_DATA_T *data, UINT8 seem_rec); //10726B22

// проверяет  recordId,   0 - no error
UINT8 DL_DbShortcutGetRecordByRecordId(IFACE_DATA_T *data, UINT8 seem_rec); //10726ACC

// получает номер доступной записи
UINT16 DL_DbShortcutGetNumOfRecordsAvailable(void); //10726AF4

// получает номер используемой записи
UINT16 DL_DbShortcutGetNumOfRecordsUsed(BOOL voice_shortcut); //10726AF8

// получает первый доступный номер
UINT8 DL_DbGetFirstAvailableNumber(void); //10726B1E

// получает номер используемой  записи URL
UINT16 DL_DbShortcutGetNumOfURLRecordsUsed(void); //10726C70

// проверяет URLId,   0 - no error
UINT32 DL_DbShortcutGetURLByURLId(IFACE_DATA_T *data, UINT32 URLId); //10726C74

// получение типа записи
SC_TYPE_T DL_DbShortcutGetshortcutType(UINT8 index); //10726CCA

// получает номер доступной записи URL
UINT16 DL_DbShortcutGetNumOfURLRecordsAvailable(void); //10726CD2

#endif
