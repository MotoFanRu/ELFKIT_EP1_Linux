#define DEBUG

#ifndef APP_H
#define APP_H

#include <dl.h>
#include <apps.h>
#include <java.h>
#include <loader.h>
#include <bluetooth.h>
#include <filesystem.h>
#include <resources.h>
#include <utilities.h>

#define kJavaDir L"mobile/kjava/installed/"

const char app_name[APP_NAME_LEN] = "JavaTransfer";

typedef enum
{
    HW_STATE_ANY,
    HW_STATE_INIT,
    HW_STATE_MAIN,
    HW_STATE_MAX
} HW_STATES_T;

typedef struct
{
    BOOL selected;
    WCHAR midlet_name[ MIDLET_NAME_LEN + 1 ];   //��� ��� ����������
    UINT32 midlet_index;                        //������ ����������
    DL_FS_MID_T midlet_app_id;                  //���������� ������������� ��� ����������
    DL_FS_MID_T midlet_icon_id;                 //���������� ������������� ������ ��� ����������
} MIDLET;

typedef enum
{
	RES_LIST_CAPTION=0,
	RES_ACTION1,
	RES_ACTION2,
	RES_MAX
} HW_RESOURCE;
RESOURCE_ID Resources[RES_MAX];

//use thisapp(app) to access to struct APP_T
typedef struct
{
    APPLICATION_T apt;
} APP_T;
#define thisapp(app) ((APP_T *)(app))

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 MainStateEnter (EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);
UINT32 MainStateExit (EVENT_STACK_T *ev_st, APPLICATION_T *app, EXIT_STATE_TYPE_T type);

UINT32 HandleSend (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleDialogDone (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleNavigate (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleListReq (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleSelect (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleSwitchDevice(EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 InitDlgActions( ACTIONS_T *action_list );
UINT32 InitResources( void );
UINT32 RemoveResources( void );

UINT32 SendListItems( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
UINT32 Update (EVENT_STACK_T *ev_st, APPLICATION_T *app, UINT32 sItem);

UINT32 SendViaBT(DL_FS_MID_T *ids, UINT32 count);   //������� ���� �� bluetooth;
UINT32 decodeJ2MEST(void);                          //������������� J2MEST

#endif
