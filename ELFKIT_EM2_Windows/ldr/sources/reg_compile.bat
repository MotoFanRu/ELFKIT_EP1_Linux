@echo off

set src_files=reg_src\main.c

set sdk_path=C:\MCORE_EM2\sdk
set gcc_path=C:\MCORE_EM2\bin

set lib_path=lib.bin
set out_name=elfloader.bin
set fpa_name=elfloader.fpa

set rom_addr=01200000

lib2asm.exe %lib_path%
IF NOT EXIST "lib.asm" GOTO END

@echo on

%gcc_path%\mcore-elf-gcc -nostdinc -nostdlib -fshort-wchar -funsigned-char -fpic -fpie -mbig-endian -m340 -I%sdk_path% -c %src_files%
%gcc_path%\mcore-elf-as -EB lib.asm
%gcc_path%\mcore-elf-ld -nostdinc -nostdlib -o lib.elf -EB a.out
%gcc_path%\mcore-elf-ld -nostdinc -nostdlib -Bstatic main.o lib.elf -T%gcc_path%\..\mcore-elf\lib\linker_reg.ld -o%out_name%

@echo off

REM this address for v3x r252211ld_u_85.9b.e6p
BinToFpa.exe  %rom_addr% %out_name%

del *.out
del *.elf
del *.o
del lib.asm
del %out_name%
EXIT

:END
@echo off
echo ERROR. file lib.asm does not exist!

pause
EXIT
