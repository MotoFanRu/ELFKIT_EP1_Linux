#include "FlexEditor.h"

int	split( char *, char **, int, char * );
int xtoi( char *, unsigned int * );

#define  EVENT_DO   0xE000

typedef struct
{
    WCHAR Name[ 64 ];
    UINT32 SeemNo;
    UINT16 Offset;
    UINT8 Bit;
    BOOL ReservedItem;
} FlexEdit;

FlexEdit * FlexEdits = NULL;

UINT32              SELECTED_IDX = 0;
UINT8               * SEEM_00XX = NULL;
UINT32              ENTRIES_NUM = 0;

LIST_ENTRY_T		* plist=NULL;

ElfLoaderApp         FlexEditor = { 0, FALSE, 0, 0, NULL, NULL };

const char app_name[APP_NAME_LEN] = "FlexEditor"; 

UINT32 Do_Action( EVENT_STACK_T *ev_st,  void *app );
UINT32 HandleListReq( EVENT_STACK_T *ev_st,  void *app );
UINT32 Navigate (EVENT_STACK_T *ev_st,  void *app );

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
    { EV_DIALOG_DONE,               FlexEditorExit },
    { EV_REQUEST_LIST_ITEMS,        HandleListReq  },
    { EV_LIST_NAVIGATE,				Navigate       }, 
    { EV_SELECT,                    Select         },
    { EVENT_DO,                     Do_Action      },
    { EV_DONE,                      FlexEditorExit },
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

int _main( ElfLoaderApp ela )
{
    UINT32 status = RESULT_OK;

    memcpy( ( void * )&FlexEditor, ( void * )&ela, sizeof( ElfLoaderApp ) );

    status = APP_Register( &FlexEditor . evcode,
                           1,
                           state_handling_table,
                           HW_STATE_MAX,
                           (void*)FlexEditorStart );

    LoaderShowApp( &FlexEditor );

    return 1;
}


UINT32 FlexEditorStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_FLEXEDITOR_T     *app = NULL;
    UINT32 status = RESULT_OK;

    app = (APP_FLEXEDITOR_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_FLEXEDITOR_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              1, 1, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_INIT,
                        state_handling_table,
                        FlexEditorExit,
                        app_name,
                        0 );

    return RESULT_OK;
}

UINT32 Navigate (EVENT_STACK_T *ev_st,  void *app )
{
    EVENT_T     *event = ( EVENT_T     * )AFW_GetEv(ev_st);

	SELECTED_IDX = event->data.index;

    APP_ConsumeEv( ev_st, app );

	return RESULT_OK;
}

UINT32 FlexEditorExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    suFreeMem( FlexEdits );

    RemoveResources( );

    APP_UtilUISDialogDelete(  &papp->dialog );

    status = APP_Exit( ev_st, app, 0 );
    
    LoaderEndApp( &FlexEditor );

    return status;
}


void LoadFlexDB( )
{
    WCHAR flexdb[] =  {'f', 'i', 'l', 'e', ':', '/', '/', 'a', '/', 'f', 'l', 'e', 'x', '_', 'e', 'd', 'i', 't', 'o', 'r', '.', 'c', 'f', 'g', 0};
    UINT32 filesize;
    UINT32 r;
    FILE_HANDLE_T   f;
    char * buffer;
    char ** edits;
    char ** edit;
    int res;
    int rows = 0;
    
    f = DL_FsOpenFile( flexdb, FILE_READ_MODE, 0 );
    filesize = DL_FsGetFileSize( f );
    
    if( filesize <= 0 )
    {
        DL_FsCloseFile( f );
        return;
    }
    
    buffer = ( char * )suAllocMem( filesize, NULL );
    DL_FsReadFile( buffer, filesize, 1, f, &r );
    DL_FsCloseFile( f );
    
    int i;
    for( i = 0; i < filesize; i++ )
        if( buffer[ i ] == 0x0A ) rows++;
    
    edits = ( char ** )suAllocMem( ( rows + 1 ) * 64, NULL );
    edit  = ( char ** )suAllocMem( 5 * 64, NULL );
    
    FlexEdits = ( FlexEdit * )suAllocMem( ( rows + 1 ) * sizeof( FlexEdit ), NULL );
    
    split( buffer, edits, rows, "\n" );
    
    ENTRIES_NUM = 0;
    
    for( i = 0; i < rows; i++ )
    {
        split( edits[ i ], edit, 4, ";" );

        u_atou( edit[ 0 ], FlexEdits[ i ] . Name ); //name into structure
        xtoi( edit[ 1 ], &res );
        FlexEdits[ i ] . SeemNo = res;
        xtoi( edit[ 2 ], &res );
        FlexEdits[ i ] . Offset = res;      
        FlexEdits[ i ] . Bit = edit[ 3 ][ 0 ] - '0'; //bit
        FlexEdits[ i ] . ReservedItem = FALSE;

        ENTRIES_NUM++;
    } 
    
    FlexEdits[ i ] . ReservedItem = TRUE;

    suFreeMem( edit );
    suFreeMem( edits );
}

UINT32					starting_num = 0;

UINT32 Update( EVENT_STACK_T *ev_st,  void *app )
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

    SendListItems( ev_st, app, 1, starting_num );

    return RESULT_OK;
}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
	if(type!=ENTER_STATE_ENTER) return RESULT_OK;
	
    LoadFlexDB( );
    ENTRIES_NUM += 1;
    return Update( ev_st, app );
}

typedef struct
{
    UINT8 opt7: 1;
    UINT8 opt6: 1;
    UINT8 opt5: 1;
    UINT8 opt4: 1;
    UINT8 opt3: 1;
    UINT8 opt2: 1;
    UINT8 opt1: 1;
    UINT8 opt0: 1;
} SeemOff;

void FeatureSet( UINT32 seem, UINT16 offset, UINT8 bit )
{
    SeemOff * seem_offset;

    SEEM_00XX = ( UINT8 * )suAllocMem( 2048, NULL );
    SEEM_FDI_OUTSIDE_SEEM_ACCESS_read( seem, 1, SEEM_00XX, 1024 );

    seem_offset = ( SeemOff * )&SEEM_00XX[ offset ];

    switch( bit )
    {
        case 0:
            seem_offset -> opt0 = !seem_offset -> opt0;
        break;
        case 1:
            seem_offset -> opt1 = !seem_offset -> opt1;
        break;
        case 2:
            seem_offset -> opt2 = !seem_offset -> opt2;
        break;
        case 3:
            seem_offset -> opt3 = !seem_offset -> opt3;
        break;
        case 4:
            seem_offset -> opt4 = !seem_offset -> opt4;
        break;
        case 5:
            seem_offset -> opt5 = !seem_offset -> opt5;
        break;
        case 6:
            seem_offset -> opt6 = !seem_offset -> opt6;
        break;
        case 7:
            seem_offset -> opt7 = !seem_offset -> opt7;
        break;
    }

    SEEM_FDI_OUTSIDE_SEEM_ACCESS_write( 0, seem, 1, SEEM_00XX, 1024 );

    suFreeMem( SEEM_00XX );
}

BOOL FeatureTest( UINT32 seem, UINT16 offset, UINT8 bit )
{
    SeemOff * seem_offset;
    BOOL val;
    
    SEEM_00XX = ( UINT8 * )suAllocMem( 2048, NULL );
    SEEM_FDI_OUTSIDE_SEEM_ACCESS_read( seem, 1, SEEM_00XX, 1024 );

    seem_offset = ( SeemOff * )&SEEM_00XX[ offset ];

    switch( bit )
    {
        case 0:
            val = seem_offset -> opt0;
        break;
        case 1:
            val = seem_offset -> opt1;
        break;
        case 2:
            val = seem_offset -> opt2;
        break;
        case 3:
            val = seem_offset -> opt3;
        break;
        case 4:
            val = seem_offset -> opt4;
        break;
        case 5:
            val = seem_offset -> opt5;
        break;
        case 6:
            val = seem_offset -> opt6;
        break;
        case 7:
            val = seem_offset -> opt7;
        break;
    }

    suFreeMem( SEEM_00XX );
    
    return val;
}

WCHAR                   reboot[] = {'R', 'e', 'b', 'o', 'o', 't', 0 };
WCHAR                   p1[] = { '[', 0 };
WCHAR                   p2[] = { ']', 0 };

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

        if( FlexEdits[ i - 1 ] . ReservedItem )
        {
            UIS_MakeContentFromString( "q1s0q2", &( plist[index].content.static_entry.text ), LANG_RESTART, p1, p2 ); 
            continue;
        }

        if( FeatureTest( FlexEdits[ i - 1 ] . SeemNo, FlexEdits[ i - 1 ] . Offset, FlexEdits[ i - 1 ] . Bit ) )
            UIS_MakeContentFromString( "q0Sp1", &( plist[index].content.static_entry.text ), FlexEdits[ i - 1 ] . Name, DRM_SELECTED );
        else
            UIS_MakeContentFromString( "q0", &( plist[index].content.static_entry.text ), FlexEdits[ i - 1 ] . Name );
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

UINT32 Do_Action( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T			*papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    CONTENT_T               content;
    UIS_DIALOG_T            dialog = 0;
    EVENT_T     *event = ( EVENT_T     * )AFW_GetEv(ev_st);

    APP_ConsumeEv( ev_st, app );

    if( SELECTED_IDX == ENTRIES_NUM )
        pu_main_powerdown( 1 ); /*1: reboot, 0: normal pwrdwn, 3: boot*/

    FeatureSet( FlexEdits[ SELECTED_IDX - 1 ] . SeemNo, FlexEdits[ SELECTED_IDX - 1 ] . Offset, FlexEdits[ SELECTED_IDX - 1 ] . Bit );

	return Update( ev_st, app );
}


UINT32 Select( EVENT_STACK_T *ev_st,  void *app)
{
    Do_Action( ev_st, app );

	return RESULT_OK;
}


UINT32 InitResources( )
{
    UINT32						status;
    RES_ACTION_LIST_ITEM_T		action[1];
    
	WCHAR list_caption[] = { 'C', 'o', 'r', '3', '\'', 's', ' ', 'F', 'l', 'e', 'x', 'E', 'd', 'i', 't', 'o', 'r', 0 };

    Resources = ( RESOURCE_ID * )suAllocMem( sizeof( RESOURCE_ID ) * ( RES_MAX + 1 ), NULL );

    status = DRM_CreateResource( &Resources[RES_LIST_CAPTION], RES_TYPE_STRING, (void*)list_caption, (u_strlen(list_caption)+1)*sizeof(WCHAR) );

    Resources[RES_SOFTKEY_LABEL] = LANG_APPLY;

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

UINT32 StateExit( EVENT_STACK_T *ev_st,  void *app,  EXIT_STATE_TYPE_T type )
{
	APP_FLEXEDITOR_T *papp = (APP_FLEXEDITOR_T*) app;
	APP_UtilUISDialogDelete( &papp->apt.dialog );
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

	/* not reached */
}

int isxdigit(char c)
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
