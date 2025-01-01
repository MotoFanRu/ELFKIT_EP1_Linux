@echo off
rem The target platform, change this to yours (LTE | LTE2 | V3i)
set PLATFORM=LTE

if %PLATFORM%==NONE (
			@echo			****  ERROR  ****
			@echo		You should specify the target platform!
			pause
			goto :EOF
                     )
if "%1"=="" ( 
		@echo			****  ERROR  ****
		@echo		You should pass the target CG1 as a parameter!
		pause
		goto :EOF
             )

del /Q /S build\
mkdir build

if %PLATFORM%==LTE (set base=0x10080000) else ( if %PLATFORM%==LTE2 (set base=0x10092000) else (set base=0x100A0000) )
fsize %1 >res.txt
set /p off=<res.txt
set /a "off=(off&-16)+16"
set /a addr="off+base"
dec2hex %off% >res.txt
set /p off=<res.txt
echo %off%
dec2hex %addr% >res.txt
set /p addr=<res.txt
echo %addr%


cd libgen


FunctionFinder ..\%1 functions.pat %base%
pause
set confirm=y
findstr "не найдена" symdef.txt
if %errorlevel%==0 (
			@echo			****  ERROR  ****
			@echo		Some functions were not found!
			set /p confirm="Do you want to continue(y/n)?"
                   )
if %confirm%==n goto :EOF

findstr "_region_table" symdef.txt >res.txt
set /p region_table=<res.txt
set region_table=%region_table:~0,10%

RAMTrans ..\%1 symdef.txt %region_table% %base%

if %PLATFORM%==LTE (	
			FunctionFinder ..\%1 LTE.pat %base%
			type symdef.txt>>functions.sym
			) else (
			FunctionFinder ..\%1 LTE2.pat %base%
			type symdef.txt>>functions.sym
			type LTE2_IROM.sym>>functions.sym
			)
                   
findstr "APP_SyncML_MainRegister" functions.sym >res.txt
set /p register_addr=<res.txt
set /a register_addr=%register_addr:~0,10%+1
..\dec2hex %register_addr% >res.txt
set /p register_addr=<res.txt
echo %register_addr%
@echo Register D %register_addr:~2%>Register.pat
FunctionFinder ..\%1 Register.pat 0x0

move /Y symdef.txt ..\obj\Register.txt
move /Y functions.sym ..\obj\
del res.txt Register.pat
pause
cd ..\obj

tcc -I"..\..\SDK" -c -bigend -apcs /interwork sysinfo.c
armlink -ro-base %addr% -symdefs elfpack.sym -o elfpack.elf Autorun.o ElfloaderApp.o elfloader.o sysinfo.o libc.o functions.sym
fromelf elfpack.elf -bin -output elfpack.bin
BinToFpa %off:~2% elfpack.bin
findstr "UtilLogStringData namecmp u_utoa LdrStartApp LdrLoadELF LdrUnloadELF LdrGetPhoneName LdrGetPlatformName LdrGetFirmwareMajorVersion LdrGetFirmwareMinorVersion" elfpack.sym > LibaryEditor.sym
findstr "AutorunMain" elfpack.sym >res.txt
set /p register_addr=<res.txt
set /a register_addr=%register_addr:~0,10%+1
..\dec2hex %register_addr% >res.txt
set /p register_addr=<res.txt
echo %register_addr%


findstr "Register" Register.txt >res.txt
set /p register_off=<res.txt
@echo [Patch_Info]>Register.fpa
@echo Description=Registers ELFpack v1.0>>Register.fpa
@echo Author=Andy51>>Register.fpa
@echo [Patch_Code]>>Register.fpa
@echo %register_off:~2,8%: %register_addr:~2%>>Register.fpa

findstr "Ldr UtilLogStringData namecmp u_utoa _ll_cmpu" elfpack.sym >>functions.sym
findstr /V "APP_SyncML_MainRegister APP_CALC_MainRegister _region_table" functions.sym >elfpack.sym

move /Y elfpack.sym functions.sym
move /Y functions.sym ..\libgen\
move /Y elfpack.fpa ..\build\
move /Y Register.fpa ..\build\
move /Y LibaryEditor.sym ..\libgen\

del elfpack.bin elfpack.elf sysinfo.o Register.txt res.txt

cd ..\libgen


Libgen functions.sym
armasm -16 -bigend -apcs /interwork functions.asm -o Lib.o

move /Y functions.bin ..\build\elfloader.lib
move /Y Lib.o ..\build\

del functions.asm functions.sym LibaryEditor.sym
cd ..

del res.txt
pause
