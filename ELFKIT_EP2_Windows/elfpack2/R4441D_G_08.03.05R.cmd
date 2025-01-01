@echo off

set PATH=%PATH%;..\

set FW=R4441D_G_08.03.05R

set OPT=-DPATCH -DR4441D_G_08_03_05R -DFTR_V3i
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"V3i\"
set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_THREE_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FAST_KEY=KEY_FAST_ACCESS
rem OPT=%OPT% -DDEBUG -DLOG_TO_FILE -DFTR_LOG_FILE_URI=L\"file://a/ep2.log\"
set OPT=%OPT% -DDEBUG 

rem -DLOG_TO_FILE -DDUMP_ELF

rem ������ �����
set fw_start=0x100A0000

rem ���� ������ ����
set off=0x00D00000

rem ���� �� ������ ��
set register_off=0x00266BDC

rem �������� ��� ����� UpdateDisplay
set UDInjOff=0x00014BA4

rem ����� ������ ��� EP2, �� ������ 0x400 ����
set RAMAddr=0x1446DDF4


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
