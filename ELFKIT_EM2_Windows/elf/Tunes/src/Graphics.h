#define DEBUG

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <apps.h>
#include <canvas.h>
#include <filesystem.h>
#include <utilities.h>
#include <uis.h>

extern UIS_DIALOG_T dialog;
extern WCHAR TunesU[128];
extern RESOURCE_ID ICO_RES[];

typedef enum
{
    PIC_SOFT_LINE=0,
    PIC_TITLE_LINE,
    PIC_PREV,
    PIC_PLAY,
    PIC_PAUSE,
    PIC_STOP,
    PIC_NEXT,
    PIC_PREV_USE,
    PIC_PLAY_USE,
    PIC_PAUSE_USE,
    PIC_STOP_USE,
    PIC_NEXT_USE,
    PIC_RND,
    PIC_REP_ONE,
    PIC_REP_ALL,
    PIC_MUTE,

    PIC_ANI,// всегда предпоследн€€ в enum

    PIC_MAX

} PICTURES;

typedef enum
{
    ICO_UPWARDS=0,
    ICO_DISK,
    ICO_DIR,
    ICO_NONE,
    ICO_AUDIO,
    ICO_NEXT,
    ICO_SELECTED,

    ICO_MAX

} ICONS;

UINT32 DrawLine(INT16 x1, INT16 y1, INT16 x2, INT16 y2);
UINT32 DrawRect(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 FillRect(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 SetFillColor(UINT32 color);
UINT32 SetForegroundColor(UINT32);
UINT32 SetBackgroundColor(UINT32 color);
UINT32 DrawText(WCHAR *str, INT32 x, INT32 y, UINT16 anchor);
UINT32 DrawImageId( UINT32 n, INT16 x, INT16 y, UINT16 anchor);
GRAPHIC_POINT_T GetImageIdSize( UINT32 n);
GRAPHIC_METRIC_T GetStringSize(WCHAR *str, UINT8 font_id, UINT8 font_style);
UINT32 StartAnimationId( INT16 x, INT16 y, UINT16 anchor);
UINT32 StopAnimationId( void );

UINT32 LoadSkin(WCHAR *folder);
UINT32 FreeMemSkin(void);

UINT32 LoadIcons(WCHAR *folder);
UINT32 FreeMemIcons(void);

#endif
