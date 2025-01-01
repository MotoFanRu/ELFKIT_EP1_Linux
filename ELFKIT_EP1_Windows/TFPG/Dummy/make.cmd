@echo off

set OPT=-DEP2 -DSETCANVASCONTROL

del /Q prelink.elf
arm-eabi-gcc -Wall -c -mbig-endian -mthumb -I..\SDK -fshort-wchar -nostdlib -mthumb-interwork %OPT% app.c main.c gfx.c resources.c
arm-eabi-ld -pie -EB -O -nostdlib app.o main.o gfx.o resources.o std.sa rt.lo -wrap memcpy -o prelink.elf
del /Q *.o
del /Q Dummy.elf
postlink prelink.elf -o Dummy.elf
pause