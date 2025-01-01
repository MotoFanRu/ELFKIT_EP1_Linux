#ifndef SESSIONS_H
#define SESSIONS_H

#include <apps.h>
#include <typedefs.h>

//���������� � ������
typedef struct
{
	UINT8 	ss_id;		//ID ������
	UINT8 	apps_num;	//���-�� ���������� � ������
} SESSION_INFO_T;


//���������� � APP
typedef struct
{
	APPLICATION_T	*app;   //��������� �� ��������� ����������
	APP_ID_T	app_id; //ID ����������
	UINT8		ss_id;  //ID ������
} APP_INFO_T;

//�������� ID �������� ������
UINT8 AFW_GetActiveSession (void);

//�������� ���-�� ������
UINT8 AFW_GetNumSessions (void);

//�������� ������ ������
UINT8 AFW_GetSessionList (SESSION_INFO_T *ss_list, UINT8 ss_num);

//�������� ������ ���������� � ������ � �������� ss_id
UINT8 AFW_GetAppsInSession (UINT8 ss_id, APP_INFO_T *apps_list, UINT8 apps_num);

#endif
