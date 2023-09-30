#ifndef UIS_H
#define UIS_H

#include <typedefs.h>
#include <resources.h>
#include <time_date.h>

enum
{
        ACTION_OP_ADD = 0,
        ACTION_OP_DELETE,
        ACTION_OP_UPDATE

};
typedef UINT8 ACTION_OP;


typedef struct
{
        ACTION_OP        operation;        // Что делать с текущим пунктом (ACTION_OP_*)
        EVENT_CODE_T     event;            // Ивент, который создаётся при выборе этого действия
        RESOURCE_ID      action_res;       // Ресурс, описывающий действие (см. RES_ACTION_LIST_ITEM_T)

} ACTION_ENTRY_T;


//Взято с SDK для LTE (не проверено)
typedef struct
{
        EVENT_CODE_T      event;            // Ивент, который создаётся при выборе этого действия
        RESOURCE_ID       action_res;       // Ресурс, описывающий действие (см. RES_ACTION_LIST_ITEM_T)

} CORE_ACTION_ENTRY_T;


#define UIS_MAX_NUM_ACTION_LIST_ITEMS 16
typedef struct
{
        UINT8                count;                                    // Кол-во элементов
        ACTION_ENTRY_T       action[UIS_MAX_NUM_ACTION_LIST_ITEMS];    // Список команд дл работы со списком действий

} ACTIONS_T;


//взято с SDK для LTE (не проверено)
typedef struct UIS_TAG_AND_INDEX_T
{
        INT32        index;
        INT32        tag;

} UIS_TAG_AND_INDEX_T;


//Взято с SDK для LTE (не проверено)
typedef union
{
        RESOURCE_ID                  name;
        CLK_TIME_T                   time;
        CLK_DATE_T                   date;
        UINT32                       number;
        WCHAR                        *string;
        UINT8                        status_ID;
        UIS_TAG_AND_INDEX_T          list_entry_id;
        struct CONTENT_T             *content;
//	DL_FS_MID_T		     file_id;

        UINT32                       set_fill_percent; // Progress Bar - fill percentage
        WCHAR                        *set_meter_value; // Progress Bar - Meter value string
        WCHAR                        *set_object_name; // Progress Bar - Object Name string

} UIS_VALUE_T; // 8 bytes


#define UIS_FORMAT_SIZE 40
#define UIS_MAX_NUM_VALUES 20

typedef struct CONTENT_T
{
        char               format[UIS_FORMAT_SIZE];
        UINT32             count;
        UIS_VALUE_T	   value[UIS_MAX_NUM_VALUES];

} CONTENT_T;	// 204 bytes

enum
{
        LIST_IMAGE_MEDIA_PATH_T = 0,
        LIST_IMAGE_RESOURCE_ID_T,
        LIST_IMAGE_DL_FS_MID_T,                         // именно это и юзает список яваприложений
        LIST_IMAGE_MEDIA_PATH_WITH_OFFSET_T,
        LIST_IMAGE_POINTER

};
typedef UINT8 LIST_IMAGE_TYPE_T;


//???????????????
typedef struct
{
        WCHAR        *file_name;       // имя файла
        UINT32       offset;           // оффсет
        UINT32       size;             // размер
        UINT8        mime_type;        // тип
	UINT8        pad[3];           // выравниваем к реальному размеру
} LIST_IMAGE_FILE_T;


typedef struct
{
        void        *file_name;        // указатель на картинку
        UINT32      image_size;        // размер

} LIST_IMAGE_POINTER_T;


typedef union
{
        LIST_IMAGE_FILE_T        image_file;
        LIST_IMAGE_POINTER_T     image_ptr;
        DL_FS_MID_T              file_id;
        WCHAR                    *file_name;
        RESOURCE_ID              resource_id;

} LIST_IMAGE_T;


//Проверено и работает
typedef struct
{
        LIST_IMAGE_T             image;		//+0x00, +++
        UINT16                   image_index;   //+0x10, +++
        LIST_IMAGE_TYPE_T        image_type;    //+0x12, +++
	UINT8                    pad;		//+0x13, +++ = 0

} LIST_IMAGE_ELEMENT_T;				//size = 0x14



