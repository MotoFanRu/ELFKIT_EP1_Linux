#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <typedefs.h>

#define PB_RECORD_NAME_LEN 24
#define PB_RECORD_NUMBER_LEN 64

typedef enum 
{
	PHONEBOOK_DEVICE_PHONE,
	PHONEBOOK_DEVICE_SIM,
	PHONEBOOK_DEVICE_ALL
} PHONEBOOK_DEVICE_T;

// запись справочника
typedef struct 
{
	UINT32	index;				  // индекс записи
	UINT16	unk;				  //??????
	WCHAR	name[PB_RECORD_NAME_LEN + 1];	  // имя
	WCHAR	number[PB_RECORD_NUMBER_LEN + 1]; // номер
	UINT8	unk1[27];
	UINT8	group;
	UINT8	pad[342];
} PHONEBOOK_RECORD_T; // 0x21C

// поиск записи по номеру телефона. Отправляет ивент EV_PHONEBOOK_READ_RECORD
UINT32 DL_DbPhoneBookFindRecordByTelno(IFACE_DATA_T *data, UINT8 list_type /*0 - phone, 1 - sim card*/, WCHAR *tel_number, UINT8 sort_type /*0*/);

// возвращает кол-во категорий
// UINT32 DL_DbPhoneBookGetNumberofCategories(UINT8 list_type, PHONEBOOK_DEVICE_T device, UINT16 *count_categories);

// получение категории. Отправляет ивент EV_PHONEBOOK_READ_RECORD
// UINT32 DL_DbPhoneBookGetCategoryRecordByIndex(IFACE_DATA_T *data, UINT8 list_type, PHONEBOOK_DEVICE_T device, UINT8 category_index);
#endif
