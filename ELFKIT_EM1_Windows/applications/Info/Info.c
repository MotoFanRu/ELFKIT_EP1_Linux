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


#include "Info.h"

ElfLoaderApp         Info = { 0, FALSE, 0, 0, NULL, NULL };

const char app_name[APP_NAME_LEN] = "Info"; 

RESOURCE_ID * Resources = NULL;

UINT8 * font_buffer = NULL;
IMG_FONT_T * font = NULL;
RESOURCE_ID * font_res = NULL;

const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_KEY_PRESS,                 HandleKeypress },
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

    memcpy( ( void * )&Info, ( void * )&ela, sizeof( ElfLoaderApp ) );

    status = APP_Register( &Info . evcode,
                           1,
                           state_handling_table,
                           HW_STATE_MAX,
                           (void*)InfoStart );

    LoaderShowApp( &Info );

    return 1;
}

UINT32 * PicturesBuffers[] =
{
    NULL,
    NULL
};

WCHAR Pictures[][64] = {
    L"file://a/mobile/picture/info/b100.gif",
    L"file://a/mobile/picture/info/b080.gif",
    L"file://a/mobile/picture/info/b070.gif",
    L"file://a/mobile/picture/info/b050.gif",
    L"file://a/mobile/picture/info/b035.gif",
    L"file://a/mobile/picture/info/b015.gif",
    L"file://a/mobile/picture/info/b005.gif",
    
    L"file://a/mobile/picture/info/s100.gif",
    L"file://a/mobile/picture/info/s080.gif",
    L"file://a/mobile/picture/info/s060.gif",
    L"file://a/mobile/picture/info/s040.gif",
    L"file://a/mobile/picture/info/s020.gif",
    L"file://a/mobile/picture/info/s000.gif",
    
    L"file://a/mobile/picture/info/p100.gif",
    L"file://a/mobile/picture/info/p090.gif",
    L"file://a/mobile/picture/info/p080.gif",
    L"file://a/mobile/picture/info/p070.gif",
    L"file://a/mobile/picture/info/p060.gif",
    L"file://a/mobile/picture/info/p050.gif",
    L"file://a/mobile/picture/info/p040.gif",
    L"file://a/mobile/picture/info/p030.gif",
    L"file://a/mobile/picture/info/p020.gif",
    L"file://a/mobile/picture/info/p010.gif",
    L"file://a/mobile/picture/info/p000.gif"
};

enum
{
    RES_BATTERY_100 = 0,
    RES_BATTERY_080,
    RES_BATTERY_070,
    RES_BATTERY_050,
    RES_BATTERY_035,
    RES_BATTERY_015,
    RES_BATTERY_005,
    
    RES_SIGNAL_100,
    RES_SIGNAL_080,
    RES_SIGNAL_060,
    RES_SIGNAL_040,
    RES_SIGNAL_020,
    RES_SIGNAL_000,
    
    RES_PROGRESS_100,
    RES_PROGRESS_090,
    RES_PROGRESS_080,
    RES_PROGRESS_070,
    RES_PROGRESS_060,
    RES_PROGRESS_050,
    RES_PROGRESS_040,
    RES_PROGRESS_030,
    RES_PROGRESS_020,
    RES_PROGRESS_010,
    RES_PROGRESS_000
};

#define PICS 24
UINT8 ClockState = 0;

char * VolumeInfo = NULL;

