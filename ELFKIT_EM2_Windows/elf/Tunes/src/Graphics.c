#include "Graphics.h"

// картинки скина
static UINT8 *PIC[PIC_MAX];
static UINT32 PIC_SIZE[PIC_MAX];

// иконки
static UINT8  *ICO[ICO_MAX];
static UINT32 ICO_SIZE[ICO_MAX];
RESOURCE_ID   ICO_RES[ICO_MAX];

static CANVAS_IMAGE_HANDLE_T ani_image_handle;

UINT32 DrawLine(INT16 x1, INT16 y1, INT16 x2, INT16 y2)
{
    GRAPHIC_POINT_T begin;
    GRAPHIC_POINT_T end;

    begin.x = x1;
    begin.y = y1;
    end.x = x2;
    end.y = y2;

    UIS_CanvasDrawLine(begin, end, dialog);
    return RESULT_OK;
}

UINT32 DrawRect(INT32 x, INT32 y, UINT32 w, UINT32 h)
{
    GRAPHIC_REGION_T region;

    region.ulc.x = x;
    region.ulc.y = y;
    region.lrc.x = x+w;
    region.lrc.y = y+h;

    UIS_CanvasDrawRect(region, 0, dialog);
    return RESULT_OK;
}

UINT32 FillRect(INT32 x, INT32 y, UINT32 w, UINT32 h)
{
    GRAPHIC_REGION_T region;

    region.ulc.x = x;
    region.ulc.y = y;
    region.lrc.x = x+w;
    region.lrc.y = y+h;

    UIS_CanvasFillRect(region, dialog);
    return RESULT_OK;
}

UINT32 SetFillColor(UINT32 color)
{
    COLOR_T colorx;

    colorx.red = (color & 0xFF000000) >> 24;
    colorx.green = (color & 0x00FF0000) >> 16;
    colorx.blue = (color & 0x0000FF00) >> 8;
    colorx.transparent = color & 0x000000FF;

    UIS_CanvasSetFillColor(colorx);
    return RESULT_OK;
}

UINT32 SetForegroundColor(UINT32 color)
{
    COLOR_T colorx;

    colorx.red = (color & 0xFF000000) >> 24;
    colorx.green = (color & 0x00FF0000) >> 16;
    colorx.blue = (color & 0x0000FF00) >> 8;
    colorx.transparent = color & 0x000000FF;

    UIS_CanvasSetForegroundColor(colorx);
    return RESULT_OK;
}

UINT32 SetBackgroundColor(UINT32 color)
{
    COLOR_T colorx;

    colorx.red = (color & 0xFF000000) >> 24;
    colorx.green = (color & 0x00FF0000) >> 16;
    colorx.blue = (color & 0x0000FF00) >> 8;
    colorx.transparent = color & 0x000000FF;

    UIS_CanvasSetBackgroundColor(colorx);
    return RESULT_OK;
}

UINT32 DrawText(WCHAR *str, INT32 x, INT32 y, UINT16 anchor)
{
    GRAPHIC_POINT_T anchor_point;

    anchor_point.x = x;
    anchor_point.y = y;

    UIS_CanvasDrawColorText (str, 0, (UINT16)u_strlen(str), anchor_point, anchor, dialog);
    return RESULT_OK;
}

GRAPHIC_POINT_T GetImageIdSize(UINT32 n)
{
    PICTURE_TYPE_UNION_T picture_data;
    GRAPHIC_POINT_T image_size;

    picture_data.PointerValue.resourcePicturePointerValue = PIC[n];
    picture_data.PointerValue.imageSize = PIC_SIZE[n];

    UIS_CanvasGetImageSize(&image_size, STORED_IN_MEMORY_TYPE, picture_data, 0, 0, 0);
    return image_size;
}

//не работает прозрачность
UINT32 DrawImageId( UINT32 n, INT16 x, INT16 y, UINT16 anchor)
{
    PICTURE_TYPE_UNION_T picture_data;
    GRAPHIC_POINT_T anchor_point;

    CANVAS_IMAGE_HANDLE_T image_handle;
    GRAPHIC_POINT_T image_size;
    GRAPHIC_REGION_T image_region;

    picture_data.PointerValue.resourcePicturePointerValue = PIC[n];
    picture_data.PointerValue.imageSize = PIC_SIZE[n];

    anchor_point.x = x;
    anchor_point.y = y;

    image_handle = UIS_CanvasOpenImage(STORED_IN_MEMORY_TYPE, picture_data);
    UIS_CanvasGetImageSize(&image_size, STORED_IN_MEMORY_TYPE, picture_data, 0, 0, 0);

    //прозрачность ---
    UIS_CanvasSetImageAttribute(image_handle, IA_OBEY_TRANSPARENCY, NULL);

    UIS_CanvasRenderImageToBuffer(image_handle,
                                  image_size,
                                  anchor_point,
                                  anchor,
                                  &image_region,
                                  Copy,
                                  dialog);

    UIS_CanvasCloseImage(image_handle);
    return RESULT_OK;
}

//не работает прозрачность
UINT32 StartAnimationId(INT16 x, INT16 y, UINT16 anchor)
{
    PICTURE_TYPE_UNION_T picture_data;
    GRAPHIC_POINT_T anchor_point;

    
    GRAPHIC_POINT_T image_size;
    GRAPHIC_REGION_T image_region;

    if (PIC[PIC_ANI] == NULL) 
        return RESULT_FAIL;

    if (ani_image_handle == 0) 
    {
    
        picture_data.PointerValue.resourcePicturePointerValue = PIC[PIC_ANI];
        picture_data.PointerValue.imageSize = PIC_SIZE[PIC_ANI];
    
        anchor_point.x = x;
        anchor_point.y = y;

        if(ani_image_handle != NULL)
            StopAnimationId( );

        ani_image_handle = UIS_CanvasOpenImage(STORED_IN_MEMORY_TYPE, picture_data);
        UIS_CanvasGetImageSize(&image_size, STORED_IN_MEMORY_TYPE, picture_data, 0, 0, 0);

    
        //прозрачность ---
        UIS_CanvasSetImageAttribute(ani_image_handle,
                                    IA_OBEY_TRANSPARENCY,
                                    NULL);
    
        UIS_CanvasRenderImageToBuffer(ani_image_handle,
                                      image_size,
                                      anchor_point,
                                      anchor,
                                      &image_region,
                                      Copy,
                                      dialog);
        UIS_CanvasIsImageAnimated(ani_image_handle);
    }

    return RESULT_OK;
}

UINT32 StopAnimationId () 
{
    if(PIC[PIC_ANI] == NULL)
        return RESULT_FAIL;

    UIS_CanvasPauseAnimation(ani_image_handle);
    UIS_CanvasCloseImage(ani_image_handle);
    ani_image_handle = NULL;

    return RESULT_OK;
}

GRAPHIC_METRIC_T GetStringSize (WCHAR *string, UINT8 font_id, UINT8 font_style)
{
    FONT_ATTRIB_T font_attrib;
    GRAPHIC_METRIC_T string_size;

    UIS_CanvasGetAttributesFromFontID(&font_attrib, font_id);
    font_attrib.font_style = font_style;

    GET_STRING_SIZE(string, &string_size, font_attrib);
    return string_size;
}

