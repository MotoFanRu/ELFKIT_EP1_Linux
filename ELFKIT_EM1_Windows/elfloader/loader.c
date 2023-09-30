/*
    Copyright (c) 2008 theCor3 & flash.tato

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/


int beginning( );

int _main( )
{
    return beginning( );
}

#include <apps.h>
#include <uis.h>
#include <typedefs.h>
#include <filesystem.h>
#include <lang_strings.h>
#include <canvas.h> 
#include <loader.h>
#include <time_date.h>
#include <util.h>
#include <drm_icons.h>
#include "elf_format.h"
#include "loader.h"

#define  EVENT_ABOUT   0xABCD
#define  EVENT_RUN     EVENT_ABOUT+1
#define  EVENT_DEL     EVENT_RUN+1
#define  EVENT_AUTORUN EVENT_DEL+1

#define    InFusio_NameResID 0x01000673
#define    InFusio_IconResID 0x12000822
#define    InFusio_IcLsResID 0x12000821
#define    InFusio_IcAnResID 0x12000820

UINT16     ENTRIES_NUM = 0;
UINT32     evcode_base = 0xEEED; /* 0x3FD; // was Airplane mode*/
UINT32     InFusio_EvCdResID = 0x1400006F; //InFusio Games

UINT32     InFusio_Resource[] = { 0x000EEED, InFusio_NameResID, 0x01000674, InFusio_IcLsResID, 0x00000000, InFusio_IconResID, InFusio_IcAnResID };

UINT32     SelectedElf = 0;
FILEINFO * fname = NULL;
UINT32     CBssAdr = 0;

typedef int ( * ELF_ENTRY)( ElfLoaderApp );

const char app_name[APP_NAME_LEN] = "ElfLoader"; 
WCHAR u_app_name[ APP_NAME_LEN ] = L"ElfLoader";
    
WCHAR startupcfg[] = L"file://a/startup.cfg";

ElfLoaderApp ** ElfStack = NULL;
ElfFunctions * Symbols = ( ElfFunctions * )NULL;

int elfs = 0;

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

EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_DIALOG_DONE,               AppLoaderExit },
    { EV_REQUEST_LIST_ITEMS,        HandleListReq }, 
    { EV_LIST_NAVIGATE,				Navigate      },
    { EV_SELECT,                    Select        },
    { EVENT_RUN,                    Run_Action    },
    { EVENT_DEL,                    Del_Action    },
    { EVENT_ABOUT,                  About_Action  },
    { EVENT_AUTORUN,                AutRun_Action },
    { EV_DONE,                      AppLoaderExit },
    { STATE_HANDLERS_END,           NULL          },
};

EVENT_HANDLER_ENTRY_T about_state_handlers[] =
{
    
    { EV_GRANT_TOKEN,             	  HandleUITokenGranted },
    { EV_DIALOG_DONE,                           AboutExit  },
    { STATE_HANDLERS_END,         			     	  NULL },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    {
        HW_STATE_ANY,
        NULL,
        NULL,
        any_state_handlers
    },
    {
        HW_STATE_INIT,
        NULL,
        NULL,
        init_state_handlers
    },
    {
        HW_STATE_MAIN,
        MainStateEnter,
        MainStateExit,
        main_state_handlers
    },
    {
        HW_STATE_ABOUT,
        AboutStateEnter,
        NULL,
        about_state_handlers
    }
};

int beginning( )
{
    UINT32 status = RESULT_OK;

    //overwrites Infusio menu entry resource ids
    DRM_SetResource( InFusio_EvCdResID, InFusio_Resource, sizeof( UINT32 ) * sizeof( InFusio_Resource ) );
    
    DRM_SetResource( InFusio_NameResID, ( void * )u_app_name, APP_NAME_LEN * 2 );
    
    DRM_SetResource( InFusio_IconResID, ( void * )APP_ICON     , 1377 );
    DRM_SetResource( InFusio_IcLsResID, ( void * )APP_ICON_LIST, 952 );
    DRM_SetResource( InFusio_IcAnResID, ( void * )APP_ICON_ANI , 1480 );
    
    status = APP_Register( &evcode_base,
                           1,
                           state_handling_table,
                           HW_STATE_MAX, 
                           (void*)AppLoaderStart ); 

    //ElfStack = ( ElfLoaderApp ** )suAllocMem( sizeof( ElfLoaderApp ) * 128, NULL ); //will be useful
    
    LoadSymbolDB( ); //Loads firmware symbols database
    DoAutorun( );    //Autorun function

    return status;
}

UINT32 AppLoaderStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_APPLOADER_T     *app = NULL;
    UINT32 status = RESULT_OK;

    if( AFW_InquireRoutingStackByRegId( reg_id ) == RESULT_OK )
        return RESULT_FAIL;

    app = (APP_APPLOADER_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_APPLOADER_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              1, 1, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_INIT,
                        state_handling_table,
                        AppLoaderExit,
                        app_name,
                        0 );
    
    return RESULT_OK;
}

UINT32 AppLoaderExit( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
    UINT32  status;

	RemoveResources();

    suFreeMem( fname );

    APP_UtilUISDialogDelete( &papp->dialog );

    status = APP_Exit( ev_st, app, NULL );

    return RESULT_OK;
}

UINT32 Update( EVENT_STACK_T *ev_st,  void *app, UINT32 sItem )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    UIS_DIALOG_T            dialog = 0;
	UINT32					starting_num;
	ACTIONS_T				action_list;
    
    if ( papp->dialog != NULL )
	{	
		APP_UtilUISDialogDelete( &papp->dialog );
		papp->dialog = NULL;
        RemoveResources();
	}
	
	FindAppS( );

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

	SendListItems(ev_st, app, 1, starting_num);

    return RESULT_OK;

}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    if( type != ENTER_STATE_ENTER ) return RESULT_OK;

    return Update( ev_st, app, 0 );
}

UINT32 MainStateExit( EVENT_STACK_T *ev_st,  void *app,  EXIT_STATE_TYPE_T type )
{
	APPLICATION_T           *papp = (APPLICATION_T*) app;

	APP_UtilUISDialogDelete( &papp->dialog );

	return RESULT_OK;
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

UINT32 HandleListReq( EVENT_STACK_T *ev_st,  void *app )
{
	APPLICATION_T			*papp = (APPLICATION_T*) app;
	EVENT_T					*event;
	UINT32					start, num;
    
	if( !papp->focused ) return RESULT_OK;

	event = AFW_GetEv( ev_st );

	start = event->data.list_items_req.begin_idx;
	num = event->data.list_items_req.count;

	APP_ConsumeEv( ev_st, app );

	return SendListItems( ev_st, app, start, num );
}

UINT32 Navigate (EVENT_STACK_T *ev_st,  void *app )
{
    EVENT_T     *event = ( EVENT_T     * )AFW_GetEv(ev_st);

	SelectedElf = event->data.index;

	//APP_ConsumeEv( ev_st, app );

	return RESULT_OK;
}

UINT32 SendListItems( EVENT_STACK_T *ev_st,  void *app, UINT32 start, UINT32 num)
{
	APPLICATION_T			*papp = (APPLICATION_T*) app;
	LIST_ENTRY_T			*plist;
	UINT32					i, index, status=RESULT_OK;
    
    if( num==0 ) return RESULT_FAIL;

	plist = (LIST_ENTRY_T*) suAllocMem( sizeof(LIST_ENTRY_T)*num, NULL );
	if( plist==NULL ) return RESULT_FAIL;

	for( index=0, i=start; (i<start+num) && (i<=ENTRIES_NUM); i++, index++)
	{
		plist[index].editable = FALSE;
		plist[index].content.static_entry.formatting = 1;

        if( fname[ i - 1 ].in_autorun > 0 )
            UIS_MakeContentFromString( "p1q0Sp2", &( plist[index].content.static_entry.text ), fname[ i - 1 ].name, Resources[RES_ICON], DRM_SELECTED );
        else
            UIS_MakeContentFromString( "p1q0", &( plist[index].content.static_entry.text ), fname[ i - 1 ].name, Resources[RES_ICON] );
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

UINT32 u_fix( WCHAR * str )
{
    int i;
    int l = u_strlen( str ) * 2;
    for( i = 3; i < l; i += 2 )
    {
        if( ( ( char * )str )[ i ] < 91 && ( ( char * )str )[ i ] > 64 )
            ( ( char * )str )[ i ] += 32;
    }
    return RESULT_OK;
}

UINT32 InitResources( )
{
    RES_ACTION_LIST_ITEM_T		action[4];
    UINT32						status;

    status = DRM_CreateResource( &Resources[RES_LIST_CAPTION], RES_TYPE_STRING, (void*)u_app_name, (u_strlen(u_app_name)+1)*sizeof(WCHAR) );

    Resources[RES_LABEL1] = LANG_RUN;
    Resources[RES_LABEL2] = LANG_ABOUT;
    Resources[RES_LABEL3] = LANG_DEL1;

    WCHAR Add[ 12 ], Remove[ 18 ];
    WCHAR Autorun[ 64 ];
    UINT32 len;
    
    DRM_GetResourceLength( LANG_ADD, &len );
    DRM_GetResource( LANG_ADD, Add, len );
    
    DRM_GetResourceLength( LANG_REMOVE, &len );
    DRM_GetResource( LANG_REMOVE, Remove, len );
    
    u_fix( Remove );
    
    u_strcpy( Autorun, Add );
    u_strcat( Autorun, L"/" );
    u_strcat( Autorun, Remove );
    u_strcat( Autorun, L" autorun" );

    status = DRM_CreateResource( &Resources[RES_LABEL4], RES_TYPE_STRING, ( void * )Autorun, ( u_strlen( Autorun ) + 1 ) * sizeof( WCHAR ) );

    status = DRM_CreateResource( &Resources[RES_ICON], RES_TYPE_GRAPHICS, ( void * )APP_ICON_LIST, sizeof( APP_ICON_LIST ) );
    
	if( status!=RESULT_OK ) return status;

    action[0].softkey_label = 0;
    action[0].list_label = Resources[RES_LABEL1];
    action[0].softkey_priority = 0;
    action[0].list_priority = 3;
    action[0].isExit = FALSE;
    action[0].sendDlgDone = FALSE;

    status = DRM_CreateResource( &Resources[RES_ACTION1], RES_TYPE_ACTION, ( void * )&action[0], sizeof(RES_ACTION_LIST_ITEM_T) );
	if( status!=RESULT_OK ) DRM_ClearResource( Resources[RES_ACTION1] );
	
    action[3].softkey_label = 0;
    action[3].list_label = Resources[RES_LABEL4];
    action[3].softkey_priority = 0;
    action[3].list_priority = 2;
    action[3].isExit = FALSE;
    action[3].sendDlgDone = FALSE;

    status = DRM_CreateResource( &Resources[RES_ACTION4], RES_TYPE_ACTION, ( void * )&action[3], sizeof(RES_ACTION_LIST_ITEM_T) );
	if( status!=RESULT_OK ) DRM_ClearResource( Resources[RES_ACTION4] );
	
    action[1].softkey_label = 0;
    action[1].list_label = Resources[RES_LABEL2];
    action[1].softkey_priority = 0;
    action[1].list_priority = 0;
    action[1].isExit = FALSE;
    action[1].sendDlgDone = FALSE;

    status = DRM_CreateResource( &Resources[RES_ACTION2], RES_TYPE_ACTION, ( void * )&action[1], sizeof(RES_ACTION_LIST_ITEM_T) );
	if( status!=RESULT_OK ) DRM_ClearResource( Resources[RES_ACTION2] );
	
    action[2].softkey_label = 0;
    action[2].list_label = Resources[RES_LABEL3];
    action[2].softkey_priority = 0;
    action[2].list_priority = 1;
    action[2].isExit = FALSE;
    action[2].sendDlgDone = FALSE;

    status = DRM_CreateResource( &Resources[RES_ACTION3], RES_TYPE_ACTION, ( void * )&action[2], sizeof(RES_ACTION_LIST_ITEM_T) );
	if( status!=RESULT_OK ) DRM_ClearResource( Resources[RES_ACTION3] );
	
    return status;
}

UINT32 RemoveResources( )
{
	UINT32				status = RESULT_OK;
	UINT32				i;

	for(i=0;i<RES_MAX;i++)
	{
		status |= DRM_ClearResource( Resources[i] );
	}

	return status;

}

UINT32 About_Action( EVENT_STACK_T *ev_st,  void *app )
{
    //EVENT_T     *event = AFW_GetEv(ev_st);
    
    APP_UtilChangeState( HW_STATE_ABOUT, ev_st, app );

	APP_ConsumeEv( ev_st, app );

	return RESULT_OK;
}

UINT32 AutRun_Action( EVENT_STACK_T *ev_st,  void *app )
{
	APP_ConsumeEv( ev_st, app );

    if( SelectedElf > 0 ) --SelectedElf;

    FILE_HANDLE_T   f;
    UINT32 w;

    f = DL_FsOpenFile( startupcfg, FILE_READ_PLUS_MODE, NULL );
    
    char * rbuffer;
    UINT32 filesize;
    UINT32 alloc_size;
    AUTORUN_Header Head;
    AUTORUN_Entry Entry;
    int i, off;
    
    if( f == FILE_HANDLE_INVALID )
    {
        f = DL_FsOpenFile( startupcfg, FILE_WRITE_PLUS_MODE, NULL );
        filesize = 0;
    }
    else
        filesize = DL_FsGetFileSize( f );
    
    if( fname[ SelectedElf ] . in_autorun > 0 )
    {
        UINT32 element = fname[ SelectedElf ] . in_autorun - 1;
        
        rbuffer = ( char * )suAllocMem( filesize, NULL );
        
        DL_FsReadFile( rbuffer, filesize, 1, f, &w );
        DL_FsFSeekFile( f, 0, SEEK_WHENCE_SET );
        DL_FsReadFile( &Head, sizeof( AUTORUN_Header ), 1, f, &w );
        
        --Head . Elements;
        
        memcpy( rbuffer, &Head, sizeof( AUTORUN_Header ) );
        
        int j = 0;
        for( i = 0; i < Head . Elements + 1; i++ )
        {
            if( i == element ) continue;
            
            off = sizeof( AUTORUN_Header ) + sizeof( AUTORUN_Entry ) * j;

            DL_FsReadFile( &Entry, sizeof( AUTORUN_Entry ), 1, f, &w );
                
            memcpy( ( void * )rbuffer + off, &Entry, sizeof( AUTORUN_Entry ) );
            ++j;
        }
        
        alloc_size = sizeof( AUTORUN_Header ) + sizeof( AUTORUN_Entry ) * j;
        
        DL_FsCloseFile( f );
        
        f = DL_FsOpenFile( startupcfg, FILE_WRITE_PLUS_MODE, NULL );
        
        DL_FsWriteFile( rbuffer, alloc_size, 1, f, &w );
        DL_FsCloseFile( f );
    }
    else
    {
        if( filesize <= 0 )
        {
            rbuffer = ( char * )suAllocMem( sizeof( AUTORUN_Header ) + sizeof( AUTORUN_Entry ), NULL );
            alloc_size = sizeof( AUTORUN_Header ) + sizeof( AUTORUN_Entry );
        }
        else
        {
            rbuffer = ( char * )suAllocMem( filesize + sizeof( AUTORUN_Entry ), NULL );
            alloc_size = filesize + sizeof( AUTORUN_Entry );
        }
        
        DL_FsReadFile( rbuffer, filesize, 1, f, &w );

        if( filesize <= 0 )
        {
            filesize += sizeof( AUTORUN_Header );
            memset( ( void * )&Head, 0, sizeof( AUTORUN_Header ) );
        }
        else
        {
            DL_FsFSeekFile( f, 0, SEEK_WHENCE_SET );
            DL_FsReadFile( &Head, sizeof( AUTORUN_Header ), 1, f, &w );
        }

        Head . Elements += 1;
        
        memcpy( rbuffer, &Head, sizeof( AUTORUN_Header ) );

        for( i = 0; i < Head . Elements - 1; i++ )
        {
            off = sizeof( AUTORUN_Header ) + sizeof( AUTORUN_Entry ) * i;
            
            if( filesize > 0 )
                DL_FsReadFile( &Entry, sizeof( AUTORUN_Entry ), 1, f, &w );
                
            memcpy( ( void * )rbuffer + off, &Entry, sizeof( AUTORUN_Entry ) );
        }

        memset( ( void * )&Entry, 0, sizeof( AUTORUN_Entry ) );
        u_strcpy( Entry . ElfPath, fname[ SelectedElf ] . u_fullname );
        Entry . Event_Code = 0;

        off = sizeof( AUTORUN_Header ) + sizeof( AUTORUN_Entry ) * i;
        memcpy( ( void * )rbuffer + off, &Entry, sizeof( AUTORUN_Entry ) );
            
        DL_FsFSeekFile( f, 0, SEEK_WHENCE_SET );
        DL_FsWriteFile( rbuffer, alloc_size, 1, f, &w );
        
        DL_FsCloseFile( f );

    }

    suFreeMem( rbuffer );

    Update( ev_st, app, SelectedElf + 1 );

	return RESULT_OK;
}


UINT32 Run_Action( EVENT_STACK_T *ev_st,  void *app )
{
	APP_ConsumeEv( ev_st, app );

    if( SelectedElf > 0 ) --SelectedElf;

    LoadElf( fname[ SelectedElf ] . u_fullname, 0 );

    AppLoaderExit( ev_st, app );

	return RESULT_OK;
}

UINT32 Del_Action( EVENT_STACK_T *ev_st,  void *app )
{
	APP_ConsumeEv( ev_st, app );

    if( SelectedElf > 0 ) --SelectedElf;

    DL_FsDeleteFile( fname[ SelectedElf ] . u_fullname, NULL );

    Update( ev_st, app, 0 );

	return RESULT_OK;
}

UINT32 Select( EVENT_STACK_T *ev_st,  void *app)
{
	APP_ConsumeEv( ev_st, app );

    if( SelectedElf > 0 ) --SelectedElf;

    LoadElf( fname[ SelectedElf ] . u_fullname, 0 );

    AppLoaderExit( ev_st, app );

	return RESULT_OK;
}

UINT32 InitDlgActions( ACTIONS_T *action_list )
{
    action_list->action[0].operation = ACTION_OP_ADD; 
    action_list->action[0].event = EVENT_RUN;
    action_list->action[0].action_res = Resources[RES_ACTION1]; //Run

    action_list->action[1].operation = ACTION_OP_ADD; 
    action_list->action[1].event = EVENT_ABOUT;
    action_list->action[1].action_res = Resources[RES_ACTION2]; //About

    action_list->action[2].operation = ACTION_OP_ADD; 
    action_list->action[2].event = EVENT_DEL;
    action_list->action[2].action_res = Resources[RES_ACTION3]; //Delete

    action_list->action[3].operation = ACTION_OP_ADD; 
    action_list->action[3].event = EVENT_AUTORUN;
    action_list->action[3].action_res = Resources[RES_ACTION4]; //Add to autorun

    action_list->count = 4;

    return RESULT_OK;
}

UINT32 FindAppS(void)
{
    UINT32                  i;
	FS_SEARCH_PARAMS_T      fs_param;
    FS_SEARCH_HANDLE_T      fs_handle;
    FS_SEARCH_RESULT_T      fs_result; 
    char                    uri[256];
    int id = 0;

    WCHAR                   filter[] = L"file://a/\xFFFE*.elf";
    WCHAR                   file[] =   L"file://a";

	UINT16 res_count, count = 1; 
    fs_param.flags = 0x1C;
	fs_param.attrib = 0x0000;
	fs_param.mask = 0x0000; 

	DL_FsSSearch( fs_param, filter, &fs_handle, &res_count, 0x0); 

    ENTRIES_NUM = res_count;

    if( !fname ) suFreeMem( fname );

    fname = ( FILEINFO * ) suAllocMem( ENTRIES_NUM * sizeof( FILEINFO ), NULL );

    int corr = 0;

    for( i = 0; i < ENTRIES_NUM; i++ )
    {
        DL_FsSearchResults( fs_handle, i, &count, &fs_result );

        strcpy( fname[i].fullname, "file:/" );
        u_strcpy( fname[i].u_fullname, file );

        u_utoa( fs_result.name, uri );
        
        u_strcat( fname[i].u_fullname, fs_result.name );
        strcat(fname[i].fullname, uri);
        
        fname[i].in_autorun = InAutorun( i );
    }

    DL_FsSearchClose(fs_handle);

    Fullname2Name();

	return RESULT_OK;
}

//Thanks to om2804
UINT32 Fullname2Name(void)
{
    int i=0, j=0, k=0;

    for (i=0; i < ENTRIES_NUM; i++)
    {
        if (fname[i].fullname[0] == ';' || fname[i].fullname[0] == 0)
        {
           for (j=0; j < 31; j++)
           {
               fname[i].name[j] = fname[i].fullname[j];
           }
           fname[i].name[j] = 0;
        }
        else
        {
              for (j=strlen(fname[i].fullname)-1; fname[i].fullname[j] != '/' && j >= 0; j--);
              j++;
        
              for (k=j; k < strlen(fname[i].fullname); k++)
                  fname[i].name[k-j] = fname[i].fullname[k];
              fname[i].name[k-j] = 0;
        }
    }

    return RESULT_OK;
}


UINT32 AboutExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32                  status = 0;
	APP_APPLOADER_T        *papp = (APP_APPLOADER_T*) app;

	APP_UtilUISDialogDelete( &papp->apt.dialog );

    status = APP_UtilChangeState( HW_STATE_MAIN, ev_st, app ); // 

	AFW_CreateInternalQueuedEvAux( evcode_base, FBF_LEAVE, 0, 0 );
	
    return status;
}

UINT32 AboutStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    CONTENT_T               content;
    UIS_DIALOG_T            dialog = 0;
	
	WCHAR msg[] = L"ElfLoader for v3x coded by theCor3 & flash.tato";

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;
    
    UIS_MakeContentFromString( "MCq0", &content, msg );
    
    dialog = UIS_CreateTransientNotice( &port, &content, NOTICE_TYPE_OK );
 
    if(dialog == 0) return RESULT_FAIL;

    papp->dialog = dialog;

    return RESULT_OK;
}


ElfLoaderApp ParseElf( BYTE *, UINT32 );
UINT32 Entry;

UINT32 LoaderShowApp( ElfLoaderApp * eapp )
{
    return AFW_CreateInternalQueuedEvAux( eapp -> evcode, FBF_LEAVE, 0, 0 ); //raise application event code
}

UINT32 LoaderEndApp( ElfLoaderApp * eapp )
{
    /*
        Application closed, it's time to free memory
    */
    eapp -> isFree = TRUE;

    suFreeMem( eapp -> textptr );
    suFreeMem( eapp -> dataptr );
	if(eapp -> bssptr)
		suFreeMem( eapp -> bssptr );
	
    return 0;
    //return AFW_CreateInternalQueuedEvAux( evcode_base, FBF_LEAVE, 0, 0 );
}

UINT32 EvCode = 0xA000; //Base evcode for our elfs

UINT32 CalcBssSize(BYTE * ElfImage, UINT32 ElfVA)
{
	PElfFile 		ElfFile;
	PElfSection 	ElfSection, ESection;
	PElfRelocation  ElfReloc;
	UINT32 			CurrentSymbol,symbols,CurrentSection;
	INT				CurrentRelocation;
	INT				RelocationSize;
	UINT32 tmp = 0;
	PElfSymbol Symbol;
	UINT32 BssSize = 0;
	DWORD tmpAddr = 0;
	PElfSection ElfSectionStrTab, SymTab;
	
	PFprintf("CalcBssSize enter\n");

	ElfFile = ( PElfFile )ElfImage;
	
	for( CurrentSection = ElfFile -> SectionsNumb-1; CurrentSection > 0 ; CurrentSection++ )
	{
        ElfSectionStrTab = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );
				 
		if( ElfSectionStrTab -> Type == 3 )
		{
            //PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Section StrTab 0x%x\n", ElfSectionStrTab -> Offset );
            break;
        }
    }
	
	for( CurrentSection = 0; CurrentSection < ElfFile -> SectionsNumb; CurrentSection++ )
	{
        SymTab = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );
				 
		if( SymTab -> Type == 2 )
		{
            break;
        }
    }
	
	symbols = SymTab -> Size / SymTab -> EntrySize;
	
	for( CurrentSymbol = 0; CurrentSymbol < symbols; CurrentSymbol++ )
	{
		Symbol = (PElfSymbol)( ElfImage + SymTab -> Offset + SymTab -> EntrySize * CurrentSymbol );
		char * name = ElfImage + ElfSectionStrTab -> Offset + Symbol -> Name;
		if(Symbol -> Section == SHN_COMMON)
			{
				tmp = BssSize +  Symbol -> Value - (BssSize % Symbol -> Value);
				BssSize += Symbol -> Size;
				Symbol -> Value = tmp;
				PFprintf("Symbol %s size 0x%x, value 0x%x, BssSize 0x%x\n", name, Symbol -> Size, Symbol -> Value, BssSize);
			}
		//PFprintf("Symbol %s size 0x%x, value 0x%x\n", name, Symbol -> Size, Symbol -> Value);
	}
	
	return BssSize;
}//---------------------------------------------------------------------------------------------------

