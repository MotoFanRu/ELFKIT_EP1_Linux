#ifndef RESOURCES_H
#define RESOURCES_H

#include <typedefs.h>

/* ID ������� ����������� ����� ������� � �������� �������� ����� � ������� ������� �������� ����
    ��������, 0x01000496 = "��������� ������"
    �������������� ��� ����� � �������� �������, ��������������, �� �������� � DRM */


#define RES_TYPE_STRING         0x01
#define RES_TYPE_ACTION		    0x06
#define RES_TYPE_GRAPHICS       0x12
#define RES_TYPE_CONTENT 		0x0C
#define RES_TYPE_INTEGER 		0x04
#define RES_TYPE_BITMAP 		0x02


typedef UINT32	RESOURCE_ID;

// ������� ������
UINT32 DRM_CreateResource( RESOURCE_ID      *res_id,
                           UINT32           type,       //RES_TYPE_*
                           void             *data,
                           UINT32           size );

// �������� ���������� �������
UINT32 DRM_GetResource( RESOURCE_ID     res_id,
                        void            *buf,
                        UINT32          size );

// �������� ������ �������
UINT32 DRM_GetResourceLength( RESOURCE_ID res_id,  UINT32 *size );

// �������� ������
UINT32 DRM_SetResource( RESOURCE_ID     res_id,
                        void            *data,
                        UINT32          size );

// ���������� ������
UINT32 DRM_ClearResource( RESOURCE_ID res_id );



/* ��������� ��� �������� ������� ��� Action-� */
typedef struct
{
	RESOURCE_ID			softkey_label;		// ������� �� ����-������
    RESOURCE_ID			list_label;			// ������� � ������ ��������
    INT8				softkey_priority;	// ��������� ������������ �� �����������, ������������ - 
    										// �� ����� ����, ������������� - �� ������
    INT8				list_priority;		// ��������� ������������ � ������ ��������, ��� ������
    										// � ��������� ����������� ������������� �� �����������, � ������ ��������� softkey_priority
    BOOL            	isExit;				// ?? ������� �� �������� ������ �� ����������
    BOOL            	sendDlgDone;		// �������� �� ������������� ����� EV_DIALOG_DONE
} RES_ACTION_LIST_ITEM_T; 

// �������� ������� ����
UINT8 DRM_GetCurrentLanguage(UINT8 * LGID);

/*
LGID:
04 Chinese Complex 
05 Chinese Simple
03 British English
01 English
*/


#endif
