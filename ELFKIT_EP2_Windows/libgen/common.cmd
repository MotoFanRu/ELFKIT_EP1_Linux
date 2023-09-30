@echo off

if not defined fw (
	echo This file is not supposed to be run separately!
	echo Instead, run libgen_*.cmd files
	pause
	goto :EOF
) 

date /t >ver.txt
set /p d=<ver.txt
:: set /a version=%d:~8,2%%d:~3,2%%d:~0,2%1
set /a version=1701231

echo BUILD LIB %fw% %version%

del std.sa 2>nul
del library.bin 2>nul
del ver.txt 2>nul

postlink.exe -stdlib syms\%fw%.sym -def ldrAPI.def -fw %fw% -v %version% -header ..\SDK\consts.h

if ERRORLEVEL 1 (
	pause
) else (
	ren std.lib library.bin
	copy /Y library.bin libs\%fw%.bin
)