#ifndef CANVAS_H
#define CANVAS_H

#include <fonts.h>
#include <typedefs.h>
#include <resources.h>
#include <utilities.h>
#include <uis.h>

// Точка привязки для текста и картинок
// вертикальная
#define ANCHOR_BASELINE               64 // только текст
#define ANCHOR_BOTTOM                 32
#define ANCHOR_TOP                    16
#define ANCHOR_VCENTER                2  // только картинка

// горизонтальная
#define ANCHOR_RIGHT                  8
#define ANCHOR_LEFT                   4
#define ANCHOR_HCENTER                1

//  использовать знак "или". Например: ANCHOR_LEFT | ANCHOR_TOP

/*******************************/

typedef enum
{
    // изображения
    MIME_TYPE_IMAGE_GIF = 0,
    MIME_TYPE_IMAGE_BMP,
    MIME_TYPE_IMAGE_WBMP,
    MIME_TYPE_IMAGE_PNG,
    MIME_TYPE_IMAGE_JPEG,

    // аудио
    MIME_TYPE_AUDIO_MID = 11,
    MIME_TYPE_AUDIO_MIDI,
    MIME_TYPE_AUDIO_MIX,
    MIME_TYPE_AUDIO_BAS,
    MIME_TYPE_AUDIO_MP3,
    MIME_TYPE_AUDIO_AAC,
    MIME_TYPE_AUDIO_AMR = 24,
    MIME_TYPE_AUDIO_MP4 = 26,
    MIME_TYPE_AUDIO_M4A,

    // видео
    MIME_TYPE_VIDEO_ASF = 34,
    MIME_TYPE_VIDEO_MP4 = 36,
    MIME_TYPE_VIDEO_MPEG4,
    MIME_TYPE_VIDEO_H263

} MIME_TYPE_T;

typedef struct
{
   UINT8 *buf;     // буфер для рисования. Должен быть NULL
   UINT16 w;       // ширина
   UINT16 h;       // высота
} DRAWING_BUFFER_T;

typedef struct
{
    UINT8 red;
    UINT8 green;
    UINT8 blue;
    UINT8 transparent; // прозрачность
} COLOR_T;

#define COLOR_RED        (COLOR_T)(0xFF000000)
#define COLOR_GREEN        (COLOR_T)(0x00FF0000)
#define COLOR_BLUE        (COLOR_T)(0x0000FF00)
#define RGB(r,g,b)        (COLOR_T)((BYTE)(r)<<24 | (BYTE)(g)<<16 | (BYTE)(b)<<8)
#define RGBT(r,g,b,t)    (COLOR_T)((UINT8)(r)<<24 | (UINT8)(g)<<16 | (UINT8)(b)<<8 | (UINT8)(t))
#define RGB_getr(c)        (BYTE)((UINT32)(c) >> 24 & 0xFF)// RED
#define RGB_getg(c)        (BYTE)((UINT32)(c) >> 16 & 0xFF)// GREEN
#define RGB_getb(c)        (BYTE)((UINT32)(c) >>  8 & 0xFF)// BLUE
#define RGB_gett(c)        (BYTE)((UINT32)(c)       & 0xFF)// TRANSPARENT

typedef struct
{
    INT16 x;   // горизонтальная координата x
    INT16 y;   // вертикальная координата
} GRAPHIC_POINT_T;

typedef struct
{
    GRAPHIC_POINT_T ulc;  // верхний левый угол
    GRAPHIC_POINT_T lrc;  // нижний правый угол
} GRAPHIC_REGION_T;

enum  // drawOp
{
    AlphaBlend,
    Copy,
        NotUsed1,
    Invert,
    NotCopy,
    Resize,
    Xor,
        NotUsed2,
    Move
};

enum  //тип картинки ... picture_type
{
    DRM_PICTURE_TYPE,
    IMAGE_PATH_TYPE,
    STORED_IN_MEMORY_TYPE,
    FILE_HANDLE_TYPE,
    IMAGE_PATH_WITH_OFFSET
};

typedef WCHAR *IMAGE_PATH_T; // путь к картинке

typedef struct
{
    void *resourcePicturePointerValue;  // Местоположение указателя в памяти
    UINT32 imageSize;                   // размер изображения
} PICTURE_POINTER_T;

typedef struct
{
    FILE_HANDLE_T fileHandle;
    UINT32 imageOffset;
    UINT32 imageSize;
} UIS_FILE_HANDLE_T;

typedef struct
{
    IMAGE_PATH_T        PathValue;
    MIME_TYPE_T       mimeFormat; // тип картинки
    UINT32            imageOffset;
    UINT32            imageSize;
} FILE_PATH_WITH_OFFSET_T;

