#ifndef RESOURCES_H
#define RESOURCES_H

#include <res_def.h>
#include <typedefs.h>

/* ID ресурса формируется типом ресурса в качестве старшего байта и номером ресурса текущего типа
    Например, 0x01000496 = "Системная ошибка"
    Идентификаторы для строк и картинок берутся, соответственно, из ленгпака и DRM */


#define RES_TYPE_STRING		0x01
#define RES_TYPE_BITMAP		0x02
#define RES_TYPE_INTEGER	0x04
#define RES_TYPE_ACTION		0x06
#define RES_TYPE_CONTENT	0x0C
#define RES_TYPE_FIELD		0x0D
#define RES_TYPE_GRAPHICS	0x12
#define RES_TYPE_ANIMATION 	0x12

typedef UINT32	RESOURCE_ID;

// Структура для описания ресурса для Action-а
typedef struct
{
	RESOURCE_ID		softkey_label;		// Надпись на софт-кнопке
	RESOURCE_ID		list_label;		// Надпись в списке действий
	INT8			softkey_priority;	// Приоритет расположения на софткнопках, отрицательый - на левый софт, положительный - на правый
    	INT8			list_priority;		// Приоритет расположения в списке действий, два пункта с наивысшим приоритетом располагаются на софткнопках, и дальше смотрится softkey_priority
    	BOOL			isExit;			// ?? Явлется ли командой выхода из приложения
	BOOL			sendDlgDone;		// Посылать ли дополнительно ивент EV_DIALOG_DONE
} RES_ACTION_LIST_ITEM_T; 

//взято с LTE (не проверено)
enum
{
	UIS_FIELD_BINARY_CAP_STRING = 0,
	UIS_FIELD_BINARY_CURRENCY,
	UIS_FIELD_BINARY_NUMBER,
	UIS_FIELD_CAP_STRING,
	UIS_FIELD_CLEANUP,
	UIS_FIELD_CURRENCY,
	UIS_FIELD_DATE,
	UIS_FIELD_DURATION,
	UIS_FIELD_ELAPSED_TIME,
	UIS_FIELD_ENUMERATED, // 9
	UIS_FIELD_MESSAGE,
	UIS_FIELD_METER,
	UIS_FIELD_NAME,
	UIS_FIELD_NULL_CAP_STRING,
	UIS_FIELD_NULL_MESSAGE,
	UIS_FIELD_NULL_NAME,
	UIS_FIELD_NULL_NUMBER,
	UIS_FIELD_NULL_TEXT,
	UIS_FIELD_NUMBER,
	UIS_FIELD_PHONE_NUMBER,
	UIS_FIELD_ZIP_CODE,
	UIS_FIELD_SPEED_NUM,
	UIS_FIELD_TEXT,
	UIS_FIELD_TIME_OF_DAY,
	UIS_FIELD_VOICE_NAME,
	UIS_FIELD_SEGMENT,
	UIS_FIELD_NULL_SEGMENT,
	UIS_FIELD_NOTES,
	UIS_FIELD_NULL_PHONE_NUMBER,
	UIS_FIELD_DATE_DEFINED,
	UIS_FIELD_NICKNAME,
	UIS_FIELD_NUMBER_WITHOUT_LEADING_ZERO,
	UIS_FIELD_NULL_EMAIL,
	UIS_FIELD_NULL_URL,
	UIS_FIELD_NULL_PHONE_NUMBER_STORE_SIM, 
	UIS_FIELD_BOGUS_INIT = 99
};
typedef UINT16 UIS_FIELD_TYPE_T;

typedef struct
{
        UIS_FIELD_TYPE_T   type;	// Тип поля
	UINT8              unk1[2];	// 00 00 ???
        RESOURCE_ID        label;	// имя "опции"
        BOOL               view_olny;
        BOOL               show_value;
        BOOL               secure;
        UINT32             maxlen;
        RESOURCE_ID        resource;	//res_type = 0x10
	UINT8              unk2;	//???
        UINT32             auth_event;	//0xB

} FIELD_DESCR_T, FIELD_DESCR_CONTENT_T, UIS_FIELD_DESCRIPTION_RESOURCE_CONTENTS_T;

typedef enum 
{
	LNG_ENGLISH=1,
	LNG_BRITISH_ENGLISH=3,
	LNG_CHINESE_COMPLEX, 
	LNG_CHINESE_SIMPLE,
	LNG_RUSSIAN=35  // или 0x2e
	
} LANGUAGE_T;

// Создать ресурс
UINT32 DRM_CreateResource(RESOURCE_ID *res_id, UINT32 res_type, void *data, UINT32 size);

// Получить содержимое ресурса
UINT32 DRM_GetResource(RESOURCE_ID res_id, void *buf, UINT32 size);

// Получить размер ресурса
UINT32 DRM_GetResourceLength(RESOURCE_ID res_id, UINT32 *size);

// Изменить ресурс
UINT32 DRM_SetResource(RESOURCE_ID res_id, void *data, UINT32 size);

// Уничтожить ресурс
UINT32 DRM_ClearResource(RESOURCE_ID res_id);

// получить текущий язык
UINT8 DRM_GetCurrentLanguage(UINT8 *LGID);

// установить язык
UINT8 DRM_SetLanguage(UINT8 LGID);
#endif
