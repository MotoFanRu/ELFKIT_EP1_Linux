#ifndef EVENTS_H
#define EVENTS_H

#include <typedefs.h>


// ��. EVENT_T � EVENT_DATA_T @apps.h

#define EV_GRANT_TOKEN            30
#define EV_REVOKE_TOKEN           31

#define EV_KEY_PRESS              500 	// key_pressed
#define EV_KEY_RELEASE            501 	// key_pressed

// ��������� ��� ���������� ������ �������
#define EV_DIALOG_DONE            0x201C
// ��������� ��� ������� �� ������ "�����"
#define EV_DONE                   0x2020
// ��������� ��� ������� ��������� �����/���� � ������
#define EV_LIST_NAVIGATE          0x2043 // index
// ��������� ��� ������� ��������� � ������ ��� ������ "�����"
#define EV_SELECT                 0x2058 // index
// ���������� �������� ����� ��������� ��������� ������ ������
#define EV_REQUEST_LIST_ITEMS     0x202C // list_items_req
// ���������� ���������
#define EV_NO                     0x2046
#define EV_YES                    0x2079
// ������
#define EV_CANCEL				  0x2008 

// ������� ������
#define EV_DATA                   0x200F // EVENT_T::attachment
// ?
#define EV_BROWSE                 0x2006

// ��������� ��� ��������� ������������� ������-���� ����
#define EV_DATA_CHANGE            0x2011 // index
// ��������� ������
#define EV_REQUEST_DATA           0x202B // data_req

// �������� ������
#define EV_TIMER_EXPIRED		  0x8205A // (DL_TIMER_DATA_T*)EVENT_T::attachment
#define EV_CALL_RECEIVED          0x82015

// ��������� ������
#define EV_GAIN_FOCUS			  20 
// ������ ������
#define EV_LOSE_FOCUS 			  21 
// �������� ��������
#define EV_USER_ACTIVITY		  0x7EE 
// ����������� � ����
#define EV_REG_NETWORK		   	  0x8200B 

#define EV_FLIP_CLOSE             0x20C2 //0x20C0
#define EV_FLIP_OPEN              0x20C3

#define EV_CALL_END               0x8201C

typedef enum
{
      FBF_FREE = 100,  // �� ������ ���������� ������
      FBF_LEAVE        // ���������� ������ ������� �� ������������� �������
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
    UINT32                    ID;           // ? ������������� ������
    UINT32                    begin_idx;    // ��������� ������ ���������� ������ ���������
    UINT8                     count;        // ������� ��������� ������ ���������
} REQUEST_LIST_ITEMS_T;

/* ���������, ���������� �� ������, ������������ � �������� */
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

// �������� ������� event �� ������
EVENT_T* AFW_GetEv( EVENT_STACK_T *ev_st );

UINT32 AFW_GetEvSeqn( EVENT_STACK_T *ev_st );

// ������� ���������� ������ (� ��������� ��������� ������ ��� ������)
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
					  
// ��������� ����� � ������� ��� �������					  
UINT32 AFW_TranslateEvEvD( EVENT_STACK_T    *ev_st,
                           UINT16           event_code,
                           ADD_EVENT_DATA_T *data );						  

// ������ ��������� ����� �� ������
UINT32 APP_ConsumeEv( EVENT_STACK_T *ev_st,  void *app );

//  ������� Internal �����. . �������� �� ����� ����� ����� ����������
UINT32 AFW_CreateInternalQueuedEvAux( UINT32 event_code, 
									  FREE_BUF_FLAG_T  free_buf, 
									  UINT32 att_size, 
									  void* att_data ); 

UINT32 AFW_CreateInternalQueuedEvAuxD( UINT32 event_code, 
									   void *data, 
									   FREE_BUF_FLAG_T  free_buf, 
									   UINT32 size, 
									   void *aux_data);


// ��������� ����� EV_LIST_ITEMS (����� �� EV_REQUEST_LIST_ITEMS)
UINT32 APP_UtilAddEvUISListData( EVENT_STACK_T    *ev_st,
                                 void             *app,
                                 UINT32           param2, // = 0
                                 UINT32           begin_index,
                                 UINT8            count,
                                 FREE_BUF_FLAG_T  free_buf,
                                 UINT32           bufsize,
                                 void             *buffer );

// ��������� ����� EV_REQUEST_DATA ��� ���� � ������ ��������
UINT32 APP_UtilAddEvUISGetDataForItem( EVENT_STACK_T    *ev_st,
                                       void             *app,
                                       UINT32           index ); 

// ���������� �����
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
// ������� �� �������� ����� �����
UINT32 APP_UtilAddEvChangeListPosition ( EVENT_STACK_T    *p_evg,
										 void             *app,
                                         UINT32        	  position,
										 FREE_BUF_FLAG_T  free_buf,
										 UINT32           bufsize,
										 void             *buffer );						   
 
#endif
