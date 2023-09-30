#include "FontServ.h"

ElfLoaderApp         FontServ = { 0, FALSE, 0, 0, NULL, NULL };

const char app_name[APP_NAME_LEN] = "FontServ"; 

#define  EVENT_DO   0xE000

UINT32              SELECTED_IDX = 0;
UINT32              ENTRIES_NUM = 0;

#define CG4 0x13BC0000

WCHAR font_cfg[] = { 'f', 'i', 'l', 'e', ':', '/', '/', 'a', '/', 'f', 'o', 'n', 't', '.', 'c', 'f', 'g', 0 };

#define FONT_TABLE_ADDRESS 0x09000000

unsigned char default_font_table[96] = {
	0x00, 0x00, 0x00, 0x06, 0xCA, 0x3D,
	0x00, 0x00, 0x00, 0x08, 0xCA, 0x3D,
	0x00, 0x00, 0x00, 0x08, 0xCA, 0x3D,
	0x00, 0x00, 0x00, 0x0B, 0x91, 0xEB,
	0x00, 0x00, 0x00, 0x0B, 0x30, 0xA3,
	0x00, 0x00, 0x00, 0x11, 0x30, 0xA3,
	0x00, 0x00, 0x00, 0x07, 0x97, 0x0A,
	0x00, 0x00, 0x00, 0x07, 0x97, 0x0A, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x06, 0xCA, 0x3D,
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x13, 0x2E, 0x14,
	0x00, 0x00, 0x00, 0x07, 0x97, 0x0A,
	0x00, 0x00, 0x00, 0x07, 0x97, 0x0A,
	0x00, 0x00, 0x00, 0x08, 0xCA, 0x3D,
	0x00, 0x00, 0x00, 0x08, 0xCA, 0x3D
};

typedef struct
{
    UINT8 font_id;
    WCHAR font_name[ 0x33 ];
} FONT_CFG;
    
FONT_CFG Font = { 0, NULL };

typedef struct
{
    UINT32 end_descriptor_address;
    UINT32 start_address;
    UINT32 end_address;
} FONT_TABLE_DESCRIPTOR_T;

typedef struct
{
    UINT8 id;
    UINT8 unk1;
    WCHAR font_name[ 0x33 ];
    UINT32 font_address;
    UINT32 unk2;
    UINT8 min_size;
    UINT8 max_size;
    UINT8 end[ 6 ];
} FONT_ENTRY_T;
    
FONT_ENTRY_T * FeFonts = NULL;
UINT8 fonts = 0;

UINT32 ReadCfg( );
void itoa( int, char * );

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked },
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    { EV_GRANT_TOKEN,               HandleUITokenGranted },
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_DIALOG_DONE,               FontServExit},
    { EV_REQUEST_LIST_ITEMS,        HandleListReq  },
    { EV_LIST_NAVIGATE,				Navigate       }, 
    { EV_SELECT,                    Select         },
    { EVENT_DO,                     Do_Action      },
    { EV_DONE,                      FontServExit},
    { STATE_HANDLERS_END,           NULL           },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { HW_STATE_ANY,
      NULL,
      NULL,
      any_state_handlers
    },

    { HW_STATE_INIT,
      NULL,
      NULL,
      init_state_handlers
    },
    
    { HW_STATE_MAIN,
      MainStateEnter,
      StateExit,
      main_state_handlers
    }
};

WCHAR u_app_name[APP_NAME_LEN] = { 'F', 'o', 'n', 't', 0 };

UINT32     iTunes_EvCdResID = 0x14000076; //iTunes

#define    iTunes_NameResID 0x01001bc0
#define    iTunes_IcLsResID 0x120007cd
#define    iTunes_IconResID 0x120007ce
#define    iTunes_IcAnResID 0x120007cc

UINT32     iTunes_Resource[] = { 0x000C0CC0, iTunes_NameResID, 0x01000674, iTunes_IcLsResID, 0x00000000, iTunes_IconResID, iTunes_IcAnResID };

void FontTool_SetFont( );

int _main( ElfLoaderApp ela )
{
    UINT32 status = RESULT_OK;

    memcpy( ( void * )&FontServ, ( void * )&ela, sizeof( ElfLoaderApp ) );

    status = APP_Register( &FontServ . evcode,
                           1,
                           state_handling_table,
                           HW_STATE_MAX,
                           (void*)FontServStart );

    //LoaderShowApp( &FontServ );

    ReadCfg( );
    FontTool_SetFont( );

    DRM_SetResource( iTunes_EvCdResID, iTunes_Resource, sizeof( UINT32 ) * sizeof( iTunes_Resource ) );
    
    DRM_SetResource( iTunes_NameResID, ( void * )u_app_name, APP_NAME_LEN * 2 );
    
    DRM_SetResource( iTunes_IconResID, ( void * )FONT_TOOL_ICON, 2215 );
    DRM_SetResource( iTunes_IcLsResID, ( void * )FONT_TOOL_ICON_LST, 1106 );
    DRM_SetResource( iTunes_IcAnResID, ( void * )FONT_TOOL_ANI , 2529 );


    return 1;
}

