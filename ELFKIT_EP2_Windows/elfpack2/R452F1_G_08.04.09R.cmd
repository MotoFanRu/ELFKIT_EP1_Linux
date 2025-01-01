@echo off

set PATH=%PATH%;..\

set FW=R452F1_G_08.04.09R

set OPT=-DPATCH -DR452F1_G_08_04_09R -DFTR_L7E 
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"Z3\"
rem set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_TWO_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG 
rem -DLOG_TO_FILE -DDUMP_ELF

rem Начало проши
set fw_start=0x10092000

rem Куда ложить патч
rem 102B0300 - 10092000
rem set off=0x0021E300
set off=0x0153C000

rem Патч на запуск ЭП
rem 112E5FEC:	1065100B	DCD	APP_SYNCMLMGR_MainRegister+1
rem 112E5FEC - 10092000 
set register_off=0x0118B0C8

rem Смещение для патча UpdateDisplay
rem 10320148 - 10092000
set UDInjOff=0x002CD2B0

rem Адрес памяти для EP2, не меньше 0x400 байт
rem 0x145c96c8  0x3e9     uis_data_logger_buffer 
rem set RAMAddr=0x145C96C8
rem Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C  ;Ram D [14??????00003E580000FFFF]+0x4
set RAMAddr=0x14076374


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


