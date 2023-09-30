#ifndef UIS_H
#define UIS_H

#include <apps.h>
#include <typedefs.h>
#include <stdarg.h>
#include <resources.h>

enum
{
    ACTION_OP_ADD = 0,
    ACTION_OP_DELETE,
    ACTION_OP_UPDATE
};

typedef struct
{
    UINT8             operation;        // Что делать с текущим пунктом (ACTION_OP_*)
    EVENT_CODE_T      event;            // Ивент, который создаётся при выборе этого действия
    RESOURCE_ID       action_res;       // Ресурс, описывающий действие (см. RES_ACTION_LIST_ITEM_T)

} ACTION_ENTRY_T;

typedef struct
{
    UINT8             count;            // Кол-во элементов
    ACTION_ENTRY_T    action[16];       // Список команд дл работы со списком действий

} ACTIONS_T;

typedef struct
{
    char             format[40];
    UINT32            count;
    UINT32            data[40];
} CONTENT_T;


typedef enum
{
	LIST_IMAGE_MEDIA_PATH_T = 0,
	LIST_IMAGE_RESOURCE_ID_T,
    LIST_IMAGE_DL_FS_MID_T, 			// именно это и юзает список яваприложений
	LIST_IMAGE_MEDIA_PATH_WITH_OFFSET_T,
	LIST_IMAGE_POINTER
} LIST_IMAGE_TYPE_T;


typedef struct				        
{
    WCHAR 	*file_name;		// имя файла
    UINT32   offset;	    // оффсет
    UINT32   size;	        // размер
    UINT8    mime_type;		// тип
} LIST_IMAGE_FILE_T ;

typedef struct
{
    void * file_name;		// указатель на картинку
    UINT32 image_size;		// размер
} LIST_IMAGE_POINTER_T;

typedef union
{
	LIST_IMAGE_FILE_T 	    image_file;	
	LIST_IMAGE_POINTER_T    image_ptr;  
    DL_FS_MID_T             file_id; 
	WCHAR          			*file_name;
	RESOURCE_ID             resource_id;
} LIST_IMAGE_T;

typedef struct
{
	LIST_IMAGE_T      	image;
	UINT16            	image_index;
	LIST_IMAGE_TYPE_T 	image_type;
} LIST_IMAGE_ELEMENT_T;


/**************************
 Вспомогательные функции
************************* */     
UINT32 UIS_SetLanguage(UINT8 LGID);

/* Создаёт контент */
UINT32 UIS_MakeContentFromString( char *format,  CONTENT_T *dst,  ... );
/* More information about the string format: 
    The line consists of pointers to incoming parameters 
    specifiers and alignment. 
    Pointers are the type and number of function parameters, 
    possible types: 
    q - a pointer to a string WCHAR 
    s - resource line 
    i - int 
    g - hex int 
    p - resource picture 
    r - CONTENT_T 
    f - pointer to the file 
    Specificators alignment: 
    N - new line 
    S - distribution of text 
    C - text in the center 
    L - with the crop "...", trimmed with a string is too long
    M -  text on middle (vertical alignment)
    T - text on top 
    B - text from the bottom
    H -
    F -
    E - 
    I -

    For example: "MCq0p1" = "Bring the middle and at the center first 
    dst option after a string, and the second - as a picture" */

/* Стандартный обработчик EV_GRANT_TOKEN */
UINT32 APP_HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app );

/* Стандартный обработчик EV_REVOKE_TOKEN */
UINT32 APP_HandleUITokenRevoked( EVENT_STACK_T *ev_st,  void *app );

/* То же, что APP_UtilChangeState, но при этом ещё будет извлечён(consumed) event */
UINT32 APP_UtilConsumeEvChangeState( UINT8 new_state,  EVENT_STACK_T *ev_st,  void *app );

/* Передаёт ивент диалогу */
UINT32 UIS_HandleEvent( UIS_DIALOG_T dialog,  EVENT_STACK_T *ev_st );

/* Вызвать полную перерисовку UI */
UINT32 UIS_Refresh( void );
UINT32 UIS_ForceRefresh( void );

/* Удаляет диалог и выставляет его в NULL */
UINT32 APP_UtilUISDialogDelete( UIS_DIALOG_T  *pdialog );
UINT32 UIS_Delete( UIS_DIALOG_T dialog );

// установить новый список действий
UINT32 UIS_SetActionList(UIS_DIALOG_T dialog, ACTIONS_T *action_list);     

// устанавливает ивент на джойстик в центр	  
UINT32 UIS_SetCenterSelectAction (UIS_DIALOG_T dialog, EVENT_CODE_T center_select_action);

