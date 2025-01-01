#ifndef RES_DEF_H
#define RES_DEF_H

// -----------------------------------------------------------------------------------------
// RES_TYPE_GRAPHICS

#define DRM_FOLDER		0x120000CE
#define DRM_FILE_IMAGE		0x120000CF
#define DRM_FILE_VIDEO		0x120000D5
#define DRM_FILE_AUDIO		0x120000CD
#define DRM_FILE_PLAYLIST	0x120000D1

#define DRM_SELECTED		0x120000CC
#define DRM_APPJAVA		0x120000D0
#define DRM_INTERNAL_MEM	0x120000D9	//PHONE
#define DRM_EXTERNAL_MEM	0x120000DA	//TRANSFLASH

// -----------------------------------------------------------------------------------------
// RES_TYPE_STRING

#define LANG_AM			0x01000CFB	//AM
#define LANG_PM			0x01000D0C	//PM

#define LANG_EXIT		0x01000A64	// Выход
#define LANG_DELETE		0x010009D6	// Удалить
#define LANG_COPY		0x010009E1	// Копировать
#define LANG_CUT		0x010009E2	// Вырезать

#define LANG_COPYED		0x010009E5	// "Скопировано"
#define LANG_CUTED		0x010009E6	// "Вырезано"

#define LANG_BEGIN		0x010009F0	// Начало

#define LANG_HIDDEN		0x01001E20

#define LANG_PROPERTIES		0x01001CED

//#define LANG_FILENAME	0x01001CC1
#define LANG_FILENAME	0x0100086F
#define LANG_FILETYPE	0x01001CC2

#define LANG_CLOSE		0x01001CBB

#define LANG_ATT		0x01000DDC

#define LANG_AUDIO		0x01000DAC
#define LANG_MEDIA		0x01000DAD
#define LANG_VIDEO		0x01000DB0


#define LANG_MOVE1		0x01000D59 // Переместить
#define LANG_COPY1		0x010001C4 //
#define LANG_DEL1		0x01000078 //
#define LANG_FILES		0x01001160 // файл(ов)

#define LANG_ACT_RENAME	0x01000877
#define LANG_FOLDER	0x01000D43	// "Папка"


#define LANG_TYPE	0x01000157	// "Тип"
#define LANG_SIZE	0x0100087D	// "Размер"

#define LANG_FILE_TYPE	0x01000A48	// "Тип файла:"
#define LANG_FILE_SIZE	0x01000A49	// "Размер файла:"

#define LANG_BYTE		0x0100080C	// "байт"

#define LANG_BLUETOOTH		0x01000F66	// Bluetooth
#define LANG_APPLICATIONS	0x01001701	// Приложения
#define LANG_SERVICES_MANAGER	0x01000D10	// Диспетчер служб
#define LANG_FONT_TYPE		0x01001894	// Тип шрифта
#define LANG_SELECTION_MENU	0x010001B8	// Меню выбора
#define LANG_USB_SETTINGS	0x01001783	// Настройки USB

#define LANG_ABOUT		0x01000898	//
#define LANG_INFORMATION	0x01000693	//Информация


#define LANG_YES		0x0100016A
#define LANG_NO			0x010000E6


#define LANG_EDIT		0x01000D8C
#define LANG_STORE		0x01000211
#define LANG_SAVE		0x01000375
#define LANG_SAVE_AS	0x01000AD8
#define LANG_SEND       0x01000356      //отправить

#define LANG_END	0x01001204
#define LANG_RUN        0x01001283
#define LANG_ADD        0x0100066E
#define LANG_REMOVE     0x0100049D

#define LANG_SELECT     0x01000305

#define LANG_RESTART    0x010017F7

#define LANG_APPLY      0x01000E0A
#define LANG_ERROR	0x010004B8
#define LANG_COMPLETE	0x01000558

#define LANG_SYNC_WITH_SERVER	0x01000D7E


#define LANG_NAME	 0x010007B9
#define LANG_FILE_LENGTH 0x01000A49
#define LANG_DELETE_ALL	 0x010001CA
#define LANG_BITRATE	 0x01000DD5
#define LANG_TYPE2	 0x01000A48
#define LANG_PLAY	 0x0100112B
#define LANG_VOLUME	 0x01000164

#define LANG_RECORD	 0x010002FC
#define LANG_RECORDING	 0x01001B84	//Запись...
#define LANG_STOP	 0x010004FA

#define LANG_STATE	 0x010006F6	//Состояние:
#define LANG_STOPPED	 0x01000A2B	//Остановлено
#define LANG_TIME	 0x0100080A	//Время:
/*
#define LANG_FILE_SIZE
#define LANG_FILE_LENGTH
#define LANG_BITRATE

#define LANG_TYPE2
#define LANG_DELETE
#define LANG_BITRATE
*/

#define RES_FIELD_NUMBER			0x0D00001A
#define RES_FIELD_PASS_WORD			0x0D0001AD
#define RES_FIELD_VOLUME			0x0D0002E2
#define RES_FIELD_NAME				0x0D00019B
#endif
