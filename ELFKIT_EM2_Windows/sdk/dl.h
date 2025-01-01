#ifndef DL_H
#define DL_H

#include <typedefs.h>
#include <dl_db_features.h>

//Таймеры разговора
typedef struct
{
    UINT16 hour;
    UINT8  minute;
    UINT8  second;
} CALL_TIME_T;

// включает/выключает вспышку (1 - вкл, 0 - выкл)
UINT32 set_nvidia_flash_control (UINT8 state);

/******************************
   Свет
*******************************/

UINT32 UIS_SetBacklight( UINT8 P1 );

// Регулировка яркости дисплея
UINT32 UIS_SetBacklightWithIntensity (UINT8 color /* = 255*/, UINT8 intensity /* = 0...6*/);


/******************************
   DbFeature
******************************/

UINT8 DL_DbFeatureGetCurrentState(UINT16 fstate_id, UINT8 *state);    //Чтение из сима 004a
UINT8 DL_DbFeatureStoreState(UINT16 fstate_id, UINT8 state);        //Запись в сим 004а
UINT8 DL_DbFeatureGetValue(UINT16 feature_id, UINT32 *feature_value);
UINT8 DL_DbFeatureGetValueString(UINT16 feature_id, WCHAR *feature_str);
UINT8 DL_DbFeatureStoreValueString(UINT16 feature_id, WCHAR *feature_str, UINT16 str_len);
UINT8 DL_DbFeatureGetBlock(UINT16 id, void *data);             //Прочитать блок данных id в data
UINT8 DL_DbFeatureStoreBlock(UINT16 id, void *data, UINT8 size);     //Сохранить блок данных data размером size в id
UINT8 DL_DbGetFeatureAvailability(UINT16 feature_id);
//Доступность ф-ции (1 - да, 0 - нет)
//Как преобразовать сим, оффсет, бит в feature_id?
//Пример 1: seem_element_id=0x0032, seem_record=0x0001, seem_offset=0x80 bit=0x6.
//feature_id = seem_offset * 0x8 + bit - seem_element_id = 0x80 * 0x8 + 0x6 - 0x0032 = 3D4
//подходит только для сима 0032_0001!!!!!!!!!!!!!!
//Пример 2: seem_element_id=0x0032, seem_record=0x0001, seem_offset=0x80 bit=0x6.
//feature_id = 0x34E + seem_offset + bit = 0x34E + 0x80 + 6 = 3D4
//UINT16 ConvertSeem0032toID(UINT16 offset, UINT16 bit)
//{
//    UINT16 res = offset * 0x8 + ((bit > 7) ? (7) : (bit));
//    return (res > 0xFF) ? (res - 0x0032) : res;
//}

#define SEEM32_TO_ID(offset, bit) (offset * 0x8 + bit)
#define SEEM32_TO_ID_1(s, r, o, b) (o * 0x8 + b - s)
#define SEEM32_TO_ID_2(o, b) (0x34E + o + b)


//версия прошивки
UINT32 DL_DbFeatureGetSoftwareVersion (WCHAR* Version, UINT32 unk); //unk = 0xFFFFFFFF;
#define GET_SOFTWARE_VERSION(Version) DL_DbFeatureGetSoftwareVersion(Version, 0xFFFFFFFF);



/****************************
  Питание
*****************************/

//Функция выключения.
// 0 - Выключение
// 1 - Перезагрузка
// 2 - ???
// 3 - Вход в boot
// 4 - ???
// 5 - ???
void pu_main_powerdown(UINT32 r0);

//Перезагрузка телефона.
void DL_PudSoftReset(void);

/****************************
  Громкость
****************************/

enum // volume_type
{
    BASE = 0,    // громкость звонка

    PHONE, // громкость клавиатуры
    VOICE, // громкость мультимедия  LTE
    MICROPHONE, // громкость разговора
    MULTIMEDIA, //  громкость мультимедия LTE2 и V3i
    PTT_TONES,
    MUTABLE_MAX,
    IMMUTABLE_MAX,
    MAX = IMMUTABLE_MAX
};

// Установить громкость
void DL_AudSetVolumeSetting(UINT8 volume_type, UINT8 volume);
// Получить текущую громкость
void DL_AudGetVolumeSetting(UINT8 volume_type, UINT8 *volume);


/****************************
  Звонки
****************************/

#define MAX_CALLS                   7

typedef struct
{
    UINT16  call_id;
    UINT8   call_state;
} CALL_ID_T;

typedef struct
{
    UINT8       number_of_calls;
    UINT8       overall_call_state;
    CALL_ID_T   call_state_info[MAX_CALLS];
} CALL_STATES_T;

