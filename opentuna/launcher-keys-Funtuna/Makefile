EE_TARGET=payload
EE_BIN = $(EE_TARGET).elf
EE_BIN_STRIPPED = $(EE_TARGET)-stripped.elf
EE_BIN_PACKED= $(EE_TARGET)-packed.elf
EE_OBJS = main.o      timer.o  loader_elf.o pad.o
EE_LDFLAGS +=  -Wl,--gc-sections 
EE_LIBS = -lc -lpatches -lpad 
EE_INCS= -I$(GSKIT)/include -I$(PS2SDK)/ports/include
EE_CFLAGS=-Os 


all:
	$(MAKE) $(EE_BIN_PACKED) 
loader_elf.s: 
	$(MAKE) -C ../loader-old/
	bin2s ../loader-old/loader.elf loader_elf.s loader_elf	

clean:
	rm -f *.elf   *.o *.s *.bin
	$(MAKE) -C ../loader-old clean

			

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -s -R .comment -R .gnu.version --strip-unneeded -o $@ $<
	

	
$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
ifdef OS
	../ps2_packer/ps2_packer.exe -v $< $@
else
	wine ../ps2_packer/ps2_packer.exe -v $< $@
endif
	



	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

