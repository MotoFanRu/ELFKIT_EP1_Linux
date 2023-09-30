#ifndef SDK_DL_H
#define SDK_DL_H

#include <typedefs.h>

#define KEY_0       	0
#define KEY_1       	1
#define KEY_2       	2
#define KEY_3       	3
#define KEY_4       	4
#define KEY_5       	5
#define KEY_6       	6
#define KEY_7       	7
#define KEY_8       	8
#define KEY_9       	9
#define KEY_STAR    	10
#define KEY_POUND   	11

#define KEY_RED     	17
#define KEY_GREEN   	18

#define KEY_LSOFT   	14 
#define KEY_RSOFT   	15 
#define KEY_SOFT_LEFT   14
#define KEY_SOFT_RIGHT  15
#define KEY_MENU    	20

#define KEY_SMART   	21

#define KEY_VOLUP   	23
#define KEY_VOLDOWN 	24

#define KEY_UP      	44
#define KEY_DOWN    	45
#define KEY_LEFT    	46
#define KEY_RIGHT   	47
#define KEY_CENTER  	61 

#define KEY_CAM    		0x3F

#define KEY_VOICE   	0x2B
#define KEY_HANDSFREE   0x33
#define KEY_ITUNES 		0x42


/******************************
   —вет 
*******************************/

/* Turn off display */
void DAL_DisableDisplay( UINT32 p1 ); // p1 = 0

/* Turn on display */
void DAL_EnableDisplay( UINT32 p1 ); // p1 = 0

UINT8 DAL_GetDisplayState( void );

void DL_KeyUpdateKeypadBacklight( UINT8 P1 ); // P1 = 1 lighting; P2 = 0

UINT32 UIS_SetBacklight( UINT8 P1 );

// –егулировка €ркости диспле€
UINT32 UIS_SetBacklightWithIntensity( UINT8 color, // = 255 
									  UINT8 intensity // = 0...6
									); 



/******************************
   DbFeature
******************************/

#define ID_IMEI                        0x9CE
#define ID_CLOCK                       0x6FF //0 Dig, 1 an, 2 off
#define ID_DATE                        0x7C3 //0 off, 1 on
#define ID_DATE_DISP                   0x6FE //0 left, 1 cent
#define ID_MAINMENU_STLYE              0x6A0 //0 icons, 1 list
#define ID_BKG_STYLE                   0x678 //0 cent, 1 mul, 2 ad
#define ID_SCREENSAVER                 0x69C //0 off, ....
#define ID_PHOTO_RESOLUTION            0x6C5 //0, 1, 2, 3
#define ID_PHOTO_QUALITY               0x75B //0, 1, 2
#define ID_VIDEO_LEN                   0x692 //0, 1, 2
#define ID_VIDEO_QUALITY               0x691 //0, 1, 2

// read from 4a
UINT8 DL_DbFeatureGetCurrentState( UINT16 fstate_ID, UINT8 *state );
// write in 4a
UINT8 DL_DbFeatureStoreState(UINT16 fstate_ID, UINT8 state);

UINT8 DL_DbFeatureGetValueString(UINT32 feature_id, WCHAR *feature_string );

/*************************
  seem
*************************/

typedef struct {
  UINT16   seem_element_id;
  UINT16   seem_record_number;
  UINT32   seem_offset;
  UINT32   seem_size;
} SEEM_ELEMENT_DATA_CONTROL_T; 


// чтение из сима
UINT16   SEEM_ELEMENT_DATA_read 	( 	SEEM_ELEMENT_DATA_CONTROL_T  *data_ctrl_ptr,
										UINT8  *data_buf,
										BOOL read_zero_byte_allowed
									);
// *data_ctrl_ptr - a pointer to a data structure containing information on request
// *data_buf - data_buf - a pointer to the buffer where the data prochitayutsya

// read_zero_byte_allowed - if true, it is possible to read the small seem with a length less than 255												
						
						
//  record in a seem													
UINT16  SEEM_ELEMENT_DATA_write	(  	SEEM_ELEMENT_DATA_CONTROL_T  *data_ctrl_ptr,
									UINT8  *seem_data_ptr
								);	
// *seem_data_ptr - a pointer to the buffer, which are stored recording data													

UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_read( UINT32 seem,  UINT32 record,  void* seem_data,  UINT32 count );

#define SEEM_WRITE_METHOD_ADD				0
#define SEEM_WRITE_METHOD_UPDATE			1
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_write( UINT32 method,  UINT32 seem,  UINT32 record,  void* seem_data,  UINT32 count );
													
													
/****************************
  Power mngmnt 
*****************************/													

/* Function shutdown */
void pu_main_powerdown(UINT32 r0);


/****************************
  Volume 
****************************/

enum // volume_type
{
    BASE = 0,	// громкость звонка

    PHONE, // громкость клавиатуры
    VOICE, // громкость мультимеди€  LTE
    MICROPHONE, // громкость разговора
    MULTIMEDIA, //  громкость мультимеди€ LTE2 и V3i
    PTT_TONES,
    MUTABLE_MAX,
    IMMUTABLE_MAX,
    MAX = IMMUTABLE_MAX
};

// Sets volume
void DL_AudSetVolumeSetting(UINT8 volume_type, UINT8 volume);
// Gets volume
void DL_AudGetVolumeSetting(UINT8 volume_type, UINT8 *volume);


/****************************
  Calls
****************************/

#define MAX_CALLS                   7

typedef struct
{       
    UINT16            call_id;  
    UINT8             call_state;
} CALL_ID_T;

typedef struct
{
    UINT8  		number_of_calls;
    UINT8 		overall_call_state;
    CALL_ID_T 	call_state_info[MAX_CALLS];
} CALL_STATES_T;


// if number_of_calls == 0, then no calls
void DL_SigCallGetCallStates(CALL_STATES_T *call_states);

// TRUE - call is voice call
BOOL APP_MMC_Util_IsVoiceCall(void); // if it is FALSE, no calls

// TRUE - call is video call
BOOL APP_MMC_Util_IsVideoCall(void); // if it is FALSE, no calls

/*******************************
  ќтправка сообщений 
*******************************/

typedef struct
{
	WCHAR  address[51];
	UINT8  addr_type; // 0 -SMS, 1 - EMAIL, 2 - No, 3 - Long Msg ??? 
	WCHAR  contents[512]; // максимальный размер 0x7BFE, только дл€ SMS, думаю, это много
} SEND_TEXT_MESSAGE_T;

UINT32 DL_SigMsgSendTextMsgReq(IFACE_DATA_T *port, SEND_TEXT_MESSAGE_T *msg_ptr);

/********************************
  ћетки
*********************************/
// непроверено !

#define SC_TYPE_URL		5
#define SC_TYPE_APP		6

typedef UINT8 SC_TYPE_T;

typedef struct
{
    UINT8            	seem_rec;    // seem_rec-1
    UINT8				type;        // тип метки 
    UINT8            	index;       // номер в списке, отсчет от 1. Ќомер метки
    UINT8            	unk1;        // 0xFE
    UINT32            	ev_code;
    UINT32            	list_index;	 // id выделенного пункта списка
    UINT32            	param1;       
	UINT32            	list2_index; // тоже какой-то id пункта
    UINT32            	param2;       
    UINT32            	data;	     // используетс€ дл€ указател€ на данные, например указатель на URL-адрес 
    UINT32		        lang_text;   // RESOURCE_ID
    WCHAR            	text[17];	 // им€ метки
    UINT16            	unk2;        // 0xffbd
    
} SEEM_0002_T;


// создание метки									
UINT8 DL_DbShortcutCreateRecord( IFACE_DATA_T  *data, SEEM_0002_T  record ); //10726BE4

// обновление метки
UINT8 DL_DbShortcutUpdateRecord( IFACE_DATA_T  *data, SEEM_0002_T  record ); //10726B4A

// удаление метки
UINT8 DL_DbShortcutDeleteRecord( IFACE_DATA_T  *data, UINT8 seem_rec); //10726B22

// провер€ет  recordId,   0 - no error
UINT8 DL_DbShortcutGetRecordByRecordId( IFACE_DATA_T  *data, UINT8  seem_rec); //10726ACC

// получает номер доступной записи
UINT16 DL_DbShortcutGetNumOfRecordsAvailable( void ); //10726AF4

// получает номер используемой записи
UINT16 DL_DbShortcutGetNumOfRecordsUsed( BOOL voice_shortcut ); //10726AF8

// получает первый доступный номер
UINT8 DL_DbGetFirstAvailableNumber( void ); //10726B1E

// получает номер используемой  записи URL
UINT16 DL_DbShortcutGetNumOfURLRecordsUsed( void ); //10726C70

// провер€ет URLId,   0 - no error
UINT32 DL_DbShortcutGetURLByURLId( IFACE_DATA_T *data, UINT32 URLId); //10726C74

// получение типа записи
SC_TYPE_T DL_DbShortcutGetshortcutType( UINT8 index ); //10726CCA

// получает номер доступной записи URL
UINT16 DL_DbShortcutGetNumOfURLRecordsAvailable( void ); //10726CD2


/********************************
  ѕрочее 
********************************/

// воспроизведение тона
UINT32 DL_AudPlayTone( UINT32 tone,  UINT8 volume ); //Current volume = 0xFF

// эмул€ци€ клавиш
void DL_KeyInjectKeyPress( UINT8 key_code, 
						   UINT8 states, // 0 - Press; 1 - Release
						   UINT8 p3 // = 0
						  );

// сила сигнала сети
typedef struct
{
    UINT8 	percent;
    INT8    dbm;
} SIGNAL_STRENGTH_T;

void DL_SigRegQuerySignalStrength(SIGNAL_STRENGTH_T *signal_strength);


// Cell Id
void DL_SigRegGetCellID(UINT16 *cell_id);

UINT8 DL_PwrGetActiveBatteryPercent( void );	// бесполезна€ ф-ци€

enum
{
    CHARGING_MODE_NONE = 0,
    CHARGING_MODE_IN_PROGRESS = 1,
    CHARGING_MODE_CHARGING_COMPLETE
};

UINT8 DL_PwrGetChargingMode( void );


#endif