void FontTool_SetFont( )
{
    int i;
    for( i = 0; i < 16; i++ )
    {
        default_font_table[ i * 6 ] = Font . font_id;
    }
 
    memcpy( ( void * )FONT_TABLE_ADDRESS, ( void * )default_font_table, 96 );
}

UINT32 LoadFonts( )
{
    FONT_TABLE_DESCRIPTOR_T descriptor;
    memcpy( &descriptor, ( void * )CG4 + 0x23C, sizeof( FONT_TABLE_DESCRIPTOR_T ) );

    memcpy( &fonts, ( void * )descriptor . end_descriptor_address, 1 );

    FeFonts = ( FONT_ENTRY_T * )suAllocMem( fonts * sizeof( FONT_ENTRY_T ), NULL );
    
    ENTRIES_NUM = 0;
    
    int i;

    for( i = 0; i < fonts; i++ )
    {
        memcpy( &FeFonts[ i ], ( void * )descriptor . start_address + ( i * sizeof( FONT_ENTRY_T ) ), sizeof( FONT_ENTRY_T ) );
        ENTRIES_NUM++;
    }

    return RESULT_OK;
}

UINT32 FontServStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_FONTSERV_T     *app = NULL;
    UINT32 status = RESULT_OK;

    app = (APP_FONTSERV_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_FONTSERV_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              1, 1, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_INIT,
                        state_handling_table,
                        FontServExit,
                        app_name,
                        0 );

    LoadFonts( );

    return RESULT_OK;
}

UINT32 ReadCfg( )
{
    UINT32 filesize;
    UINT32 r;
    FILE_HANDLE_T   f;

    
    f = DL_FsOpenFile( font_cfg, FILE_READ_MODE, 0 );
    filesize = DL_FsGetFileSize( f );
    
    if( filesize <= 0 )
    {
        DL_FsCloseFile( f );
        return RESULT_FAIL;
    }

    DL_FsReadFile( &Font, sizeof( FONT_CFG ), 1, f, &r );
    DL_FsCloseFile( f );
    
    return RESULT_OK;
}

UINT32 WriteCfg( )
{
    UINT32 r;
    FILE_HANDLE_T   f;
    
    f = DL_FsOpenFile( font_cfg, FILE_WRITE_MODE, 0 );
   
    if( f == FILE_HANDLE_INVALID )
    {
        return RESULT_FAIL;
    }

    DL_FsWriteFile( &Font, sizeof( FONT_CFG ), 1, f, &r );
    DL_FsCloseFile( f );
    
    return RESULT_OK;
}

UINT32 FontServExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;
        
    RemoveResources( );
    
    suFreeMem( FeFonts );
    
    APP_UtilUISDialogDelete(  &papp->dialog );
    
    status = APP_Exit( ev_st, app, 0 );
    
    //LoaderEndApp( &FontServ );
    //PFprintf( "DONE!\n" );
    
    return status;
}

UINT32					starting_num = 0;

UINT32 Update( EVENT_STACK_T *ev_st,  void *app, UINT32 sItem )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    CONTENT_T               content;
    UIS_DIALOG_T            dialog = 0;
	ACTIONS_T				action_list;

    if ( papp->dialog != NULL )
	{	
		APP_UtilUISDialogDelete( &papp->dialog );
		papp->dialog = NULL;
        RemoveResources();
	}	

    InitResources();
    InitDlgActions( &action_list );

    dialog = UIS_CreateList( &port,
							 0,
							 ENTRIES_NUM,
							 0,
							 &starting_num,
							 0,
							 2,
							 &action_list,
							 Resources[RES_LIST_CAPTION] );

    if(dialog == 0) return RESULT_FAIL;

    papp->dialog = dialog;
    
	if (sItem != 0)
	{
		APP_UtilAddEvUISListChange( ev_st,
                                app,
                                0, // = 0
                                sItem,
                                ENTRIES_NUM,
                                TRUE,
                                2,          // = 2
                                FBF_LEAVE,
                                NULL,
                                NULL );	

		UIS_HandleEvent( dialog,  ev_st );		
	}

    SendListItems( ev_st, app, 1, starting_num );

    return RESULT_OK;
}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

    ReadCfg( );

    return Update( ev_st, app, 0 );
}


UINT32 InitResources( )
{
    UINT32						status;
    RES_ACTION_LIST_ITEM_T		action[1];
    
	//WCHAR list_caption[] = { 'S', 'e', 'l', 'e', 'c', 't', ' ', 'a', ' ', 'F', 'o', 'n', 't', 0 };

    Resources = ( RESOURCE_ID * )suAllocMem( sizeof( RESOURCE_ID ) * ( RES_MAX + 1 ), NULL );

    //status = DRM_CreateResource( &Resources[RES_LIST_CAPTION], RES_TYPE_STRING, (void*)list_caption, (u_strlen(list_caption)+1)*sizeof(WCHAR) );

    Resources[RES_SOFTKEY_LABEL] = LANG_SELECT;
    Resources[RES_LIST_CAPTION]  = LANG_FONT_TYPE;

    action[0].softkey_label = Resources[RES_SOFTKEY_LABEL];
    action[0].list_label = 0;
    action[0].softkey_priority = -1;
    action[0].list_priority = 0;
    action[0].isExit = FALSE;
    action[0].sendDlgDone = FALSE;

    DRM_CreateResource( &Resources[RES_ACTION1], RES_TYPE_ACTION, (void*)&action, sizeof(RES_ACTION_LIST_ITEM_T));

    return status;
}

