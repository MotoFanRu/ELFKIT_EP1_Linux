#include "graphics.h"

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

UINT32 DrawArc(INT32 x, INT32 y, UINT32 w, UINT32 h)
{
    GRAPHIC_REGION_T        region;

    region.ulc.x = x;
    region.ulc.y = y;
    region.lrc.x = x+w;
    region.lrc.y = y+h;

    UIS_CanvasDrawArc(region, 0, 360, TRUE, dialog);

    return RESULT_OK;
}


UINT32 DrawRect(INT32 x, INT32 y, UINT32 w, UINT32 h) // прямоугольник
{
    GRAPHIC_REGION_T        region;

    region.ulc.x = x;
    region.ulc.y = y;
    region.lrc.x = x+w;
    region.lrc.y = y+h;

    UIS_CanvasDrawRect(region, 0, dialog);

    return RESULT_OK;
}


UINT32 FillRect(INT32 x, INT32 y, UINT32 w, UINT32 h) // прямоугольник с заливкой
{
    GRAPHIC_REGION_T        region;

    region.ulc.x = x;
    region.ulc.y = y;
    region.lrc.x = x+w;
    region.lrc.y = y+h;

    UIS_CanvasFillRect(region, dialog);

    return RESULT_OK;
}

UINT32 FillRoundRect(INT32 x, INT32 y, UINT16 archW, UINT16 archH, UINT32 w, UINT32 h) // прямоугольник с заливкой
{
    GRAPHIC_REGION_T        region;

    region.ulc.x = x;
    region.ulc.y = y;
    region.lrc.x = x+w;
    region.lrc.y = y+h;

    UIS_CanvasDrawRoundRect(region, archW, archH, TRUE, dialog);
    return RESULT_OK;
}

UINT32 SetFillColor(UINT32 color) // заливка для прямоугольника
{
    COLOR_T                 colorx;

    colorx.red = (color & 0xFF000000) >> 24;
    colorx.green = (color & 0x00FF0000) >> 16;
    colorx.blue = (color & 0x0000FF00) >> 8;
    colorx.transparent = color & 0x000000FF;

    UIS_CanvasSetFillColor(colorx);

    return RESULT_OK;
}

UINT32 SetForegroundColor(UINT32 color) // устанавливает цвет для линии, текста...
{
    COLOR_T                 colorx;

    colorx.red = (color & 0xFF000000) >> 24;
    colorx.green = (color & 0x00FF0000) >> 16;
    colorx.blue = (color & 0x0000FF00) >> 8;
    colorx.transparent = color & 0x000000FF;

    UIS_CanvasSetForegroundColor(colorx);

    return RESULT_OK;
}

UINT32 SetBackgroundColor(UINT32 color) // устанавливает цвет фона
{
    COLOR_T                 colorx;

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


    UIS_CanvasDrawColorText (str,
							 0,
							 (UINT16)u_strlen(str),  	// длинна строки
							 anchor_point,              // координаты точки привязки
							 anchor,	    // значение точки привязки. Например: ANCHOR_LEFT | ANCHOR_TOP
							 dialog);
    return RESULT_OK;
}