typedef struct
{
        RESOURCE_ID        descr_res;              // 0x00 Ресурс с FIELD_DESCR_T (смотри resources.h)
        UIS_VALUE_T        value;                  // 0x04 Текущее значение (RESOURCE_ID/WCHAR*/CONTENT_T*/UINT32)
        BOOL               defind;                 // 0x0c Combobinary: blank/value
        BOOL               value_on;               // 0x0d Combobinary: только когда defind == TRUE, OFF/value
        BOOL               app_control_editing;    // 0x0e = 0

} FIELD_T, UIS_FIELD_T;

#define UIS_MAX_NUM_MARKS 2
typedef struct
{
        CONTENT_T          text;
        UINT32             number;                          //for numbered list only
        RESOURCE_ID        marks[UIS_MAX_NUM_MARKS];
        RESOURCE_ID        unk3[2];
        RESOURCE_ID        unk4[2];
//        UINT8              unk5;
	UINT8		   can_mark;   //???
        UINT8              formatting;                      // = 1

} STATIC_LIST_ENTRY_T;


typedef union
{
        STATIC_LIST_ENTRY_T        static_entry;     // Используется, когда editable == FALSE
        FIELD_T                    editable_entry;   // Используется, когда editable == TRUE
        UINT8                      pad[0xEC];

} LIST_CONTENT_T;


typedef struct
{
        BOOL                       editable;        // Возможно ли изменять содержимое
        INT32                      tag;             //
        LIST_CONTENT_T             content;         //

	UINT8                      pad1[4];
        UINT8                      unk;
        UINT8                      pad2[7];

} LIST_ENTRY_T;


typedef struct
{
	RESOURCE_ID stat;	//Статическая иконка
	RESOURCE_ID ani;	//Динамическая иконка
	RESOURCE_ID list;	//Иконка для списка
	RESOURCE_ID text;	//Название пункта

} MENU_ICON_T;


typedef struct
{
	UINT8	num;		//Кол-во пунктов
	UINT8	cursor;		//Выделенный пункт
	UINT16	unk;		//??? = 0

} MENU_ATTRIB_T;


/**************************
 Вспомогательные функции
************************* */

// Создаёт контент
UINT32 UIS_MakeContentFromString( char *format,  CONTENT_T *dst,  ... );
/*  Подробнее про строку format:
    Строка состоит из указателей на входящие параметры
    и спецификаторов выравнивания.
    Указатели формируются как тип и номер параметра в функции,

    возможные типы:
    a - анимация
    d - дата
    q - указатель на WCHAR строку
    s - ресурс строки
    i - int
    j - LIST_IMAGE_ELEMENT_T
    l - long
    g - hex int
    p - ресурс картинки
    r - CONTENT_T
    f - указатель на файл
    t - время

    Спецификаторы выравнивания:
    N - новая строка
    S - распределение строки
    C - текст по центру
    L - обрезать с "...", если строка слишком длинная
    M - текст посередине (вертикальное выравнивание)
    T - текст сверху
    B - текст снизу
    H -
    F -
    E -
    I -

    Например: "MCq0p1" = "Вывести посередине и по центру первый
    параметр после dst как строку, а второй - как картинку"
    Интересное задание форматной строки "p0 s1Nr2r3r4r5r6r7r8r9r:r;r<r=r>r?r@qAqB"  */

// Передаёт ивент диалогу
UINT32 UIS_HandleEvent(UIS_DIALOG_T dialog, EVENT_STACK_T *ev_st);

// Вызвать полную перерисовку UI
UINT32 UIS_Refresh (void);
UINT32 UIS_ForceRefresh (void);                //Отправляет ивент EV_UIS_FORCE_REFRESH

// Удаляет диалог и выставляет его в NULL
UINT32 APP_UtilUISDialogDelete (UIS_DIALOG_T *pdialog);
UINT32 UIS_Delete (UIS_DIALOG_T dialog);

// установить новый список действий
UINT32 UIS_SetActionList(UIS_DIALOG_T dialog, ACTIONS_T *action_list);

// устанавливает ивент на джойстик в центр
UINT32 UIS_SetCenterSelectAction (UIS_DIALOG_T dialog, EVENT_CODE_T center_select_action);

// устанавливает ивент на кнопку С
UINT32 UIS_SetClearKeyAction (UIS_DIALOG_T dialog, CORE_ACTION_ENTRY_T clear_action);


