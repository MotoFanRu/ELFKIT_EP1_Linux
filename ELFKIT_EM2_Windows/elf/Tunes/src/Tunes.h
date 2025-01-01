#define DEBUG

#ifndef TUNES_H
#define TUNES_H


#define  VER_MAJOR         4
#define  VER_MINOR         0


#include <apps.h>
#include <canvas.h>
#include <dl.h>
#include <dl_keypad.h>
#include <java.h>
#include <loader.h>
#include <mme.h>
#include <filesystem.h>
#include <utilities.h>
#include <uis.h>
#include <z.h>


#define TAG_SIZE            26 // ���-�� �������� ��� ����
#define COUNT_TIMERS        14 // ���-�� ��������
#define LIST_ITEMS_COUNT    12 // ���-�� ������� � ����� �� ���� �����


/******* PLAYER ******/

typedef struct
{
    UINT8 Auto_next; // ���� ������������ �����
	UINT8 Play_call; // ������������� ����� ������
	UINT8 Play_rnd; // ��������: 0 - ����, 1 - ���
    UINT8 Play_rep; // ������: 0 - ���, 1 - ����, 2 - ���

    UINT8 Show_name; // ��� �����: 1 - ����������, 0 - �� ����������
    UINT8 Show_time; // �����: 1 - ����������, 0 - �� ����������
    UINT8 Lock_kb;   // ������������ ���������� ����������: 0 - ���, 1 - ��  
    UINT8 Read_tags;    // 1 - ������ ����; 0 - �� ������

    UINT8 Fading_vol; // ��������� ��������� � �����
    UINT8 Groth_vol; // ���������� ��������� � ������
    UINT8 Queue_save; // ��������� �������, 0 -���, 1-��, 2-���� ����� ������
    UINT8 Active_display; // ����. ���������� �������

    WCHAR Skin_name[32]; // ��� �����
    UINT32 ev_code;
    
} SETTING_T;// ���������


typedef enum
{
    DELETE,
    PLAY,
    PAUSE,
    STOP,
    REWIND
} STATES_PLAYER_T; //���������  ��������� ������


typedef struct
{
    WCHAR author[TAG_SIZE];
    WCHAR title[TAG_SIZE];
    WCHAR album[TAG_SIZE];
} TAGS_T;

typedef enum {
    VOLUME_MUTE,
    VOLUME_INCREMENT,
    VOLUME_DECREMENT
} VOLUME_MODE_T;

typedef enum {
    ADD_NONE, // ������ ��� ���������� ������
    ADD_QUEUE, // � �������
    ADD_ONE_TRACK   // ��������� ���� � ���������
}TYPE_ADD_TUNESLIST_T;

typedef struct {
    WCHAR uri[256];
} URI_LIST_T;

static void upd_id_file_queue(void);
static void load_file_queue(void);
static void save_file_queue(void);
static UINT32 add_in_tunes_list(UINT64 *tunes, UINT32 new_size, UINT8 type);
static UINT32 del_item_tunes_list(UINT32 item);
static UINT32 rands(UINT64 *tunes, UINT32 new_size);

void My_APP_HandleEvent( EVENT_STACK_T *ev_st, APPLICATION_T *app, APP_ID_T appid, REG_ID_T regid );

