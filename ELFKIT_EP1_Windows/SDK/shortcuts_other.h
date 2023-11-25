#ifndef SC_H
#define SC_H

/********************************
  �����
*********************************/

#include <typedefs.h>
#include <events.h>


#ifdef __cplusplus
extern "C" {
#endif


#define SC_TYPE_URL		5
#define SC_TYPE_APP		6

typedef UINT8 SC_TYPE_T;

// ���������� ������� � ������ ��� ������ ������
typedef struct
{
/*+00*/    UINT8            	seem_rec;    // seem_rec-1
/*+01*/    UINT8				type;        // ��� ����� 
/*+02*/    UINT8            	index;       // ����� � ������, ������ �� 1. ����� �����
/*+03*/    UINT8            	unk1;        // 0xFE
/*+04*/    UINT32            	ev_code;
/*+08*/    UINT32            	list1_index;	 // id ����������� ������ ������
/*+12*/    UINT32            	param1;       
/*+16*/	   UINT32            	list2_index; // ���� �����-�� id ������
/*+20*/    UINT32            	param2;       
/*+24*/    UINT32            	data;	     // 
/*+28*/    UINT32		        lang_text;   // RESOURCE_ID
/*+32*/    WCHAR            	text[17];	 // ��� ����� // 34
/*+66*/    UINT8            	state;       // ����� state, ��� ������� ������� �����
} SEEM_0002_T;
// 68

#define SHORTCUT_RECORD_T		SEEM_0002_T

/* ���������� � ������ � ������� 
#define EV_HANDLE_SHORTCUT			0x20C5 
*/
typedef struct 
{
/*+00*/		UINT8			type;
/*+04*/		UINT32			param1;
/*+08*/		UINT32			list1_index;
/*+0C*/		UINT32			param2;
/*+10*/		UINT32			list2_index;
/*+14*/		WCHAR			text[17]; // 0x22 = 34
/*+38*/		UINT32			lang_text;
} SHORTCUT_CREATE_T; // 0x3c = 60


UINT32 APP_UtilShortcutBuildRecord(EVENT_STACK_T *ev_st, SHORTCUT_RECORD_T *sc_data, UINT32 ev_code, UINT32 state, UINT32 data);
/* ��������� sc_data �� ������ ���������� ev_code, state, data � ������ �� ������ 
����� ���� SHORTCUT_CREATE_T
*/

UINT32 APP_UtilShortcutCopyRecordFromEv(EVENT_STACK_T *ev_st, void *app);
/* �������� ����� (SHORTCUT_RECORD_T) � app->sc_data (�������� ������) 
*/

UINT32 APP_UtilShortcutAcceptCreate(EVENT_STACK_T *ev_st, void *app, UINT32 ev_code, UINT32 state, UINT32 data);
/* �������� ����������� ������ �������� �����
��������� ������ �� ������ ���������� ev_code, state � ������ �� ������
ev_code - �����, ���������� ������
state - ��������� ����������, ���� ��� ����� ����� ����� � ���-���� ������
data - ���. ������, ���� ��� ����� ����� ����� � ���-���� ������
*/

UINT32 APP_UtilShortcutCreateAppShortcut(EVENT_STACK_T *ev_st, void *app, UINT32 ev_code);
/* �������� ����������� ������ �������� �����
��������� ������ �� ������ ���������� ev_code � ������ �� ������
ev_code - �����, ���������� ������
SHORTCUT_RECORD_T.type = SC_TYPE_APP
SHORTCUT_RECORD_T.state = 0xFF
*/

UINT32 APP_UtilShortcutGetShortcutType(UINT32 t /*SC_RECORD_T.type*/);
// �������� ��� ����� �� ������ (�� ����� SHORTCUT_RECORD_T.type)

UINT32 APP_UtilShortcutRejectInvoke(EVENT_STACK_T *ev_st, void *app);
/* �������� ������ � ������� ������ �����
 ������ ������� ��� �� app->sc_data ��� �� ������ */

UINT32 APP_UtilShortcutRejectCreate(EVENT_STACK_T *ev_st, void *app);
/* ���� ����� != EV_HANDLE_SHORTCUT �� ������ RESULT_FAIL
����� �������� ������ � ������� �������� ����� */



/********************************
  �����
*********************************/
// ����������� !

// ��� ������ �����
#define EV_SHORTCUT_READ_RECORD                0x820F7 // � ������ SHORTCUT_READ_RECORD_T
// ��� ������ URL �����
#define EV_SHORTCUT_READ_URL	               0x82112 // � ������  SHORTCUT_READ_URL_T

typedef struct 
{
    UINT8       	result; // 0 - ���� �������
    UINT8       	unk0;
    SEEM_0002_T 	*record;
} SHORTCUT_READ_RECORD_T;

typedef struct 
{ 
    UINT8  			result; 
    UINT16          url_strlen;
    UINT8		    url[100];
} SHORTCUT_READ_URL_T;


// �������� �����									
UINT8 DL_DbShortcutCreateRecord( IFACE_DATA_T  *data, SEEM_0002_T  record );

// ���������� �����
UINT8 DL_DbShortcutUpdateRecord( IFACE_DATA_T  *data, SEEM_0002_T  record );

// �������� �����
UINT8 DL_DbShortcutDeleteRecord( IFACE_DATA_T  *data, UINT8 seem_rec);

// ��������� �����. ������� �����������. ���������� EV_SHORTCUT_READ_RECORD. � aatchment  ����� SHORTCUT_READ_RECORD_T
UINT8 DL_DbShortcutGetRecordByRecordId( IFACE_DATA_T  *data, UINT8  seem_rec);

// �������� ��-�� ��������� �������
UINT16 DL_DbShortcutGetNumOfRecordsAvailable( void ); 

// �������� ���-�� ������������ �������
UINT16 DL_DbShortcutGetNumOfRecordsUsed( BOOL voice_shortcut );

// �������� ������ ��������� �����
UINT8 DL_DbGetFirstAvailableNumber( void ); 

// �������� ���-�� ������������  ������� URL
UINT16 DL_DbShortcutGetNumOfURLRecordsUsed( void );

// �������� ���-�� ��������� ������� URL
UINT16 DL_DbShortcutGetNumOfURLRecordsAvailable( void );

// �������� URL. ������� �����������. ���������� EV_SHORTCUT_READ_URL
UINT32 DL_DbShortcutGetURLByURLId( IFACE_DATA_T *data, UINT32 URLId);

// ��������� ���� ������
SC_TYPE_T DL_DbShortcutGetshortcutType( UINT8 index );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif