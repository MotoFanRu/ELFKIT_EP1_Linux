#ifndef USSD_H
#define USSD_H

#include <typedefs.h>

#define USSD_INVALID_SS_ID 0xFFFF
typedef UINT16 USSD_SS_ID_T;

//���������
typedef struct
{
    WCHAR str[200];
} USSD_STRING_T;

//���������
typedef struct
{
    USSD_SS_ID_T    ss_id;
    USSD_STRING_T   ussd_string;
} USSD_REQUEST_NOTYFY_T;

//���������
typedef struct
{
    USSD_SS_ID_T    ss_id;
    UINT8           result;
    UINT8           cause;
    USSD_STRING_T   ussd_string;
} USSD_COMPLETE_T;

//��������� ������� ���������, �� �� �� �����
typedef struct
{
   USSD_SS_ID_T	 ss_id;
   UINT8         result;
   UINT8         cause;
   UINT8         operation;	   // = 6
   UINT8	 ss_code; 
   UINT8	 service_type;
   UINT8	 service_code;
   UINT8	 no_reply_time;
   UINT8         return_data_type; // = 7
   USSD_STRING_T return_data;
} USSD_OPERATION_T; 		   //Size = 1010 bytes

typedef struct
{
    WCHAR ss_string[200];
    UINT8 ton; // = 1
    UINT8 npi; // = 1
} USSD_REQ_STRING_T;


//�������� USSD �������, ����� ss_id (unk = 0)
USSD_SS_ID_T DL_SigCallSSOperationReq (IFACE_DATA_T *iface_data, USSD_REQ_STRING_T * str, UINT8 unk);

//�������� ������� USSD � ����� �� EV_USSD_REQUEST (id - ���������� id � ��������� �� 0�0000 �� 0�FFFE, str - ����� �������)
void DL_SigCallSSSendUSSDResponseReq (IFACE_DATA_T *if_data, USSD_SS_ID_T id, USSD_STRING_T *str);

//������ ������� USSD									  
void DL_SigCallSSAbortUSSDReq (USSD_SS_ID_T id);

#endif
