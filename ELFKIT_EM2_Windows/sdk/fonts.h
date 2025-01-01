#ifndef FONTS_H
#define FONTS_H

#include <typedefs.h>

//Адрес таблицы шрифтов в прошиве
EXTERN_LIB FONT_TABLE;
#define FontTable (UINT8 *)FONT_TABLE

//Размер таблицы шрифтов
#define FONT_TABLE_SIZE 96

//Размер имени шрифта
#define FONT_NAME_SIZE 0x32

// стиль шрифта
enum
{
    FONT_STYLE_PLAIN = 0,           //Обычный
    FONT_STYLE_BOLD,                //Жирный
    FONT_STYLE_ITALIC,              //Курсив
    FONT_STYLE_ITALIC_BOLD          //Жирный + курсив (FONT_STYLE_ITALIC | FONT_STYLE_BOLD)
};

typedef struct 
{
	UINT8          font_id;                //0x00, Индекс шрифта
	UINT8          font_style;             //0x01, Стиль шрифта
	UINT16         font_point_size;        //0x02, Размер шрифта (font_size = font_size & 0xF !!!!).использовать FONT_SET_SIZE для установки размера!
	UINT16         font_frac_point_size;   //0x04
} FONT_ATTRIB_T;

#define FONT_SET_SIZE(size) size & 0xF

typedef struct
{
    UINT32 end_descriptor_address;
    UINT32 start_address;
    UINT32 end_address;
} FONT_TABLE_DESCRIPTOR_T;

typedef struct
{
    UINT8 id;
    UINT8 unk1;
    WCHAR font_name[FONT_NAME_SIZE + 1];
    UINT32 font_address;
    UINT32 unk2;
    UINT8 min_size;
    UINT8 max_size;
    UINT8 end[6];
} FONT_ENTRY_T;

UINT32 UIS_CanvasGetAttributesFromFontID(FONT_ATTRIB_T *font_attribs, UINT8 font_id);
#endif
