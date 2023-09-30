#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <canvas.h>
#include <uis.h>

extern UIS_DIALOG_T dialog;

UINT32 DrawLine(INT16 x1, INT16 y1, INT16 x2, INT16 y2);
UINT32 DrawArc(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 DrawRect(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 FillRect(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 FillRoundRect(INT32 x, INT32 y, UINT16 archW, UINT16 archH, UINT32 w, UINT32 h);
UINT32 SetFillColor(UINT32 color);
UINT32 SetForegroundColor(UINT32);
UINT32 SetBackgroundColor(UINT32 color);
UINT32 DrawText(WCHAR *str, INT32 x, INT32 y, UINT16 anchor);

#endif
