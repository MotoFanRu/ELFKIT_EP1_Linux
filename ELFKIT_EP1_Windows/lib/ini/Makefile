# This Makefile was edited by EXL, 07-Dec-2022.
# Default platform is Motorola P2K, ElfPack v1.0, ADS1.2 [Build 848] on Linux.
# Warning: `-nodebug` flag option for `armlink` is buggy.

# Compiler path.
ARM_PATH = ../..

# SDK path.
SDK_PATH = $(ARM_PATH)/SDK

# Defines.
DEFINES = -D__P2K__ -DOPTIMIZE_1

# Libraries.
LIB_NAME_CCFILE = ccfile
LIB_NAME_CINI = cini

all:
	$(ARM_PATH)/bin/tcpp -I$(SDK_PATH) $(DEFINES) -bigend -apcs /interwork -O2 -c $(LIB_NAME_CCFILE).c \
		-o $(LIB_NAME_CCFILE).o
	$(ARM_PATH)/bin/tcpp -I$(SDK_PATH) $(DEFINES) -bigend -apcs /interwork -O2 -c $(LIB_NAME_CINI).c \
		-o $(LIB_NAME_CINI).o

clean:
	-rm -f $(LIB_NAME_CCFILE).o
	-rm -f $(LIB_NAME_CINI).o
