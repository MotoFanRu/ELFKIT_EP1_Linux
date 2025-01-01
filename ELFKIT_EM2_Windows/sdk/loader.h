#ifndef LOADER_H
#define LOADER_H

#include <apps.h>
#include <typedefs.h>

//�������� �����
typedef struct tagldrElf
{
    UINT32           evbase;    //����� �������
    char             *name;     //��� ����������
    APPLICATION_T    *app;      //��������� �� APP
    DL_FS_MID_T      id;        //���������� ������������� �����

    UINT8            *image;    //��������� �� ����� ����� � ������
    struct tagldrElf *next;     //��������� ������
} ldrElf;

//��������� ����
typedef struct 
{
    UINT32 number_of_records;
    UINT32 records_table_offset;
    UINT32 names_table_offset;
    UINT32 pad;
} LIBRARY_HEADER_T;

//������� ����
typedef struct 
{
    UINT32 record_value;
    char * record_name;
} LIBRARY_RECORD_T;

//��������� �����������
typedef struct
{
    UINT32 records_num;
    UINT32 reserved[3];
} AUTORUN_HEADER_T;

//������� �����������
typedef struct
{
    DL_FS_MID_T file_id;
    UINT32 evcode_base;
    UINT32 reserved;
} AUTORUN_RECORD_T;

//
typedef enum
{
    AUTORUN_ACTION_ADD = 0,
    AUTORUN_ACTION_CLEAR,
    AUTORUN_ACTION_DELETE,
    AUTORUN_ACTION_FIND,
    AUTORUN_ACTION_LOAD
} AUTORUN_ACTION_T;

UINT32  ldrAutorunAction (AUTORUN_RECORD_T *autorun_record, AUTORUN_ACTION_T autorun_action);

UINT32  ldrLoadDefLibrary (void);
LIBRARY_RECORD_T * ldrSearchInDefLibrary (char *record_name);

UINT32  ldrSendEvent (UINT32 evcode);
UINT32  ldrSendEventToApp (AFW_ID_T afwid, UINT32 evcode, void *attachment, UINT32 att_size, FREE_BUF_FLAG_T fbf);
UINT32  ldrSendEventToAppEmpty (AFW_ID_T afwid, UINT32 evcode);

UINT32  ldrLoadElf (WCHAR *uri, WCHAR *params, UINT32 evcode);
UINT32  ldrUnloadElf (ldrElf *Elf);

UINT32  ldrInitEventHandlersTbl (EVENT_HANDLER_ENTRY_T *tbl, UINT32 base);
UINT32  ldrFindEventHandlerTbl (const EVENT_HANDLER_ENTRY_T *tbl, EVENT_HANDLER_T *hfn);

ldrElf* ldrFindElf  (const char *name);
BOOL    ldrIsLoaded (const char *name);
ldrElf* ldrGetElfsList (UINT32 *count);

#endif
