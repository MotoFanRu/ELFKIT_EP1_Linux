#define DEBUG

#ifndef UTILS_H
#define UTILS_H

#include <apps.h>
#include <dl.h>
#include <canvas.h>
#include <bluetooth.h>
#include <filesystem.h>
#include <resources.h>
#include <utilities.h>
#include <uis.h>

UINT32 GetResourceString(WCHAR *str, RESOURCE_ID resID);
void AutoPress(UINT8 key);
UINT32 fReadDB(UINT32 offset);
WCHAR* SplitPath(WCHAR* path, WCHAR* spliter);
BOOL WorkingTable(void);
BOOL JavaApp(void);
BOOL KeypadLock(void);
UINT32 util_asc2ul( const char * s, UINT32 count );
UINT32 param2uri(WCHAR *param, WCHAR *uri);
UINT32 file_write(WCHAR* uri, UINT8 mode, void* buffer, UINT32 size);

UINT32 OneAction( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res, UINT8 op );
UINT32 ActionAdd( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res );
UINT32 ActionDel( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res );
UINT32 ActionUpd( ACTIONS_T * list, EVENT_CODE_T event, RESOURCE_ID res );

BOOL isAudio(WCHAR *ext);
BOOL CmpStr(WCHAR *str1, WCHAR *str2);
UINT32 SendViaBT(WCHAR* uri);

#endif
