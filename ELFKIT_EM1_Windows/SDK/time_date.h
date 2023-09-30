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

// �������� ��������� ����
BOOL DL_ClkGetDate( CLK_DATE_T * );
// �������� ��������� ����� 
BOOL DL_ClkGetTime( CLK_TIME_T * );

//�������� ��������� ����� � ����� 
UINT64 suPalReadTime( void ); 
// ��������� ����� �� ����� � ������������  
UINT64 suPalTicksToMsec( UINT64 ticks );

/***************************************
   ������� ������� 
****************************************/

/* ����� ������ �����������, �� ������ ����� EV_TIMER_EXPIRED, attachment �������� ��������� �� DL_TIMER_DATA_T */

typedef struct
{
	UINT32		time;
	UINT32		ID;
} DL_TIMER_DATA_T;

/*  ��������� ������, ������� ��������� ����� time �� */
UINT32 APP_UtilStartTimer( UINT32 time,  UINT32 ID,  void *app ); // ID ����� � DL_TIMER_DATA_T ��� ��������� ������

/*  ��������� ������, ������� ����� ���������� ����� ������ time �� */
UINT32 APP_UtilStartCyclicalTimer( UINT32 time,  UINT32 ID,  void *app );

/* ���������� ������ */
UINT32 APP_UtilStopTimer( void *app );


#endif