typedef union
{
    RESOURCE_ID                   DrmValue;
    IMAGE_PATH_T                PathValue;
    PICTURE_POINTER_T           PointerValue;
    UIS_FILE_HANDLE_T           FileHandleValue;
    FILE_PATH_WITH_OFFSET_T     PathWithOffsetValue;
} PICTURE_TYPE_UNION_T;

typedef UINT8 CANVAS_IMAGE_HANDLE_T;

typedef struct
{
    UINT16 height;
    UINT16 width;
} GRAPHIC_METRIC_T;

typedef struct
{
    GRAPHIC_REGION_T offset_region;
    UINT8*  background_buffer;
    UINT16 width;
    UINT16 height;
} IMAGE_BACKGROUND_BUFFER;

typedef struct
{
    COLOR_T begin_color;
    COLOR_T end_color;
} GRADIENT_T;


typedef enum
{
    IA_STILL_IMAGE=4, // первый кадр анимации
    IA_BACKGROUND_TRANSPARENT_IMAGE, // IMAGE_BACKGROUND_BUFFER
    IA_BACKGROUND_TRANSPARENT_COLOR, // COLOR_T
    IA_BACKGROUND_TRANSPARENT_GRADIENT, // GRADIENT_T

    IA_BACKGROUND_TRANSPARENT_TILED_IMAGE,
    IA_BACKGROUND_WALLPAPER_NEEDED,

    IA_START_ANIMATION=10, // старт анимации
    IA_BACKGROUND_EXCLUDING_IMAGE, // COLOR_T

    IA_NO_PAINT_WHITE=13,  // не рисовать белый
    IA_NO_PAINT_WHITE_WITH_CROSS,
    IA_PAINT_WHITE_AND_PAINT_WHITE_WITH_CROSS,
    IA_NO_OF_CYCLES_OF_ANIMATION, // UINT8 номер цикла
    IA_DECODE_WALLPAPER_GRAYSCALE,
    IA_PRESERVE_BACKGROUND_GRAYSCALE,

    IA_OBEY_TRANSPARENCY=23,

} IMAGE_ATTRIBUTE_TYPE_T;

/*******************************/

//создать Canvas
UIS_DIALOG_T UIS_CreateColorCanvas( SU_PORT_T  *port,
                                    DRAWING_BUFFER_T * drawing_buffer,
                                    BOOL use_soft_icon_area );

/*    Цвета поумолчанию
BackgroundColor - white
FillColor  - white
ForegroundColor  - blue
*/

// с картинкой
UIS_DIALOG_T UIS_CreateColorCanvasWithWallpaper (  SU_PORT_T  *port,
                                                   DRAWING_BUFFER_T * drawing_buffer,
                                                   BOOL use_status_line_area,
                                                   BOOL wallpaper_flag );

/**************************
  Графические примитивы
***************************/

// рисует точку
void UIS_CanvasDrawPixel(GRAPHIC_POINT_T point, UIS_DIALOG_T handle);

//  рисует  линию
void UIS_CanvasDrawLine(GRAPHIC_POINT_T begin,
                        GRAPHIC_POINT_T end,
                        UIS_DIALOG_T handle);

// рисует закрашенный прямоугольник
void UIS_CanvasFillRect(GRAPHIC_REGION_T region,
                        UIS_DIALOG_T handle);

//  рисует прямоугольник
void UIS_CanvasDrawRect(GRAPHIC_REGION_T region,
                        BOOL fill, //  заливка
                        UIS_DIALOG_T handle);

// рисует закруглённый прямоугольник
void UIS_CanvasDrawRoundRect(GRAPHIC_REGION_T region,
                            UINT16 arcW, // горизонтальный диаметр скругления
                            UINT16 arcH, // вертикальный диаметр скругления
                            BOOL fill,
                            UIS_DIALOG_T handle);

// рисует дугу окружности или эллипса
void UIS_CanvasDrawArc(GRAPHIC_REGION_T region,
                        UINT16 startAngle,    // начальный угол.
                        UINT16 arcAngle,      // конечный угол
                        BOOL fill,
                        UIS_DIALOG_T handle);

// рисует многоугольник
void UIS_CanvasDrawPoly(UINT16 number_of_vertices,
                        GRAPHIC_POINT_T  *array,
                        BOOL fill,
                        UIS_DIALOG_T handle);

/*******************************/

// рисует строку
void UIS_CanvasDrawColorText( WCHAR *str,
                              UINT16 offset,
                              UINT16 str_len,                 // длинна строки
                              GRAPHIC_POINT_T anchor_point, // координаты точки привязки
                              UINT16 anchor_value,           // значение точки привязки. Например: ANCHOR_LEFT | ANCHOR_TOP
                              UIS_DIALOG_T handle);

