@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

set SDK=..\..\SDK
set symfile=%FW%.sym

echo BUILD ELFPACK %FW%

set /a addr="off+fw_start"
dec2hex %addr% >res.txt
set /p addr=<res.txt
rem echo %addr%

set /a UDInjAddr="UDInjOff+fw_start"
dec2hex %UDInjAddr% >res.txt
set /p UDInjAddr=<res.txt

echo.
echo ------------------------------ Compiling Elfpack -------------------------------
echo.

pushd mainsrc\

rem ### C
set src_input=
for %%c in (*.c) do (
	set src_input=!src_input! %%c
	rem echo %%c
)
rem echo %src_input%

tcc -I"%SDK%" -c -bigend -apcs /interwork -O2 %OPT% %src_input%
rem -errors ..\obj\errors_c.txt
if ERRORLEVEL 1 goto :error_c

rem ### ASM
set src_input=
for %%c in (*.asm) do (
	set src_input=!src_input! %%c
	armasm -16 -bigend -apcs /interwork %%c 
	rem -errors ..\obj\errors_%%c.txt
	if ERRORLEVEL 1 goto :error_asm
	rem echo %%c
)
rem echo %src_input%

@move /Y *.o ..\obj\ >nul

popd

echo DONE
echo.
echo ----------------------------- Compiling UpdDispInj -----------------------------
echo.

pushd patchsrc\

armasm -32 -bigend -apcs /interwork UpdateDisplayInjection.asm
rem -errors ..\obj\errors_UDI.txt
if ERRORLEVEL 1 goto :error_asm

@move /Y *.o ..\obj\ >nul

popd

echo DONE
echo.
echo ----------------------------------- Linkage ------------------------------------
echo.

pushd obj\

echo.>scatter.txt
echo UpdDisplInjection %UDInjAddr% 0x00000004>>scatter.txt
echo {>>scatter.txt
echo 	UpdDisplInjection.bin +0>>scatter.txt
echo 	{>>scatter.txt
echo 		UpdateDisplayInjection.o(UpdateDisplayInj)>>scatter.txt
echo 	}>>scatter.txt
echo }>>scatter.txt
echo.>>scatter.txt
echo.>>scatter.txt
echo patch %addr% 0x00011BBC>>scatter.txt
echo {>>scatter.txt
echo 	patch.bin +0 FIXED>>scatter.txt
echo 	{>>scatter.txt
echo 		veneers.o(Veneers, +FIRST)>>scatter.txt
echo 		*.o(+RO)>>scatter.txt
echo 		font.o(+RO, +LAST)>>scatter.txt
echo 		logo.o(+RO, +LAST)>>scatter.txt
echo 		palette.o(+RO, +LAST)>>scatter.txt
echo 	}>>scatter.txt
echo.>>scatter.txt
echo 	ven +0>>scatter.txt
echo 	{>>scatter.txt
echo 		*(Veneer$$Code)>>scatter.txt
echo 	}>>scatter.txt
echo }>>scatter.txt
echo.>>scatter.txt
echo RAM %RAMAddr% 0x3e9>>scatter.txt
echo {>>scatter.txt
echo 	RAM1 +0 0x200>>scatter.txt
echo 	{>>scatter.txt
echo 		*.o(+RW,+BSS,+ZI)>>scatter.txt
echo 	}>>scatter.txt
echo }>>scatter.txt


set obj_input=
for %%c in (*.o) do (
	set obj_input=!obj_input! %%c
	rem echo %%c
)
rem echo %obj_input%

armlink -scatter scatter.txt -via viafile.txt -symdefs out.sym %obj_input% %symfile% -o patch.elf
if ERRORLEVEL 1 goto :error_link
rem pause

@move /Y patch.elf ..\build\ >nul
@move /Y out.sym ..\build\ >nul

set obj_remove=
for %%c in (*.o) do (
	if /I "%%c" NEQ "libc.o" (
		set obj_remove=!obj_remove! %%c
	)
	rem echo %%c
)
rem echo %obj_input%

@del /Q %obj_remove% >nul
@del /Q scatter.txt >nul

popd

echo DONE
echo.
echo -------------------------------- Making Patches --------------------------------
echo.

pushd build\

rem increment %build% counter
if not exist build.txt echo 0 >build.txt
set /p build=<build.txt
set /a build="build+1"
echo %build% >build.txt
echo BUILD: %build%

rem
@del /Q patch.fpa Register.fpa ResultPatch.fpa UpdDisplInjection.fpa >nul

fromelf patch.elf -bin -output .
if ERRORLEVEL 1 goto :error_fe

echo %addr% %off%
BinToFpa %off:~2% patch.bin
BinToFpa %UDInjOff:~2% UpdDisplInjection.bin

rem @del /Q patch.bin UpdDisplInjection.bin >nul


findstr "elfpackEntry_ven" out.sym >res.txt
set /p register_addr=<res.txt
set /a register_addr=%register_addr:~0,10%+1
dec2hex %register_addr% >res.txt
set /p register_addr=<res.txt
echo %register_addr%
@del /Q res.txt >nul


@echo [Patch_Info]>Register.fpa
@echo SW_Ver=%FW%>>Register.fpa
@echo Description=Registers ELFpack v2.0.%build%>>Register.fpa
@echo Author=Andy51, tim apple>>Register.fpa
@echo [Patch_Code]>>Register.fpa
@echo %register_off:~2,8%: %register_addr:~2%>>Register.fpa

rem 
@del /Q out.sym >nul


echo DONE
echo.
echo --------------------------- Create All-In-One patch ----------------------------
echo.

rem получение номера ревизии
SubWCRev .. SubWCRev.tmpl SubWCRev.txt
set /p revision=<SubWCRev.txt
set revision=r.%revision%
echo %revision%


@echo [Patch_Info]>Uniter.ini
@echo SW_Ver=%FW%>>Uniter.ini
@echo Description=ELFPack v2.0 %revision% b.%build%>>Uniter.ini
@echo Version=2.0>>Uniter.ini
@echo Author=Andy51, tim apple>>Uniter.ini

Uniter UpdDisplInjection.fpa Register.fpa patch.fpa
copy /Y ResultPatch.fpa patch\%FW%.fpa


popd

echo DONE
echo.
echo ===================================== END ======================================
echo.

@del /Q res.txt >nul
rem @del /Q patch.elf >nul

goto :exit

:error_c
echo.
echo ERROR IN COMPILE *.c STAGE!
goto :exit

:error_asm
echo.
echo ERROR IN COMPILE *.asm STAGE!
goto :exit

:error_link
echo.
echo ERROR IN LINKAGE STAGE!
goto :exit

:error_fe
echo.
echo ERROR IN FROMELF STAGE!
goto :exit

:exit
rem pause
exit
