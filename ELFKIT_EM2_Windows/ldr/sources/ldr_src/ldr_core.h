#ifndef LDR_CORE_H
#define LDR_CORE_H

#include <loader.h>
#include <filesystem.h>
#include <utilities.h>

typedef UINT32 (*ELF_ENTRY)(ldrElf *ela, WCHAR *params); // Entry point prototype

UINT32 ldrSendEvent (UINT32 evcode);
UINT32 ldrSendEventToApp (AFW_ID_T afwid, UINT32 evcode, void *attachment, UINT32 att_size, FREE_BUF_FLAG_T fbf);
UINT32 ldrSendEventToAppEmpty (AFW_ID_T afwid, UINT32 evcode);

UINT32 ldrLoadElf (WCHAR *uri, WCHAR *params, UINT32 evcode);
UINT32 ldrUnloadElf (ldrElf *elf);

UINT32 ldrInitEventHandlersTbl (EVENT_HANDLER_ENTRY_T *tbl, UINT32 base);
UINT32 ldrFindEventHandlerTbl (const EVENT_HANDLER_ENTRY_T *tbl, EVENT_HANDLER_T *hfn);

#endif
