@echo off

set PATH=%PATH%;..\

set FW=R373_G_0E.30.70R

set OPT=-DPATCH -DR373_G_0E_30_70R -DFTR_E398
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE\" -DFTR_PHONE_NAME=\"E398\"
set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_THREE_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FASTRUN_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG
rem -DLOG_TO_FILE -DDUMP_ELF

rem ������ �����
set fw_start=0x10080000

rem ���� ������ ����
set off=0x00C73000

rem ���� �� ������ ��
set register_off=0x0025D0B0

rem �������� ��� ����� UpdateDisplay
set UDInjOff=0x00029504

rem ����� ������ ��� EP2, �� ������ 0x400 ����
set RAMAddr=0x125015D8


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


