#ifndef EV_CODES_H
#define EV_CODES_H

#include <typedefs.h>

//Чтобы хапать значения ивентов из либы используем EXTERN_LIB

//Возникает при выключении телефона
EXTERN_LIB EVENT_POWERDOWN;
#define EV_POWERDOWN (EVENT_CODE_T)EVENT_POWERDOWN

//отправка файлов по Bluetooth
EXTERN_LIB EVENT_BT;
#define EV_BT_SEND (EVENT_CODE_T)EVENT_BT

//запуск предпросмотра при приеме файлов по Bluetooth и wap
//в аттаче имя принимаемого файла и имя временного файла с путём
EXTERN_LIB EVENT_PREVIEW_START;
#define EV_START_PREVIEW (EVENT_CODE_T)EVENT_PREVIEW_START

EXTERN_LIB EVENT_IDLE_FOCUS;
#define EV_IDLE_FOCUS (EVENT_CODE_T)EVENT_IDLE_FOCUS

//Render
#define EV_RENDER			0x19

//Возникает при полном выключении дисплея
#define EV_DISPLAY_NO_ACTIVE		0xd2e

//Возникает при включении заставки
#define EV_SCREENSAVER_START		0xd2f


//Возникает при включении дисплея/активности пользователя
//#define EV_USER_ACTIVITY		0x87d
#define EV_DISPLAY_ACTIVE		0x87d


#define EV_UIS_ACTIVITY			0x87C

//Возникает при выключении подсветки дисплея
#define EV_DISPLAY_BACKLIGHT_OFF	0x880

//Тоже самое. Эти ивенты ловятся в прошивке
#define EV_BACKLIGHT_TIMEOUT_1		0x87F
#define EV_BACKLIGHT_TIMEOUT_2		0x880
#define EV_BACKLIGHT_TIMEOUT_3		0x881


//Возникает при блокировке клавиатуры
#define EV_KEYB_LOCKED			0x812

//Возникает при разблокировке клавиатуры
#define EV_KEYB_UNLOCKED		0x813

//Возникает при изменении Стиля звонка
#define EV_CHANGE_RINGSTYLE		0x834

//Ивент отображения информации об изменении Стиля звонка
#define EV_CHANGE_RINGSTYLE_MESSAGE	0x861

//Ивент вызова "Обновление ПО"
#define EV_SOFTWARE_UPDATE		0xFF7


#define EV_GRANT_TOKEN            0x1E //30
#define EV_REVOKE_TOKEN           0x1F //31

// key_pressed возникает при нажатии на кнопку
#define EV_KEY_PRESS              500

// key_pressed возникает при отпускании кнопки
#define EV_KEY_RELEASE            501

// для определения нажатия на клавиши ловим эти ивенты (в канвас)
// key_pressed возникает при нажатии на кнопку
#define EV_INK_KEY_PRESS           0x2034 //  EVENT_DATA_T

// key_pressed возникает при отпускании кнопки
#define EV_INK_KEY_RELEASE         0x2035 //  EVENT_DATA_T

// Возникает при завершении работы диалога
#define EV_DIALOG_DONE            0x201C

// Возникает при нажатии на кнопку "Назад"
#define EV_DONE                   0x2020

// Возникает при нажатии джойстика вверх/вниз в списке
#define EV_LIST_NAVIGATE          0x2043 // index

// Возникает при нажатии джойстика в списке или кнопки "Выбор"
#define EV_SELECT                 0x2058 // index

// Посылается списками чтобы запросить очередную порцию данных
#define EV_REQUEST_LIST_ITEMS     0x202C // list_items_req

// нажатие на качельку громкости в GUI. НЕ ПРОВЕРЕНО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define EV_VOL_CHANGE_REQUEST	  0x20CA

// Посылается запросами
#define EV_NO                     0x2046
#define EV_YES                    0x2079

// Отмена
#define EV_CANCEL		  0x2008

// Приняты данные
#define EV_DATA                   0x200F //EVENT_T::attachment

// ?
#define EV_BROWSE                 0x2006

// Возникает при изменении пользователем какого-либо поля
#define EV_DATA_CHANGE            0x2011 //index

// Запросить данные.   data_req
#define EV_REQUEST_DATA           0x202B

// Сработал таймер.
#define EV_TIMER_EXPIRED	  0x8205A //(DL_TIMER_DATA_T*)EVENT_T::attachment