UINT32 InfoStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )
{
    APP_INFO_T     *app = NULL;
    UINT32 status = RESULT_OK;

    app = (APP_INFO_T*)APP_InitAppData( (void *)APP_HandleEvent,
                                              sizeof(APP_INFO_T),
                                              reg_id,
                                              0, 1,
                                              1,
                                              2, 0, 0 );

    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_MAIN,
                        state_handling_table,
                        InfoExit,
                        app_name,
                        0 );
    
    FILE_HANDLE_T f;
    UINT32 filesize;
    UINT32 r;

    LoadFont( );

    Resources = ( RESOURCE_ID * )suAllocMem( sizeof( RESOURCE_ID ) * PICS, NULL );

    int i;
    for( i = 0; i < PICS; i++ )
    {
        f = DL_FsOpenFile( Pictures[ i ], FILE_READ_MODE, 0 );
        filesize = DL_FsGetFileSize( f );

        if( filesize <= 0 )
        {
            DL_FsCloseFile( f );
            continue;
        }
        
        PicturesBuffers[ i ] = ( UINT32 * )suAllocMem( filesize, NULL );
        DL_FsReadFile( PicturesBuffers[ i ], filesize, 1, f, &r );
        DL_FsCloseFile( f );

        DRM_CreateResource( &Resources[ i ], RES_TYPE_GRAPHICS, PicturesBuffers[ i ], filesize );
    }
    
    VolumeInfo = ( char * )suAllocMem( 512, NULL );

    APP_UtilStartCyclicalTimer( 5000, 1, app );
    
    DL_DbFeatureGetCurrentState( ID_CLOCK, &ClockState );
    DL_DbFeatureStoreState( ID_CLOCK, 2 ); //off
    
    return RESULT_OK;
}

UINT32 InfoExit( EVENT_STACK_T *ev_st,  void *app )
{
    UINT32  status;
    APPLICATION_T           *papp = (APPLICATION_T*) app;

    APP_UtilStopTimer( app );

    DL_DbFeatureStoreState( ID_CLOCK, ClockState );

    int i;
    for( i = 0; i < PICS; i++ )
    {
        DRM_ClearResource( Resources[ i ] );
        suFreeMem( PicturesBuffers[ i ] );
    }
    
    suFreeMem( VolumeInfo );
    suFreeMem( Resources );

    status = APP_Exit( ev_st, app, 0 );
    
    LoaderEndApp( &Info );

    return status;
}

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  void *app,  ENTER_STATE_TYPE_T type )
{
    return RESULT_OK;
}

typedef struct
{
    INT16 battery_x;
    INT16 battery_y;
    UINT16 battery_h;
    UINT16 battery_w;
    
    INT16 signal_x;
    INT16 signal_y;
    UINT16 signal_h;
    UINT16 signal_w;
    
    INT16 time_x;
    INT16 time_y;    
    
    INT16 freespace_x;
    INT16 freespace_y;
    UINT16 freespace_h;
    UINT16 freespace_w;
} info_config;

info_config Config = { -5, 200, 80, 80, 140, 200, 80, 80, 55, 80, 25, 130, 4, 172 };

UINT8 BatteryAdjust( UINT8 val, UINT8 mode )
{
    if( !mode )
    {
        if( val >= 60 )
            return RES_BATTERY_100;
        else if( val <  60 && val >= 50 )
            return RES_BATTERY_080;
        else if( val <  50 && val >= 40 )
            return RES_BATTERY_070;
        else if( val <  40 && val >= 30 )
            return RES_BATTERY_050;
        else if( val <  30 && val >= 20 )
            return RES_BATTERY_035;
        else if( val <  20 && val >= 10 )
            return RES_BATTERY_015;
        else if( val <  10 )
            return RES_BATTERY_005;
    }
    else
    {
        if( val >= 60 )
            return 100;
        else if( val <  60 && val >= 50 )
            return 80;
        else if( val <  50 && val >= 40 )
            return 70;
        else if( val <  40 && val >= 30 )
            return 50;
        else if( val <  30 && val >= 20 )
            return 35;
        else if( val <  20 && val >= 10 )
            return 15;
        else if( val <  10 )
            return 5;
    }
}

UINT8 SignalAdjust( UINT8 val )
{
    if( val >= 80 )
        return RES_SIGNAL_100;
    else if( val >= 60 )
        return RES_SIGNAL_080;
    else if( val >= 40 )
        return RES_SIGNAL_060;
    else if( val >= 20 )
        return RES_SIGNAL_040;
    else if( val > 0 )
        return RES_SIGNAL_020;
    else
        return RES_SIGNAL_000;

}

UINT8 battery = 0;
SIGNAL_STRENGTH_T signal = { 0, 0 };
CLK_TIME_T time = {0, 0, 0};
VOLUME_DESCR_T * volnfo = NULL;
UINT8 vols = 0;

void GetInfo( )
{ 
    battery = DL_PwrGetActiveBatteryPercent( );
    DL_SigRegQuerySignalStrength( &signal );
    DL_ClkGetTime( &time );

	WCHAR Volumes[ 32 ];
	DL_FsVolumeEnum( Volumes );
	
	sprintf( VolumeInfo, "" );
		
	int i = 0, j = 0;
    vols = ( ( u_strlen( Volumes ) * 2 ) / 6 ) + 1;
    
    if( volnfo == NULL )
        volnfo = ( VOLUME_DESCR_T * )suAllocMem( sizeof( VOLUME_DESCR_T ) * vols, NULL );

	for( i = 3; i < u_strlen( Volumes ) * 2; i += 6 )
	{
        VOLUME_DESCR_T buff;
        
        WCHAR vol[ 3 ];
        vol[ 0 ] = '/';
        vol[ 1 ] = ( WCHAR )( ( char * )Volumes )[i];
        vol[ 2 ] = 0;
        
        DL_FsGetVolumeDescr( vol, &volnfo[ j++ ] );
	}
}

void UpdateInfo( )
{
    GRAPHIC_REGION_T region;

    region . ulc . x = Config . battery_x;
    region . ulc . y = Config . battery_y;
    region . lrc . x = Config . battery_x + Config . battery_w;
    region . lrc . y = Config . battery_y + Config . battery_h;

    utility_draw_image( Resources[ BatteryAdjust( battery, 0 ) ], region, NULL, NULL, 0 );
        
    region . ulc . x = Config . signal_x;
    region . ulc . y = Config . signal_y;
    region . lrc . x = Config . signal_x + Config . signal_w;
    region . lrc . y = Config . signal_y + Config . signal_h;

    utility_draw_image( Resources[ SignalAdjust( signal . percent ) ], region, NULL, NULL, 0 );
    
    char xCent[ 32 ];
    
    sprintf( xCent, "%i%%", BatteryAdjust( battery, 1 ) );
    DrawStrWT( xCent, Config . battery_x + 25, Config . battery_y - 10, 0 );
    
    sprintf( xCent, "%i%%", signal . percent );
    DrawStrWT( xCent, Config . signal_x + 25, Config . signal_y - 10, 0 );

    sprintf( xCent, "%02i:%02i", time . hour, time . minute );
    DrawStrWT( xCent, Config . time_x, Config . time_y, 1 );

    int i, j = 0;
    for( i = 0; i < vols; i++ )
    {
        UINT32 percent = ( volnfo[ i ] . free_size * 100 ) / volnfo[ i ] . vol_size;
            
        region . ulc . x = Config . freespace_x;
        region . ulc . y = Config . freespace_y + ( i * ( Config . freespace_h * 6 ) );
        region . lrc . x = Config . freespace_x + Config . freespace_w;
        region . lrc . y = Config . freespace_y + ( i * ( Config . freespace_h * 6 ) ) + Config . freespace_h;

        if( percent <= 0 )
            utility_draw_image( Resources[ RES_PROGRESS_000 ], region, NULL, NULL, 0 );
        else if( percent > 0 && percent <= 10 )
            utility_draw_image( Resources[ RES_PROGRESS_010 ], region, NULL, NULL, 0 );
        else if( percent > 10 && percent <= 20 )
            utility_draw_image( Resources[ RES_PROGRESS_020 ], region, NULL, NULL, 0 );
        else if( percent > 20 && percent <= 30 )
            utility_draw_image( Resources[ RES_PROGRESS_030 ], region, NULL, NULL, 0 );
        else if( percent > 30 && percent <= 40 )
            utility_draw_image( Resources[ RES_PROGRESS_040 ], region, NULL, NULL, 0 );
        else if( percent > 40 && percent <= 50 )
            utility_draw_image( Resources[ RES_PROGRESS_050 ], region, NULL, NULL, 0 );
        else if( percent > 50 && percent <= 60 )
            utility_draw_image( Resources[ RES_PROGRESS_060 ], region, NULL, NULL, 0 );
        else if( percent > 60 && percent <= 70 )
            utility_draw_image( Resources[ RES_PROGRESS_070 ], region, NULL, NULL, 0 );
        else if( percent > 70 && percent <= 80 )
            utility_draw_image( Resources[ RES_PROGRESS_080 ], region, NULL, NULL, 0 );
        else if( percent > 80 && percent <= 90 )
            utility_draw_image( Resources[ RES_PROGRESS_090 ], region, NULL, NULL, 0 );
        else if( percent > 90 )
            utility_draw_image( Resources[ RES_PROGRESS_100 ], region, NULL, NULL, 0 );
        
        sprintf( VolumeInfo, "%c %i kb free (%i%%)\n", ( ( char * )volnfo[ i ] . volume )[ 3 ], volnfo[ i ] . free_size / 1024, percent );
        DrawStrWT( VolumeInfo, region . ulc . x - 20, region . ulc . y - 15, 0);
    }
}

UINT32 FlipOpen( EVENT_STACK_T *ev_st,  void *app )
{
    DL_DbFeatureStoreState( ID_CLOCK, 2 );
    
    GetInfo( );
    UIS_ForceRefresh( );
    APP_UtilStopTimer( app );
    APP_UtilStartCyclicalTimer( 10,  2,  app );
    
    return RESULT_OK;
}

UINT32 FlipClose( EVENT_STACK_T *ev_st,  void *app )
{
    PFprintf( "Clock was %x\n", ClockState );
    DL_DbFeatureStoreState( ID_CLOCK, ClockState );
    return RESULT_OK;
}

//battery volt temp 0x8C66440

UINT32 Timer( EVENT_STACK_T *ev_st,  void *app )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;

    EVENT_T * ev = ( EVENT_T * )AFW_GetEv( ev_st );
    DL_TIMER_DATA_T * _timer = ( DL_TIMER_DATA_T * )&ev->data;

    UINT8 DialogType;
    UIS_GetActiveDialogType( &DialogType );

    if( DialogType == DialogType_Homescreen )
	{
        if( _timer -> ID == 1 )
        {
            UIS_ForceRefresh( );
            GetInfo( );
            APP_UtilStopTimer( app );
            APP_UtilStartCyclicalTimer( 100,  2,  app );
        }
        else if( _timer -> ID == 2 )
        {
            UpdateInfo( );
            APP_UtilStopTimer( app );
            APP_UtilStartCyclicalTimer( 15000,  1,  app );
        }
	}

	return RESULT_OK;
}

BOOL exit_key_pressed[ 3 ] = { 0, 0, 0 };
UINT8 exit_keys_seq[ 3 ] = { KEY_STAR, KEY_0, KEY_POUND };

UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  void * app )
{
    UINT8 DialogType;
    UIS_GetActiveDialogType( &DialogType );
    
    UINT8 keypress = ( AFW_GetEv( ev_st ) ) -> data . key_pressed;
    if( keypress == KEY_RED && DialogType == DialogType_Homescreen )
    {
        GetInfo( );
        UIS_ForceRefresh( );
        APP_UtilStopTimer( app );
        APP_UtilStartCyclicalTimer( 10,  2,  app );
        
        return RESULT_OK;
    }
    
    if( exit_key_pressed[ 0 ] )
    {
        if( exit_key_pressed[ 1 ] )
        {
            if( !exit_key_pressed[ 2 ] && keypress == exit_keys_seq[ 2 ] )
                exit_key_pressed[ 2 ] = 1;
        }
        else
            if( keypress == exit_keys_seq[ 1 ] ) exit_key_pressed[ 1 ] = 1;
    }
    else
        if( keypress == exit_keys_seq[ 0 ] ) exit_key_pressed[ 0 ] = 1;

    if( keypress != exit_keys_seq[ 0 ] &&
        keypress != exit_keys_seq[ 1 ] &&
        keypress != exit_keys_seq[ 2 ] )
    {
        exit_key_pressed[ 0 ] = exit_key_pressed[ 1 ] = exit_key_pressed[ 2 ] = 0;
    }

    if( exit_key_pressed[ 0 ] &&
        exit_key_pressed[ 1 ] &&
        exit_key_pressed[ 2 ] )
    InfoExit( ev_st, app );

    return RESULT_OK;
}

//thanks to om2804
#define BETWEEN_LINES   2

UINT32 DrawStrWT( char * str, UINT16 x, UINT16 y, BOOL f2 )
{
    GRAPHIC_REGION_T    region;
    GRAPHIC_POINT_T     display;

    UINT16              y_max = 0;

    UINT32              i = 0;

    display . x = 240;
    display . x = 320;

    region.ulc.x = x;
    region.ulc.y = y;

    for( i = 0; i < strlen( str ); i++ )
    {
        if( str[ i ] == '\r' )
            continue;
            
        if( f2 )
            str[ i ] += 197;

        region.lrc.x = region.ulc.x + font[ str[ i ] ].img_size.width;
        region.lrc.y = region.ulc.y + font[ str[ i ] ].img_size.height;
        
        if( str[ i ] == '\n' || region.lrc.x  > display.x )
        {
            region.ulc.x = x;
            region.ulc.y += y_max + BETWEEN_LINES;
            region.lrc.x = region.ulc.x + font[str[i]].img_size.width;
            region.lrc.y = region.ulc.y + font[str[i]].img_size.height;
        }
        y_max = max(y_max, font[str[i]].img_size.height);

        utility_draw_image( font_res[ str[ i ] ], region, NULL, NULL, 0 );

        region.ulc.x += font[ str[ i ] ].img_size.width;
    }

    return RESULT_OK;
}

UINT32 LoadFont( )
{
    UINT16              i, j = 0;
    WCHAR               font_name[ 256 ];

    UINT32              font_size = 0;
    UINT32              r;
    FILE_HANDLE_T       f;
    UINT32              char_offset= 256 * 4;
    UINT32              char_size = 0;
    
    font_res = ( RESOURCE_ID * )suAllocMem( 256 * sizeof( RESOURCE_ID ), NULL );

    u_strcpy( font_name, L"file://a/mobile/picture/info/Font.dat" );

    font_size = DL_FsSGetFileSize( font_name,  0 );

    font_buffer = suAllocMem( font_size, NULL );
    font = suAllocMem( sizeof( IMG_FONT_T ) * 256, NULL );

    memset( font, 0, sizeof( IMG_FONT_T ) * 256 );

    f = DL_FsOpenFile( font_name,  FILE_READ_MODE,  0 );
    if( f == FILE_HANDLE_INVALID )
        return RESULT_FAIL;
    DL_FsReadFile( font_buffer, 1, font_size, f, &r );
    DL_FsCloseFile( f );

    for( i = 0; i < 256 * 4; i += 4 )
    {
        char_offset += char_size;
        char_size = ( font_buffer[ i ] * 0x1000000 + font_buffer[ i + 1 ] * 0x10000 + font_buffer[ i + 2 ] * 0x100 + font_buffer[ i + 3 ] );
        if( !char_size )
        {
            font_res[ j ] = NULL;
            font[ j ].img_size.width = 0;
            font[ j ].img_size.height = 0;
        }
        else
        {
            DRM_CreateResource( &font_res[ j ], RES_TYPE_GRAPHICS, ( void * )( font_buffer + char_offset ), char_size );
            utility_get_image_size( font_res[ j ], &font[ j ].img_size.width, &font[j].img_size.height );
        }
        j++;
    }

    return RESULT_OK;
}
