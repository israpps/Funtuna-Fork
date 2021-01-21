# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: Makefile 577 2004-09-14 14:41:46Z pixel $


IOP_OBJS_DIR = ./
IOP_BIN_DIR = ./
IOP_SRC_DIR = ./
IOP_INC_DIR = ./

IOP_BIN  = init.irx
IOP_OBJS = init.o imports.o

IOP_CFLAGS  +=  -fno-builtin -G0 -O2 
IOP_LDFLAGS += -s

all: $(IOP_OBJS_DIR) $(IOP_BIN_DIR) $(IOP_BIN) copy

clean:
	rm -f -r $(IOP_OBJS) $(IOP_BIN)

copy:
	cp init.irx ../init.irx

include $(PS2SDKSRC)/Defs.make
include $(PS2SDKSRC)/iop/Rules.make
include $(PS2SDKSRC)/iop/Rules.release
