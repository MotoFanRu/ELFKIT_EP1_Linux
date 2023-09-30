
#include "gfx.h"

AHIDEVCONTEXT_T				gCtx;

AHISURFACE_T				gDrawSurf;
AHISURFACE_T				gDispSurf;
AHISURFACE_T				gCacheSurf;

u32							gCacheSize;

AHIRECT_T					gScreenRect = { 0, 0, SCREEN_W, SCREEN_H };

u32							sFlushCount;


BOOL gfxInit()
{
	AHIDEVICE_T			device;
	AHIPOINT_T			cacheSize;
	u32					size, align;
	u32					status;

	/*
		Create new Context
	*/

	device = ldrGetAhiDevice();

	status = AhiDevOpen( &gCtx, device, "Dummy", 0 );

	if( status != 0 )
		return FALSE;

	dbgf("gCtx = 0x%08X", gCtx);


	/*
		Surfaces init
	*/

	gDrawSurf = DAL_GetDrawingSurface(0);
	dbgf("gDrawSurf = 0x%08X", gDrawSurf);
	
	AhiDispSurfGet( gCtx, &gDispSurf );
	dbgf("gDispSurf = 0x%08X", gDispSurf);


	/*
		Memory tests
	*/

	cacheSize = DAL_GetCacheSize();

	gCacheSize = cacheSize.x * cacheSize.y;

	dbgf("DAL Cache size: %d x %d", cacheSize.x, cacheSize.y);


	AhiSurfGetLargestFreeBlockSize( gCtx, AHIFMT_8BPP, &size, &align, 0);
	dbgf("VRAM: %d", size);
	

	// DAL allocates 120x64 by default
	gCacheSurf = DAL_GetCachedSurface( cacheSize );


	
	/*
		Drawing settings init
	*/

	AhiDrawRopSet( gCtx, AHIROP3(AHIROP_SRCCOPY));

	AhiDrawSurfDstSet( gCtx, gDrawSurf, 0 );
	
	AhiDrawClipDstSet( gCtx, &gScreenRect );
	AhiDrawClipSrcSet( gCtx, NULL );

	
	return TRUE;
}

BOOL gfxTerminate()
{
	// Surfaces should be on their initial places
	if( (sFlushCount & 1) == 1 )
		gfxFlushDoublebuffer();

	AhiDevClose( gCtx );

	return TRUE;
}

void gfxFlushDoublebuffer()
{
	AHISURFACE_T			tmpSurf;

	AhiDispSurfSet(gCtx, gDrawSurf, 0);

	tmpSurf = gDrawSurf;
	gDrawSurf = gDispSurf;
	gDispSurf = tmpSurf;

	AhiDrawSurfDstSet( gCtx, gDrawSurf, 0 );

	sFlushCount++;
}