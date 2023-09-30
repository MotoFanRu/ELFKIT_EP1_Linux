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

#include <filesystem.h>
#include <util.h>
#include <dl.h>
#include <uis.h>
#include <events.h>
#include <time_date.h>

#define APP_FILE_HANDLE  ( ( FILE_HANDLE_T * )0x9400000 )[ 0 ]
#define BUFFER           ( ( UINT32 * )0x9402000 )

#define APP_BASE 0x09200000

WCHAR rloader[] = { 'f', 'i', 'l', 'e', ':', '/', '/', 'a', '/', 'l', 'd', 'r', '.', 'b', 'i', 'n', 0 };

typedef int ( *f_Entry )( );
f_Entry Entry = ( f_Entry )APP_BASE;

typedef struct
{ 
    APPLICATION_T           apt; 
} APP_INFO_T;

typedef enum
{
    HW_STATE_ANY,
    HW_STATE_MAIN,
   
    HW_STATE_MAX
} HW_STATES_T;

const char app_name[APP_NAME_LEN] = "MainLdr"; 

UINT32 InfoStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 InfoExit( EVENT_STACK_T *ev_st,  void *app );
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type );

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
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

UINT32 evcode = 0x9FFF;
#define FONT_TABLE_ADDRESS 0x09500000

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

int beginning( )
{
    memcpy( ( void * )FONT_TABLE_ADDRESS, ( void * )default_font_table, 96 );

    APP_Register( &evcode,
                  1,
                  state_handling_table,
                  HW_STATE_MAX,
                  (void*)InfoStart );

    AFW_CreateInternalQueuedEvAux( evcode, FBF_LEAVE, 0, 0 );

    return 0;
}

UINT32 InfoExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    status = APP_Exit( ev_st, app, 0 );

    return status;
}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    APP_FILE_HANDLE = DL_FsOpenFile( rloader, FILE_READ_MODE, NULL );

    if( DL_FsGetFileSize( APP_FILE_HANDLE ) > 0 )
    {
        DL_FsReadFile( ( char * )APP_BASE,
                        DL_FsGetFileSize( APP_FILE_HANDLE ),
                        1,
                        APP_FILE_HANDLE,
                        BUFFER );

        DL_FsCloseFile( APP_FILE_HANDLE );

        if( ( ( char * )APP_BASE )[ 0 ] != 0 )
            Entry( );
    }

    return RESULT_OK;
}

UINT32 InfoStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_INFO_T     *app = NULL;

    app = (APP_INFO_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_INFO_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              2, 0, 0 );

    APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_MAIN,
                        state_handling_table,
                        InfoExit,
                        app_name,
                        0 );
             
    return RESULT_OK;
}
