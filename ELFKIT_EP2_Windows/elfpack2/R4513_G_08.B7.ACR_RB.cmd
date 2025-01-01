@echo off

set PATH=%PATH%;..\

set FW=R4513_G_08.B7.ACR_RB

set OPT=-DPATCH -DR4513_G_08_B7_ACR_RB -DFTR_L7
set OPT=%OPT% -DFTR_PHONE_PLATFORM=\"LTE2\" -DFTR_PHONE_NAME=\"L7\"
set OPT=%OPT% -DFTR_KEYPAD_TYPE=KP_THREE_POLE -DFTR_NOAUTORUN_KEY=KEY_0
set OPT=%OPT% -DFTR_FASTRUN_KEY=KEY_FAST_ACCESS

set OPT=%OPT% -DDEBUG
rem -DLOG_TO_FILE -DDUMP_ELF

rem ������ �����
set fw_start=0x10092000

rem ���� ������ ����
set off=0x00C8D630

rem ���� �� ������ ��
set register_off=0x0027DD30

rem �������� ��� ����� UpdateDisplay
set UDInjOff=0x00014568

rem ����� ������ ��� EP2, �� ������ 0x400 ����
set RAMAddr=0x124EBBA0


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


