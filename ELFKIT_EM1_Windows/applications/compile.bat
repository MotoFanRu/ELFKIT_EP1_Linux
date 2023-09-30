REM Example compile file

..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -c -o GetMyPDS\GetMyPDS.o GetMyPDS\GetMyPDS.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init GetMyPDS\GetMyPDS.o -T..\GCC_MCore\mcore-elf\lib\linker_elf_pds.ld -oGetMyPDS.elf
..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o FlexEditor\FlexEditor.o FlexEditor\FlexEditor.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init FlexEditor\FlexEditor.o -T..\GCC_MCore\mcore-elf\lib\linker_elf.ld -oFlexEditor.elf
..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o Info\Info.o Info\Info.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init Info\Info.o -T..\GCC_MCore\mcore-elf\lib\linker_elf.ld -oInfo.elf
..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o MultiBkg\MultiBkg.o MultiBkg\MultiBkg.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init MultiBkg\MultiBkg.o -T..\GCC_MCore\mcore-elf\lib\linker_elf.ld -oMultiBkg.elf
..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o SwitchToBoot\SwitchToBoot.o SwitchToBoot\SwitchToBoot.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init SwitchToBoot\SwitchToBoot.o -T..\GCC_MCore\mcore-elf\lib\linker_elf.ld -oSwitchToBoot.elf
..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o FunL\FunL.o FunL\FunL.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init FunL\FunL.o -T..\GCC_MCore\mcore-elf\lib\linker_elf.ld -oFunLights.elf
..\GCC_MCore\bin\mcore-elf-gcc -fshort-wchar -mbig-endian -nostdinc -m340 -I..\SDK -fomit-frame-pointer -nostdlib -fno-builtin -c -o FontServ\FontServ.o FontServ\FontServ.c
..\GCC_MCore\bin\mcore-elf-ld -dn -r -no-bss-init FontServ\FontServ.o -T..\GCC_MCore\mcore-elf\lib\linker_elf.ld -oFontServ.elf

del /S *.o
pause
