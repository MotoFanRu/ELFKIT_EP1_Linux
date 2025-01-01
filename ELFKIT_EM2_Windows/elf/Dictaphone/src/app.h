#ifndef APP_H
#define APP_H

#include <apps.h>
#include <canvas.h>
#include <loader.h>
#include <filesystem.h>
#include <dl_keypad.h>
#include <mme.h>
#include <uis.h>
#include <utilities.h>
#include <resources.h>

#define DEF_LINE_HEIGHT 31
#define DEF_TAB_LENGHT 20
#define DEF_INDENT 3

typedef struct
{
    UINT32 LineHeight;                              //Высота линии
    UINT32 TabLenght;                               //Длина табулятора в пикселях
    UINT32 Indent;                                  //Отступ от краёв экрана в пикселях
    WCHAR RecordFolder[FS_MAX_URI_NAME_LENGTH + 1]; //Папка для записи аудио
} DICTAPHONE_CONFIG_T;

typedef enum
{
    DICTAPHONE_STATE_STOP = 0,
    DICTAPHONE_STATE_PAUSE,
    DICTAPHONE_STATE_RECORD
} DICTAPHONE_STATES_T;

typedef enum
{
    APP_STATE_ANY,
    APP_STATE_INIT,
    APP_STATE_MAIN,
    APP_STATE_MAX
} APP_STATES_T;

//use thisapp(app) to access to struct APP_T
typedef struct
{
	APPLICATION_T apt;                                      // описание приложения
    DRAWING_BUFFER_T    bufd;                               // буффер для рисования
	MME_GC_MEDIA_FILE   mhandle;                            // идентификатор файла
	DICTAPHONE_CONFIG_T config;                             // конфиг приложения
	DICTAPHONE_STATES_T state;                              // сосояние
    UINT16              time;                               // время записи
	WCHAR               uri[FS_MAX_URI_NAME_LENGTH + 1];    // имя записываемого файла
} APP_T;

UINT32 ELF_Entry (ldrElf *ela, WCHAR* Params);
UINT32 ELF_Start (EVENT_STACK_T *ev_st, REG_ID_T reg_id, REG_INFO_T *reg_info);
UINT32 ELF_Exit  (EVENT_STACK_T *ev_st, APPLICATION_T *app);

//main state handlers
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );
UINT32 MainStateExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  EXIT_STATE_TYPE_T type );

UINT32 HandleTimer (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleKeypress (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleStartRecord (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleRecordComplete (EVENT_STACK_T *ev_st, APPLICATION_T *app);
UINT32 HandleUITokenGranted (EVENT_STACK_T *ev_st, APPLICATION_T *app);

UINT32 CaptureCreate(APPLICATION_T *app);
UINT32 StopRecord(APPLICATION_T *app);

//utils
void paint(APPLICATION_T *app);
void GetFileName(APPLICATION_T *app);
void sec2hms(UINT32 seconds, WCHAR *str);
UINT32 GetResourceString(WCHAR *str, RESOURCE_ID res_id);
UINT32 ReadConfig (DL_FS_MID_T *id, DICTAPHONE_CONFIG_T *config);

#endif
