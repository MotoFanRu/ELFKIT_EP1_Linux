# This Makefile was edited by EXL, 07-Dec-2022.
# Default platform is Motorola P2K, ElfPack v1.0, ADS1.2 [Build 848] on Linux.
# Warning: `-nodebug` flag option for `armlink` is buggy.

# Compiler path.
ARM_PATH = ../..

# SDK path.
SDK_PATH = $(ARM_PATH)/SDK

# Defines.
DEFINES = -D__P2K__

# Library name.
LIB_NAME = armlib

all:
	$(ARM_PATH)/bin/armasm -32 -bi -apcs /interwork $(LIB_NAME).asm \
		$(LIB_NAME).o

clean:
	-rm -f $(LIB_NAME).o
