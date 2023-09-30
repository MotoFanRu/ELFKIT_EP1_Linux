Motorola ElfPack 2.x SDK & LibGen
=================================

This package for Windows platform contains the following tools:

* ARM C/C++ GCC Compiler 4.4.0 (devkitARM release 26) for compiling source code.
* LibGen and PostLink utility by Andy51 and tim_apple for generate ELFs and ELF libraries.
* PostLink utility and ElfPack 2.0 source code by Andy51 and tim_apple.
* SDK header files by Andy51, G-XaD, om2804, and other developers from MotoFan.Ru forum.

## Install

Unpack archive and copy all files to `C:\devkitARM` directory, add it to `%PATH%` environment variable (optional).

## Usage SDK

Set proper paths to GCC compiler, libraries and SDK headers in `make.cmd`, `make.bat`, `build.cmd`, and other build scripts.

```
C:\Project> make
C:\Project> build
```

## Authors

Andy51, G-XaD, om2804, kadukmm, etc.

## Additional Information

[ELFs developing and porting](https://forum.motofan.ru/index.php?showforum=184)

© MotoFan.Ru, 2007-2023.