#define EV_CALL_RECEIVED          0x82015
#define EV_CALL_PLACE		  0x82450
#define EV_CALL_PLACE2		  0x530
#define EV_CALL_PLACE_DONE 	  0x8201A
#define EV_CALL_SUSPEND		  0x550		//0x553 0x713 ??? на 0х550 весит обработчик CallPlaceHandleSuspend
#define EV_CALL_ANSWERED EV_CALL_PLACE2 + 2


#define EV_CALL_END               0x8201C
#define EV_CALL_STATE_CHANGED     0x82027

#define EV_CALL_ALERT		  0x82805 //???

//Событие возникает при поиске номера в справочнике
#define EV_PBK_READ_RECORD	  0x82052

// получение фокуса
#define EV_GAIN_FOCUS		  20

// потеря фокуса
#define EV_LOSE_FOCUS		  21

// закрытие заставки
#define EV_USER_ACTIVITY	  0x7EE


#define EV_SIM_CACHED				0x82008
#define EV_REG_NETWORK				0x8200B
#define EV_REG_QUERY_SIGNAL_STRENGHT_CHANGE	0x8200c
#define EV_NPN_CHANGE				0x82042
#define EV_SPN_CHANGE				0x82042
#define EV_GPRS_STATUS_CHANGE			0x82121

#define EV_ROAM_STATUS_CHANGE_1			0x8200A
#define EV_ROAM_STATUS_CHANGE_2			0x8240C


#define EV_CREATE_SHORTCUT	0x20C9 //Зажатие меню
#define EV_UIS_FORCE_REFRESH	0x21FB //Возникает при использовании ф-ции UIS_ForceRefresh()

// FLIP EVENTS
#define EV_FLIP_CLOSE           0x20C2 //закрытие флипа. 0x20C0??
#define EV_FLIP_OPEN            0x20C3 //открытие флипа

// EXTERNAL DEVICE EVENTS
#define EV_DEVICE_ATTACH	0x820BD	//подключение устройства
#define EV_DEVICE_DETACH 	0x820BE //отключение устройства

// USSD EVENTS
#define EV_USSD_NOTIFY		0x82039 //от USSD-службы пришло оповещение. отвечать на него не нужно, просто показать. В аттаче USSD_REQUEST_NOTYFY_T
#define EV_USSD_REQUEST   	0x8203A //от USSD-службы пришел ответ, требующий новых данных. отвечать нужно через DL_SigCallSSSendUSSDResponseReq используя новый ss_id. В аттаче USSD_REQUEST_NOTYFY_T
#define EV_USSD_COMPLETE  	0x8203B //USSD-служба сообщает что запрос выполнен.  текст может быть а может нет. В аттаче  USSD_COMPLETE_T
#define EV_USSD_OPERATION	0x82028 //получен ответ на запрос DL_SigCallSSOperationReq. В аттаче USSD_OPERATION_T

// BATTERY EVENTS
#define EV_BATTARY_CHANGE	0x8200D	//Возникает при смене батареи???
#define EV_BATTERY_POWER_CHANGE 0x8207B	//Возникает при изменении питания??? (подключение зарядки и т.д.)
#define EV_BATTERY_LOW		0x820C9	//Возникает при разрядке батареи. Тут запускается таймер который пибикает блин

// DATEBOOK EVENTS
#define EV_DBK_ASYNC_OPERATION	0x8206A
#define EV_DBK_ADD_EVENT	0x8206B
#define EV_DBK_DATABASE_QUERY	0x8206C	//ответ на запрос
#define EV_DBK_DATABASE_SLOT	0x8206D
#define EV_DBK_DATABASE_ALTERED	0x8206E	//какие-то изменения в базе
#define EV_DBK_DATABASE_ALARM	0x8206F		
#define EV_DBK_NUM_OF_EVENTS	0x82070
#define EV_DBK_DATABASE_IDLE	0x820D5	//база ничем не занята
#define EV_DBK_WEEK_VIEW	0x82126	//ответ на запрос недельного вида
#define EV_DBK_MONTH_VIEW	0x821E3	//ответ на запрос месячного вида

// ALARMCLK EVENTS
#define EV_ALARMCLK_EXPIRED	0x82059
#define EV_ALARMCLK_REMINDER_S  0xf49

// Midlet list events
//ROM:10AAF8F0 state_Get_Run_App_Waiting_table:.long 0x82513  DATA XREF: ROM:10AAF36Co
//ROM:10AAF8F4                 .long DBKGetSuiteMIDletListHandler
//ROM:10AAF8F8                 .long 0x82515
//ROM:10AAF8FC                 .long DBKGetSuiteMIDletListHandler
#endif
