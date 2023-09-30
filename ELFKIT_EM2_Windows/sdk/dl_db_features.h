#ifndef DL_DB_FEATURES_H
#define DL_DB_FEATURES_H

#include <typedefs.h>

//��������� �����. 0 - ���� ������, 1 - ���� ������ (UINT8)
EXTERN_LIB FLIP_STATE;
#define flip_state *((UINT8 *)FLIP_STATE)

//������ �� �������� ����� IMEI
EXTERN_LIB SEEM_IMEI;
#define DB_FEATURE_IMEI (UINT16)SEEM_IMEI //WCHAR IMEI[16];

//������ �� �������� ����� IMSI
EXTERN_LIB SEEM_IMSI;
#define DB_FEATURE_IMSI (UINT16)SEEM_IMSI //WCHAR IMSI[16];

// ��������� ����������: 1 - ������. 0 - �������
EXTERN_LIB KEYPAD_STATE;
#define DB_FEATURE_KEYPAD_STATE (UINT16)KEYPAD_STATE

#define BEGIN_4A__IN_DB             0x5C1 //null byte of seem 0004a_0001. u must add this value to offset from seem 004a_0001
#define CURRENT_RING_STYLE          BEGIN_4A__IN_DB + 0xA    //0 - �������, 1 - �����, 2 - �����, 3 - ����� � ������, 4 - ���������
#define ID_CLOCK                    0x6FF //0 Dig, 1 an, 2 off
#define ID_DATE                     0x7C3 //0 off, 1 on
#define ID_DATE_DISP                0x6FE //0 left, 1 cent
#define ID_MAINMENU_STYLE           0x6A0 //0 icons, 1 list
#define ID_BKG_STYLE                0x678 //0 cent, 1 mul, 2 ad
#define ID_SCREENSAVER              0x69C //0 off, ....
#define ID_PHOTO_RESOLUTION         0x6C5 //0, 1, 2, 3
#define ID_PHOTO_QUALITY            0x75B //0, 1, 2
#define ID_VIDEO_LEN                0x692 //0, 1, 2
#define ID_VIDEO_QUALITY            0x691 //0, 1, 2
#define ID_JAVA_BKLT                0x71C //0 - ����, 1 - ���
#define ID_JAVA_VIBRATION           0x71E //0 - ����, 1 - ���
#define ID_JAVA_BKLT_DURATION       0x74D //0 - Unlimited, 1 - Limited (�� ������ ������� ����)
#define ID_MISSED_CALLS_RINGER      0x85A //���� 4, �� ���������� � ����������� ����. (�������� ��� ������ ����� CURRENT_RING_STYLE)
#define ID_TIME_FORMAT              0x5C2 //0 - 12 �������� ������, 1 - 24 ������� ������
#define ID_IS_DATA_READY            0x602 //0, 1
#define ID_IS_BROWSER_READY         0x604 //0, 1
#define ID_IS_MP3_READY             0x6A5 //0, 1    //0x5C1 + 0xE4
#define ID_AIRPLANE_MODE            0x7C6 //0 - ����, 1 - ���
#define ID_AIRPLANE_MODE_NOTICE     0x7C7 //0 - ����, 1 - ���

//�������� ������ ������
#define ID_NOTEPAD_STRING           0xA8F //�������. WCHAR notepad[42];
#define ID_SECURITY_CODE            0xA96
#define ID_UNLOCK_CODE              0xA95
#define ID_TECHNOLOGY               0xAAF //WCHAR technology[20];

//�������� ���� ������.
#define ID_DISP_BACKLIGHT           0xB57 //UINT8 data[16]. data[7] = ������� �������!!!, data[3] = ������������� �������!!!
#define ID_RC_DIALED_CALLS_TIME     0xB6D //Dialed Calls.   sizeof(CALL_TIME_T)
#define ID_RC_RECEIVED_CALLS_TIME   0xB6E //Received Calls. sizeof(CALL_TIME_T)
#define ID_RC_LAST_CALL_TIME        0xB6F //Last Call. sizeof(CALL_TIME_T)
#define ID_RC_ALL_CALLS_TIME        0xB70 //All Calls. sizeof(CALL_TIME_T)
#define ID_RC_LIFE_TIME             0xB71 //Life Time. sizeof(CALL_TIME_T)
#define ID_ESN                      0xB50 //UINT8 data[4]
#define ID_FLEX                     0xB58 //UINT8 data[80]
#define ID_UNK                      0xB59 //UINT8 data[3]

#define ID_SW_UPDATE_FEATURE_AVAILABILITY   0x3D4
#define ID_AIRPLLNAME_MODE_AVAILABILITY     0x4CF

#endif
