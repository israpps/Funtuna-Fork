EE_BIN = uncompressed_poweroff.elf
EE_BIN_PACKED = POWEROFF.ELF
EE_LIBS = -lpoweroff -lpatches -lc -lkernel

EE_OBJS = main.o poweroff_irx.o

all: $(EE_BIN)
	 $(PS2DEV)/bin/ps2-packer $(EE_BIN) $(EE_BIN_PACKED)

poweroff_irx.c: $(PS2SDK)/iop/irx/poweroff.irx
	bin2c $(PS2SDK)/iop/irx/poweroff.irx poweroff_irx.c poweroff_irx

clean:
	rm -f $(EE_BIN) $(EE_OBJS) $(EE_BIN_PACKED) *_irx.c
	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal