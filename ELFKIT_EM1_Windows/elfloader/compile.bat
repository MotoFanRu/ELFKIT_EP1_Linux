..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -I../SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o loader.o loader.c
..\GCC_MCore\bin\mcore-elf-as -EB functions.asm
..\GCC_MCore\bin\mcore-elf-ld -o functions.elf -EB a.out
..\GCC_MCore\bin\mcore-elf-ld -Bstatic loader.o functions.elf -T..\GCC_MCore\mcore-elf\lib\linker_ldr.ld -oldr.bin

del *.out
del *.elf
del *.o
pause
