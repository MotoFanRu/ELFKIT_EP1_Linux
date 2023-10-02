Motorola ElfPack 1.0 SDK
========================

This package for Linux platform contains the following tools:

* ARM C/C++ Compiler, ADS1.2 [Build 848] for compiling source code.
* SDK header files by Andy51, G-XaD, and other developers from MotoFan.Ru forum.
* FBrowser ELF application by Andy51 and auto.run file.
* QBS build system module files by kuzulis.
* PDF documentation by ARM Limited Ltd.

## Install

Unpack archive and copy all files to `/opt/arm` directory, then add `/opt/arm/bin` catalog to `$PATH` environment variable.

```sh
7za x *.tar.7z
# Password: MotoFan
sudo tar -xvf arm.tar -C /opt
echo 'PATH=$PATH:/opt/arm/bin' >> ~/.bashrc
```

## Usage SDK

Set proper paths to ADS compiler, libraries and SDK headers in `Makefile` build script.

```sh
make clean
make
```

## Pack SDK

After some SDK changes.

```sh
cd /opt/
tar -cvf ~/Downloads/arm.tar arm
cd ~/Downloads/
7za a -t7z -m0=lzma -mx=0 -mfb=64 -md=32m -ms=on -mhc=on -mhe=on -pMotoFan arm.tar.7z arm.tar
```

## Authors

Andy51, G-XaD, om2804, kadukmm, EXL, kuzulis, etc.

## Additional Information

[ELFs developing and porting](https://forum.motofan.ru/index.php?showforum=184)

© MotoFan.Ru, 2007-2023.
