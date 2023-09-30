#define DEBUG
#define __V3X__

#ifndef HexEditor_H
#define HexEditor_H

#include <apps.h>
#include <loader.h>

#include <canvas.h>
#include <dl_keypad.h>
#include <filesystem.h>
#include <resources.h>
#include <utilities.h>
#include <uis.h>

const char app_name[APP_NAME_LEN] = "HexEditor";

typedef struct
{
	//координаты сетки
	UINT8 GRID_X;
	UINT8 GRID_Y;

	//кол-ва линий и колонок
	UINT8 LINES_COUNT;
	UINT8 COLUMNS_COUNT;
	UINT8 CELLS_COUNT;

	// высота линии
	UINT8 LINE_H;

	//ширина линии
	UINT8 COLUMN_W;
} GRAPHICAL_CONSTS;

const GRAPHICAL_CONSTS consts_240_320 = {90, 20, 14, 6, 84, 20, 25};
const GRAPHICAL_CONSTS consts_176_220 = {76, 20, 9, 4, 36, 20, 25};

typedef struct
{ 
    APPLICATION_T apt;
} APP_HEXEDITOR_T;

typedef enum
{
    HE_STATE_ANY,
    HE_STATE_INIT,
    HE_STATE_MAIN,
    HE_STATE_EDIT,
    HE_STATE_MENU,
    HE_STATE_FBROWSER,
    HE_STATE_FIND,
    HE_STATE_MAX
} HE_STATES_T;

typedef enum 
{
    MENU_FILE_OPEN=1,
    MENU_FIND,
    MENU_MAX=MENU_FIND
} MENU_ITEMS_T;

typedef enum 
{
    HEX_MODE,
    DEC_MODE,
    TEXT_MODE,
    MAX_MODE=TEXT_MODE
} VIEW_MODES_T;


typedef struct
{ 
	WCHAR		name[64];
	UINT16		attrib;
} FILEINFO;	

typedef enum
{
	RES_LIST_CAPTION=0,
	RES_EDIT_CAPTION,
	RES_MAX
} HE_RESOURCE;
RESOURCE_ID Resources[RES_MAX];


UINT32 startApp( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 destroyApp( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 ReadSetting(WCHAR *cfg_file);
UINT32 SaveSetting(WCHAR *cfg_file);
UINT32 InitResources(void);
UINT32 RemoveResources(void);

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 HandleKeypress( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 EditStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 EditData( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 EditOk( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 MenuStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 mSelectItem( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) ;
UINT32 mSendListItems( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);

UINT32 StateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );
UINT32 Back( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 HandleListReq( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 FbrowserStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );
UINT32 FbrowserStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 fbSendListItems( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 start, UINT32 num);
UINT32 fbSelectItem( EVENT_STACK_T *ev_st,  APPLICATION_T *app ) ;
UINT32 UpdateList( EVENT_STACK_T *ev_st,  APPLICATION_T *app, UINT32 sItem );

UINT32 FindStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 FindData( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 FindFile( WCHAR *folder, WCHAR *filtr ); // поиск файлов
UINT32 OpenFile(WCHAR *uri);
UINT32 LoadBuffer(INT32 off_set);
UINT32 SaveOffset(INT32 off_set, UINT8 value);
UINT32 GoTo(INT32 off_set);
INT32 FindPattern(char *pattern, INT32 off_set);
UINT32 Data2Pattern(WCHAR *data, char *pattern);
INT32 RelativeOffset(INT32 off_set);

UINT32 paint(void); // рисуем 

GRAPHIC_POINT_T  XY(UINT32 sItem);
WCHAR* dec2hex( UINT32 dec, WCHAR *hex, UINT16 size );
UINT32 UCS2toCP1251(WCHAR *src, char *trg);

WCHAR* SplitPath( WCHAR* path, WCHAR* spliter ) ;
#endif
