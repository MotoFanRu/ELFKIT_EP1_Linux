@echo off

set PATH=%PATH%;..\

set FW=R452B_G_08.02.0DR

set OPT=-DPATCH -DR452B_G_08_02_0DR -DFTR_L7E 
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
set off=0x01456000

rem Патч на запуск ЭП
rem 112E5FEC:	1065100B	DCD	APP_SYNCMLMGR_MainRegister+1
rem 112E5FEC - 10092000 
set register_off=0x0118AD54

rem Смещение для патча UpdateDisplay
rem 10320148 - 10092000
set UDInjOff=0x002E6328

rem Адрес памяти для EP2, не меньше 0x400 байт
rem 0x145c96c8  0x3e9     uis_data_logger_buffer 
rem set RAMAddr=0x145C96C8
rem Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C #14073328 либо 1407332C но скорее всего первое
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


