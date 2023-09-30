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

#include "GetMyPDS.h"

ElfLoaderApp         GetMyPDS = { 0, FALSE, 0, 0, NULL, NULL };

const char app_name[APP_NAME_LEN] = "GetMyPDS"; 

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
    { EV_DIALOG_DONE,               HelloWorldExit },
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
      NULL,
      main_state_handlers
    }
};


typedef UINT32 (*f_FlashDevEraseBlock)( UINT32 addr );
f_FlashDevEraseBlock FlashDevEraseBlock = ( f_FlashDevEraseBlock )0x101CEE68;


int _main( ElfLoaderApp ela )
{
    UINT32 status = RESULT_OK;

    memcpy( ( void * )&GetMyPDS, ( void * )&ela, sizeof( ElfLoaderApp ) );

    status = APP_Register( &GetMyPDS . evcode,
                           1,
                           state_handling_table,
                           HW_STATE_MAX,
                           (void*)HelloWorldStart );

    LoaderShowApp( &GetMyPDS );

    return 1;
}


UINT32 HelloWorldStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_HELLOWORLD_T     *app = NULL;
    UINT32 status = RESULT_OK;

    app = (APP_HELLOWORLD_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_HELLOWORLD_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              1, 1, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_INIT,
                        state_handling_table,
                        HelloWorldExit,
                        app_name,
                        0 );

    return RESULT_OK;
}


UINT32 HelloWorldExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    APP_UtilUISDialogDelete(  &papp->dialog );

    status = APP_Exit( ev_st, app, 0 );
    
    LoaderEndApp( &GetMyPDS );

    return status;
}

#define PDS_AREA      0x10010000
#define PDS_AREA_SIZE 0x20000
WCHAR pds[] = {'f', 'i', 'l', 'e', ':', '/', '/', 'a', '/', 'p', 'd', 's', '.', 'b', 'a', 'k', 0};

void Backup( )
{
    FILE_HANDLE_T hnd_pds;
    UINT32 written;

    hnd_pds = DL_FsOpenFile( pds, FILE_WRITE_PLUS_MODE, NULL );

    DL_FsWriteFile( ( char * )PDS_AREA, PDS_AREA_SIZE, 1, hnd_pds, &written );
    
    DL_FsCloseFile( hnd_pds );
}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    CONTENT_T               content;
    UIS_DIALOG_T            dialog = 0;
    DRAWING_BUFFER_T		bufd;

	WCHAR done[] = {'P','D','S',' ','S', 'a', 'v', 'e', 'd', ' ', 't', 'o', ' ', 0};

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;

    Backup( );
    
    UIS_MakeContentFromString("MCq0q1", &content, done, pds);
    dialog = UIS_CreateTransientNotice( &port, &content, NOTICE_TYPE_OK );

    if(dialog == 0) return RESULT_FAIL;

    papp->dialog = dialog;

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
