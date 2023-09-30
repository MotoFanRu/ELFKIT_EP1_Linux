@echo off

set PATH=%PATH%;..\

set FW=R4513_G_08.B7.ACR

set OPT=-DPATCH -DR4513_G_08_B7_ACR -DFTR_V360
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"V360\"
set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_THREE_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG 
rem -DLOG_TO_FILE -DDUMP_ELF

rem Начало проши
set fw_start=0x10092000

rem Куда ложить патч
set off=0x00C8D000

rem Патч на запуск ЭП
set register_off=0x0027DD30

rem Смещение для патча UpdateDisplay
set UDInjOff=0x00014568

rem Адрес памяти для EP2, не меньше 0x400 байт
rem 0x12200000-0x122008CC = Datalogger A51
rem set RAMAddr=0x122008E0
set RAMAddr=0x124EB7B0

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