UINT32 LoadSkin (WCHAR *folder)
{
    int n=0; // кол-во картинок
	UINT32 readen;
	FILE_HANDLE_T f;
    WCHAR pic_name[256];
    WCHAR pic_num[8];

	// парсим конфиг
	udbg("folder = ", folder);

    for (n=0; n < PIC_MAX-1; n++)
    {
        u_strcpy(pic_name, folder);
        u_ltou( n,  pic_num );
        u_strcat(pic_name, pic_num);
        u_strcat(pic_name, L".gif");

        if (!DL_FsFFileExist(pic_name)) 
        {
            pic_name[u_strlen(pic_name)-3] = 0;
            u_strcat(pic_name, L"GIF");
        }

        udbg("SkinPic = ", pic_name);
        

        f = DL_FsOpenFile(pic_name, FILE_READ_MODE, 0);
        if (f == FILE_HANDLE_INVALID ) 
        {
                PIC[n] = NULL;
        } 
        else
        {
            PIC_SIZE[n] = DL_FsGetFileSize(f);
            PIC[n] = malloc(PIC_SIZE[n]);
            if (PIC[n] != NULL) 
            {
                DL_FsReadFile(PIC[n], PIC_SIZE[n], 1, f, &readen);
                DL_FsCloseFile(f);
            }
            else
            {
                dbgf("PIC[n] = NULL");
            }
        }
    }

    // анимация
    u_strcpy(pic_name, folder);
    u_strcat(pic_name, L"ani.gif");

    if (!DL_FsFFileExist(pic_name)) 
    {
        pic_name[u_strlen(pic_name)-3] = 0;
        u_strcat(pic_name, L"GIF");
    }

    // грузим картинку
    f = DL_FsOpenFile(pic_name, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID ) 
    {
        PIC[n] = NULL;
    }
    else
    {
        PIC_SIZE[n] = DL_FsGetFileSize(f);
        PIC[n] = malloc( PIC_SIZE[n]);
        if (PIC[n] != NULL) 
        {
            DL_FsReadFile( PIC[n], PIC_SIZE[n], 1, f, &readen );
            DL_FsCloseFile(f);
        }
        else
        {
            dbgf("PIC[n] = NULL");
        }
    }

    return RESULT_OK;
}

UINT32 FreeMemSkin()
{
    UINT32 n;
    for(n = 0; n < PIC_MAX; n += 1)
       free(PIC[n]);

    return RESULT_OK;
}


UINT32 LoadIcons(WCHAR  *folder)
{
    int n=0; // кол-во картинок
	UINT32 readen;
	FILE_HANDLE_T f;
    WCHAR ico_name[256];


	udbg("folder = ", folder);


    for (n=0; n < ICO_MAX; n++)
    {
        u_strcpy(ico_name, folder);
        u_ltou( n,  ico_name+u_strlen(ico_name) );
        u_strcat(ico_name, L".gif");

        if (!DL_FsFFileExist(ico_name)) {
            ico_name[u_strlen(ico_name)-3] = 0;
            u_strcat(ico_name, L"GIF");
        }
        
        // грузим иконки
        f = DL_FsOpenFile(ico_name, FILE_READ_MODE, 0);
        if (f == FILE_HANDLE_INVALID ) 
        {
            ICO[n] = NULL;
        } 
        else 
        {
            ICO_SIZE[n] = DL_FsGetFileSize(f);
            ICO[n] = malloc( ICO_SIZE[n]);
            if (ICO[n] != NULL) 
            {
                DL_FsReadFile( ICO[n], ICO_SIZE[n], 1, f, &readen );
                DL_FsCloseFile(f);
            }
        }

        // создаём ресурс
        if (ICO[n]) DRM_CreateResource( &ICO_RES[n], RES_TYPE_GRAPHICS, (void*)ICO[n], ICO_SIZE[n] );
        else ICO_RES[n] = NULL;

        udbg("Icon = ", ico_name);
        dbgf("Ico_res = 0x%x", ICO_RES[n]);

    }

    return RESULT_OK;
}

UINT32 FreeMemIcons()
{
    UINT32 n;

    for (n=0; n < ICO_MAX; n++)
    {
       if(ICO_RES[n]) DRM_ClearResource(ICO_RES[n]);
       free(ICO[n]);
    }

    return RESULT_OK;
}    