void LoadElf( WCHAR * FileName, UINT32 EvCode_Override )
{
	FILE_HANDLE_T Elf = 0;
	BYTE * ElfImage;
	ELF_ENTRY ElfImageEntryPoint;
	INT FileSize;
	UINT32 rd;

	Elf = DL_FsOpenFile( FileName, FILE_READ_MODE, NULL );
	if( Elf == FILE_HANDLE_INVALID )
	{
        char a_str[ 64 ];
        u_utoa( FileName, a_str );
		PFprintf( "[ ELF_RNNR_LOG ]Can't open file: %s -> handle %x\n", a_str, Elf );
		return;
	}

	FileSize = DL_FsGetFileSize( Elf );
	
	ElfImage = ( BYTE * )suAllocMem( FileSize, NULL );
	if( ElfImage == NULL )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Out of memory\n" );
		DL_FsCloseFile( Elf );
        return;
	}

	DL_FsReadFile( ElfImage,
                   FileSize,
                   1,
                   Elf,
                   &rd );

    ElfLoaderApp eapp;
	eapp = ParseElf( ElfImage, FileSize );
    
    suFreeMem( ElfImage );
    
	DL_FsCloseFile( Elf );
	
	if( eapp . textptr != NULL &&
        eapp . dataptr != NULL )
    {
        if( EvCode_Override > 0 )
            eapp . evcode = EvCode_Override;
        else
        {
            eapp . evcode = EvCode;
            ++EvCode;
        }

        eapp . pid = elfs++;
        //ElfStack[ eapp . pid ] = &eapp;

        ( ( ELF_ENTRY )Entry )( eapp );
    }
    
    return;
}

UINT32 BASE_ADDR;
INT ExternalSymbolLookup( char *, unsigned * );

PElfSection 	ElfSectionStrTab, ElfDataSection, ElfBssSection;

UINT32 GetElfSymbol( BYTE * ElfImage, UINT32 i, unsigned * SymbolValue, unsigned SymTabSection, UINT32 ElfVA, PElfRelocation Relocation )
{
	PElfFile 	ElfFile;
	PElfSection Section;
	PElfSymbol  Symbol;
	char * 		SymbolName;
	unsigned 	Address;
	int 		Error;
	
	ElfFile  = ( PElfFile )ElfImage;
	if( SymTabSection >= ElfFile -> SectionsNumb )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Bad symbol table section number %d ( max %u )\n", SymTabSection, ElfFile -> SectionsNumb - 1 );
		return 1;
	}
	
	Section = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
							   ElfFile -> ShTabEntSize * SymTabSection );
							  
	if( i >= Section -> Size )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Offset into symbol table ( %u ) exceeds symbol table size (%lu), 0x%x\n", i, Section -> Size, Section -> Offset );
		return 2;
	}
	
	Symbol = ( PElfSymbol )( ElfImage + Section -> Offset ) + i;
	
	if( Symbol -> Section == 0 )
	{
		Section = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
								   ElfFile -> ShTabEntSize * Section -> Link );
		
		SymbolName = ( char * )ElfImage + Section -> Offset + Symbol -> Name;
		
		Error = ExternalSymbolLookup( SymbolName, SymbolValue );
		
		if( Error != 0 )
			return Error;
	}
	else
	{
		Section = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
								   ElfFile -> ShTabEntSize * Symbol -> Section );

		Address = ( unsigned )Section -> VirtualAddress;

		//if( Section -> VirtualAddress == BASE_ADDR && Relocation -> Addend != 0 )
        //    Relocation -> Addend -= 0xA;
		
		*SymbolValue = Symbol -> Value + Relocation -> Addend + Address;
	}
	return 0;
}



