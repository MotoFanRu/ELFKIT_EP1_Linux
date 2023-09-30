@echo off

set PATH=%PATH%;..\

set FW=R373_G_0E.30.49R

set OPT=-DPATCH -DR373_G_0E_30_49R -DFTR_E398
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE\" -DFTR_PHONE_NAME=\"E398\"
set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_THREE_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG 
rem -DLOG_TO_FILE -DDUMP_ELF

rem Начало проши
set fw_start=0x10080000

rem Куда ложить патч
set off=0x00C73000

rem Патч на запуск ЭП
set register_off=0x0025A434

rem Смещение для патча UpdateDisplay
set UDInjOff=0x00028DF0

rem Адрес памяти для EP2, не меньше 0x400 байт
rem 0x12200000-0x122008CC = Datalogger A51
rem set RAMAddr=0x122008E0
set RAMAddr=0x124FD320

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

