rem
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set PATH=%PATH%;..\

set OPT=-DEP2

set SDK=..\..\SDK

rem ------------------- Compile Elfloader ---------------



pushd mainsrc\

set src_input=
rem 
for %%c in (*.c) do (
	set src_input=!src_input! %%c
	rem echo %%c
)
echo %src_input%

tcc -I"%SDK%" -c -bigend -apcs /interwork %OPT% elfloaderApp.c autorun.c config.c console.c parser.c API.c lolvl.c
rem tcc -I"%SDK%" -c -bigend -apcs /interwork %OPT% %src_input%
armasm -16 -bigend -apcs /interwork patchlib.asm

popd

pause
exit
