#ifndef CANVAS_H
#define CANVAS_H

#include <typedefs.h>
#include <util.h>
#include <resources.h>


/*******************************/
// ��� ����������� ������� �� ������� ����� ��� ������
#define EV_INK_KEY_PRESS 		  0x2034 //  EVENT_DATA_T
#define EV_INK_KEY_RELEASE		  0x2035 //  EVENT_DATA_T

/*******************************/

// ����� �������� ��� ������ � ��������
// ������������						
#define ANCHOR_BASELINE               64 // ������ �����
#define ANCHOR_BOTTOM                 32 
#define ANCHOR_TOP                    16 
#define ANCHOR_VCENTER                2  // ������ ��������

// ��������������
#define ANCHOR_RIGHT                  8 
#define ANCHOR_LEFT                   4  
#define ANCHOR_HCENTER                1  

//  ������������ ���� "���". ��������: ANCHOR_LEFT | ANCHOR_TOP

/*******************************/

// ����� ������ ... font_style
#define FONT_STYLE_PLAIN		0xFF
#define FONT_STYLE_ITALIC		0x01
#define FONT_STYLE_BOLD			0x02
#define FONT_STYLE_BOLD_ITALIC	(FONT_STYLE_ITALIC | FONT_STYLE_BOLD)

/*******************************/

typedef enum
{
	// �����������
	MIME_TYPE_IMAGE_GIF = 0,
	MIME_TYPE_IMAGE_BMP,
	MIME_TYPE_IMAGE_WBMP,
	MIME_TYPE_IMAGE_PNG,
	MIME_TYPE_IMAGE_JPEG,

	// ����� 
	MIME_TYPE_AUDIO_MID = 11,
	MIME_TYPE_AUDIO_MIDI,
	MIME_TYPE_AUDIO_MIX,
	MIME_TYPE_AUDIO_BAS,
	MIME_TYPE_AUDIO_MP3,
	MIME_TYPE_AUDIO_AAC,
	MIME_TYPE_AUDIO_AMR = 24,
	MIME_TYPE_AUDIO_MP4 = 26,
	MIME_TYPE_AUDIO_M4A,

	// �����
	MIME_TYPE_VIDEO_ASF = 34,
	MIME_TYPE_VIDEO_MP4 = 36,
	MIME_TYPE_VIDEO_MPEG4,
	MIME_TYPE_VIDEO_H263
    
} MIME_TYPE_T;

typedef struct
{
   UINT8 *buf;     // ����� ��� ���������. ������ ���� NULL
   UINT16 w;       // ������
   UINT16 h;       // ������
} DRAWING_BUFFER_T;

typedef struct
{
	UINT8 red;
	UINT8 green;
	UINT8 blue;
	UINT8 transparent; // ������������
} COLOR_T;

#define COLOR_RED		(COLOR_T)(0xFF000000)
#define COLOR_GREEN		(COLOR_T)(0x00FF0000)
#define COLOR_BLUE		(COLOR_T)(0x0000FF00)
#define RGB(r,g,b)		(COLOR_T)((BYTE)(r)<<24 | (BYTE)(g)<<16 | (BYTE)(b)<<8)
#define RGBT(r,g,b,t)	(COLOR_T)((UINT8)(r)<<24 | (UINT8)(g)<<16 | (UINT8)(b)<<8 | (UINT8)(t))
#define RGB_getr(c)		(BYTE)((UINT32)(c) >> 24 & 0xFF)// RED
#define RGB_getg(c)		(BYTE)((UINT32)(c) >> 16 & 0xFF)// GREEN
#define RGB_getb(c)		(BYTE)((UINT32)(c) >>  8 & 0xFF)// BLUE
#define RGB_gett(c)		(BYTE)((UINT32)(c)       & 0xFF)// TRANSPARENT

typedef struct 
{
    INT16 x;   // �������������� ���������� x
    INT16 y;   // ������������ ����������
} GRAPHIC_POINT_T;

