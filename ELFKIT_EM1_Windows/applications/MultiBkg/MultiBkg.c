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



#include "MultiBkg.h"

UINT32 ListBkgs( EVENT_STACK_T *ev_st,  void *app );

ElfLoaderApp         MultiBkg    = { 0, FALSE, 0, 0, NULL, NULL };
FILEINFO           * fname       = NULL;
UINT32               bkgs        = 0;
UINT32               current_bkg = 0;

const char app_name[APP_NAME_LEN] = "MultiBkg"; 

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_TIMER_EXPIRED,       		Timer          },
    { EV_FLIP_CLOSE,                FlipClose      },
    { EV_FLIP_OPEN,                 FlipOpen       },
    { STATE_HANDLERS_END,           NULL           },
};

static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { HW_STATE_ANY,
      NULL,
      NULL,
      any_state_handlers
    },

    { HW_STATE_MAIN,
      MainStateEnter,
      NULL,
      main_state_handlers
    }
};

int _main( ElfLoaderApp ela )
{
    UINT32 status = RESULT_OK;

    memcpy( ( void * )&MultiBkg, ( void * )&ela, sizeof( ElfLoaderApp ) );

    status = APP_Register( &MultiBkg . evcode,
                           1,
                           state_handling_table,
                           HW_STATE_MAX,
                           (void*)MultiBkgStart );

    LoaderShowApp( &MultiBkg );

    return 1;
}

UINT32 MultiBkgStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_MULTIBKG_T     *app = NULL;
    UINT32 status = RESULT_OK;

    app = (APP_MULTIBKG_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_MULTIBKG_T),
                                              reg_id,
                                              0, 1,
                                              2,
                                              2, 0, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_MAIN,
                        state_handling_table,
                        MultiBkgExit,
                        app_name,
                        0 );

    APP_UtilStartTimer( 10000, 1, app );
    
    return RESULT_OK;
}

BOOL KillMe = FALSE;

UINT32 ListBkgs( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32                  i;
	FS_SEARCH_PARAMS_T      fs_param;
    FS_SEARCH_HANDLE_T      fs_handle;
    FS_SEARCH_RESULT_T      fs_result; 

    WCHAR                   filter[] = L"file://e/mobile/picture/multi/\xFFFE*.jpg\xFFFE*.png\xFFFE*.gif";

	UINT16 res_count, count = 1; 
    fs_param.flags = 0x1C;
	fs_param.attrib = 0x0000;
	fs_param.mask = 0x0000; 

	DL_FsSSearch( fs_param, filter, &fs_handle, &res_count, 0x0); 

    bkgs = res_count;
    
    if( bkgs <= 0 )
    {
        PFprintf( "No bkgs\n" );
        KillMe = TRUE;
        return NULL;
    }

    if ( fname != NULL ) suFreeMem( fname );

    fname = ( FILEINFO * ) suAllocMem( bkgs * sizeof( FILEINFO ), NULL );

    for (i=0; i < bkgs; i++)
    {
        DL_FsSearchResults( fs_handle, i, &count, &fs_result );
        u_strcpy( fname[i].u_fullname,  fs_result.name );
    }

    DL_FsSearchClose(fs_handle);

	return RESULT_OK;

}

UINT32 MultiBkgExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    APP_UtilStopTimer( app );

    status = APP_Exit( ev_st, app, 0 );
    
    suFreeMem( fname );
    
    LoaderEndApp( &MultiBkg );

    return status;
}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    ListBkgs( ev_st, app );
    return RESULT_OK;
}

UINT32 FlipOpen( EVENT_STACK_T *ev_st,  void *app )
{
    return RESULT_OK;
}

UINT32 FlipClose( EVENT_STACK_T *ev_st,  void *app )
{
    return RESULT_OK;
}

BOOL InitTimer = TRUE;

UINT32 Timer( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;

    SETWALLPAPER_SETTINGS_T wp;

    DL_TIMER_DATA_T timer;

    EVENT_T * ev = ( EVENT_T * )AFW_GetEv( ev_st );
    DL_TIMER_DATA_T * _timer = ( DL_TIMER_DATA_T * )&ev->data;

    if( KillMe )     MultiBkgExit( ev_st, app );

    if( _timer -> ID == 1 )
    {
        wp . url = fname[ current_bkg ] . u_fullname;
        UIS_SetWallpaper( &wp );
    
        if( current_bkg < bkgs - 1 )
            current_bkg++;
        else
            current_bkg = 0;
            
        APP_UtilStopTimer( app );
        APP_UtilStartTimer( 30000, 1, app );
    }

	return RESULT_OK;
}