//This values from LTE
//#define DialogType_Dialling 0x05 //dial-up window
//#define DialogType_EnterName 0x08 //input names. . .
//#define DialogType_SetDate 0x0B //Set Date
//#define DialogType_SetTime 0x0D //set-up time
//#define DialogType_Homescreen 0x0E //desktop standby
//#define DialogType_list 0x10 //Item List
//#define DialogType_SelectionList 0x13 //options
//#define DialogType_BatteryandMemory 0x15 //power, memory usage information, such as window howed that the grid
//#define DialogType_JumpOut 0x17 //pop-up window, such as "keyboard is locked, unlocked by XX", "please wait" category
//#define DialogType_MessageBox 0x1B //information inbox
//#define DialogType_GameAni 0x1D //games and applications when they enter the animation
//#define DialogType_WapMenu 0x1E //Wap browser-related menu
//#define DialogType_VideoCamera 0x25 //Camera viewfinder window
//#define DialogType_Password 0x26 //Password window
//#define DialogType_PB_SC_Other 0x28 //phone book, Shortcuts like, a lot of this state
//#define DialogType_MainMenu 0x2E //Main Menu
//#define DialogType_SecondLevelMenu 0x2F //2 of the three menu and the menu
//#define DialogType_WriteText 0x32 //input text
//#define DialogType_Brightness 0x37 //Settings - brightness - highlighted regulation
//#define DialogType_Picture 0x39 //Multimedia - picture

//This values from Rainbow POG (RAZR V3x R252211LD_U_85.9B.E6P)
#define DialogType_None 0x00 /* Custom value. */
#define DialogType_NoActiveDialog 0x1
#define DialogType_Canvas 0x2
#define DialogType_ComboBinaryCharacterEditor 0x3
#define DialogType_ComboBinaryNumberEditor  0x4
#define DialogType_DialingEditor 0x5                        //dial-up window
#define DialogType_EmDialingEditor 0x6
#define DialogType_InCallDialingEditor 0x7
#define DialogType_CharacterEditor 0x8
#define DialogType_NumberEditor 0x9
#define DialogType_CleanupPicker 0xa
#define DialogType_DatePicker 0xb                        //Set Date
#define DialogType_DurationPicker 0xc
#define DialogType_TimePicker 0xd                        //Set Time
#define DialogType_Idle 0xe                                //Desktop standby
#define DialogType_Dynamic_Idle 0xf
#define DialogType_InCall 0x10                                //Incall dialog
#define DialogType_List 0x11                                //Item List
#define DialogType_Continuous List 0x12
#define DialogType_Form 0x13
#define DialogType_SelectionEditor 0x14
#define DialogType_TimeSortedList 0x15
#define DialogType_Meter 0x16
#define DialogType_MeterEditor 0x17
#define DialogType_Notice 0x18
#define DialogType_ExtendedNotice 0x19
#define DialogType_ProgressBar 0x1a
#define DialogType_ProgressBarEditor 0x1b
#define DialogType_SegmentedDataList 0x1c
#define DialogType_SpeedCallNumberEditor 0x1d
#define DialogType_SplashScreen 0x1e
#define DialogType_Viewer 0x1f
#define DialogType_Week_View 0x20
#define DialogType_NotesEditor 0x21
#define DialogType_Calculator 0x22
#define DialogType_FormattedEditor 0x23
#define DialogType_CompositeDialog 0x24
#define DialogType_ImageViewer 0x25
#define DialogType_MPlayer 0x26
#define DialogType_SecureNumberEditor 0x27
#define DialogType_SecureCharacterEditor 0x28
#define DialogType_SearchList 0x29
#define DialogType_PictureViewer 0x2a
#define DialogType_ScreenSaver 0x2b
#define DialogType_MonthView 0x2c
#define DialogType_VolumeMeterEditor 0x2d
#define DialogType_IconicMenu 0x2e
#define DialogType_IconicApplicationPickerTab 0x2f
#define DialogType_NullDialog 0x30
#define DialogType_SecureFormattedEditor 0x31
#define DialogType_MmsPicker 0x32
#define DialogType_MixedContentEditor 0x33
#define DialogType_MixedContentViewer 0x34
#define DialogType_MixedContentList 0x35
#define DialogType_SplitScreenEditor 0x36
#define DialogType_ListMultipleSelection 0x37
#define DialogType_MediaSlider 0x38
#define DialogType_XMLDialog 0x39
#define DialogType_TwoWayVideoTelephonyDialog 0x3c
#define DialogType_MiniTransient 0x3d
#define DialogType_ListPictureAndTextList 0x3e
#define DialogType_ListGenericList 0x3f
#define DialogType_ListEnhancedSearchList 0x40
#define DialogType_EnhancedSearchEditor 0x41
#define DialogType_EnhancedTextViewer 0x42
#define DialogType_NestedMenu 0x45
#define DialogType_none 0xff

