@echo off

echo BUILD ELFPACK

start /B /WAIT build\zclear.cmd

for %%i in ( R*.cmd ) do (
	call %%i
)


copy /Y build\patch\*.fpa ..\testpack\elfpack\

exit
