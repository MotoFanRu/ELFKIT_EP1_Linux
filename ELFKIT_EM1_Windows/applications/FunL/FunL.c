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

#include "FunL.h"

ElfLoaderApp         FunL    = { 0, FALSE, 0, 0, NULL, NULL };

const char app_name[APP_NAME_LEN] = "FunL"; 

typedef struct
{
    UINT8 magic[ 3 ];
    UINT8 version;
} FNL_Header; //

typedef struct
{
    UINT8 event;
    UINT32 time: 16; //fake UINT32, is 16bit long
} FNL_Event;

FNL_Event * FNLEvents = NULL;
UINT8 * EventStates = NULL;
UINT32 evs = 0;

#define STRUCTURE_SIZE_DEFINED_COS_GCC_IS_GAY 3

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_KEY_PRESS,                 HandleKeypress },
    { EV_TIMER_EXPIRED,       		Timer          },
    { EV_CALL_END,                  CallEnded      },
    { EV_CALL_RECEIVED,             CallReceived   },
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

    memcpy( ( void * )&FunL, ( void * )&ela, sizeof( ElfLoaderApp ) );

    status = APP_Register( &FunL . evcode,
                           1,
                           state_handling_table,
                           HW_STATE_MAX,
                           (void*)FunLStart );

    LoaderShowApp( &FunL );

    return 1;
}

UINT32 FunLStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_FUNL_T     *app = NULL;
    UINT32 status = RESULT_OK;

    app = (APP_FUNL_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_FUNL_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              2, 0, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_MAIN,
                        state_handling_table,
                        FunLExit,
                        app_name,
                        0 );

    LoadConfig( );
    
    return RESULT_OK;
}

UINT32 FunLExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    APP_UtilStopTimer( app );
    
    suFreeMem( EventStates );
    suFreeMem( FNLEvents );
    
    status = APP_Exit( ev_st, app, 0 );

    LoaderEndApp( &FunL );

    return status;
}

INT32 LoadConfig( )
{
    WCHAR * config = L"file://a/mobile/system/funlights.pat";
    
    FILE_HANDLE_T   f;
    UINT32 filesize, r;
    UINT8 * buffer;

    f = DL_FsOpenFile( config, FILE_READ_MODE, 0 );
    filesize = DL_FsGetFileSize( f );

    if( filesize <= 0 )
    {
        DL_FsCloseFile( f );
        return 1;
    }
 
    evs = ( filesize - 4 ) / 3;
    
    if( evs <= 0 )
    {
        DL_FsCloseFile( f );
        return 2;
    }

    FNLEvents   = ( FNL_Event * )suAllocMem( ( STRUCTURE_SIZE_DEFINED_COS_GCC_IS_GAY * evs ) + 3, NULL );
    EventStates = ( UINT8 * )suAllocMem( evs * 2, NULL );

    memset( EventStates, 0, evs );

    UINT32 Head;

    DL_FsReadFile( &Head, sizeof( UINT32 ), 1, f, &r );
    
    if( Head != 0x464E4C01L ) //FNL(01)
        return 3;

    int i;
    
    for( i = 0; i < evs; i++ )
    {
        DL_FsReadFile( &FNLEvents[ i ], STRUCTURE_SIZE_DEFINED_COS_GCC_IS_GAY, 1, f, &r );
    }
    
    DL_FsCloseFile( f );
    
    return RESULT_OK;
}

enum
{
    FNL_FLASH   = 1,
    FNL_KEYPAD  = 2,
    FNL_DISPLAY = 4,
    FNL_CLI     = 8,
    FNL_LED     = 16,
    FNL_KPTOP   = 32,
    FNL_KPBOT   = 64
};

asm
(
"ll_call:                \n"
    "lrw    r7, 0x8000064\n"
    "subi   sp, 8        \n"
    "ixw    r7, r3       \n"
    "ld.w   r7, (r7, 0)  \n"
    "st.w   r15, (sp, 0) \n"
    "jsr    r7           \n"
    "ld.w   r15, (sp, 0) \n"
    "addi   sp, 8        \n"
    "jmp    r15          \n"
);

#define STATE_ON    1
#define STATE_OFF   0

enum
{
    FUNCTION_KEYPAD = 0,    //0
    FUNCTION_DISPLAY,       //4
    FUNCTION_FLASHLIGHT,    //8
    FUNCTION_CLI,           //C   
    FUNCTION_KEYPAD_BOTTOM, //10
    FUNCTION_KEYPAD_TOP,    //14
    FUNCTION_NULL,          //18
    FUNCTION_CHARGING_LED   //1C
};

extern void ll_call( int p1, int function );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    return RESULT_OK;
}

int FlipState = 1;

UINT32 FlipOpen( EVENT_STACK_T *ev_st,  void *app )
{
    FlipState = 1;
    return RESULT_OK;
}

UINT32 FlipClose( EVENT_STACK_T *ev_st,  void *app )
{
    FlipState = 0;
    return RESULT_OK;
}

UINT8 state = 1;
UINT32 curr_ev = 0;

BOOL InCall = 0;

UINT32 CallReceived( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32 curr_ev;

    InCall = 1;

    for( curr_ev = 0; curr_ev < evs; curr_ev++ )
    {
        APP_UtilStartCyclicalTimer( FNLEvents[ curr_ev ] . time, curr_ev, app );
    }

    return RESULT_OK;
}

void ResetStates( )
{
    ll_call( STATE_OFF, FUNCTION_CHARGING_LED );
    ll_call( STATE_OFF, FUNCTION_FLASHLIGHT );
    ll_call( STATE_ON, FUNCTION_DISPLAY );
}

UINT32 CallEnded( EVENT_STACK_T *ev_st,  void *app )
{
    InCall = 0;
    ResetStates( );
    return RESULT_OK;
}

UINT32 Timer( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;

    EVENT_T * ev = ( EVENT_T * )AFW_GetEv( ev_st );
    DL_TIMER_DATA_T * _timer = ( DL_TIMER_DATA_T * )&ev->data;

    UINT32 event = _timer -> ID;

    if( event > evs || event < 0 )
        return RESULT_OK;
        
    if( !InCall )
    {
        APP_UtilStopTimer( app );
        return RESULT_OK;
    }

    EventStates[ event ] = !EventStates[ event ];
    
    if( FNLEvents[ event ] . event & FNL_FLASH )
        ll_call( EventStates[ event ], FUNCTION_FLASHLIGHT );
    
    if( FNLEvents[ event ] . event & FNL_KEYPAD )
        ll_call( EventStates[ event ], FUNCTION_KEYPAD );
            
    if( ( FNLEvents[ event ] . event & FNL_DISPLAY ) && FlipState )
        ll_call( EventStates[ event ], FUNCTION_DISPLAY );
    
    if( ( FNLEvents[ event ] . event & FNL_CLI ) && !FlipState )
        ll_call( EventStates[ event ], FUNCTION_CLI );
        
    if( FNLEvents[ event ] . event & FNL_LED )
        ll_call( EventStates[ event ], FUNCTION_CHARGING_LED );
                
    if( FNLEvents[ event ] . event & FNL_KPTOP )
        ll_call( EventStates[ event ], FUNCTION_KEYPAD_TOP );
        
    if( FNLEvents[ event ] . event & FNL_KPBOT )
        ll_call( EventStates[ event ], FUNCTION_KEYPAD_BOTTOM );

    return RESULT_OK;
}

UINT32 HandleKeypress( EVENT_STACK_T * ev_st,  void * app )
{
    EVENT_T * ev = ( EVENT_T * )AFW_GetEv( ev_st );
    UINT8 keypress = ev -> data . key_pressed;
    if( ( keypress == KEY_GREEN || keypress == KEY_RED ) && InCall )
    {
        ResetStates( );
        InCall = 0;
        //FunLExit( ev_st, app );
    }

    return RESULT_OK;
}

