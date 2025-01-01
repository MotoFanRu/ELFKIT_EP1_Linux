@echo off

set PATH=%PATH%;..\

set FW=R452H6_G_08.00.05R

set OPT=-DPATCH -DR452H6_G_08_00_05R -DFTR_L7E 
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"Z3\"
rem set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_TWO_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG 
rem -DLOG_TO_FILE -DDUMP_ELF

rem ������ �����
set fw_start=0x10092000

rem ���� ������ ����
rem 102B0300 - 10092000
rem set off=0x0021E300
set off=0x0145B000

rem ���� �� ������ ��
rem 112E5FEC:	1065100B	DCD	APP_SYNCMLMGR_MainRegister+1
rem 112E5FEC - 10092000 
set register_off=0x0118B0FC

rem �������� ��� ����� UpdateDisplay
rem 10320148 - 10092000
set UDInjOff=0x002E6228

rem ����� ������ ��� EP2, �� ������ 0x400 ����
rem 0x145c96c8  0x3e9     uis_data_logger_buffer 
rem set RAMAddr=0x145C96C8
rem Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C #14073328 ���� 1407332C �� ������ ����� ������ 
set RAMAddr=0x14073328


rem ================================================================================

start /B /WAIT Build_common.cmd

if ERRORLEVEL 1 (
	echo.
	echo DONE WITH ERRORS
	pause
) else (
	echo.
	echo DONE SUCCESS
)


