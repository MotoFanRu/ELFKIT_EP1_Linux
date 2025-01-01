#ifndef SESSIONS_H
#define SESSIONS_H

#include <apps.h>
#include <typedefs.h>

//Информация о сессии
typedef struct
{
	UINT8 	ss_id;		//ID сессии
	UINT8 	apps_num;	//Кол-во приложений в сессии
} SESSION_INFO_T;


//Информация о APP
typedef struct
{
	APPLICATION_T	*app;   //Указатель на структуру приложения
	APP_ID_T	app_id; //ID приложения
	UINT8		ss_id;  //ID сессии
} APP_INFO_T;

//Получить ID активной сессии
UINT8 AFW_GetActiveSession (void);

//Получить кол-во сессий
UINT8 AFW_GetNumSessions (void);

//Получить список сессий
UINT8 AFW_GetSessionList (SESSION_INFO_T *ss_list, UINT8 ss_num);

//Получить список приложений в сессии в индексом ss_id
UINT8 AFW_GetAppsInSession (UINT8 ss_id, APP_INFO_T *apps_list, UINT8 apps_num);

#endif
