EE_BIN = BOOT.ELF
EE_BIN_PACKED = CFG.ELF
EE_OBJS = main.o pad.o config.o elf.o draw.o loader.o  filer.o\
	timer.o \
	font_uLE.o

EE_INCS := -I$(PS2DEV)/gsKit/include -I$(PS2DEV)/libjpg/include\
	-I$(PS2SDKOLD)/sbv/include -I$(PS2DEVOLD)/libcdvd/ee

EE_LDFLAGS := -L$(PS2DEV)/gsKit/lib -L$(PS2DEV)/libjpg\
	-L$(PS2SDKOLD)/sbv/lib -L$(PS2DEVOLD)/libcdvd/lib -s
EE_LIBS = -lpad -lgskit -ldmakit -lmc -lmf -lpatches -ldebug -lc

all:	$(EE_BIN)

run: comp
	ps2client -h 192.168.1.4 -t 1 execee host:FMCB_CFG.ELF
reset: clean
	ps2client -h 192.168.1.4 reset
comp: all
	ps2_packer $(EE_BIN) $(EE_BIN_PACKED)

loader.s:
	$(MAKE) -C loader
	bin2s loader/loader.elf loader.s loader_elf
	
	
format-check:
	@! find . -type f -a \( -iname \*.h -o -iname \*.c \) | xargs clang-format -style=file -output-replacements-xml | grep "<replacement " >/dev/null
	

clean:
	rm -f *.o *.a *.s BOOT.ELF BOOTc.ELF CFG.ELF

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
