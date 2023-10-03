Motorola ElfPack 1.x SDK & PortKit & TFPG
=========================================

This package for Windows platform contains the following tools:

* ARM C/C++ Compiler, ADS1.2 [Build 848] for compiling source code.
* PortKit by Andy51 for porting ElfPack 1.0 to various Motorola phones.
* FBrowser ELF application by Andy51 and auto.run file.
* TFPG by Andy51 for porting Tasks features for ELFs to various Motorola phones.
* LibraryEditor, ELFFuncV, LibCmp by kadukmm for editing and inspecting compiled ELF libraries and applications.
* SDK header files by Andy51, tim_apple, Macho, G-XaD, om2804, EXL, fkcoder and other developers from MotoFan.Ru forum.
* QBS build system module files by kuzulis.
* CMake build system module by EXL.
* PDF documentation by ARM Limited Ltd.
* Ninja v1.11.1 build tool (x86_64 arch version).

## Install

Unpack archive and copy all files to `C:\ARM` directory, add `C:\ARM` to `%PATH%` environment variable.

## Install via Git Bash

```sh
cd /c/
git clone https://github.com/MotoFanRu/ELFKIT_EP1_Windows.git ARM --depth=1 -b master
git clone git@github.com:MotoFanRu/ELFKIT_EP1_Windows.git ARM --depth=1 -b master
```

## Usage SDK

Set proper paths to ADS compiler, libraries and SDK headers in `make.cmd`, `make.bat`, `build.cmd`, and other build scripts.

```bat
C:\Project> make
C:\Project> build
```

## Usage CMake and Ninja

```bat
C:\Project> cmake . -G Ninja -DELFPACK=EP1
C:\Project> ninja -v
```

## Usage PortKit

```bat
C:\ARM> make_lib.cmd CG1_E398_R373_49R.smg
C:\ARM> make_lib.cmd CG1_L6_R3443H_0BR.smg
```

## Usage TFPG

Edit `generate.cmd` file.

```bat
C:\ARM\TFPG> generate.cmd
```

## Authors

Andy51, tim_apple, Macho, G-XaD, om2804, kadukmm, fkcoder, EXL, kuzulis, etc.

## Additional Information

[ELFs developing and porting](https://forum.motofan.ru/index.php?showforum=184)

© MotoFan.Ru, 2007-2023.
