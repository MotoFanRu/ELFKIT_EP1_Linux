#include "ldr_core.h"
#include "ldr_features.h"

///Data
UINT32 evbase = 0xA000;

///Code
UINT32 ldrSendEvent (UINT32 evcode)
{
    return AFW_CreateInternalQueuedEvAux(evcode, FBF_LEAVE, 0, 0);
}

UINT32 ldrSendEventToApp (AFW_ID_T afwid, UINT32 evcode, void *attachment, UINT32 att_size, FREE_BUF_FLAG_T fbf)
{
    UINT32 status;
    status = AFW_CreateInternalQueuedEvPriv (evcode, 0, afwid, 0, 0, 0, fbf, att_size, attachment, 0);
    return status;
}

UINT32 ldrSendEventToAppEmpty (AFW_ID_T afwid, UINT32 evcode)
{
    UINT32 status;
    status = AFW_CreateInternalQueuedEvPriv (evcode, 0, afwid, 0, 0, 0, FBF_LEAVE, 0, NULL, 0);
    return status;
}

UINT32 ldrUnloadElf (ldrElf *elf)
{    
    elfListFindAndDel(elf);
    free(elf->image);
    free(elf);

    return RESULT_OK;
}

UINT32 ldrLoadElf (WCHAR *uri, WCHAR *params, UINT32 evcode)
{
    BYTE *image;         //образ elf файла
    UINT32 size;         //размер образа в байтах
    UINT32 ep_address;   //адрес точки входа
    FILE_HANDLE_T file;  //хэндл файла

    file = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if(file == FILE_HANDLE_INVALID)
    {
        PFprintf("[ELF_LDR] Can't open file\n");
        return RESULT_FAIL;
    }

    size = DL_FsGetFileSize(file);
    image = (BYTE *)malloc(size);
    if(image == NULL)
    {
        PFprintf("[ELF_LDR] Out of memory\n");
        DL_FsCloseFile(file);
        return RESULT_FAIL;
    }

    DL_FsReadFile(image, size, 1, file, &size);
    DL_FsCloseFile(file);

    ldrElf *app = (ldrElf *)malloc(sizeof(ldrElf));
    if(app == NULL)
    {
        PFprintf("[ELF_LDR] Out of memory\n");
        free(image);
        return RESULT_FAIL;
    }

    ep_address = ldrParseImage(image, app);
    
    if(ep_address != 0)
    {
        if(evcode > 0)
        {
            app->evbase = evcode;
        }
        else
        {
            app->evbase = evbase;
            evbase += EVBASE_RESERVE;
        }

        DL_FsGetIDFromURI(uri, &app->id);
        switch( ((ELF_ENTRY)ep_address)(app, params) )
        {
            case RESULT_OK:
                elfListAdd(app);
                break;

            default:
                if(evcode == 0) 
                    evbase -= EVBASE_RESERVE;
                ldrUnloadElf(app);
                break;
        }
    }
    else
    {
        free(image);
        return RESULT_FAIL;
    }

    return RESULT_OK;
}

UINT32 ldrInitEventHandlersTbl (EVENT_HANDLER_ENTRY_T *tbl, UINT32 base)
{
    UINT32 i=0;
    if(!tbl) return base;

    while(tbl[i].code != STATE_HANDLERS_END)
    {
        if(tbl[i].code == STATE_HANDLERS_RESERVED)    
        {
            tbl[i].code = base++;
        }

        if(tbl[i].code == STATE_HANDLERS_PM_API_EXIT)
        {
            tbl[i].code = EV_PM_API_EXIT;
        }
        i++;
    }
    return base;
}

UINT32 ldrFindEventHandlerTbl (const EVENT_HANDLER_ENTRY_T *tbl, EVENT_HANDLER_T *hfn)
{
    UINT32 i = 0;
    if(!tbl) return 0;

    while(tbl[i].code != STATE_HANDLERS_END)
    {
        if(tbl[i].hfunc == hfn) return tbl[i].code;
        i++;
    }

    return 0;
}
