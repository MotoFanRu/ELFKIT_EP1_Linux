#ifndef EVENTS_H
#define EVENTS_H

#include <typedefs.h>


// см. EVENT_T и EVENT_DATA_T @apps.h

#define EV_GRANT_TOKEN            30
#define EV_REVOKE_TOKEN           31

#define EV_KEY_PRESS              500 	// key_pressed
#define EV_KEY_RELEASE            501 	// key_pressed

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
// Посылается запросами
#define EV_NO                     0x2046
#define EV_YES                    0x2079
// Отмена
#define EV_CANCEL				  0x2008 

// Приняты данные
#define EV_DATA                   0x200F // EVENT_T::attachment
// ?
#define EV_BROWSE                 0x2006

// Возникает при изменении пользователем какого-либо поля
#define EV_DATA_CHANGE            0x2011 // index
// Запросить данные
#define EV_REQUEST_DATA           0x202B // data_req

// Сработал таймер
#define EV_TIMER_EXPIRED		  0x8205A // (DL_TIMER_DATA_T*)EVENT_T::attachment
#define EV_CALL_RECEIVED          0x82015

// получение фокуса
#define EV_GAIN_FOCUS			  20 
// потеря фокуса
#define EV_LOSE_FOCUS 			  21 
// закрытие заставки
#define EV_USER_ACTIVITY		  0x7EE 
// регистрация в сети
#define EV_REG_NETWORK		   	  0x8200B 

#define EV_FLIP_CLOSE             0x20C2 //0x20C0
#define EV_FLIP_OPEN              0x20C3

#define EV_CALL_END               0x8201C

typedef enum
{
      FBF_FREE = 100,  // ОС должна освободить буффер
      FBF_LEAVE        // Приложение должно следить за освобождением буффера
} FREE_BUF_FLAG_T;

typedef struct
{
    UINT32                    bufsize;
    UINT32                    index;
    UINT8                     unk2;
} REQUEST_DATA_T;

typedef struct
{
    char uri[64];
    char params[64];
} ELF_PARAMS_T;

typedef struct
{
    UINT32                    ID;           // ? Идентификатор списка
    UINT32                    begin_idx;    // Начальный индекс требуемого списка элементов
    UINT8                     count;        // Сколько эдементов списка требуется
} REQUEST_LIST_ITEMS_T;

/* Структура, отвечающая за данные, передаваемые с событием */
typedef union
{
    UINT8                           key_pressed;
    UINT32                          index; //45
    REQUEST_LIST_ITEMS_T            list_items_req; //54
    REQUEST_DATA_T                  data_req; //63

	ELF_PARAMS_T				start_params; //192
	//padding to real size
	UINT8						pad[0xFB]; 

} EVENT_DATA_T;

typedef struct
{
    UINT32                    data_tag;
    EVENT_DATA_T              data;
} ADD_EVENT_DATA_T;

typedef struct tagEVENT_T
{
    EVENT_CODE_T              code; //4
    UINT32                    unk1; //8
    UINT32                    ev_port; //12
    UINT8                     unk2; //13
    BOOL                      is_consumed; //14
    BOOL                      is_firstpass; //15
    UINT8                     unk3; //16
    UINT32                    unk4; //20
    UINT32                    unk5; //24
    struct tagEVENT_T        *next; //28
    UINT32                    data_tag; //32
    //UINT8                   pad[8];
    UINT32                    unk6; //36
    UINT32                    unk7; //40

    EVENT_DATA_T              data; //482
    
    BOOL                      is_attach; //483
    UINT16                    att_size; //485
    
    void                      *attachment; //239
    UINT8                     unk8; //240
} EVENT_T; //size = 296

typedef struct
{
    EVENT_CODE_T              code;
    UINT8                     unk1;
    UINT8                     unk2;
    EVENT_T                   *top;
    void                      *unk3;
    UINT8                     unk4[6];
    UINT8                     unk5;
} EVENT_STACK_T;

// Получить текущий event из списка
EVENT_T* AFW_GetEv( EVENT_STACK_T *ev_st );

UINT32 AFW_GetEvSeqn( EVENT_STACK_T *ev_st );

// Функции добавления ивента (в различных вариациях данных для ивента)
UINT32 AFW_AddEvNoD( EVENT_STACK_T    *ev_st,  
                     UINT32           event_code );

UINT32 AFW_AddEvEvD( EVENT_STACK_T    *ev_st,
                     UINT32           event_code,
                     ADD_EVENT_DATA_T *data );

UINT32 AFW_AddEvAux( EVENT_STACK_T    *ev_st,
                     UINT32           event_code,
                     FREE_BUF_FLAG_T  free_buf,
                     UINT32           att_size,
                     void             *attachment );

UINT32 AFW_AddEvAuxD( EVENT_STACK_T     *ev_st,
                      UINT32            event_code,
                      ADD_EVENT_DATA_T  *data,
                      FREE_BUF_FLAG_T  free_buf,
                      UINT32           att_size,
                      void             *attachment );
					  
// добавляет ивент и передаёт его диалогу					  
UINT32 AFW_TranslateEvEvD( EVENT_STACK_T    *ev_st,
                           UINT16           event_code,
                           ADD_EVENT_DATA_T *data );						  

// Удалет последний ивент из списка
UINT32 APP_ConsumeEv( EVENT_STACK_T *ev_st,  void *app );

//  Создает Internal ивент. . Реакцией на ивент будет старт приложения
UINT32 AFW_CreateInternalQueuedEvAux( UINT32 event_code, 
									  FREE_BUF_FLAG_T  free_buf, 
									  UINT32 att_size, 
									  void* att_data ); 

UINT32 AFW_CreateInternalQueuedEvAuxD( UINT32 event_code, 
									   void *data, 
									   FREE_BUF_FLAG_T  free_buf, 
									   UINT32 size, 
									   void *aux_data);


// Добавляет ивент EV_LIST_ITEMS (ответ на EV_REQUEST_LIST_ITEMS)
UINT32 APP_UtilAddEvUISListData( EVENT_STACK_T    *ev_st,
                                 void             *app,
                                 UINT32           param2, // = 0
                                 UINT32           begin_index,
                                 UINT8            count,
                                 FREE_BUF_FLAG_T  free_buf,
                                 UINT32           bufsize,
                                 void             *buffer );

// Добавляет ивент EV_REQUEST_DATA для поля с нужным индексом
UINT32 APP_UtilAddEvUISGetDataForItem( EVENT_STACK_T    *ev_st,
                                       void             *app,
                                       UINT32           index ); 

// обновление листа
UINT32 APP_UtilAddEvUISListChange( EVENT_STACK_T    *ev_st,
                                   void             *app,
                                   UINT32           param2, // = 0
                                   UINT32           position,
                                   UINT8            size,
                                   BOOL             refresh_data,
                                   UINT8            param6,          // = 2
                                   FREE_BUF_FLAG_T  free_buf,
                                   UINT32           bufsize,	// NULL ?
                                   void             *buffer );	// NULL ?
// переход на заданный пункт листа
UINT32 APP_UtilAddEvChangeListPosition ( EVENT_STACK_T    *p_evg,
										 void             *app,
                                         UINT32        	  position,
										 FREE_BUF_FLAG_T  free_buf,
										 UINT32           bufsize,
										 void             *buffer );						   
 
#endif
