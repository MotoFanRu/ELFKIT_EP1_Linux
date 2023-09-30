Motorola ElfPack 1.x SDK & PortKit & TFPG
=========================================

This package for Windows platform contains the following tools:

* ARM C/C++ Compiler, ADS1.2 [Build 848] for compiling source code.
* PortKit by Andy51 for porting ElfPack 1.0 to various Motorola phones.
* FBrowser ELF application by Andy51 and auto.run file.
* TFPG by Andy51 for porting Tasks features for ELFs to various Motorola phones.
* LibraryEditor, ELFFuncV, LibCmp by kadukmm for editing and inspecting compiled ELF libraries and applications.
* SDK header files by Andy51, G-XaD, om2804, and other developers from MotoFan.Ru forum.
* QBS build system module files by kuzulis.
* PDF documentation by ARM Limited Ltd.

## Install

Unpack archive and copy all files to `C:\ARM` directory, add it to `%PATH%` environment variable.

## Usage SDK

Set proper paths to ADS compiler, libraries and SDK headers in `make.cmd`, `make.bat`, `build.cmd`, and other build scripts.

```
C:\Project> make
C:\Project> build
```

## Usage PortKit

```
C:\ARM> make_lib.cmd CG1_E398_R373_49R.smg
C:\ARM> make_lib.cmd CG1_L6_R3443H_0BR.smg
```

## Usage TFPG

Edit `generate.cmd` file.

```
C:\ARM\TFPG> generate.cmd
```

## Authors

Andy51, G-XaD, om2804, kadukmm, EXL, kuzulis, etc.

## Additional Information

[ELFs developing and porting](https://forum.motofan.ru/index.php?showforum=184)

© MotoFan.Ru, 2007-2023.