UINT32 InitDlgActions( ACTIONS_T *action_list )
{

    RESOURCE_ID                  DaRes;

    action_list->action[0].operation = ACTION_OP_ADD; 
    action_list->action[0].event = EVENT_DO;
    action_list->action[0].action_res = Resources[RES_ACTION1];

    action_list->count = 1;

    return RESULT_OK;
}

UINT32 HandleListReq( EVENT_STACK_T *ev_st,  void *app )
{
	APPLICATION_T			*papp = (APPLICATION_T*) app;
	EVENT_T					*event;
	UINT32					start, num;
    UINT32                  rtv;
    
	if( !papp->focused ) return RESULT_OK;

	event = AFW_GetEv( ev_st );

	start = event->data.list_items_req.begin_idx;
	num = event->data.list_items_req.count;

    //PFprintf( "start: %x, num: %x\n", start, num );

	APP_ConsumeEv( ev_st, app );

	return SendListItems( ev_st, app, start, num );
}

UINT32 StateExit( EVENT_STACK_T *ev_st,  void *app,  EXIT_STATE_TYPE_T type )
{
	APP_FONTSERV_T *papp = (APP_FONTSERV_T*) app;
	APP_UtilUISDialogDelete( &papp->apt.dialog );
	return RESULT_OK;
}

UINT32 Select( EVENT_STACK_T *ev_st,  void *app)
{
    Do_Action( ev_st, app );

	return RESULT_OK;
}


UINT32 Do_Action( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    CONTENT_T               content;
    UIS_DIALOG_T            dialog = 0;
    EVENT_T     *event = ( EVENT_T     * )AFW_GetEv(ev_st);

    APP_ConsumeEv( ev_st, app );

    //PFprintf( "Do_Action (%x)\n", SELECTED_IDX );

    Font . font_id = SELECTED_IDX - 1;

    memcpy( Font . font_name, FeFonts[ SELECTED_IDX - 1 ] . font_name, sizeof( WCHAR ) * 0x32 );

    WriteCfg( );

    FontTool_SetFont( );
    
    UINT8 Lang;
    
    DRM_GetCurrentLanguage( &Lang );    
    UIS_SetLanguage( Lang );

	return Update( ev_st, app, SELECTED_IDX );
}

LIST_ENTRY_T		* plist=NULL;

UINT32 SendListItems( EVENT_STACK_T *ev_st,  void *app, UINT32 start, UINT32 num)
{
	APPLICATION_T			*papp = (APPLICATION_T*) app;
	UINT32					i, index, status=RESULT_OK;

    if( num==0 ) return RESULT_FAIL;

	plist = (LIST_ENTRY_T*) suAllocMem( sizeof(LIST_ENTRY_T)*num, NULL );
	if( plist==NULL ) return RESULT_FAIL;

	for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM); i++, index++)
	{
		plist[index].editable = FALSE;
		plist[index].content.static_entry.formatting = 1;

        if( Font . font_id == ( i - 1 ) )
            UIS_MakeContentFromString( "q0Sp1", &( plist[index].content.static_entry.text ), FeFonts[ i - 1 ] . font_name, DRM_SELECTED );
        else
            UIS_MakeContentFromString( "q0", &( plist[index].content.static_entry.text ), FeFonts[ i - 1 ] . font_name );
	}

	status = APP_UtilAddEvUISListData( ev_st, app, 0,
									   start, num,
									   FBF_LEAVE,
									   sizeof(LIST_ENTRY_T) * num,
									   plist );
	if( status != RESULT_FAIL )
	{
		status = UIS_HandleEvent(papp->dialog, ev_st);
	}

	suFreeMem( plist );

	return status;
}

UINT32 Navigate (EVENT_STACK_T *ev_st,  void *app )
{
    EVENT_T     *event = ( EVENT_T     * )AFW_GetEv(ev_st);

	SELECTED_IDX = event->data.index;

    APP_ConsumeEv( ev_st, app );

	return RESULT_OK;
}

UINT32 RemoveResources( )
{
	UINT32				status = RESULT_OK;
	UINT32				i;

	for(i=0;i<RES_MAX;i++)
	{
		status |= DRM_ClearResource( Resources[i] );
	}
	
	suFreeMem( Resources );

	return status;
}

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    UINT32                  status;

    status = APP_HandleUITokenGranted( ev_st, app );

    if( (status == RESULT_OK) && (papp->token_status == 2) )
    {
        status = APP_UtilChangeState( HW_STATE_MAIN, ev_st, app );
    }

    return status;
}