typedef struct 
{
    GRAPHIC_POINT_T ulc;  // ������� ����� ����
    GRAPHIC_POINT_T lrc;  // ������ ������ ����   
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

enum  //��� �������� ... picture_type 
{
    DRM_PICTURE_TYPE,
    IMAGE_PATH_TYPE,
    STORED_IN_MEMORY_TYPE,
    FILE_HANDLE_TYPE,
    IMAGE_PATH_WITH_OFFSET
};

typedef WCHAR *IMAGE_PATH_T; // ���� � ��������

typedef struct 
{
	void *resourcePicturePointerValue;  // �������������� ��������� � ������
    UINT32 imageSize;                   // ������ �����������
} PICTURE_POINTER_T;

typedef struct 
{
    FILE_HANDLE_T fileHandle;
    UINT32 imageOffset;
    UINT32 imageSize;
} UIS_FILE_HANDLE_T;

typedef struct 
{
    IMAGE_PATH_T  	  PathValue;
    MIME_TYPE_T       mimeFormat; // ��� ��������
    UINT32            imageOffset;
    UINT32            imageSize;
} FILE_PATH_WITH_OFFSET_T;

typedef union
{
    RESOURCE_ID           		DrmValue;
    IMAGE_PATH_T            	PathValue;
    PICTURE_POINTER_T       	PointerValue;
	UIS_FILE_HANDLE_T           FileHandleValue;  
    FILE_PATH_WITH_OFFSET_T 	PathWithOffsetValue;
} PICTURE_TYPE_UNION_T; 

typedef UINT8 CANVAS_IMAGE_HANDLE_T;

typedef struct 
{
    UINT16 height;
    UINT16 width;
} GRAPHIC_METRIC_T;

typedef enum
{
    WALLPAPER_LAYOUT_CURRENT = 0,           
    WALLPAPER_LAYOUT_CENTERED,          
    WALLPAPER_LAYOUT_TILED
	
} WALLPAPER_LAYOUT_T;	

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
    IA_STILL_IMAGE=4, // ������ ���� ��������
    IA_BACKGROUND_TRANSPARENT_IMAGE, // IMAGE_BACKGROUND_BUFFER
    IA_BACKGROUND_TRANSPARENT_COLOR, // COLOR_T
    IA_BACKGROUND_TRANSPARENT_GRADIENT, // GRADIENT_T
	
    IA_BACKGROUND_TRANSPARENT_TILED_IMAGE,
    IA_BACKGROUND_WALLPAPER_NEEDED,
 
    IA_START_ANIMATION=10, // ����� ��������
    IA_BACKGROUND_EXCLUDING_IMAGE, // COLOR_T

    IA_NO_PAINT_WHITE=13,  // �� �������� �����
    IA_NO_PAINT_WHITE_WITH_CROSS,
    IA_PAINT_WHITE_AND_PAINT_WHITE_WITH_CROSS,
    IA_NO_OF_CYCLES_OF_ANIMATION, // UINT8 ����� �����
    IA_DECODE_WALLPAPER_GRAYSCALE,
    IA_PRESERVE_BACKGROUND_GRAYSCALE,

    IA_OBEY_TRANSPARENCY=23,

} IMAGE_ATTRIBUTE_TYPE_T;


/*******************************/

//������� Canvas
UIS_DIALOG_T UIS_CreateColorCanvas( SU_PORT_T  *port,
                                    DRAWING_BUFFER_T * drawing_buffer,
                                    BOOL use_soft_icon_area );

/*	����� �����������				   
BackgroundColor - white
FillColor  - white
ForegroundColor  - blue
*/								
					   
/*******************************/		

// ������ �����
void UIS_CanvasDrawPixel(GRAPHIC_POINT_T point, UIS_DIALOG_T handle);		

//  ������  �����						
void UIS_CanvasDrawLine(GRAPHIC_POINT_T begin,
                        GRAPHIC_POINT_T end,
						UIS_DIALOG_T handle);	   

// ������ ����������� �������������
void UIS_CanvasFillRect(GRAPHIC_REGION_T region,
                        UIS_DIALOG_T handle);

//  ������ �������������					
void UIS_CanvasDrawRect(GRAPHIC_REGION_T region,
                        BOOL fill, //  �������
                        UIS_DIALOG_T handle);

// ������ ����������� �������������					
void UIS_CanvasDrawRoundRect(GRAPHIC_REGION_T region,  
							UINT16 arcW, // �������������� ������� ����������
							UINT16 arcH, // ������������ ������� ����������
							BOOL fill,
							UIS_DIALOG_T handle);						
	
// ������ ���� ���������� ��� �������	
void UIS_CanvasDrawArc(GRAPHIC_REGION_T region,
						UINT16 startAngle,    // ��������� ����.
						UINT16 arcAngle,	  // �������� ����
						BOOL fill,
						UIS_DIALOG_T handle);	
					
// ������ �������������					
void UIS_CanvasDrawPoly(UINT16 number_of_vertices,
						GRAPHIC_POINT_T  *array,
						BOOL fill,
						UIS_DIALOG_T handle);	
						
/*******************************/

// ������ ������						
void UIS_CanvasDrawColorText( WCHAR *str,
							  UINT16 offset,
							  UINT16 str_len,  			   // ������ ������
							  GRAPHIC_POINT_T anchor_point, // ���������� ����� ��������
							  UINT16 anchor_value,		   // �������� ����� ��������. ��������: ANCHOR_LEFT | ANCHOR_TOP
							  UIS_DIALOG_T handle);	
							 
// ����� �����
UINT32 UIS_CanvasSetFont( UINT8 font_id, // 0 - NetMonitor, 1- General Font... ��������� � MotoLangEditor
						  UIS_DIALOG_T handle);


	
//���������� ����� ������
UINT8 UIS_CanvasGetFontStyle(UIS_DIALOG_T handle);		
// ������ �����	������					  
UINT32 UIS_CanvasSetFontStyle(UINT8 font_style, UIS_DIALOG_T handle);		  
						 

// ���������� ������ ������
UINT32 UIS_CanvasGetStringSize( WCHAR * str,
								GRAPHIC_METRIC_T * string_size, // ������ ������
								UINT8 font_id );	