// Возвращает "тип" верхнего диалога. В приложениях прошивки часто используется для определения нахождения на рабочем столе.
// Если в res значение 0xE, значит на рабочем столе. Поэксперементируйте если вам это нужно.
UINT32 UIS_GetActiveDialogType(UINT8* res);


/***************************
Функции создания диалогов
****************************/


//Создаёт NullDialog (DialogType_NullDialog)
UIS_DIALOG_T UIS_CreateNullDialog (SU_PORT_T *port);


// Типы для Trancient Notice
enum
{
        NOTICE_TYPE_DEFAULT = 0,                        // Значек "внимание"
        NOTICE_TYPE_OK,                                 // Галочка
        NOTICE_TYPE_FAIL,                               // Крестик
        NOTICE_TYPE_WAIT,
        NOTICE_TYPE_WAIT_NO_KEY = NOTICE_TYPE_WAIT,     // кнопки блокируются, к контенту в конце добавляются точки, приложение должно само както вывести из стэйта
                                                        // кнопками можно вывести только вызвав метку (при этом закроется приложение)
        NOTICE_TYPE_WAIT_2,                             // точек нет, при нажатии на любую кнопку генерируется ивент EV_DIALOG_DONE
        NOTICE_TYPE_INFO_NO_KEY,                        // на кнопки не реагирует
        NOTICE_TYPE_INFO,                               //
        NOTICE_TYPE_OK_2,                               //

        NOTICE_TYPE_DELETED = 12,
        NOTICE_TYPE_NEW_MESSAGE,
        NOTICE_TYPE_SMS,
        NOTICE_TYPE_SEND,
        NOTICE_TYPE_SEND_CENTER,                        // на скине mpx_ultra криво выглядет!

        NOTICE_TYPE_WAIT_3 = 27,                        // с точками, кнопки не блокируются
        NOTICE_TYPE_WAIT_4,                             // текст слева, с точками, при нажатии красной генерируется EV_DIALOG_DONE
        NOTICE_TYPE_INFO_LONG,
        NOTICE_TYPE_WAIT_FAST,                          // диалог ожидания на пол секунды

        NOTICE_TYPE_WAIT_5 = 32,                        // также как NOTICE_TYPE_WAIT_2

        NOTICE_TYPE_WAIT_6 = 37,                        // центр, с точками, при нажатии любой кнопки генерируется EV_DIALOG_DONE

        NOTICE_TYPE_CHAT = 45,
        NOTICE_TYPE_NEW_CHAT_MESSAGE

        // дальше ничего интересного вроде
};
typedef UINT8 NOTICE_TYPE_T;

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


// Создаёт статичекий диалог-список
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
  Значения edit_type:
0 - Каждое Слово С Большой Буквы
1 - простой текст
2 - Каждое предложение. С большой буквы
3 - аналогично, для очень длинных строк
4 - номер телефона
5 - ВСЕ БУКВЫ БОЛЬШИЕ
6 - строка как последовательность числовых сегментов
7 - простой текст без смены режимов
8 - аналогично, без детализации
9 - как 4, без доп меню
10 - аналог, без детализации
11 - как 0, с возможностью ввода пустой строки
12 - как 1, с возможностью ввода пустой строки
23 - url
28 - строка с аттачами
29 - тоже, но можно и пусто
32 - только числа
*/

// диалог для просмотра
UIS_DIALOG_T UIS_CreateViewer( SU_PORT_T             *port,
                               CONTENT_T       *contents,
                               ACTIONS_T       *actions );
// список с картинками и тестом
UIS_DIALOG_T UIS_CreatePictureAndTextList( SU_PORT_T             *port,
                                           UINT32          param1,
                                           UINT32          count,
                                           UINT32          *starting_num,
                                           UINT8           param4, // = 2
                                           UINT8           param5, // = 0
                                           UINT8           param6, // = 1, try 0,2,...
                                           ACTIONS_T       *actions,
                                           RESOURCE_ID     dlgres );

// Создать диалог-заставку из файла (картинки)
UIS_DIALOG_T UIS_CreateSplashScreenFs( SU_PORT_T * port, WCHAR * uri, UINT32 unk );        // unk = 0x7D0

// Создает диалог-заставку из ресурса картинки
UIS_DIALOG_T UIS_CreateSplashScreen( SU_PORT_T * port, RESOURCE_ID img, UINT32 unk );        // unk = 0xD3 , 0x7D0

