// UTF-8 w/o BOM

#ifndef __SDK_TIME_DATE_H__
#define __SDK_TIME_DATE_H__

#include <typedefs.h>

typedef UINT32 CLK_CLOCK_T;

typedef struct
{
	UINT8		day;
	UINT8		month;
	UINT16		year;
} CLK_DATE_T;

typedef struct
{
	UINT8		hour;
	UINT8		minute;
	UINT8		second;
} CLK_TIME_T;

typedef struct
{
	CLK_TIME_T	time;  
	CLK_DATE_T	date;  
	UINT32		unk;  
} DL_ALARM_DATA_T;

typedef struct
{
	UINT32		time;
	UINT32		ID;
} DL_TIMER_DATA_T;

#ifdef __cplusplus
extern "C" {
#endif

//Получить системное время в тиках 
UINT64 suPalReadTime( void ); 
// Перевести время из тиков в миллисекунды  
UINT64 suPalTicksToMsec( UINT64 ticks );

// Получить системную дату
BOOL DL_ClkGetDate( CLK_DATE_T * date);
// установить дату
BOOL DL_ClkSetDate( CLK_DATE_T date );
// Получить системное время 
BOOL DL_ClkGetTime( CLK_TIME_T * time);
// установить время
BOOL DL_ClkSetTime( CLK_TIME_T time );

/***************************************
   Функции Alarm
****************************************/

// установить Alarm
BOOL DL_ClkStoreIndividualEvent( IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data );

// удалить Alarm
BOOL DL_ClkDeleteIndividualEvent( IFACE_DATA_T *data, DL_ALARM_DATA_T alarm_data,
								  UINT8 mask ); // 1 - time, 2- date....  63 - полное удаление?


/***************************************
   Функции Timer 
****************************************/

/* Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED, attachment которого указывает на DL_TIMER_DATA_T */

/*  Запустить таймер, который сработает через time мс */
UINT32 APP_UtilStartTimer( UINT32 time,  UINT32 ID,  void *app ); // ID будет в DL_TIMER_DATA_T при обработке ивента

/*  Запустить таймер, который будет срабаывать через каждые time мс */
UINT32 APP_UtilStartCyclicalTimer( UINT32 time,  UINT32 ID,  void *app );

/* Остановить таймер */
UINT32 APP_UtilStopTimer( void *app );



UINT32 DL_ClkStartTimer( IFACE_DATA_T* iface, UINT32 period, UINT32 id );
UINT32 DL_ClkStartCyclicalTimer( IFACE_DATA_T* iface, UINT32 period, UINT32 id );
UINT32 DL_ClkStopTimer( IFACE_DATA_T* iface );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __SDK_TIME_DATE_H__