#define DialogType_None 0x00 // none
#define DialogType_Dialling 0x05 //dial-up window 
#define DialogType_EnterName 0x08 //input names. . .
#define DialogType_SetDate 0x0B //Set Date 
#define DialogType_SetTime 0x0D //set-up time 
#define DialogType_Homescreen 0x0E //desktop standby 
#define DialogType_list 0x10 //Item List 
#define DialogType_SelectionList 0x13 //options
#define DialogType_BatteryandMemory 0x15 //power, memory usage information, such as window howed that the grid 
#define DialogType_JumpOut 0x17 //pop-up window, such as "keyboard is locked, unlocked by XX", "please wait" category 
#define DialogType_MessageBox 0x1B //information inbox
#define DialogType_GameAni 0x1D //games and applications when they enter the animation 
#define DialogType_WapMenu 0x1E //Wap browser-related menu 
#define DialogType_VideoCamera 0x25 //Camera viewfinder window 
#define DialogType_Password 0x26 //Password window 
#define DialogType_PB_SC_Other 0x28 //phone book, Shortcuts like, a lot of this state 
#define DialogType_Menu 0x2D //Main Menu 
#define DialogType_SecondLevelMenu 0x2E //2 of the three menu and the menu
#define DialogType_WriteText 0x32 //input text 
#define DialogType_Brightness 0x37 //Settings - brightness - highlighted regulation
#define DialogType_Picture 0x39 //Multimedia - picture

//Возвращает "тип" верхнего диалога. В приложениях прошивки часто используется для определения нахождения на рабочем столе.
// Если в res значение 0xE, значит на рабочем столе. Для обычного списка в res значение 0x10. Поэксперементируйте если вам это нужно.
UINT32 UIS_GetActiveDialogType(UINT8* res);

/***************************
Функции создания диалогов 
****************************/
		
enum
{
    NOTICE_TYPE_DEFAULT = 0,
    NOTICE_TYPE_OK,
    NOTICE_TYPE_FAIL,
    NOTICE_TYPE_WAIT
  /* На самом деле их больше, попробуйте перебрать    варианты  */
};


/* Создаёт окошко с сообщением. type = NOTICE_TYPE_* */
UIS_DIALOG_T UIS_CreateTransientNotice( SU_PORT_T *port,  CONTENT_T *content,  UINT8 type );

/* Создаёт диалог-запрос */
UIS_DIALOG_T UIS_CreateConfirmation( SU_PORT_T *port,  CONTENT_T *content );
  

/* Создаёт диалог-список */
UIS_DIALOG_T UIS_CreateList( SU_PORT_T       *port,
                             UINT32          param1,          // = 0  ID?
                             UINT32          count,           // Количество пунктов в списке
                             UINT32          marks,           // Что-то связанное с пометками
                             UINT32          *starting_num,   // Сколько пунктов требуется передать для инициализации
                             UINT8           param5,          // = 0
                             UINT8           param6,          // = 2
                             ACTIONS_T       *actions,        // Список действий
                             RESOURCE_ID     caption );       // Заголовок списка

typedef struct
{
    UINT16	            type;
    RESOURCE_ID	        label;
    BOOL	              readonly;
    BOOL	              show;
    BOOL	              unk5;
    UINT32	            maxlen;
    RESOURCE_ID	        resource;
    UINT32	            unk8;
} FIELD_DESCR_T;

typedef struct
{
    RESOURCE_ID       descr_res;    // Ресурс с FIELD_DESCR_T
    UINT32            data;         // Текущее значение (RESOURCE_ID/WCHAR*/CONTENT_T*/UINT32)
    BOOL              unk3;     
    BOOL              unk4;
    BOOL              unk5;         // = 0
} FIELD_T; 


typedef struct
{
    CONTENT_T           text;
    UINT32              unk1;
    RESOURCE_ID         marks[2];
    RESOURCE_ID         unk3[2];
    RESOURCE_ID         unk4[2];
    UINT8               unk5;
    UINT8               formatting; // = 1
} STATIC_LIST_ENTRY_T;  

typedef union
{
    STATIC_LIST_ENTRY_T     static_entry;     // Используется, когда editable == FALSE
    FIELD_T                 editable_entry;   // Используется, когда editable == TRUE
    UINT8                   pad[0xEC];
} LIST_CONTENT_T;


typedef struct
{
    BOOL                    editable;   // Возможно ли изменять содержимое
    UINT32                  unk1;       // ??
    LIST_CONTENT_T          content;
    
    UINT8                   pad[0xF8-0xEC];
} LIST_ENTRY_T;