typedef struct
{
    CALL_ID_T call;
    UINT8 unk;
    WCHAR number[69];
} CALL_ATT_T;

enum
{
    DL_SIG_CALL_CAUSE_NO_CAUSE,
    DL_SIG_CALL_CAUSE_CC_BUSY,
    DL_SIG_CALL_CAUSE_PARAMETER_ERROR,
    DL_SIG_CALL_CAUSE_INVALID_NUMBER,
    DL_SIG_CALL_CAUSE_OUTGOING_CALL_BARRED,
    DL_SIG_CALL_CAUSE_TOO_MANY_CALLS_ON_HOLD,
    DL_SIG_CALL_CAUSE_NORMAL,
    DL_SIG_CALL_CAUSE_INTERCEPT,
    DL_SIG_CALL_CAUSE_REORDER,
    DL_SIG_CALL_CAUSE_NO_SERVICE,
    DL_SIG_CALL_CAUSE_DROPPED,
    DL_SIG_CALL_CAUSE_SYSTEM_LOCKED,
    DL_SIG_CALL_CAUSE_NETWORK,
    DL_SIG_CALL_CAUSE_INVALID_CALL_ID,
    DL_SIG_CALL_CAUSE_NORMAL_CLEARING,
    DL_SIG_CALL_CAUSE_CALL_ALREADY_ANSWERED,
    DL_SIG_CALL_CAUSE_TOO_MANY_ACTIVE_CALLS,
    DL_SIG_CALL_CAUSE_UNASSIGNED_NUMBER,
    DL_SIG_CALL_CAUSE_NO_ROUTE_TO_DEST,
    DL_SIG_CALL_CAUSE_RESOURCE_UNAVAILABLE,
    DL_SIG_CALL_CAUSE_CALL_BARRED,
    DL_SIG_CALL_CAUSE_USER_BUSY,
    DL_SIG_CALL_CAUSE_NO_ANSWER,
    DL_SIG_CALL_CAUSE_CALL_REJECTED,
    DL_SIG_CALL_CAUSE_NUMBER_CHANGED,
    DL_SIG_CALL_CAUSE_DEST_OUT_OF_ORDER,
    DL_SIG_CALL_CAUSE_SIGNALING_ERROR,
    DL_SIG_CALL_CAUSE_NETWORK_ERROR,
    DL_SIG_CALL_CAUSE_NETWORK_BUSY,
    DL_SIG_CALL_CAUSE_NOT_SUBSCRIBED,
    DL_SIG_CALL_CAUSE_INCOMING_BARRED_IN_CUG,
    DL_SIG_CALL_CAUSE_SERVICE_UNAVAILABLE,
    DL_SIG_CALL_CAUSE_SERVICE_NOT_SUPPORTED,
    DL_SIG_CALL_CAUSE_PREPAY_LIMIT_REACHED,
    DL_SIG_CALL_CAUSE_USER_NOT_MEMBER_OF_CUG,
    DL_SIG_CALL_CAUSE_INCOMPATIBLE_DEST,
    DL_SIG_CALL_CAUSE_OUTSIDE_CALL_NOT_ALLOWED,
    DL_SIG_CALL_CAUSE_NO_CUG_SELECTED,
    DL_SIG_CALL_CAUSE_UNKNOWN_CUG_INDEX,
    DL_SIG_CALL_CAUSE_CUG_INDEX_INCOMPAT,
    DL_SIG_CALL_CAUSE_CUG_FAIL_UNSPECIFIED,
    DL_SIG_CALL_CAUSE_WEAK_SIGNAL,
    DL_SIG_CALL_CAUSE_RESTRICTED_LOCATION,
    DL_SIG_CALL_CAUSE_ACCESS_DENIED,
    DL_SIG_CALL_CAUSE_SIGNAL_FADED,
    DL_SIG_CALL_CAUSE_FEATURE_NOT_AVAILABLE,
    DL_SIG_CALL_CAUSE_WRONG_CALL_STATE,
    DL_SIG_CALL_CAUSE_SIGNALING_TIMEOUT,
    DL_SIG_CALL_CAUSE_MAX_MPTY_PARTICIPANTS_EXCEEDED,
    DL_SIG_CALL_CAUSE_SYSTEM_FAILURE,
    DL_SIG_CALL_CAUSE_DATA_MISSING,
    DL_SIG_CALL_CAUSE_BASIC_SERVICE_NOT_PROVISIONED,
    DL_SIG_CALL_CAUSE_ILLEGAL_SS_OPERATION,
    DL_SIG_CALL_CAUSE_SS_INCOMPATIBILITY,
    DL_SIG_CALL_CAUSE_SS_NOT_AVAILABLE,
    DL_SIG_CALL_CAUSE_SS_SUBSCRIPTION_VIOLATION,
    DL_SIG_CALL_CAUSE_INCORRECT_PASSWORD,
    DL_SIG_CALL_CAUSE_TOO_MANY_PASSWORD_ATTEMPTS,
    DL_SIG_CALL_CAUSE_PASSWORD_REGISTRATION_FAILURE,
    DL_SIG_CALL_CAUSE_ILLEGAL_EQUIPMENT,
    DL_SIG_CALL_CAUSE_UNKNOWN_SUBSCRIBER,
    DL_SIG_CALL_CAUSE_ILLEGAL_SUBSCRIBER,
    DL_SIG_CALL_CAUSE_ABSENT_SUBSCRIBER,
    DL_SIG_CALL_CAUSE_USSD_BUSY,
    DL_SIG_CALL_CAUSE_UNKNOWN_ALPHABET,
    DL_SIG_CALL_CAUSE_CANNOT_TRANSFER_MPTY_CALL,
    DL_SIG_CALL_CAUSE_BUSY_WITH_UNANSWERED_CALL,
    DL_SIG_CALL_CAUSE_FAILED_WITH_STATUS,
    DL_SIG_CALL_CAUSE_UNANSWERED_CALL_PENDING,
    DL_SIG_CALL_CAUSE_USSD_CANCELED,
    DL_SIG_CALL_CAUSE_FAST_RELEASE,
    DL_SIG_CALL_CAUSE_NOT_ALLOWED_BY_CC,
    DL_SIG_CALL_CAUSE_MODIFIED_TO_SS_BY_CC,
    DL_SIG_CALL_CAUSE_MODIFIED_TO_CALL_BY_CC ,
    DL_SIG_CALL_CAUSE_CALL_MODIFIED_BY_CC,
    DL_SIG_CALL_CAUSE_POWER_DOWN
};
typedef UINT8 DL_SIG_CALL_CAUSE_T;

// если number_of_calls == 0, то вызово нет
void DL_SigCallGetCallStates(CALL_STATES_T *call_states);

// Ответ на входящий вызов
void DL_SigCallReleaseReq (IFACE_DATA_T *iface_data, UINT16 call_id, DL_SIG_CALL_CAUSE_T cause);

// TRUE - звовнок (входящий/исходящий)
BOOL APP_MMC_Util_IsVoiceCall(void); // если FALSE, то вызовов нет

// TRUE - вывео звовнок (входящий/исходящий)
BOOL APP_MMC_Util_IsVideoCall(void); // если FALSE, то вызовов нет

/********************************
  Прочее
********************************/

// Воспроизведение тона
UINT32 DL_AudPlayTone (UINT32 tone, UINT8 volume); //Current volume = 0xFF
// остановка тона
void DL_AudStopTone(UINT32 tone, UINT32 seq_num);

// Получение кодов
UINT32 DL_DbSigNamGetSecurityCode (WCHAR *SCode);
UINT32 DL_DbSigNamGetUnlockCode (WCHAR *UCode);

// Установка кодов
UINT32 DL_DbSigNamStoreSecurityCode (WCHAR *SCode);
UINT32 DL_DbSigNamStoreUnlockCode (WCHAR *UCode);

// сила сигнала сети
typedef struct
{
    UINT8   percent;
    INT8    dbm;
} SIGNAL_STRENGTH_T;

// Получить силу сигнала сети
void DL_SigRegQuerySignalStrength (SIGNAL_STRENGTH_T *signal_strength);

// Cell Id
void DL_SigRegGetCellID (UINT16 *cell_id);

//Статус подключения устройства (1 - вкл., 0 - выкл) по dev_id
BOOL DL_AccGetConnectionStatus(UINT8 dev_id);

// запсь на флеш, перед записью нужно сотрать блок
// относительно адреса 0x10000000, т.е. 0x0 = 0x10000000
UINT32 FlashDevWrite(UINT8 *src, UINT32 addr, UINT32 size);

// чтение с флеш.
// относительно адреса 0x10000000, т.е. 0x0 = 0x10000000
UINT32 FlashDevRead(UINT8 *dst, UINT32 addr, UINT32 size);

// стирает блок в 128 кб (забивает 0xFF)
UINT32 FlashDevEraseBlock(UINT32 adr);

void suSleep(int time, INT32 *result); // задержка

#endif
