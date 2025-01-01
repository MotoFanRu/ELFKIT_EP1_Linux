#ifndef WEBSESSIONS_H
#define WEBSESSIONS_H

//Всё проверено на RAZR V3x R252211LD_U_85.9B.E6P
#include <typedefs.h>

// в аттаче WS_EVENT_T
#define EV_WEBSESSION_OPERATION 0x82143

#define WS_RECORD_NAME_LEN	16
#define WS_RECORD_URL_LEN	100
#define WS_RECORD_IP_LEN	8
#define WS_RECORD_CSD_LEN	32
#define WS_RECORD_LOGIN_LEN	32
#define WS_RECORD_PASS_LEN	16
#define WS_RECORD_APN_LEN	100

enum
{
	WS_RESULT_OK = 0,
	WS_RESULT_IN_PROGRESS,
	WS_RESULT_FAILED
};
typedef UINT8 WS_RESULT_T;


typedef enum
{
	WS_TIMEOUT_NULL = 0,
	WS_TIMEOUT_1_MIN,
	WS_TIMEOUT_2_MIN,
	WS_TIMEOUT_5_MIN,
	WS_TIMEOUT_10_MIN,
	WS_TIMEOUT_15_MIN
} WS_TIMEOUT_T;

typedef struct
{
	WCHAR	apn[WS_RECORD_APN_LEN + 1];
	WCHAR	login[WS_RECORD_LOGIN_LEN + 1];
	WCHAR	password[WS_RECORD_PASS_LEN + 1];
} WS_GPRS_RECORD_T;

typedef struct
{
	char	csd_number[WS_RECORD_CSD_LEN + 1];	//0x00	CSD No
	WCHAR	user_name[WS_RECORD_LOGIN_LEN + 1];	//0x22	Имя пользователя
	WCHAR	password[WS_RECORD_PASS_LEN + 1];	//0x64	Пароль
	UINT8	speed;					//0x86	Скорость (б/c)
	UINT8	line_type;				//0x87	Тип линии
	UINT8	pad;					//0x88
} WS_CSD_RECORD_T;

typedef struct
{
	UINT8		 attr;				//+++
	UINT8		 pad1;				//+++ = 0
	WCHAR		 name[WS_RECORD_NAME_LEN+1];	//+++
	WCHAR		 home_page[WS_RECORD_URL_LEN+1];//+++
	UINT16		 ip1[WS_RECORD_IP_LEN];		//+++
	UINT16		 port1;				//+++
	WCHAR		 domain1[WS_RECORD_URL_LEN+1];	//+++
	UINT8		 service_type1;			//+++
	UINT8		 pad2;				//+++ = 0
	UINT16		 ip2[WS_RECORD_IP_LEN];		//+++
	UINT16		 port2;				//+++
	WCHAR		 domain2[WS_RECORD_URL_LEN+1];	//+++
	UINT8		 service_type2;			//+++
	UINT8		 pad3;				//+++ = 0
	UINT16		 dns1[WS_RECORD_IP_LEN];	//+++
	UINT16		 dns2[WS_RECORD_IP_LEN];	//+++
	UINT8		 timeout;			//+++
	UINT8		 pad4;				//+++ = 0
	WS_CSD_RECORD_T	 *csd_record1;			//+++
	WS_CSD_RECORD_T	 *csd_record2;			//+++
	WS_GPRS_RECORD_T *gprs_record;			//+++
} WS_RECORD_T;

// приходит в аттаче EV_WEBSESSION_OPERATION
typedef struct
{
	WS_RESULT_T		result;
	UINT8			index;
} WS_EVENT_T;


// Получение записи Web-сессии по индексу
WS_RESULT_T DL_DbWebSessionsGetSessionByIndex (IFACE_DATA_T *iface_data, UINT8 index, WS_RECORD_T *ws_record);

// Получение индекса текущей Web-сессии
WS_RESULT_T DL_DbWebSessionsGetDefaultSessionIndex (UINT8 *index);

// добавить запись (device_type = 0, unk = 0)
WS_RESULT_T DL_DbWebSessionsAddSession (IFACE_DATA_T *iface_data, UINT8	device_type, WS_RECORD_T *ws_record, UINT8 unk);

// удалить запись (unk = 0)
WS_RESULT_T DL_DbWebSessionsDeleteSessionByIndex (IFACE_DATA_T *iface_data, UINT8 index, UINT8 unk);

// кол-во  возможных записей (device_type = 0, unk = 0)
WS_RESULT_T DL_DbWebSessionsGetNumberGoodSessions (UINT8 device_type, UINT8 *total_num, UINT8 unk);

// кол-во используемых записей (device_type = 0, unk = 0)
WS_RESULT_T DL_DbWebSessionsGetNumberUsedSessions (UINT8 device_type, UINT8 *used_num, UINT8 unk);

// кол-во доступных записей (device_type = 0, unk = 0)
WS_RESULT_T DL_DbWebSessionsGetNumberAvailableSessions (UINT8 device_type, UINT8 *available_num, UINT8 unk);

// Сделать сессию с индексом index текущей
UINT32 DL_DbWebSessionsSetDefaultSessionIndex(SU_PORT_T* port, UINT8 index);

UINT32 DL_DbWebSessionsFindAbsIndexByName(WCHAR* name, UINT8* index, UINT32 unk); // unk = 4

#endif
