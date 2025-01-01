#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <apps.h>
#include <canvas.h>
#include <uis.h>
#include <resources.h>

extern UIS_DIALOG_T dialog;

UINT32 DrawText(WCHAR *str, INT32 x, INT32 y, UINT16 anchor);
UINT32 DrawLine(INT16 x1, INT16 y1, INT16 x2, INT16 y2);
UINT32 DrawRect(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 FillRect(INT32 x, INT32 y, UINT32 w, UINT32 h);
UINT32 SetFillColor(UINT32 color);
UINT32 SetForegroundColor(UINT32);
UINT32 SetBackgroundColor(UINT32 color);

#endif
