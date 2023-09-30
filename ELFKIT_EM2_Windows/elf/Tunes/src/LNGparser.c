#include "LNGparser.h"

LNG_RES_T  Lang;  // языковой ресурс

// чтение файла €зыкового ресурса
UINT32 ReadLang(WCHAR *uri)
{
	UINT32          readen;
	FILE_HANDLE_T   f;
	UINT32          fSize; // размер файла
	UINT16          i = 0, numStr = 1;
    INT32           err;


	f = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if (f == FILE_HANDLE_INVALID) return RESULT_FAIL;

    Lang.res = NULL;
	fSize = DL_FsGetFileSize(f);
	Lang.res = (WCHAR*)malloc(fSize+2);
    if(Lang.res == NULL) return RESULT_FAIL;

	DL_FsReadFile( Lang.res, 1, fSize, f, &readen );
    DL_FsCloseFile(f);

    if (Lang.res[0] == 0xFEFF) 
    {
        Lang.str[0] = Lang.res+1;
    } else 
    {
        Lang.str[0] = Lang.res;
    }
			
	for (i=0; i < fSize/2 && numStr < LNG_MAX; i++) 
    {
        if (Lang.res[i] == '\r') Lang.res[i] = 0;
        if (Lang.res[i] == '\n') 
        {
           Lang.res[i] = 0;
           Lang.str[numStr] = Lang.res+i+1;
           numStr++;
        }
    }

    Lang.res[(fSize/2)] = 0;

	return RESULT_OK;
}

void CloseLang(void)
{
    if (Lang.res != NULL) 
    {
        free(Lang.res);
        Lang.res = NULL;
    }
}
