@echo off

set PATH=%PATH%;..\

set FW=R4515_G_08.BD.D3R

set OPT=-DPATCH -DR4515_G_08_BD_D3R -DFTR_V3r
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"V3r\"
set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_THREE_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS
rem OPT=%OPT% -DDEBUG -DLOG_TO_FILE -DFTR_LOG_FILE_URI=L\"file://a/ep2.log\"
set OPT=%OPT% -DDEBUG 

rem -DLOG_TO_FILE -DDUMP_ELF

rem Начало проши
set fw_start=0x10092000

rem Куда ложить патч
set off=0x00C60EF0

rem Патч на запуск ЭП
set register_off=0x0025E6D8

rem Смещение для патча UpdateDisplay
set UDInjOff=0x0001CF2C

rem Адрес памяти для EP2, не меньше 0x400 байт
set RAMAddr=0x144C7C68


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

exit
