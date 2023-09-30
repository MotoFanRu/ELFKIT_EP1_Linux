..\GCC_MCore\bin\mcore-elf-gcc -mbig-endian -nostdinc -I../SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o main.o main.c
..\GCC_MCore\bin\mcore-elf-as -EB functions.asm
..\GCC_MCore\bin\mcore-elf-ld -o functions.elf -EB a.out
..\GCC_MCore\bin\mcore-elf-ld -Bstatic main.o functions.elf -T..\GCC_MCore\mcore-elf\lib\linker_reg.ld -oout.bin

del *.out
del *.o
pause
