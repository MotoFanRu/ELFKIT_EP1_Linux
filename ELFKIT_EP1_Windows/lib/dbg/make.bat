:: This make.bat script was edited by EXL, 07-Dec-2022.
:: Default platform is Motorola P2K, ElfPack v1.0, ADS1.2 [Build 848] on Windows.
:: Warning: `-nodebug` flag option for `armlink` is buggy.

:: Uncomment it for verbose output.
:: @echo off

:: Compiler path.
set ARM_PATH=..\..\

:: SDK path.
set SDK_PATH=%ARM_PATH%\SDK

:: Defines.
set DEFINES=-D__P2K__

:: Library name.
set LIB_NAME=dbg

:: Compiling step.
%ARM_PATH%\tcc -I%SDK_PATH% %DEFINES% -bigend -apcs /interwork -O2 -c %LIB_NAME%.c -o %LIB_NAME%.o

if /I "%1"=="clean" (
	del %LIB_NAME%.o
)
