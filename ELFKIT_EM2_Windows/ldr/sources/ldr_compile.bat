@echo off

set src_files=ldr_src\___gui___.c ldr_src\ldr_autorun.c ldr_src\ldr_core.c ldr_src\ldr_elflist.c ldr_src\ldr_library.c ldr_src\ldr_parser.c

set sdk_path=C:\MCORE_EM2\sdk
set gcc_path=C:\MCORE_EM2\bin

set lib_path=lib.bin
set out_name=ldr.bin

lib2asm.exe %lib_path%
IF NOT EXIST "lib.asm" GOTO END

@echo on

%gcc_path%\mcore-elf-gcc -nostdinc -nostdlib -fshort-wchar -fomit-frame-pointer -mbig-endian -m340 -I%sdk_path% -c %src_files%
%gcc_path%\mcore-elf-as -EB lib.asm
%gcc_path%\mcore-elf-ld -o lib.elf -EB a.out
%gcc_path%\mcore-elf-ld -Bstatic ___gui___.o ldr_autorun.o ldr_core.o ldr_elflist.o ldr_library.o ldr_parser.o lib.elf ^
	-T%gcc_path%\..\mcore-elf\lib\linker_ldr.ld -z muldefs -o%out_name%

@echo off
pause

del *.out
del *.elf
del *.o
del lib.asm
EXIT

:END
@echo off
echo ERROR. file lib.asm does not exist!

pause
EXIT