Motorola ElfPack EM1 M*CORE SDK
===============================

This package for Windows platform contains the following tools:

* M*CORE C/C++ GCC Compiler 3.4.6 for compiling source code.
* Elfs sources and build scripts.
* ElfLoader sources and build scripts.
* SDK header files by theC0r3, flash.tato, z3DD3r, Chik_v, Andy51, tim_apple, Macho, G-XaD, om2804, kadukmm, fkcoder, EXL, and other developers from MotoFan.Ru forum.
* CMake build system module by EXL.

## Install

Unpack archive and copy all files to `C:\MCORE_EM1` directory, add `C:\MCORE_EM1\GCC_MCore\bin` to `%PATH%` environment variable (optional).

## Install via Git Bash

```sh
cd /c/
git clone https://github.com/MotoFanRu/ELFKIT_EM1_Windows.git MCORE_EM1 --depth=1 -b master
git clone git@github.com:MotoFanRu/ELFKIT_EM1_Windows.git MCORE_EM1 --depth=1 -b master
```

## Usage SDK

Set proper paths to GCC compiler, libraries and SDK headers in `make.cmd`, `make.bat`, `build.cmd`, `compile.bat` and other build scripts.

```bat
C:\Project> make
C:\Project> build
C:\Project> compile
```

## Usage CMake and Ninja

```bat
C:\Project> cmake . -G Ninja -DELFPACK=EM1
C:\Project> ninja -v
```

## Authors

theC0r3, flash.tato, z3DD3r, Chik_v, Andy51, tim_apple, Macho, G-XaD, om2804, kadukmm, fkcoder, EXL etc.

## Additional Information

* [SDK on theC0r3's GitHub](https://github.com/TheProjecter/rainbowelfloader)
* [SDK on flash.tato's GitHub](https://github.com/AntonioL/rainbowelfloader)
* [ELFs developing and porting](https://forum.motofan.ru/index.php?showforum=184)

© MotoFan.Ru, 2007-2023.
