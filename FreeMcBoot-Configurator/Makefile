EE_BIN = CFG_UNPACKED.ELF
EE_BIN_PACKED = CFG.ELF
EE_BIN_STRIPPED = CFG_STRIPPED.ELF
EE_OBJS = main.o pad.o config.o elf.o draw.o loader.o  filer.o\
	timer.o \
	font_uLE.o
PS2CLIENT_IP = 192.168.0.4


EE_INCS := -I$(PS2DEV)/gsKit/include -I$(PS2SDK)/ports/include -Ioldlibs/libcdvd/ee

NEWLIB_NANO = 1
KERNEL_NOPATCH = 1
DUMMY_LIBC_INIT = 1

EE_LDFLAGS := -L$(PS2DEV)/gsKit/lib
EE_LIBS = -lpad -lgskit -ldmakit -lmc -lpatches -ldebug -lc -lm -lfileXio
EE_CFLAGS := -mno-gpopt -G0 -ffunction-sections
all: $(EE_BIN_PACKED)
	@echo "================ Compiling FreeMcBoot-Configurator ==============="

run: comp
	ps2client -h $(PS2CLIENT_IP) -t 1 execee host:$(EE_BIN_PACKED)
reset: clean
	ps2client -h $(PS2CLIENT_IP) reset
	
$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer $< $@

loader.s:
	@echo "--------------- compiling ELF loader ---------------"
	@$(MAKE) -C loader > /dev/null
	@echo "---------------"
	@echo embedding loader
	bin2s loader/elf_loader.elf loader.s loader_elf 
	@echo "---------------"
	
	
format-check:
	@! find . -type f -a \( -iname \*.h -o -iname \*.c \) | xargs clang-format -style=file -output-replacements-xml | grep "<replacement " >/dev/null
	

clean:
	@echo "---------- Cleaning ----------"
	@echo "--*.o "
	@rm -f *.o 
	@echo "--*.a "
	@rm -f *.a 
	@echo "--*.s "
	@rm -f *.s 
	@echo "--EE_BIN "
	@rm -f $(EE_BIN) 
	@echo "--EE_BIN_PACKED "
	@rm -f $(EE_BIN_PACKED) 
	@echo "--ELF_LOADER"
	@$(MAKE) -C loader clean
	

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
