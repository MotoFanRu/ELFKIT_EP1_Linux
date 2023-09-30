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
set DEFINES=-D__P2K__ -DOPTIMIZE_1

:: Libraries.
set LIB_NAME_CCFILE=ccfile
set LIB_NAME_CINI=cini

:: Compiling step.
%ARM_PATH%\tcpp -I%SDK_PATH% %DEFINES% -bigend -apcs /interwork -O2 -c %LIB_NAME_CCFILE%.cpp -o %LIB_NAME_CCFILE%.o
%ARM_PATH%\tcpp -I%SDK_PATH% %DEFINES% -bigend -apcs /interwork -O2 -c %LIB_NAME_CINI%.cpp -o %LIB_NAME_CINI%.o

if /I "%1"=="clean" (
	del %LIB_NAME_CCFILE%.o
	del %LIB_NAME_CINI%.o
)