UINT32 RelocateElf( BYTE * ElfImage, PElfRelocation Relocation, PElfSection Section, UINT32 ElfVA, ElfLoaderApp * ela )
{
	unsigned Address, SymbolValue;
	PElfSection ESection;
	PElfFile    ElfFile;
	PDWORD 		Where;
	INT			Error;
	PElfSymbol  Symbol;
	UINT32      Addr;
	
	
	ElfFile  = ( PElfFile )ElfImage;
	ESection = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + ElfFile -> ShTabEntSize * Section -> Link );
			
	
	Symbol = ( PElfSymbol )( ElfImage + ESection -> Offset ) + ELF32_R_SYM( Relocation -> Info );
    //Address  = ( unsigned )BASE_ADDR + ESection -> Offset;
	
	
	if( Section -> Info == 1 )
	    Where = ( PDWORD )( ela -> TextAddr + Relocation -> Address );
	
    else 
		if( Section -> Info == 3 )
			Where = ( PDWORD )( ela -> DataAddr + Relocation -> Address );
			
	Error = GetElfSymbol( ElfImage, ELF32_R_SYM( Relocation -> Info ), &SymbolValue, Section -> Link, ElfVA, Relocation );
	if( Error != 0 )
		return Error;
		
	if(Symbol -> Section == SHN_COMMON)
		{
			*Where = ela -> BssAddr + Symbol -> Value;
			//CBssAdr+=Symbol -> Size;
			PFprintf("Reloc into BSS at 0x%x data 0x%x value 0x%x\n",Where,*Where, Symbol -> Value);
			//PFprintf("Relocation type %i\n",ELF32_R_TYPE( Relocation -> Info ));
			
			
			return 0;
		}
		
	switch( ELF32_R_TYPE( Relocation -> Info ) )
	{
		case 1:
			*Where = SymbolValue;
		break;
		case 2:
			*Where = SymbolValue + *Where - ( unsigned )Where;
		break;
		case 6:
		case 7:
			*Where = SymbolValue;
		break;
		default:
			PFprintf( "[ ELF_RNNR_LOG ]Unknown/unsupported relocation type %i "
				"(must be 1 or 2, 6 or 7)\n", ELF32_R_TYPE( Relocation -> Info ) );
			return -1;
	}

	return 0;
}

