#ifndef TIME_DATE_H
#define TIME_DATE_H

#include <typedefs.h>

typedef struct
{
    UINT8     day;
    UINT8     month;
    UINT16    year;
} CLK_DATE_T;

typedef struct
{
    UINT8     hour;
    UINT8     minute;
    UINT8     second;
} CLK_TIME_T;

// Получить системную дату
BOOL DL_ClkGetDate( CLK_DATE_T * );
// Получить системное время 
BOOL DL_ClkGetTime( CLK_TIME_T * );

//Получить системное время в тиках 
UINT64 suPalReadTime( void ); 
// Перевести время из тиков в миллисекунды  
UINT64 suPalTicksToMsec( UINT64 ticks );

/***************************************
   Функции таймера 
****************************************/

/* Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED, attachment которого указывает на DL_TIMER_DATA_T */

typedef struct
{
	UINT32		time;
	UINT32		ID;
} DL_TIMER_DATA_T;

/*  Запустить таймер, который сработает через time мс */
UINT32 APP_UtilStartTimer( UINT32 time,  UINT32 ID,  void *app ); // ID будет в DL_TIMER_DATA_T при обработке ивента

/*  Запустить таймер, который будет срабаывать через каждые time мс */
UINT32 APP_UtilStartCyclicalTimer( UINT32 time,  UINT32 ID,  void *app );

/* Остановить таймер */
UINT32 APP_UtilStopTimer( void *app );


#endif
