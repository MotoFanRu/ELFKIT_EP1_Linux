#include "ldr_library.h"
#include "ldr_features.h"

///Data
BYTE * extLib = NULL;
const LIBRARY_RECORD_T ldrLib[] = 
{
    { (UINT32)ldrAutorunAction,       "ldrAutorunAction"        },

    { (UINT32)ldrLoadDefLibrary,      "ldrLoadDefLibrary"       },
    { (UINT32)ldrSearchInDefLibrary,  "ldrSearchInDefLibrary"   },

    { (UINT32)ldrSendEvent,           "ldrSendEvent"            },
    { (UINT32)ldrSendEventToApp,      "ldrSendEventToApp"       },
    { (UINT32)ldrSendEventToAppEmpty, "ldrSendEventToAppEmpty"  },

    { (UINT32)ldrLoadElf,             "ldrLoadElf"              },
    { (UINT32)ldrUnloadElf,           "ldrUnloadElf"            },

    { (UINT32)ldrInitEventHandlersTbl,"ldrInitEventHandlersTbl" },
    { (UINT32)ldrFindEventHandlerTbl, "ldrFindEventHandlerTbl"  },

    { (UINT32)ldrFindElf,             "ldrFindElf"              },
    { (UINT32)ldrIsLoaded,            "ldrIsLoaded"             },
    { (UINT32)ldrGetElfsList,         "ldrGetElfsList"          }
};

///Code
UINT32 ldrLoadDefLibrary()
{
    UINT32 fSize, R, i;
    FILE_HANDLE_T hFile;

    hFile = DL_FsOpenFile(LIBRARY_PATH, FILE_READ_MODE, NULL);
    fSize = DL_FsGetFileSize(hFile);
    
    if(hFile == FILE_HANDLE_INVALID || fSize < sizeof(LIBRARY_HEADER_T))
        return RESULT_FAIL;

    if(extLib != NULL)
        free(extLib);

    extLib = (BYTE *)malloc(fSize);
    if(extLib == NULL)
    {
        DL_FsCloseFile(hFile);
        PFprintf("[ELF_LDR] Can't load library. Out of memory!\n" );
        return RESULT_FAIL;
    }

    DL_FsReadFile(extLib, fSize, 1, hFile, &R);
    DL_FsCloseFile(hFile);

    UINT32 libPhysAddr = (UINT32)(extLib);
    LIBRARY_HEADER_T *hdr = (LIBRARY_HEADER_T *)(libPhysAddr);
    LIBRARY_RECORD_T *rec = (LIBRARY_RECORD_T *)(libPhysAddr + hdr->records_table_offset);
    for(i=0; i<hdr->number_of_records; i++)
        rec[i].record_name += libPhysAddr;

    return RESULT_OK;
}

LIBRARY_RECORD_T * ldrSearchInDefLibrary (char *record_name)
{
    if(extLib == NULL || ldrLib == NULL || record_name == NULL)
        return NULL;

    UINT32 i, libPhysAddr = (UINT32)(extLib);
    LIBRARY_HEADER_T *hdr = (LIBRARY_HEADER_T *)(libPhysAddr);
    LIBRARY_RECORD_T *rec = (LIBRARY_RECORD_T *)(libPhysAddr + hdr->records_table_offset);

    for(i=0; i<sizeof(ldrLib)/sizeof(LIBRARY_RECORD_T); i++)
        if(!strcmp(record_name, ldrLib[i].record_name))
            return (LIBRARY_RECORD_T *)&ldrLib[i];

    for(i=0; i<hdr->number_of_records; i++)
        if(!strcmp(record_name, rec[i].record_name))
            return (LIBRARY_RECORD_T *)&rec[i];

    return NULL;
}