UINT32 UIS_CanvasDrawText ( UINT8* draw_buf,
                            WCHAR *str,
                            GRAPHIC_REGION_T region,
                            UINT8 font_id,
                            BOOL unk1,
                            COLOR_T color,
                            void *unk2 );

// задаёт шрифт
UINT32 UIS_CanvasSetFont( UINT8 font_id, // 0 - NetMonitor, 1- General Font... загляните в MotoLangEditor
                          UIS_DIALOG_T handle);



//возвращает стиль шрифта
UINT8 UIS_CanvasGetFontStyle(UIS_DIALOG_T handle);
// задает стиль    шрифта
UINT32 UIS_CanvasSetFontStyle(UINT8 font_style, UIS_DIALOG_T handle);


// возвращает размер строки
// для задания var_1 и var_2 использовать SET_VAR_1 и SET_VAR_2 соответственно
// или использовать GET_STRING_SIZE
UINT32 UIS_CanvasGetStringSize(WCHAR *string, GRAPHIC_METRIC_T *string_size, UINT32 var_1, UINT32 var_2);
//UINT32 UIS_CanvasGetStringSize(WCHAR *string, GRAPHIC_METRIC_T *string_size, FONT_ATTRIB_T font_attr);

// возвращает размер региона вывода строки
// для задания var_1 и var_2 использовать SET_VAR_1 и SET_VAR_2 соответственно
// или использовать GET_STRING_REGION
UINT32 UIS_CanvasGetStringRegion(WCHAR *string, GRAPHIC_METRIC_T *string_region, UINT32 var_1, UINT32 var_2);
//UINT32 UIS_CanvasGetStringRegion(WCHAR *string, GRAPHIC_REGION_T *string_region, FONT_ATTRIB_T font_attr);

#define SET_VAR_1(font_attrib) (UINT32)(((font_attrib.font_id << 24) | (font_attrib.font_style << 16)) | font_attrib.font_point_size)
#define SET_VAR_2(font_attrib) (UINT32)(font_attrib.font_frac_point_size << 16)
#define GET_STRING_SIZE(string, string_size, font_attrib) UIS_CanvasGetStringSize(string, string_size, SET_VAR_1(font_attrib), SET_VAR_2(font_attrib))
#define GET_STRING_REGION(string, string_region, font_attrib) UIS_CanvasGetStringRegion(string, string_region, SET_VAR_1(font_attrib), SET_VAR_2(font_attrib))



// размер шрифта
UINT32 UIS_CanvasGetFontSize (UINT8 font_id, GRAPHIC_METRIC_T * font_size);
UINT32 UIS_CanvasSetFontSize (UINT16 font_size, UIS_DIALOG_T dialog);


UINT32 utility_draw_image(RESOURCE_ID res_id, GRAPHIC_REGION_T region, void *p_attr, void *drawBuf, UINT32 unk1);

void utility_get_image_size(RESOURCE_ID res_id, UINT16 *width, UINT16 *height);

/*******************************/

// рисует картику
UINT32 UIS_CanvasDrawImage(UINT8 picture_type,            // тип. откуда будем брать картинку. DRM, файл....
                           PICTURE_TYPE_UNION_T picture_data,
                           GRAPHIC_POINT_T anchor_point,
                           UINT16 anchor_value,
                           UINT8 drawOp,
                           UIS_DIALOG_T handle);

UINT32 UIS_CanvasDrawWallpaper(CANVAS_IMAGE_HANDLE_T image_handle,
                               GRAPHIC_REGION_T src_area,
                               GRAPHIC_REGION_T dest_region,
                               UINT8 drawOp,
                               WALLPAPER_LAYOUT_T layout,
                               UIS_DIALOG_T handle);


UINT32 UIS_CanvasDrawColorBitmap( void*         picture_bytes,  // первый байт - ширина, второй байт высота
                                                                       // остальное 8-bpp(RGB332)  или  16-bpp(RGB565)
                                GRAPHIC_POINT_T  ulc_anchor_point,
                                UIS_DIALOG_T  handle );

// открывает картинку
CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImage(UINT8 picture_type,
                                          PICTURE_TYPE_UNION_T picture_data);

// открывает картинку     и получает её размер
//CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImageAndGetSize(UINT8 picture_type, PICTURE_TYPE_UNION_T picture_data, GRAPHIC_POINT_T *attributes); unk2, unk3, unk4 = 0;
CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImageAndGetSize(UINT8 picture_type, PICTURE_TYPE_UNION_T picture_data, UINT32 unk2, UINT32 unk3, UINT32 unk4, GRAPHIC_POINT_T *attributes);

// закрывает картинку
UINT32 UIS_CanvasCloseImage(CANVAS_IMAGE_HANDLE_T image_handle);