//Создает диалог "хранитель экрана" из картинки. Поддерживаются: GIF, JPG, PNG, BMP, и др.
UIS_DIALOG_T UIS_CreateScreenSaver( SU_PORT_T * port, WCHAR * uri );

//Создает диалог "хранитель экрана" из картинки. Поддерживаются: GIF, JPG, PNG, BMP, и др.
UIS_DIALOG_T UIS_CreateScreenSaverPicAlbum( SU_PORT_T * port, WCHAR * uri );

//Главное меню
UIS_DIALOG_T UIS_CreateIconicMenu (SU_PORT_T *port, ACTIONS_T *actions, MENU_ICON_T *icons, MENU_ATTRIB_T attr, RESOURCE_ID dlgres);

//Подменю
UIS_DIALOG_T UIS_CreateIconicAppPickerTab (SU_PORT_T *port, ACTIONS_T *actions, MENU_ICON_T *icons, MENU_ATTRIB_T attr, RESOURCE_ID dlgres);

// Создаёт прогрессбар
UIS_DIALOG_T UIS_CreateProgressBar( SU_PORT_T *        port,
                                                                        RESOURCE_ID        res_img,        // 0x12000055
                                                                        CONTENT_T *        content,
                                                                        UINT8                u3,                                // =0
                                                                        UINT8                perc,                        // точно? я не уверен!        // &(app) + 0x4d
                                                                        UINT32                u6,                                // =0
                                                                        WCHAR *                str,                        // ?
                                                                        ACTIONS_T *        actions,
                                                                        RESOURCE_ID        caption );                // can be =0
#define EV_PROGRESS_UPDATE  0x213F
typedef struct
{
        UINT32                u1;
        WCHAR *                str;
        UINT8                pad[0x5C];
} PROGRESS_UPDATE_DATA_T ;


/******************************************
Изменение элементов  рабочего стола
******************************************/

#define BATTERY_STRENGTH_STATUS     0x00 // батарея: значения integer от 0 до 3
#define SIGNAL_STRENGTH_STATUS      0x03 // сигнал сети: значения integer от 0 до 5
#define AUDIO_STATUS                0x69
#define GPRS_STATUS                 0x1D
#define NETWORK_STATUS              0x27 // строка в поле оператора
#define READY_STATUS                47
#define DATE_STATUS                 0x37 //строка в поле даты +++
#define PLAYING_SONG_STATUS         0x6A // Имя
#define ITUNES_STATE_STATUS         0x6A //

typedef union
{
        INT32        integer;
        BOOL         boolean;
        WCHAR        *quoted_string;
        UINT64       unk;
} STATUS_VALUE_T;

// запись инфы на рабочий стол
UINT32 UIS_SetStatus(UINT8 status_id, STATUS_VALUE_T value);

// чтение инфы с рабочего стола
UINT32 UIS_GetCurrentStatusValue(UINT8 status_id,  STATUS_VALUE_T *value);
enum
{
        WALLPAPER_LAYOUT_CURRENT = 0,
        WALLPAPER_LAYOUT_CENTERED,
        WALLPAPER_LAYOUT_TILED,
        WALLPAPER_LAYOUT_FIT_TO_SCREEN
};
typedef UINT8 WALLPAPER_LAYOUT_T;

typedef struct
{
    WCHAR              *url;
    WALLPAPER_LAYOUT_T layout;
} SETWALLPAPER_SETTINGS_T;

// установить картинку на рабочий стол
UINT32 UIS_SetWallpaper(SETWALLPAPER_SETTINGS_T * );

UIS_DIALOG_T uis_get_active_dialog( void );
void * uis_get_dialog_ptr(UIS_DIALOG_T dialog);

UINT32 UIS_SetBacklight( UINT8 P1 );

//Функции для работы с ITAP
UINT32 UIS_ITAPOpen (void);
UINT32 UIS_ITAPMasterClear (void);
UINT32 UIS_ITAPClose (void);

//Выбрать язык
UINT32 UIS_SetLanguage(UINT8 LGID);

//Функции формируют строки с датой или временем в соответствии с форматом вывода в телефоне (то что мы видим на рабочем столе)
UINT32 UIS_FormatDate (CLK_DATE_T date, WCHAR *output_string);
UINT32 UIS_FormatTime (CLK_TIME_T time, WCHAR *output_string);


#endif
