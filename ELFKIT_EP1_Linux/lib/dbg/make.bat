@echo off

set output=dbg.o

set armdir=..\..\
set sdkdir=..\..\SDK

if exist %output% del %output%

set src_input=dbg.c

%armdir%\tcc -c -I"%sdkdir%" -bigend -O2 -apcs /interwork %src_input%


if exist %output% (
	echo ACK: File `%output%` created
) else (
	echo ERR: File `%output%` not created
)

pause