// изменение размера
UINT32 UIS_CanvasResizeImage(CANVAS_IMAGE_HANDLE_T image_handle,
                             UINT16 percent, // zoom в процентах
                             GRAPHIC_REGION_T src_area, // Исходный размер изображения
                             GRAPHIC_REGION_T dest_region,// новый размер
                             UINT8 drawOp,
                             UIS_DIALOG_T handle);
// возвращает размер изображения
//GRAPHIC_POINT_T UIS_CanvasGetImageSize(UINT8 picture_type, PICTURE_TYPE_UNION_T picture_data); unk2, unk3, unk4 = 0;
UINT32 UIS_CanvasGetImageSize(GRAPHIC_POINT_T *img_size, UINT8 picture_type, PICTURE_TYPE_UNION_T picture_data, UINT32 unk2, UINT32 unk3, UINT32 unk4);

//вовращает размер картинки в байтах
UINT32 UIS_CanvasGetImageSizeInBytes(CANVAS_IMAGE_HANDLE_T image_handle);


UINT32 UIS_CanvasIsImageAnimated(CANVAS_IMAGE_HANDLE_T image_handle);
UINT32 UIS_CanvasPauseAnimation(CANVAS_IMAGE_HANDLE_T image_handle);


// устанавливает аттрибут
UINT32 UIS_CanvasSetImageAttribute(CANVAS_IMAGE_HANDLE_T image_handle,
                                    IMAGE_ATTRIBUTE_TYPE_T attribute_type,
                                    void* attribute_value);
// получает аттрибут
UINT32 UIS_CanvasGetImageAttribute(CANVAS_IMAGE_HANDLE_T image_handle,
                                    IMAGE_ATTRIBUTE_TYPE_T attribute_type,
                                    void* attribute_value);
// вставляет изображение в буфер
UINT32 UIS_CanvasRenderImageToBuffer(CANVAS_IMAGE_HANDLE_T image_handle, //+++
                                     GRAPHIC_POINT_T image_size,         //
                                     GRAPHIC_POINT_T anchorpoint,
                                     UINT16 anchor_value,                //+++
                                     GRAPHIC_REGION_T * image_region,    //+++ size = 4 bytes
                                     UINT8 drawOp,                       //+++
                                     UIS_DIALOG_T handle);

// преобразует картинку в буфер pointer
UINT32 UIS_CanvasDecodeImageToPointer(CANVAS_IMAGE_HANDLE_T image_handle, void* pointer);

// вставляет буфер pointer
UINT32 UIS_CanvasInsert(UIS_DIALOG_T handle,void* pointer, GRAPHIC_POINT_T point);

// перемещает регион
UINT32 UIS_CanvasMoveRegion(GRAPHIC_REGION_T source_region,
                            GRAPHIC_REGION_T dest_region,
                            UIS_DIALOG_T handle);



/*******************************/

// задает цвет заливки
void UIS_CanvasSetFillColor(COLOR_T color);
// возвращает цвет заливки
COLOR_T UIS_CanvasGetFillColor(void);


// задает цвет для линий, прямоугольников, текста.....
void UIS_CanvasSetForegroundColor(COLOR_T color);
// возвращает цвет для линий, прямоугольников, текста.....
COLOR_T UIS_CanvasGetForegroundColor(void);

// цвет фона
void UIS_CanvasSetBackgroundColor(COLOR_T color);
COLOR_T UIS_CanvasGetBackgroundColor(void);


// ширина линии
void UIS_CanvasSetLineWidth(UINT16 width);
UINT16 UIS_CanvasGetLineWidth(void);


/*******************************/

// обновление экрана
void UIS_CanvasRefreshDisplayRegion(UIS_DIALOG_T handle, GRAPHIC_REGION_T region);
void UIS_CanvasRefreshDisplayBuffer(UIS_DIALOG_T handle);

/*******************************/

//возвращает  возможную глубину цвета в битах
UINT32 UIS_CanvasGetColorDepth (void);

//получает размер дисплея
UINT32 UIS_CanvasGetDisplaySize(GRAPHIC_POINT_T *disp_size);

// включает выключает подсветку
UINT32 UIS_CanvasSetBackLight(BOOL state);

/**********************/
enum
{
	WHOLE_DISPLAY_AREA,
	NORMAL_DISPLAY_AREA,
	COMMON_DISPLAY_AREA,
	SOFTKEY_AREA,
	STATUS_LINE_AREA,
	TITLE_BAR_AREA
};
typedef UINT32 WORKING_AREA_T;

// получение размеров области
UINT8 UIS_CanvasGetWorkingArea (   GRAPHIC_REGION_T *working_area,
								   UINT8 *count_lines,
								   UINT8 *chars_on_line,
								   WORKING_AREA_T areas,
								   BOOL use_status_line_area,
								   UINT8 font_id );

#endif
