EE_BIN = uncompressed_poweroff.elf
EE_BIN_PACKED = POWEROFF.ELF
EE_LIBS = -lpoweroff -lpatches -lc -lkernel
EE_LDFLAGS = -s -Os 
EE_OBJS = main.o POWEROFF.o

all: $(EE_BIN)
	../tools/ps2_packer/ps2_packer.exe $(EE_BIN) $(EE_BIN_PACKED)


clean:
	rm -f $(EE_BIN) $(EE_OBJS) $(EE_BIN_PACKED) *_irx.c
	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal