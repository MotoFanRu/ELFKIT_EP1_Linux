#define DEBUG

#ifndef Informer_h
#define Informer_h

#include <apps.h>
#include <loader.h>

#include <dl.h>
#include <alarm.h>
#include <canvas.h>
#include <battery.h>
#include <datebook.h>
#include <filesystem.h>
#include <dl_keypad.h>
#include <synsock_api.h>
#include <skins.h>
#include <ussd.h>

#define TIME_UPDATE_DATE        30000 // тайминг обновления данных
#define TIME_REFRESH            50    // тайминг прорисовки после рефреша

#define INFO_STRING_SIZE        128    // длинна инф. строки

#define COUNT_TIMERS            7

typedef struct
{
    UINT8       x;
    UINT8       y;
    UINT8       anchor;
    UINT8       between;
    UINT8       font;
    UINT8       font_style;
    UINT16      font_size;
    COLOR_T     color;

} INFO_STYLE_T;

typedef struct
{
    INFO_STYLE_T    style;
    WCHAR           str[INFO_STRING_SIZE];
} INFO_STRING_T;

typedef enum
{
    IF_STATE_ANY,
    IF_STATE_MAIN,
    IF_STATE_MAX
} IF_STATES_T;

typedef struct
{
    APPLICATION_T apt;
} APP_INFORMER_T;

UINT32 ELF_Entry (ldrElf *ela, WCHAR *params);
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit  (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 HandleReqExit (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleKeypress(EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 MainStateEnter(EVENT_STACK_T *ev_st, APPLICATION_T *app, ENTER_STATE_TYPE_T type);


UINT32 ReadIniSettings(WCHAR *IniFile);
void ReadIniStyle(const char *section_name, INFO_STYLE_T *style);



UINT32 IdleStart( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 RepaintStart( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 TimerHandle( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 AlarmHandle( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 RegNetwork( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

void Repaint(APPLICATION_T *app);
void Paint(void);
UINT32 UpdateData(void);

UINT32 DrawString(INFO_STRING_T  *info);
RESOURCE_ID LoadPicture(WCHAR *uri, BYTE *pic);
UINT32 DrawImage( BYTE *picture, UINT32 picture_size, INT16 x, INT16 y);


///CELLS
typedef struct
{
    UINT16  cell_id;
    char    name[32+1];
    BOOL    mask;
} CELLS_T;

typedef struct
{
    UINT8           pad;
    BOOL            Enabled;
    BOOL            Show_CellId;
    BOOL            Show_Location;
    UINT32          cells_count;
    CELLS_T         *cells;

    INFO_STRING_T   info;
} INFO_CELLS_T;

UINT32  ReadCells(WCHAR *ELF_folder);
UINT16 Cells(WCHAR *buf_con);

// Date

typedef struct
{
    UINT8           pad;
    BOOL            Enabled;
    BOOL            Show_Date;
    BOOL            Show_WeekDay;
    INFO_STRING_T   info;
} INFO_DATE_T;

CLK_DATE_T Date(WCHAR *buf_date);

// Battery

typedef struct
{
    UINT8           pad;
    BOOL            Enabled;
    BOOL            Show_Percent;
    BOOL            Show_Voltage;
	UINT16			Max_Value;
	UINT16			Min_Value;
    INFO_STRING_T   info;
} INFO_BATTERY_T;

UINT8 Battery(WCHAR *buf_con );

// Signal

typedef struct
{
    UINT8           pad;
    BOOL            Enabled;
    BOOL            Show_Percent;
    BOOL            Show_Dbm;
    INFO_STRING_T   info;
} INFO_SIGNAL_T;

SIGNAL_STRENGTH_T Signal(WCHAR *buf_con );

// Disks

typedef struct
{
    UINT8           pad;
    BOOL            Enabled;
    BOOL            Show_AllBytes;
    BOOL            Show_FreeBytes;
    INFO_STRING_T   info;
} INFO_DISKS_T;

UINT32 Disks(WCHAR *buf_con);

// Weather
typedef struct
{
    UINT8  day;
    UINT8  month;
    UINT16 year;
    UINT8  hour;
    UINT8  tod;
  // not used
    UINT8  predict;
    UINT8  weekday;
} FORECAST;

typedef struct
{
    UINT8  cloudiness;
    UINT8  precipitation;
    UINT8  rpower;
    UINT8  spower;
} PHENOMENA;

typedef struct
{
    INT8 max_val;
    INT8 min_val;
    UINT8 pad[2];
} TEMPERATURE;

typedef struct
{
    UINT8 max_val;
    UINT8 min_val;
    UINT8 direction;
    UINT8 pad;
} WIND;

typedef struct
{
    FORECAST    forecast;
    PHENOMENA   phenomena;
    TEMPERATURE temperature;
    WIND        wind;
} MMWEATHER;

#define COUNT_MMWEATHER  4 // кол-во прогнозов в файле

typedef struct
{
    char            ID[8];      // id города
    BOOL            AutoUpdate; // автообновление
    UINT8           keyupdate1;
    UINT8           keyupdate2;
    UINT8           keyupdate3;
    BOOL            Enabled;
    BOOL            Show_Date;
    BOOL            Show_Temperature; //показать температуру
    BOOL            Show_Wind;  // показать ветер
    BOOL            Show_Phenomena; // облочность, осадки
    UINT8           pad[2];
    BOOL            Show_Picture;
    INT16           Picture_X;
    INT16           Picture_Y;
    BYTE            *Picture; // картинка
    //UINT32          Picture_Size; // размер файла картинки
    RESOURCE_ID     PicRes;
    MMWEATHER       xml[COUNT_MMWEATHER];
    INFO_STRING_T   info;
} INFO_WEATHER_T;

#define HTTP_HOST			"informer.gismeteo.ru"
#define HTTP_USER_AGENT		"Informer"

#define TIME_CHECK   5*60000 // тайминг проверки актуальности погоды
#define SIZE_BUF     3*1024

UINT32 Weather(WCHAR *buf);
UINT32 HandleAPIAnsw( EVENT_STACK_T * ev_st, APPLICATION_T *app );
UINT32 HandleSockAnsw( EVENT_STACK_T *ev_st, APPLICATION_T *app );
UINT32 LoadFile(void);
UINT32 Update(void);

INT8   Weather_getNum(void);


// Balance

typedef struct
{
    WCHAR           USSD[16];
    UINT32          *Words;
    UINT32          countWords;
    UINT32          Time_Update;
    UINT8           keyupdate1;
    UINT8           keyupdate2;
    UINT8           keyupdate3;
    BOOL            Enabled;
    INFO_STRING_T   info;
} INFO_BALANCE_T;

UINT32 USSDHandleSendRequest( EVENT_STACK_T *ev_st,  APPLICATION_T *app );
UINT32 USSDHandleNotify( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

// Alarm

typedef struct
{
    BOOL            Enabled;
    UINT8           pad[3];
    UINT32          Count;
    INFO_STRING_T   info;
} INFO_ALARM_T;

//UINT32 AlarmRegister(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
//UINT32 AlarmUnregister(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
UINT32 Alarm(WCHAR *buf);

// DateBook

typedef struct
{
    BOOL            Enabled;
    BOOL            Show_Title;
    BOOL            Show_Date;
    UINT8           pad;
    INFO_STRING_T   info;
} INFO_DATEBOOK_T;

UINT32 DbkRegister(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
UINT32 DbkUnregister(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
UINT32 DBKHandleSendRequest(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
UINT32 DBKHandleQuery(EVENT_STACK_T *ev_st,  APPLICATION_T *app);
UINT32 DBKHandleView(EVENT_STACK_T *ev_st,  APPLICATION_T *app);

// CallTime
typedef struct
{
    UINT8           pad[3];
    BOOL            Enabled;
    INFO_STRING_T   info;
} INFO_CALLTIME_T;

UINT32 CallTime(WCHAR *buf);

///UTILS
UINT8 MaxDays(UINT8 month, UINT16 year);
UINT32 degree(UINT32 x, UINT32 y);
BOOL WorkingTable(void);
UINT32 str2ul (char *tmp_buf);
COLOR_T UINT32toCOLOR_T(UINT32 n);

UINT32 StartTimer( UINT32 period, UINT32 id, UINT8 type, APPLICATION_T *app );
UINT32 StopTimer( UINT32 id, APPLICATION_T *app );
UINT32 isOurTimer( UINT32 THandle);

UINT32 AlarmStart(UINT32 id, CLK_DATE_T date, CLK_TIME_T time);
UINT32 AlarmStop( UINT32 id, CLK_DATE_T date, CLK_TIME_T time);
#endif
