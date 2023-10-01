Motorola ElfPack 2.x SDK/Source Code & LibGen
=============================================

This package for Windows platform contains the following tools:

* ARM C/C++ GCC Compiler 4.4.0 (devkitARM release 26) for compiling source code.
* LibGen and PostLink utility by Andy51 and tim_apple for generate ELFs and ELF libraries.
* PostLink utility and ElfPack 2.0 source code by Andy51 and tim_apple.
* SDK header files by Andy51, G-XaD, om2804, EXL, and other developers from MotoFan.Ru forum.
* Ninja v1.11.1 build tool (x86_64 arch version).

## Install

Unpack archive and copy all files to `C:\devkitARM` directory, add  `C:\devkitARM\bin` to `%PATH%` environment variable (optional).

## Usage SDK

Set proper paths to GCC compiler, libraries and SDK headers in `make.cmd`, `make.bat`, `build.cmd`, and other build scripts.

```bat
C:\Project> make
C:\Project> build
```

## Usage CMake and Ninja

```bat
C:\Project> cmake . -G Ninja -DELFPACK=EP2
C:\Project> ninja -v
```

## Usage LibGen

```bat
C:\devkitARM\libgen> libgen_E1_49.cmd
C:\devkitARM\libgen> libgen_L7e_0A.cmd
```

## Compile ElfPack 2.x

Install [Motorola ElfPack 1.x SDK & PortKit & TFPG for Windows](https://github.com/MotoFanRu/ELFKIT_EP1_Windows) first.

```bat
C:\devkitARM\elfpack2> R373_G_0E.30.49R.cmd
```

## Authors

Andy51, tim_apple, G-XaD, om2804, kadukmm, EXL, etc.

## Additional Information

[ELFs developing and porting](https://forum.motofan.ru/index.php?showforum=184)

© MotoFan.Ru, 2007-2023.
