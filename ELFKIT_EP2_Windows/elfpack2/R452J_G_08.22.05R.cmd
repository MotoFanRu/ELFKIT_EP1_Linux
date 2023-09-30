@echo off

set PATH=%PATH%;..\

set FW=R452J_G_08.22.05R

set OPT=-DPATCH -DR452J_G_08_22_05R -DFTR_L9
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"L9\"
rem set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_TWO_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG 
rem -DLOG_TO_FILE -DDUMP_ELF

rem Начало проши
set fw_start=0x10092000

rem Куда ложить патч
rem 115C319C - 10092000
set off=0x153119C

rem Патч на запуск ЭП
rem 112B00CC:	1060085F	DCD	APP_SYNCMLMGR_MainRegister+1
rem 112B00CC - 10092000 
set register_off=0x0121E0CC

rem Смещение для патча UpdateDisplay
rem 102C8C6C - 10092000
set UDInjOff=0x00236C6C

rem Адрес памяти для EP2, не меньше 0x400 байт
rem 0x1451C1C8  0x3e9     uis_data_logger_buffer 
set RAMAddr=0x1451C1C8


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