ElfLoaderApp ParseElf( BYTE * ElfImage, UINT32 Size )
{
	PElfFile 		ElfFile;
	PElfSection 	ElfSection;
	PElfRelocation  ElfReloc;
	UINT32 			CurrentSection;
	INT				CurrentRelocation;
	INT				SectionSize;
	INT				RelocationSize;
	INT				Error;
	
	UINT32 			BssSize = 0;

    ElfLoaderApp    ElfApp;

    DWORD ElfVA = 0;

    ElfApp . isFree    = FALSE;
	ElfApp . bssptr = NULL;

    //BASE_ADDR = (UINT32)AllocImage;
	ElfFile = ( PElfFile )ElfImage;

    //ElfFile -> EntryPoint += BASE_ADDR;

	if( ElfFile -> Magic[ 0 ] != 0x7F ||
        ElfFile -> Magic[ 1 ] != 0x45 ||
        ElfFile -> Magic[ 2 ] != 0x4C ||
        ElfFile -> Magic[ 3 ] != 0x46 ) //ELF
	{
		PFprintf( "[ ELF_RNNR_LOG ]This is not ELF file\n" );
		return ElfApp;
	}

	if( ElfFile -> Bitness != 1 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]64-bit ELF not supported, only 32-bit\n" );
		return ElfApp;
	}

	if( ElfFile -> Endian != 2 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Little endian ELF not supported, only Big\n" );
		return ElfApp;
	}
	
	if( ElfFile -> ElfVer1 != 1 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Bad ELF version: %u\n", ElfFile -> ElfVer1 );
		return ElfApp;
	}
	
	if( ElfFile -> FileType != 0 && ElfFile -> FileType != 1 && ElfFile -> FileType != 2 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]File is not relocatable ELF\n" );
		return ElfApp;
	}
	
	if( ElfFile -> Machine != 0x27 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]This is not M*CORE ELF (this is 4 Machine => %x)\n", ElfFile -> Machine );
		return ElfApp;
	}
	
	if( ElfFile -> ElfVer2 != 1 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Bad ELF version: %lu\n", ElfFile -> ElfVer2 );
		return ElfApp;
	}

	Entry = 0;

    for( CurrentSection = 0; CurrentSection < ElfFile -> SectionsNumb; CurrentSection++ )
	{
        ElfSectionStrTab = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );
				 
		if( ElfSectionStrTab -> Type == 3 )
		{
            //PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Section StrTab 0x%x\n", ElfSectionStrTab -> Offset );
            break;
        }
    }
    
    for( CurrentSection = 1; CurrentSection < ElfFile -> SectionsNumb; CurrentSection++ )
	{
        ElfDataSection = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );
		
		char * name = ElfImage + ElfSectionStrTab -> Offset + ElfDataSection -> SectionName;

		if( !strcmp( name, ".data" ) )
        {
            ElfApp . dataptr = ( UINT8 * )suAllocMem( ElfDataSection -> Size, NULL );
            ElfApp . DataAddr = ( UINT32 )ElfApp . dataptr;
            
            ElfDataSection -> VirtualAddress = ( UINT32 )ElfApp . DataAddr;
            PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Section '%s' mapped at 0x%x, size 0x%x\n", name, ElfDataSection -> VirtualAddress,  ElfDataSection -> Size );
            memcpy( ( char * )ElfDataSection -> VirtualAddress, ElfImage + ElfDataSection -> Offset, ElfDataSection -> Size );
            
            break;
        }
    }


	
    
	for( CurrentSection = 0; CurrentSection < ElfFile -> SectionsNumb; CurrentSection++ )
	{
		ElfSection = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );

    	char * name = ElfImage + ElfSectionStrTab -> Offset + ElfSection -> SectionName;
        if( ElfSection -> Type == 0 ) //jumps null section
            continue;
  
        if( !strcmp( name, ".text" ) )
        {
            ElfApp . textptr = ( UINT8 * )suAllocMem( ElfSection -> Size, NULL );
            ElfApp . TextAddr = ( UINT32 )ElfApp . textptr;

            ElfSection -> VirtualAddress = ( UINT32 )ElfApp . TextAddr;
        
            ElfVA = ElfSection -> VirtualAddress;
        
            ElfFile -> EntryPoint += ElfVA;
        
            PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Section '%s' mapped at 0x%x, size 0x%x\n", ElfImage + ElfSectionStrTab -> Offset + ElfSection -> SectionName, ElfVA,  ElfSection -> Size );
            memcpy( ( void * )ElfVA, ElfImage + ElfSection -> Offset, ElfSection -> Size );
        }

		if( ( ElfSection -> Flags & 0x0004 ) == 0 )
			continue;
		
		( Entry ) = ( UINT32 )ElfFile -> EntryPoint;

		break;
	}
	//
	
	for( CurrentSection = 1; CurrentSection < ElfFile -> SectionsNumb; CurrentSection++ )
	{
        ElfBssSection = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );
		
		char * name = ElfImage + ElfSectionStrTab -> Offset + ElfBssSection -> SectionName;

		if( !strcmp( name, ".bss" ) )
        {
			BssSize = CalcBssSize(ElfImage, ElfVA);
			if(BssSize)
			{
				ElfApp . bssptr = ( UINT8 * )suAllocMem( BssSize, NULL );
				ElfApp . BssAddr = ( UINT32 )ElfApp . bssptr;
				ElfBssSection -> VirtualAddress = ( UINT32 )ElfApp . BssAddr;
				CBssAdr = 0;
				PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Section '%s' mapped at 0x%x, size 0x%x\n", name, ElfBssSection -> VirtualAddress,  BssSize);
            }
            
            break;
        }
		
    }
	
	for( CurrentSection = 0; CurrentSection < ElfFile -> SectionsNumb; CurrentSection++ )
	{
		ElfSection = ( PElfSection )( ElfImage + ElfFile -> ShTableOffset + 
									  ElfFile -> ShTabEntSize * CurrentSection );

		if( ElfSection -> Type == 4 )
			RelocationSize = 12;
		else if( ElfSection -> Type == 9 )
			RelocationSize = 8;
		else
			continue;
		
		for( CurrentRelocation = 0; CurrentRelocation < ElfSection -> Size / RelocationSize; CurrentRelocation++ )
		{
			ElfReloc = ( PElfRelocation )( ElfImage + ElfSection -> Offset +
										   RelocationSize * CurrentRelocation );
			
			Error = RelocateElf( ElfImage, ElfReloc, ElfSection, ElfVA, &ElfApp );
			
			if( Error != 0 )
			{
                suFreeMem( ElfApp . textptr );
                ElfApp . textptr = NULL;
                suFreeMem( ElfApp . dataptr );
				if(ElfApp . bssptr)
					suFreeMem( ElfApp . bssptr );
				return ElfApp;
            }
		}
	}
	
	if( ( Entry ) == 0 )
	{
		PFprintf( "[ ELF_RNNR_LOG ]Can't find entry point\n" );
		return ElfApp;
	}
	
	PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Entry point found at: 0x%x\n", Entry );
	
	return ElfApp;
}

