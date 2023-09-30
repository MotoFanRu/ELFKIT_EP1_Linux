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

BOOL DL_ClkGetDate (CLK_DATE_T *Date);		// �������� ��������� ����
BOOL DL_ClkSetDate (CLK_DATE_T Date);		// ���������� ��������� ����

BOOL DL_ClkGetTime (CLK_TIME_T *Time);		// �������� ��������� ����� 
BOOL DL_ClkSetTime (CLK_TIME_T Time);		// ���������� ��������� ����� 

UINT64 suPalReadTime (void);			// �������� ��������� ����� � ����� 
UINT64 suPalTicksToMsec (UINT64 ticks);		// ��������� ����� �� ����� � ������������  

//******************************************************************************
// ������� Timer
//
// ����� ������ �����������, �� ������ ����� EV_TIMER_EXPIRED (0x8205A), attachment �������� ��������� �� DL_TIMER_DATA_T
// ID ������� ����� � DL_TIMER_DATA_T ��� ��������� ������ EV_TIMER_EXPIRED. (�� ����� ���� ��� �� ID, � ������ �����-�� ������)
//
//******************************************************************************

// ������� APP_* ������������ ��� �������� ������ ������ �������!
UINT32 APP_UtilStartTimer (UINT32 time, UINT32 ID, APPLICATION_T *app);			// ��������� ������, ������� ��������� ����� time �� 
UINT32 APP_UtilStartCyclicalTimer (UINT32 time, UINT32 ID, APPLICATION_T *app);		// ��������� ������, ������� ����� ���������� ����� ������ time ��
UINT32 APP_UtilStopTimer (APPLICATION_T *app);						// ���������� ������

//******************************************************************************
// ������� DL_* ������������ ��� �������� ���������� ��������. iface->handle ��������� ��� ��������� �������!!!
// ����� ������ �����������, �� ������ ����� EV_TIMER_EXPIRED (0x8205A), attachment �������� ��������� �� DL_TIMER_DATA_T
// ��� ��������� ������ ���� seqnum - ���������� �������������. �� ����� �������� iface->handle ����� �������� �������
//******************************************************************************
UINT32 DL_ClkStartTimer (IFACE_DATA_T *iface, UINT32 period, UINT32 id);	// ��������� ������, ������� ��������� ����� time ��
UINT32 DL_ClkStartCyclicalTimer (IFACE_DATA_T *iface, UINT32 period, UINT32 id);// ��������� ������, ������� ����� ���������� ����� ������ time ��
UINT32 DL_ClkStopTimer (IFACE_DATA_T *iface);					// ���������� ������. ������ iface->handle ��������� ���������� ��� ������� �������

//������ ������.
//TimerPeriod - ����� ����� ������� ��������� ������.
//TimerType - ��� �������. 0 - DL_ClkStartTimer. 1 - DL_ClkStartCyclicalTimer
//HandleTimerExpired - �-��� ������� ����� ������� ��� ������������ �������
//unk = 0
UINT32 AMS_TimerCreate (UINT32 TimerPeriod, UINT8 TimerType, void *HadleTimerExpired, UINT16 unk);

#endif
