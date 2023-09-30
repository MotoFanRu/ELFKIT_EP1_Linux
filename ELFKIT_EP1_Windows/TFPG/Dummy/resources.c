
#include "resources.h"

#define LEVEL_W			20
#define LEVEL_H			16

#define TILE_W			11
#define TILE_H			11


u8						*gLevels;
WCHAR					gHomeDir[64];
WCHAR					sPathBuf[64];


BOOL resInit()
{
	return TRUE;
}

BOOL resTerminate()
{
	return TRUE;
}


void resInitHomeDir(WCHAR *elfPath)
{
	WCHAR			*last;

	u_strcpy(gHomeDir, elfPath);

	last = u_strrchr(gHomeDir, L'/');

	last++;

	*last = 0;
}

WCHAR *resGetFullPath(char *filename)
{
	u32				len;

	u_strcpy(sPathBuf, gHomeDir);

	len = u_strlen(sPathBuf);

	u_atou(filename, &sPathBuf[len]);

	return sPathBuf;
}

void* resLoadFile(WCHAR *path)
{
	FILE_HANDLE_T			f;
	void					*ptr;
	s32						result;
	u32						sz, read;


	f = DL_FsOpenFile( path, FILE_READ_MODE, 0);
	
	if(f == FILE_HANDLE_INVALID)
		return NULL;

	sz = DL_FsGetFileSize(f);

	if(sz == 0)
		return NULL;

	ptr = (void*)suAllocMem( sz, &result );

	if(ptr == NULL)
		return NULL;

	DL_FsReadFile( ptr, sz, 1, f, &read );

	DL_FsCloseFile(f);

	return ptr;
}


BOOL resLoadMap(char *filename)
{

	gLevels = (u8*)resLoadFile( resGetFullPath(filename) );

	if(gLevels == NULL)
		return FALSE;

	return TRUE;
}