UINT32 SymMax = 0;
UINT32 Checksum = 0;

WCHAR rloader[] = L"file://a/library.def";
 
INT LoadSymbolDB( )
{
    FILE_HANDLE_T SymDb;

    
	INT FileSize;
	UINT32 rd;
	char * SymDbBuffer;

	SymDb = DL_FsOpenFile( rloader, FILE_READ_MODE, NULL );
	FileSize = DL_FsGetFileSize( SymDb );
	
	if( SymDb == FILE_HANDLE_INVALID || FileSize <= 0 )
	{
		return -1;
	}


    SymDbBuffer = ( char * )suAllocMem( FileSize, NULL );
	DL_FsReadFile( SymDbBuffer,
                   FileSize,
                   1,
                   SymDb,
                   &rd );

    int i;
    int syms = 0;
    for( i = 0; i < FileSize; i++ )
        if( SymDbBuffer[ i ] == 0x0A ) syms++;

    SymMax = syms;
    
    Symbols = ( ElfFunctions * )suAllocMem( sizeof( ElfFunctions ) * syms, NULL );
    
    char ** functions = ( char ** )suAllocMem( ( syms + 1 ) * 64, NULL );
    
    split( SymDbBuffer, functions, syms, "\n" );
    
    char ** pc  = ( char ** )suAllocMem( 3 * 64, NULL );
    
    unsigned res;

    for( i = 0; i < syms; i++ )
    {
        if( functions[ i ][ 0 ] == '\n' )
            continue;
    
        split( functions[ i ], pc, 2, "=" );

        int l = strlen( pc[ 0 ] );
        if( pc[ 0 ][ l - 1 ] == ' ' )
            pc[ 0 ][ l - 1 ] = 0;
        
        pc[ 1 ] = trim( pc[ 1 ] );

        strcpy( Symbols[ i ] . Name, pc[ 0 ] );
        xtoi( pc[ 1 ], &res );
        Symbols[ i ] . SAddress = res;
        
        //PFprintf( "%s (%x) %i/%i\n", Symbols[ i ] . Name, Symbols[ i ] . SAddress, i, SymMax );
    }

    //Adds in-loader defined functions to syscalls
    strcpy( Symbols[ i ] . Name, "LoaderShowApp" );
    Symbols[ i++ ] . SAddress = ( UINT32 )LoaderShowApp;
    SymMax++;

    strcpy( Symbols[ i ] . Name, "LoaderEndApp" );
    Symbols[ i ] . SAddress = ( UINT32 )LoaderEndApp;
    SymMax++;

	DL_FsCloseFile( SymDb );

    suFreeMem( pc );
    suFreeMem( functions );
	suFreeMem( SymDbBuffer );

	return 0;
}

INT ExternalSymbolLookup( char * SymbolName, unsigned * Address )
{	
	unsigned i;

    //PFprintf( "[ ELF_RNNR_DEBUG_LOG ]Searching symbol '%s'\n", SymbolName );
	
	for( i = 0; i <= SymMax; i++ )
	{
		if( !strcmp( Symbols[ i ] . Name, ( char * )SymbolName ) )
		{
			*Address = Symbols[ i ] . SAddress;
			return 0;
		}

	}
	
	PFprintf( "[ ELF_RNNR_LOG ]Undefined external symbol '%s'\n", SymbolName );
	return 1;
}

