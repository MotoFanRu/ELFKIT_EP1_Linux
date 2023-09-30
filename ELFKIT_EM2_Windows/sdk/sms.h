//�� ���������!
#ifndef SMS_H
#define SMS_H

#include <typedefs.h>

typedef enum
{
	MSG_FOLDER_INBOX		= 0, // �������� 
	MSG_FOLDER_OUTBOX		= 1, // ��������� 
	MSG_FOLDER_INFOSRV_INBOX	= 2,
	MSG_FOLDER_QUICKNOTE		= 3, 
	MSG_FOLDER_DRAFTS_FOLDER	= 4, //+++
	MSG_FOLDER_EMAIL_INBOX		= 5,
	MSG_FOLDER_TEMP_FOLDER		= 15,
	MSG_FOLDER_VOICEMAIL		= 16,
	MSG_FOLDER_BROWSER_MSGS		= 19,
	MSG_FOLDER_ALL_FOLDERS		= 23,
	MSG_FOLDER_INVALID		= 255
} MSG_FOLDER_T;

typedef struct
{
	UINT8	pad[0x6c];
	WCHAR	phone_number[0x22]; // ����� 
	UINT8	pad2[0x1C8];
	WCHAR	text[0x100];	    // ����� 
} MSG_READ_T;

typedef struct
{
	WCHAR	address[51];
	UINT8	addr_type;	// 0 -SMS, 1 - EMAIL, 2 - No, 3 - Long Msg ??? 
	WCHAR	contents[512];	// ������������ ������ 0x7BFE, ������ ��� SMS, �����, ��� �����
} SEND_TEXT_MESSAGE_T;


// ��������� ��������� �� id. ����� ������ ������ �� ������ EV_MSG_READ. part_num = 0 or 1.
UINT8 DL_DbMessageGenericGetMessage(IFACE_DATA_T *iface_data, MSG_FOLDER_T msg_folder, UINT16 msg_id, UINT16 part_num, UINT8 unk); //unk=0

// �������� ������ id ���������. msg_id_list - ���c�� id ���������, �������� 245.
UINT8 DL_DbMessageGenericGetMessageIDList( MSG_FOLDER_T msg_folder, UINT16 *total_number, UINT16 *msg_id_list, UINT8 unk); //unk=0

// ����������� ������������� ���
UINT32 MsgUtilGetUnreadMsgsInAllFolders (UINT16 *msg_total);

// �������� ��������� 
UINT32 DL_SigMsgSendTextMsgReq (IFACE_DATA_T *iface_data, SEND_TEXT_MESSAGE_T *msg_ptr);

//�������� ���������. ������ ���������� ������!
UINT32 DL_DbMessageGenericDeleteMessage(IFACE_DATA_T *iface_data, MSG_FOLDER_T msg_folder, UINT16 msg_id, UINT8 unk1, UINT8 unk2); //unk1 = 0 or 1, unk2 = 3.
//�������� ��� unk2=    UINT8	addr_type;	// 0 -SMS, 1 - EMAIL, 2 - No, 3 - Long Msg ??? 
//� �� ����� unk1, ��. �������� 0 - �������, 1 - ���


#endif
