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

all:	
	@echo "================ Compiling FreeMcBoot-Configurator ==============="
	$(EE_BIN)
	

run: comp
	ps2client -h 192.168.1.4 -t 1 execee host:FMCB_CFG.ELF
reset: clean
	ps2client -h 192.168.1.4 reset
$(EE_BIN_PACKED): $(EE_BIN)
	../tools/ps2_packer/ps2_packer.exe -v $< $@

compressed: $(EE_BIN)
	@echo "--------------- PS2Packer ---------------"
	../tools/ps2_packer/ps2_packer.exe $(EE_BIN) $(EE_BIN_PACKED)
	@echo "---------------"
loader.s:
	@echo "--------------- compiling ELF loader ---------------"
	@$(MAKE) -C loader >nul
	@echo "---------------"
	@echo "--------------- creating loader.s ---------------"
	bin2s loader/loader.elf loader.s loader_elf 
	@echo "---------------"
	
	
format-check:
	@! find . -type f -a \( -iname \*.h -o -iname \*.c \) | xargs clang-format -style=file -output-replacements-xml | grep "<replacement " >/dev/null
	

clean:
	@echo "================ Cleaning FreeMcBoot-Configurator ================ "
	@echo "---------- Cleaning files ----------"
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
	

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
