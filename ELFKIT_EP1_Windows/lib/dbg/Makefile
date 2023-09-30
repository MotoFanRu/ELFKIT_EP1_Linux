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
LIB_NAME = dbg

all:
	$(ARM_PATH)/bin/tcc -I$(SDK_PATH) $(DEFINES) -bigend -apcs /interwork -O2 -c $(LIB_NAME).c \
		-o $(LIB_NAME).o

clean:
	-rm -f $(LIB_NAME).o