// ������ ������								
UINT32 UIS_CanvasGetFontSize ( UINT8 font_id, GRAPHIC_METRIC_T * font_size);								
						
UINT32 utility_draw_image( RESOURCE_ID res_id,
							GRAPHIC_REGION_T region,
							void * p_attr,
							void * drawBuf,
							UINT32 unk1 );	

void utility_get_image_size(RESOURCE_ID res_id, UINT16 *width, UINT16 *height);
	
/*******************************/

// ������ �������					
UINT32 UIS_CanvasDrawImage(UINT8 picture_type,			// ���. ������ ����� ����� ��������. DRM, ����....
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

							   
UINT32 UIS_CanvasDrawColorBitmap( void*         picture_bytes,  // ������ ���� - ������, ������ ���� ������
																	   // ��������� 8-bpp(RGB332)  ���  16-bpp(RGB565) 
								GRAPHIC_POINT_T  ulc_anchor_point,
								UIS_DIALOG_T  handle );		
	
// ��������� ��������					 
CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImage(UINT8 picture_type,
										  PICTURE_TYPE_UNION_T picture_data);	
										  
// ��������� ��������	 � �������� � ������										  
CANVAS_IMAGE_HANDLE_T UIS_CanvasOpenImageAndGetSize(UINT8 picture_type,
													PICTURE_TYPE_UNION_T picture_data,
													GRAPHIC_POINT_T *attributes);	
// ��������� ��������										  
UINT32 UIS_CanvasCloseImage(CANVAS_IMAGE_HANDLE_T image_handle);	
								  

// ��������� ������� 
UINT32 UIS_CanvasResizeImage(CANVAS_IMAGE_HANDLE_T image_handle,
							 UINT16 percent, // zoom � ���������
							 GRAPHIC_REGION_T src_area, // �������� ������ �����������
							 GRAPHIC_REGION_T dest_region,// ����� ������
							 UINT8 drawOp, 
							 UIS_DIALOG_T handle);		
// ���������� ������ �����������							 
GRAPHIC_POINT_T UIS_CanvasGetImageSize(UINT8 picture_type,
									   PICTURE_TYPE_UNION_T picture_data);

// ������������� ��������
UINT32 UIS_CanvasSetImageAttribute(CANVAS_IMAGE_HANDLE_T image_handle,
                                    IMAGE_ATTRIBUTE_TYPE_T attribute_type,
							        void* attribute_value);			
// �������� ��������									
UINT32 UIS_CanvasGetImageAttribute(CANVAS_IMAGE_HANDLE_T image_handle,
                                    IMAGE_ATTRIBUTE_TYPE_T attribute_type,
							        void* attribute_value);		
// ��������� ����������� � �����									
UINT32 UIS_CanvasRenderImageToBuffer(CANVAS_IMAGE_HANDLE_T image_handle,
                                     GRAPHIC_POINT_T image_size,
                                     GRAPHIC_POINT_T anchorpoint,
                                     UINT16 anchor_value,
                                     GRAPHIC_REGION_T * image_region,
                                     UINT8 drawOp,
                                     UIS_DIALOG_T handle);
									
// ����������� �������� � ����� pointer
UINT32 UIS_CanvasDecodeImageToPointer(CANVAS_IMAGE_HANDLE_T image_handle, void* pointer);									
						
// ��������� ����� pointer					
UINT32 UIS_CanvasInsert(UIS_DIALOG_T handle,void* pointer, GRAPHIC_POINT_T point);	

// ���������� ������
UINT32 UIS_CanvasMoveRegion(GRAPHIC_REGION_T source_region,
                            GRAPHIC_REGION_T dest_region,
                            UIS_DIALOG_T handle);					
							
							
						
/*******************************/	

// ������ ���� ������� 
void UIS_CanvasSetFillColor(COLOR_T color);	
// ���������� ���� ������� 
COLOR_T UIS_CanvasGetFillColor(void);


// ������ ���� ��� �����, ���������������, ������.....
void UIS_CanvasSetForegroundColor(COLOR_T color);
// ���������� ���� ��� �����, ���������������, ������.....
COLOR_T UIS_CanvasGetForegroundColor(void);

// ���� ����
void UIS_CanvasSetBackgroundColor(COLOR_T color);
COLOR_T UIS_CanvasGetBackgroundColor(void);


// ������ �����
void UIS_CanvasSetLineWidth(UINT16 width);
UINT16 UIS_CanvasGetLineWidth(void);


/*******************************/	

// ���������� ������
void UIS_CanvasRefreshDisplayRegion(UIS_DIALOG_T handle, GRAPHIC_REGION_T region);
void UIS_CanvasRefreshDisplayBuffer(UIS_DIALOG_T handle);	

/*******************************/
				   
//����������  ��������� ������� ����� � ����� 
UINT32 UIS_CanvasGetColorDepth (void);

// ���������� ������ �������
GRAPHIC_POINT_T UIS_CanvasGetDisplaySize(void);

// �������� ��������� ���������
UINT32 UIS_CanvasSetBackLight(BOOL state);


#endif
