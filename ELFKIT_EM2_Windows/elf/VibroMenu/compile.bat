@echo off

set src_files=src\app.c
set out_elfname=bin\VibroMenu.elf

set sdk_path=C:\MCORE_EM2\sdk
set gcc_path=C:\MCORE_EM2\bin

set gcc_params=-x c -fshort-wchar -funsigned-char -fomit-frame-pointer -fno-builtin -m340 -m4align -mbig-endian -nostdinc -nostdlib
set ld_params=-d -EB -N -r -s -Bstatic -Bsymbolic -Bsymbolic-functions --gc-sections -nostdinc -nostdlib --unresolved-symbols=report-all -z muldefs -z combreloc -z nodefaultlib

@echo on

%gcc_path%\mcore-elf-gcc.exe %gcc_params% -I%sdk_path% -c %src_files%
%gcc_path%\mcore-elf-ld.exe %ld_params% app.o -T%gcc_path%\..\mcore-elf\lib\linker_elf.ld -o%out_elfname%

del /S *.o
pause