static UINT32 Show( EVENT_STACK_T *ev_st,  APPLICATION_T *app, BOOL show );
static UINT32 appHide( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 appShow( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 appinit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 player_start( EVENT_STACK_T *ev_st, APPLICATION_T *app );
static UINT32 player_stop( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 player_HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 player_HandleKeyrelease( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 StartTimer( UINT32 period, UINT32 id, UINT8 type, APPLICATION_T *app );
static UINT32 StopTimer( UINT32 id, APPLICATION_T *app );
static UINT32 player_Timer( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 Play( EVENT_STACK_T *ev_st,  APPLICATION_T *app ); // ��������������� ��� ������� ��������
static UINT32 Stop(EVENT_STACK_T *ev_st,  APPLICATION_T *app); // ����
static UINT32 Pause(EVENT_STACK_T *ev_st,  APPLICATION_T *app); // �����
static UINT32 PlayComplete( EVENT_STACK_T *ev_st,  APPLICATION_T *app ); //  ��������� ���������������
static UINT32 Back( EVENT_STACK_T *ev_st,  APPLICATION_T *app ); // ����������
static UINT32 Next( EVENT_STACK_T *ev_st,  APPLICATION_T *app ); // ���������
static UINT32 Create( EVENT_STACK_T *ev_st,  APPLICATION_T *app ); // ��������
static UINT32 Delete(EVENT_STACK_T *ev_st,  APPLICATION_T *app); // ��������
static UINT32 Seek(APPLICATION_T *app, UINT32 seektime);
static UINT32 SeekPlay(EVENT_STACK_T *ev_st,  APPLICATION_T *app); // ��������������� ����� seek

static UINT32 setVolumeMode(APPLICATION_T *app, UINT8 type);
static UINT8 getVolume(void);
static void setVolume(UINT8 volume);

static UINT32 ReadTags(MME_GC_MEDIA_FILE mme_media_file); // ������ �����

static UINT32 PlayFile(WCHAR* filename, UINT8 type);
static UINT32 PlayFolder(WCHAR* foldername, UINT8 type);
static UINT32 PlayTPL(WCHAR* tplname, INT32 index, UINT8 type);
static UINT32 PlayMVPL(WCHAR* tplname, INT32 index, UINT8 type);

static UINT32 ReadPlayList(void);
static UINT32 DelItemPlayList(UINT32 item);


static UINT32 ReadSetting(void);
static UINT32 SaveSetting(void); 

static UINT32 Blink(UINT8 type, BOOL on);


/******* SHELL ******/
#define DRMRES_FOLDER		0x120000CE
#define DRMRES_FILE_AUDIO	0x120000CD
#define DRMRES_SELECTED		0x120000CC
#define DRMRES_NEXT		    0x12000052

typedef struct
{
	WCHAR		name[FS_MAX_URI_NAME_LENGTH+1]; // ��� 
	WCHAR*		ext; 	  // ����������
	UINT16		attrib;   // ���������
	BOOL        selected; 
} FILEINFO;



/* ��������� ���������� */
typedef enum { 
    SHELL_STATE_ANY, /* ANY-state ������ ������ */
    SHELL_STATE_INIT,

    SHELL_STATE_MAIN, // ������� ����
    SHELL_STATE_PLAYLISTS, // ���������
    SHELL_STATE_PLAYLIST, // ���������� ���������
    SHELL_STATE_FBROWSERPLAYER, // ���� ������� ��� ��������������� ������
    SHELL_STATE_FBROWSERPLAYLIST, // ���� ������� ��� �������� ����������
    SHELL_STATE_SKINS,   // �����
	SHELL_STATE_SETTING, // ���������
    SHELL_STATE_EDIT,
    SHELL_STATE_CANVAS, // �����
    SHELL_STATE_INFO, // ����������
    SHELL_STATE_QUEUE,
    SHELL_STATE_BACKGROUND, // ���

    SHELL_STATE_MAX /* ��� �������� */
} SHELL_STATES_T;

// ���������
typedef enum {
    SETTING_AUTO_NEXT=1,
    SETTING_PLAY_CALL,
    SETTING_PLAY_RND,
    SETTING_PLAY_REP,
    SETTING_SHOW_NAME,
    SETTING_SHOW_TIME,
    SETTING_LOCK_KB,
    SETTING_READ_TAGS,
    SETTING_FADING_VOL,
    SETTING_GROTH_VOL,
    SETTING_QUEUE_SAVE,
    SETTING_ACTIVE_DISPLAY,

    SETTING_COUNT_ITEMS=SETTING_ACTIVE_DISPLAY

} SETTING_ITEMS_T;


typedef struct
{ 
    APPLICATION_T           apt; /* ������ ����������� ��������������, ������ ������ */
    BOOL                    isShow; // �������� �� ���
    BOOL                    startPlayer; // ������� �� �����
    UINT8                   num_state; // ����� ��������� � �������
    UINT8                   prev_state[SHELL_STATE_MAX]; // ���������� state
    UINT32                  prev_item[SHELL_STATE_MAX]; // ���������� item
    UINT32                  item; // ���������� �����
    UINT32                  timer_handle[COUNT_TIMERS];
    MME_GC_MEDIA_FILE       mme_media_file; // id ��������� �����
    BOOL                    showVolume; // ����� ����������� ���������
    UINT8                   volume; // ������� ��������� ��� ����������/���������
    UINT8                   vol_mute; // ������� ��������� �� mute
    BOOL                    LongPress; // ������� �������
    WCHAR                   Operator[32]; // �������� 
    
       
} APP_SHELL_T;

typedef enum
{
	RES_LIST_CAPTION=0,
    RES_EDIT_CAPTION,
    RES_ACTION0_CAPTION,
    RES_ACTION1_CAPTION,
    RES_ACTION6_CAPTION,
    RES_ACTION9_CAPTION,
    RES_ACTION0, // �������� � �������
	RES_ACTION1, // �������
	RES_ACTION2, // �������
    RES_ACTION3, // ��������
    RES_ACTION4, // ����� 
    RES_ACTION5, // �������������
    RES_ACTION6, // ������������ ����������
    RES_ACTION7, // ������� ��
    RES_ACTION8, // Bluetooth
    RES_ACTION9, // ����������
	
	RES_MAX
} SHELL_RESOURCE;



/* ������ ���� �������� ���������� */
RESOURCE_ID Resources[RES_MAX];

UINT32 HandsFree( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 DialFinish( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 shell_startApp( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
static UINT32 shell_destroyApp( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 UpdateList( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 siItem );
static UINT32 ChangeListPosition( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 sItem );

static UINT32  ChangeState( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT8 new_state);
static UINT32  PrevState( EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32  StateMinus2( EVENT_STACK_T *ev_st,  APPLICATION_T *app);

static UINT32 shell_MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );

static UINT32 shell_PlaylistsStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_PlaylistsStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

static UINT32 shell_PlaylistStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_PlaylistStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

static UINT32 shell_FbrowserplayerStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_FbrowserplayerStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

static UINT32 shell_FbrowserplaylistStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_FbrowserplaylistStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

static UINT32 shell_SkinsStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_SkinsStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

static UINT32 shell_SettingStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );

static UINT32 shell_EditStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_EditOk( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 shell_EditData( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 shell_CanvasStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
static UINT32 shell_CanvasStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );
UINT32 focus( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 nofocus( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 shell_InfoStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );

static UINT32 shell_QueueStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );

static UINT32 shell_StateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

static UINT32 shell_HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 shell_HandleListReq( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 shell_SelectItem_Main(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Playlists(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Playlist(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Fbrowserplayer(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Fbrowserplaylist(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Skins(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Setting(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
static UINT32 shell_SelectItem_Queue(EVENT_STACK_T *ev_st,  APPLICATION_T *app);


static UINT32 shell_SendListItems_Main( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
static UINT32 shell_SendListItems_Playlists( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
static UINT32 shell_SendListItems_Playlist( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
static UINT32 shell_SendListItems_Fbrowser( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
static UINT32 shell_SendListItems_Skins( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
static UINT32 shell_SendListItems_Setting( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
static UINT32 shell_SendListItems_Queue( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);

static UINT32 shell_HandleListReq( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 Navigate (EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 NewList_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 Delete_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 DeleteAll_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 AutoCreatePL_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 Bluetooth_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 AddInQueue_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) ;
static UINT32 SelectItemFB_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 SelectItemPL_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 Randomize_Action( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 InitResources( void );
static UINT32 RemoveResources( void );
static UINT32 canvas_HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 canvas_HandleKeyrelease( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 FindFile( WCHAR *folder, WCHAR *filtr ); // ����� ������
static UINT32 FindPlaylists( WCHAR *folder );

static UINT32 AddQueuePL(void);
static UINT32 AddFilePL(WCHAR* filename);
static UINT32 AddFolderPL(WCHAR* foldername);

static UINT32 paint_stateicons( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 paint_progressbar( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 paint_volumebar( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 paint( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
static UINT32 paint_clear_time( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 repaint( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 ActiveDisplay( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

static UINT32 sec2min(UINT32 seconds, WCHAR *str);
static UINT32 ShowiTunesState(void);
static UINT32 ShowName(void);
static UINT32 ShowTime(void);
static UINT32 ShowVolume(void);
static UINT32 ShowStr(WCHAR * string, UINT8 status); // ����� ������ �� ������� ����

#endif
