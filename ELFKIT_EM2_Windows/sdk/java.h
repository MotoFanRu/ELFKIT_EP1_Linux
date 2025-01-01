#ifndef JAVA_H
#define JAVA_H

#include <typedefs.h>

#define JAVA_APP_ID UINT32

// ������� ����������� ������ �������
// �����_����� << 8 | 0x20001
#define DL_FS_MID_T_to_JAVA_APP_ID(fs_mid, java_app) {java_app = fs_mid << 8 | 0x20001}


//��������� ����� J2MEST
#define J2MEST_STEP 1104
#define MIDLET_NAME_LEN 32

typedef struct
{
    UINT8 unk1[6];                              //0x00
    char midlet_name[MIDLET_NAME_LEN + 1 ];     //0x06  ��� java ����������
    char midlet_author[MIDLET_NAME_LEN + 1];    //0x27  ����� java ����������
    UINT8 unk2[56];                             //0x48                
    DL_FS_MID_T midlet_icon;                    //0x80  ���������� ������������� ������ java ����������
    UINT8 unk3[968];                            //0x88

} J2MEST_RECORD_T;                              //������ = 1104 ���� (J2MEST_STEP)





//UINT32 JavaStartApp( JAVA_APP_ID java_id,
//				WCHAR * str,	// unknown
//				UINT64	unk1,	// =0
//				UINT32	arg_0,	// =0
//				SU_PORT_T port
//				);


//JAVA_APP_ID JavaGetMidletSuite(JAVA_APP_ID java_id);
//JAVA_APP_ID JavaGetMIDletID(WCHAR *suite_name, WCHAR *midlet_name, WCHAR *vendor_name);

//�������� �� ��� ����������. 1 - ��, 0 -���
UINT32 JavaAppRunning (void);

//����������� ����������
UINT32 AMS_ResumeUserVM (void);

//������������� ��� ����������
UINT32 AMS_SuspendUserVM (void);

//��������� ��� ����������
UINT32 AMS_StopUserVM (void);

//����� ������ � ���
EXTERN_LIB JSAPI_MARQUEE_TEXT;
#define jsapi_marquee_text (WCHAR *)JSAPI_MARQUEE_TEXT

UINT32 JavaStartMarquee (const WCHAR *text);
UINT32 JavaStopMarquee  (const WCHAR *text); // text = jsapi_marquee_text

//����� "��������� Java"
void SJAL_Start (void);

#endif
