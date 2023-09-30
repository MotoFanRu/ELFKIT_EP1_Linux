#ifndef RESOURCES_H
#define RESOURCES_H

#include <typedefs.h>

/* ID ресурса формируется типом ресурса в качестве старшего байта и номером ресурса текущего типа
    Например, 0x01000496 = "Системная ошибка"
    Идентификаторы для строк и картинок берутся, соответственно, из ленгпака и DRM */


#define RES_TYPE_STRING         0x01
#define RES_TYPE_ACTION		    0x06
#define RES_TYPE_GRAPHICS       0x12
#define RES_TYPE_CONTENT 		0x0C
#define RES_TYPE_INTEGER 		0x04
#define RES_TYPE_BITMAP 		0x02


typedef UINT32	RESOURCE_ID;

// Создать ресурс
UINT32 DRM_CreateResource( RESOURCE_ID      *res_id,
                           UINT32           type,       //RES_TYPE_*
                           void             *data,
                           UINT32           size );

// Получить содержимое ресурса
UINT32 DRM_GetResource( RESOURCE_ID     res_id,
                        void            *buf,
                        UINT32          size );

// Получить размер ресурса
UINT32 DRM_GetResourceLength( RESOURCE_ID res_id,  UINT32 *size );

// Изменить ресурс
UINT32 DRM_SetResource( RESOURCE_ID     res_id,
                        void            *data,
                        UINT32          size );

// Уничтожить ресурс
UINT32 DRM_ClearResource( RESOURCE_ID res_id );



/* Структура для описания ресурса для Action-а */
typedef struct
{
	RESOURCE_ID			softkey_label;		// Надпись на софт-кнопке
    RESOURCE_ID			list_label;			// Надпись в списке действий
    INT8				softkey_priority;	// Приоритет расположения на софткнопках, отрицательый - 
    										// на левый софт, положительный - на правый
    INT8				list_priority;		// Приоритет расположения в списке действий, два пункта
    										// с наивысшим приоритетом располагаются на софткнопках, и дальше смотрится softkey_priority
    BOOL            	isExit;				// ?? Явлется ли командой выхода из приложения
    BOOL            	sendDlgDone;		// Посылать ли дополнительно ивент EV_DIALOG_DONE
} RES_ACTION_LIST_ITEM_T; 

// получаем текущий язык
UINT8 DRM_GetCurrentLanguage(UINT8 * LGID);

/*
LGID:
04 Chinese Complex 
05 Chinese Simple
03 British English
01 English
*/


#endif
