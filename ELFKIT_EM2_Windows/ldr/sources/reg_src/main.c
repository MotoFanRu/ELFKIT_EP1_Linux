#include <filesystem.h>

#define LOADER_BASE_PATH L"file://a/elf/ldr.bin"
#define LOADER_BASE_ADDR 0x096C3D00

void APP_LDR_REGISTER ( )
{
	UINT32 fSize, R;
	FILE_HANDLE_T hFile;

	hFile = DL_FsOpenFile((WCHAR *)LOADER_BASE_PATH, FILE_READ_MODE, 0);
	fSize = DL_FsGetFileSize(hFile);

	if(fSize > 0) DL_FsReadFile((VOID *)LOADER_BASE_ADDR, fSize, 1, hFile, &R);
	DL_FsCloseFile(hFile);

	if(((UINT8 *)LOADER_BASE_ADDR)[0]) asm ("jsri 0x096C3D00\n"); //define LOADER_BASE_ADDR as UINT32 and use asm("jsri LOADER_BASE_ADDR\n");
}