/* Создаёт статичекий диалог-список */
UIS_DIALOG_T UIS_CreateStaticList( SU_PORT_T  *port,
                                   UINT32          param1,          // = 0 ID?
                                   UINT32          count,
                                   UINT32          marks,
                                   LIST_ENTRY_T    *entries, 
                                   UINT8           param5,          // = 0
                                   UINT8           param6,          // = 2
                                   ACTIONS_T       *actions,
                                   RESOURCE_ID     caption );
                                    
// Создает текстовой редактор
UIS_DIALOG_T UIS_CreateCharacterEditor( SU_PORT_T  *port,
                                        WCHAR           *text,
                                        UINT8           edit_type,
                                        UINT32          maxlen,
                                        BOOL            masked, // вводить текст в закрытом виде (звёздочками)
                                        ACTIONS_T       *actions, 
                                        RESOURCE_ID     dlgres ); 
/* 
   Edit_type Values: 
0 - Every Word From Big Letters 
1 - simple text 
2 - Each proposal. In large letters 
3 - the same for a very long lines 
4 - Telephone 
5 - ALL CAPS BIG 
6 - line as a sequence of numeric segments 
7 - a simple text without regime change 
8 - the same, without going into detail 
9 - as 4, with no extra menu 
10 - analog, without going into detail 
11 - as 0, with the possibility of entering an empty row 
12 - as 1, with the possibility of entering an empty row 
23 - url 
28 - with the line anymore 
29 - too, but you can and empty 
32 - only numbers
*/

// диалог для просмотра
UIS_DIALOG_T UIS_CreateViewer( SU_PORT_T  	   *port,
                               CONTENT_T       *contents,
                               ACTIONS_T       *actions );
// список с картинками и тестом                        
UIS_DIALOG_T UIS_CreatePictureAndTextList( SU_PORT_T  	   *port,
                                           UINT32          param1,
                                           UINT32          count, 
                                           UINT32          *starting_num, 
                                           UINT8           param4, // = 2
                                           UINT8           param5, // = 0
                                           UINT8           param6, // = 1, try 0,2,...
                                           ACTIONS_T       *actions, 
                                           RESOURCE_ID     dlgres );
// создаёт заставку 										   
UIS_DIALOG_T UIS_CreateSplashScreenFs( SU_PORT_T * port, WCHAR * uri, UINT32 unk );	// unk = 0x7D0
UIS_DIALOG_T UIS_CreateSplashScreen( SU_PORT_T * port, RESOURCE_ID img, UINT32 unk );	// unk = 0xD3 , 0x7D0

// Создаёт прогрессбар
UIS_DIALOG_T UIS_CreateProgressBar( SU_PORT_T *	port,
									RESOURCE_ID	res_img,	// 0x12000055
									CONTENT_T *	content,
									UINT8		u3,				// =0
									UINT8		perc,			// точно? я не уверен!	// &(app) + 0x4d
									UINT32		u6,				// =0
									WCHAR *		str,			// ?
									ACTIONS_T *	actions,
									RESOURCE_ID	caption );		// can be =0
#define EV_PROGRESS_UPDATE  0x213F
typedef struct {
	UINT32		u1;
	WCHAR *		str;
	UINT8		pad[0x5C];
} PROGRESS_UPDATE_DATA_T ;										   


/******************************************
Изменение элементов  рабочего стола 
******************************************/

#define BATTERY_STRENGTH_STATUS		0  //батарея: значения integer от 0 до 3
#define SIGNAL_STRENGTH_STATUS		3  //сигнал сети: значения integer от 0 до 5
#define AUDIO_STATUS 				0x69
#define GPRS_STATUS					0x1D // строка в поле даты
#define NETWORK_STATUS  			0x27 // строка в поле оператора 
#define READY_STATUS 				47
#define DATE_TIME_STATUS 		    55
#define PLAYING_SONG_STATUS 		   0x6A

typedef union
{
    INT32                            integer;
    BOOL                             boolean;
    WCHAR                            *quoted_string;
    UINT64                      	 unk;
} STATUS_VALUE_T;

// запись инфы на рабочий стол
UINT32 UIS_SetStatus(UINT8 status_id, STATUS_VALUE_T value);

// чтение инфы с рабочего стола
UINT32 UIS_GetCurrentStatusValue(UINT8 status_id,  STATUS_VALUE_T *value);


typedef struct
{
    WCHAR            *url;           
    UINT32           unk;    
} SETWALLPAPER_SETTINGS_T;  

// установить картинку на рабочий стол
UINT32 UIS_SetWallpaper(SETWALLPAPER_SETTINGS_T * );  

UIS_DIALOG_T uis_get_active_dialog( void );

UINT32 UIS_SetBacklight( UINT8 P1 );

#endif
