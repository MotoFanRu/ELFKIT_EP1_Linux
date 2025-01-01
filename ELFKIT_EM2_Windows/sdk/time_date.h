#ifndef TIME_DATE_H
#define TIME_DATE_H

#include <typedefs.h>

typedef UINT32 CLK_CLOCK_T;

typedef struct
{
    UINT32 time;
    UINT32 ID;
} DL_TIMER_DATA_T;

typedef struct
{
    UINT8  day;
    UINT8  month;
    UINT16 year;
} CLK_DATE_T;

typedef struct
{
    UINT8 hour;
    UINT8 minute;
    UINT8 second;
    UINT8 unk;		//??? unk == 0
} CLK_TIME_T;

BOOL DL_ClkGetDate (CLK_DATE_T *Date);		// Получить системную дату
BOOL DL_ClkSetDate (CLK_DATE_T Date);		// Установить системную дату

BOOL DL_ClkGetTime (CLK_TIME_T *Time);		// Получить системное время 
BOOL DL_ClkSetTime (CLK_TIME_T Time);		// Установить системное время 

UINT64 suPalReadTime (void);			// Получить системное время в тиках 
UINT64 suPalTicksToMsec (UINT64 ticks);		// Перевести время из тиков в миллисекунды  

//******************************************************************************
// Функции Timer
//
// Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED (0x8205A), attachment которого указывает на DL_TIMER_DATA_T
// ID таймера будет в DL_TIMER_DATA_T при обработке ивента EV_TIMER_EXPIRED. (на самом деле это не ID, а просто какие-то данные)
//
//******************************************************************************

// Функции APP_* использовать для создания только одного таймера!
UINT32 APP_UtilStartTimer (UINT32 time, UINT32 ID, APPLICATION_T *app);			// Запустить таймер, который сработает через time мс 
UINT32 APP_UtilStartCyclicalTimer (UINT32 time, UINT32 ID, APPLICATION_T *app);		// Запустить таймер, который будет срабаывать через каждые time мс
UINT32 APP_UtilStopTimer (APPLICATION_T *app);						// Остановить таймер

//******************************************************************************
// Функции DL_* использовать для создания нескольких таймеров. iface->handle сохранять для остановки таймера!!!
// Когда таймер срабатывает, он создаёт ивент EV_TIMER_EXPIRED (0x8205A), attachment которого указывает на DL_TIMER_DATA_T
// При обработке ивента поле seqnum - уникальный идентификатор. Он равен значению iface->handle после создания таймера
//******************************************************************************
UINT32 DL_ClkStartTimer (IFACE_DATA_T *iface, UINT32 period, UINT32 id);	// Запустить таймер, который сработает через time мс
UINT32 DL_ClkStartCyclicalTimer (IFACE_DATA_T *iface, UINT32 period, UINT32 id);// Запустить таймер, который будет срабаывать через каждые time мс
UINT32 DL_ClkStopTimer (IFACE_DATA_T *iface);					// Остановить таймер. Задать iface->handle значением сохранённым при запуске таймера

//Создаёт таймер.
//TimerPeriod - время через которое сработает таймер.
//TimerType - тип таймера. 0 - DL_ClkStartTimer. 1 - DL_ClkStartCyclicalTimer
//HandleTimerExpired - ф-ция которая будет вызвана при срабатывании таймера
//unk = 0
UINT32 AMS_TimerCreate (UINT32 TimerPeriod, UINT8 TimerType, void *HadleTimerExpired, UINT16 unk);

#endif
