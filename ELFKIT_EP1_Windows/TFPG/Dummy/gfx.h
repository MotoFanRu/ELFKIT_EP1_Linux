#ifndef GFX__H
#define GFX__H

#include <loader.h>
#include <ATI.h>
#include <DAL.h>
#include "common.h"
#include "dbg.h"

#define SCREEN_W			176
#define SCREEN_H			220


extern AHIDEVCONTEXT_T		gCtx;

extern AHISURFACE_T			gDrawSurf;
extern AHISURFACE_T			gDispSurf;

extern AHIRECT_T			gScreenRect;
extern u32					gCacheSize;



BOOL gfxInit();
BOOL gfxTerminate();
void gfxFlushDoublebuffer();

#endif