int InAutorun( UINT32 Elf )
{
    UINT32 filesize;
    UINT32 r;
    FILE_HANDLE_T   f;

    f = DL_FsOpenFile( startupcfg, FILE_READ_MODE, NULL );
    
    if( f == FILE_HANDLE_INVALID )
        return 0;
    
    filesize = DL_FsGetFileSize( f );
    
    if( filesize <= 0 )
    {
        DL_FsCloseFile( f );
        return 0;
    }

    AUTORUN_Header Head;
    AUTORUN_Entry  Entry;

    DL_FsReadFile( &Head,
                   sizeof( AUTORUN_Header ),
                   1,
                   f,
                   &r );

    int i;
    for( i = 0; i < Head . Elements; i++ )
    {
        DL_FsReadFile( &Entry,
               sizeof( AUTORUN_Entry ),
               1,
               f,
               &r );

        if( !u_strcmp( Entry . ElfPath, fname[ Elf ].u_fullname ) )
            return i + 1;
    }
    
    DL_FsCloseFile( f );

    return 0;
}

void DoAutorun( )
{
    UINT32 filesize;
    UINT32 r;
    FILE_HANDLE_T   f;

    f = DL_FsOpenFile( startupcfg, FILE_READ_MODE, NULL );
    
    if( f == FILE_HANDLE_INVALID )
        return;
    
    filesize = DL_FsGetFileSize( f );
    
    if( filesize <= 0 )
    {
        DL_FsCloseFile( f );
        return;
    }

    AUTORUN_Header Head;
    AUTORUN_Entry  Entry;
    
    DL_FsReadFile( &Head, sizeof( AUTORUN_Header ), 1, f, &r );

    int i;
    for( i = 0; i < Head . Elements; i++ )
    {
        DL_FsReadFile( &Entry, sizeof( AUTORUN_Entry ), 1, f, &r );
        
        if( DL_FsFFileExist( Entry . ElfPath ) )
        {
            PFprintf( "Ev code: %x\n", Entry . Event_Code );
            LoadElf( Entry . ElfPath, Entry . Event_Code );
        }
    }
    
    DL_FsCloseFile( f );
}

//found somewhere in the web
int	split( char *string, char *fields[], int nfields, char *sep )
{
	register char *p = string;
	register char c;			/* latest character */
	register char sepc = sep[0];
	register char sepc2;
	register int fn;
	register char **fp = fields;
	register char *sepp;
	register int trimtrail;

	/* white space */
	if (sepc == '\0') {
		while ((c = *p++) == ' ' || c == '\t')
			continue;
		p--;
		trimtrail = 1;
		sep = " \t";	/* note, code below knows this is 2 long */
		sepc = ' ';
	} else
		trimtrail = 0;
	sepc2 = sep[1];		/* now we can safely pick this up */

	/* catch empties */
	if (*p == '\0')
		return(0);

	/* single separator */
	if (sepc2 == '\0') {
		fn = nfields;
		for (;;) {
			*fp++ = p;
			fn--;
			if (fn == 0)
				break;
			while ((c = *p++) != sepc)
				if (c == '\0')
					return(nfields - fn);
			*(p-1) = '\0';
		}
		/* we have overflowed the fields vector -- just count them */
		fn = nfields;
		for (;;) {
			while ((c = *p++) != sepc)
				if (c == '\0')
					return(fn);
			fn++;
		}
		/* not reached */
	}

	/* two separators */
	if (sep[2] == '\0') {
		fn = nfields;
		for (;;) {
			*fp++ = p;
			fn--;
			while ((c = *p++) != sepc && c != sepc2)
				if (c == '\0') {
					if (trimtrail && **(fp-1) == '\0')
						fn++;
					return(nfields - fn);
				}
			if (fn == 0)
				break;
			*(p-1) = '\0';
			while ((c = *p++) == sepc || c == sepc2)
				continue;
			p--;
		}
		/* we have overflowed the fields vector -- just count them */
		fn = nfields;
		while (c != '\0') {
			while ((c = *p++) == sepc || c == sepc2)
				continue;
			p--;
			fn++;
			while ((c = *p++) != '\0' && c != sepc && c != sepc2)
				continue;
		}
		/* might have to trim trailing white space */
		if (trimtrail) {
			p--;
			while ((c = *--p) == sepc || c == sepc2)
				continue;
			p++;
			if (*p != '\0') {
				if (fn == nfields+1)
					*p = '\0';
				fn--;
			}
		}
		return(fn);
	}

	/* n separators */
	fn = 0;
	for (;;) {
		if (fn < nfields)
			*fp++ = p;
		fn++;
		for (;;) {
			c = *p++;
			if (c == '\0')
				return(fn);
			sepp = sep;
			while ((sepc = *sepp++) != '\0' && sepc != c)
				continue;
			if (sepc != '\0')	/* it was a separator */
				break;
		}
		if (fn < nfields)
			*(p-1) = '\0';
		for (;;) {
			c = *p++;
			sepp = sep;
			while ((sepc = *sepp++) != '\0' && sepc != c)
				continue;
			if (sepc == '\0')	/* it wasn't a separator */
				break;
		}
		p--;
	}
}

int isxdigit( char c )
{
    if( ( c >= 48 && c <= 57 ) || 
        ( c >= 65 && c <= 70 ) || 
        ( c >= 97 && c <= 102 ) )
        return 1;
    return 0;
}

int xtoi( char * x, unsigned int * result )
{
    UINT32 szlen = strlen( x );
    int j;

    if( szlen > 0 )
    {
        // Begin conversion here
        *result = 0;

        for( j = 0; j < szlen; j++ )
        {
            if( !isxdigit( x[ j ] ) ) return 3;
        
            *result <<= 4;
            
            if( x[ j ] >= 65 && x[ j ] <= 70 )
                *result |= ( x[ j ] - 65 ) + 10;
            else if( x[ j ] >= 97 && x[ j ] <= 102 )
                *result |= ( x[ j ] - 97 ) + 10;
            else
                *result |=  x[ j ] - 48;
        }
    }
    // Nothing to convert
    return 1;
}

char * trim( char * str )
{
    while( *str == '\n' ||
            *str == '\t' ||
            *str == '\r' ||
            *str == ' ' )
            *str++;
            
    return str;
}
