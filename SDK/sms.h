// UTF-8 w/o BOM
/********************************
  Сообщения
********************************/

#ifndef __SDK_SMS_H__
#define __SDK_SMS_H__

#include <typedefs.h>



typedef enum
{
	MSG_FOLDER_INBOX			  = 0, // Входящие 
	MSG_FOLDER_OUTBOX			 = 1, // Исходящие 
	MSG_FOLDER_INFOSRV_INBOX	  = 2,
	MSG_FOLDER_QUICKNOTE		  = 3,
	MSG_FOLDER_DRAFTS_FOLDER	  = 4,
	MSG_FOLDER_EMAIL_INBOX		= 5,
	MSG_FOLDER_TEMP_FOLDER		= 15,
	MSG_FOLDER_VOICEMAIL		  = 16,
	MSG_FOLDER_BROWSER_MSGS	   = 19,
	MSG_FOLDER_ALL_FOLDERS		= 23,
	MSG_FOLDER_INVALID			= 255
	
} MSG_FOLDER_T;


typedef struct
{
	UINT8		pad[0x6c];
	WCHAR		phone_number[0x22];//  номер 
	UINT8		pad2[0x1C8];
	WCHAR		text[0x100]; // текст 
} MSG_READ_T;

typedef enum {
	MSG_SMS,
	MSG_EMAIL,
	MSG_LONG,
	MSG_UNKNOWN
} MSG_TYPE_T;

typedef struct
{
	WCHAR		address[51];
	UINT8		addr_type; // 0 -SMS, 1 - EMAIL, 2 - No, 3 - Long Msg ??? 
	WCHAR		contents[512]; // максимальный размер 0x7BFE, только для SMS, думаю, это много
} SEND_TEXT_MESSAGE_T;


#ifdef __cplusplus
extern "C" {
#endif


// прочитать сообщение по id
// ответ должен придти по ивенту EV_MSG_READ 
UINT8 DL_DbMessageGenericGetMessage(	IFACE_DATA_T	*iface_data, 
										MSG_FOLDER_T	msg_folder, 
										UINT16			msg_id,
										UINT16			part_num, // 0 or 1
										UINT8			unk ); // = 0

// получить список id сообщений
UINT8 DL_DbMessageGenericGetMessageIDList(	MSG_FOLDER_T msg_folder,
											UINT16		*total_number,
											UINT16		*msg_id_list, // спиcок id сообщений, максимум 245
											UINT8		unk ); // = 0


// колличество непрочитанных смс
UINT32 MsgUtilGetUnreadMsgsInAllFolders( UINT16 *msg_total );


// Отправка сообщений 
UINT32 DL_SigMsgSendTextMsgReq(	IFACE_DATA_T			*port, 
								SEND_TEXT_MESSAGE_T		*msg_ptr );


#ifdef __cplusplus
}
#endif

#endif // __SDK_SMS_H__
