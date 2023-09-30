#ifndef LOADER_H
#define LOADER_H

#include <typedefs.h>
#include <stdarg.h>

typedef struct
{
    UINT32 evcode;
    BOOL isFree;
    UINT32 TextAddr;
    UINT32 DataAddr;
	UINT32 BssAddr;
    UINT8 * textptr;
    UINT8 * dataptr;
	UINT8 * bssptr;
    UINT8 pid;
} ElfLoaderApp;

UINT32 LoaderShowApp( ElfLoaderApp * );
UINT32 LoaderEndApp( ElfLoaderApp * );

#endif
