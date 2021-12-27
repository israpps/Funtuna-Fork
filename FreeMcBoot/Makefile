#  _____     ___ ____ 
#   ____|   |    ____|      PS2 Open Source Project
#  |     ___|   |____       
#  
#--------------------------------------------------------------------------
#
#    Copyright (C) 2008 - Neme & jimmikaelkael (www.psx-scene.com) 
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the Free McBoot License.
#    
#	 This program and any related documentation is provided "as is"
#	 WITHOUT ANY WARRANTIES, either express or implied, including, but not
# 	 limited to, implied warranties of fitness for a particular purpose. The
# 	 entire risk arising out of use or performance of the software remains
# 	 with you.
#    In no event shall the author be liable for any damages whatsoever
# 	 (including, without limitation, damages to your hardware or equipment,
# 	 environmental damage, loss of health, or any kind of pecuniary loss)
# 	 arising out of the use of or inability to use this software or
# 	 documentation, even if the author has been advised of the possibility
# 	 of such damages.
#
#    You should have received a copy of the Free McBoot License along with
#    this program; if not, please report at psx-scene :
#    http://psx-scene.com/forums/freevast/
#
#--------------------------------------------------------------------------     
#
# MakeFile
#
# ------------------------------------------------------------------------

LOADADDR=0x000b0000
EE_BIN = BOOT-UNC.ELF
EE_BIN_PACKED = BOOT.ELF
EE_BIN_STRIPPED = BOOT-STRIPPED.ELF

EE_INCS = -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -Wl,-Ttext -Wl,$(LOADADDR)   -L$(PS2SDK)/sbv/lib -L. 
EE_LIBS = -lcdvd -lpad -lpatches -lc -lkernel -ldebug
EE_CFLAGS +=  -DNEWLIB_PORT_AWARE

EE_OBJS = launcher2.o gs.o chkesr_rpc.o chkesr.o pad.o timer.o iomanx.o init.o dma_asm.o gs_asm.o ps2_asm.o elf_loader.o #usbhdfsd.o usbd.o 


all:
	@echo "="
	@echo "========================= Compiling FreeMcBoot ========================="
	$(MAKE) $(EE_BIN_PACKED)
	
	

$(EE_BIN_STRIPPED): $(EE_BIN)
	@echo "--------------- EE_STRIP ---------------"
	$(EE_STRIP) -s -R .comment -R .gnu.version --strip-unneeded -o $@ $<
	
$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	@echo "--------------- PS2-Packer ---------------"
	ps2-packer -v $< $@

clean:
	@echo "="
	@echo "================ Cleaning FreeMcBoot ============================="
	@echo "--------------- Cleaning init ---------------"
	@$(MAKE) -C init clean 
	@echo "--------------- Cleaning chkesr--------------- "	
	@$(MAKE) -C chkesr clean 
	@echo "--------------- Cleaning elf_loader ---------------"
	@$(MAKE) -C elf_loader clean 
	@echo "--------------- Cleaning files ---------------"
	
	@echo "--crt0.o"
	@rm -f crt0.o
	
	@echo "--*.ELF"
	@rm -f *.elf
	@rm -f *.ELF 
	
	@echo "--*.irx"
	@rm -f *.irx 
	
	@echo "--*.o"
	@rm -f *.o 
	
	@echo "--init.s "
	@rm -f init.s 
	
	@echo "--chkesr.s "
	@rm -f chkesr.s 
	
	@echo "--elf_loader.s"
	@rm -f elf_loader.s

format-check:
	@! find . -type f -a \( -iname \*.h -o -iname \*.c \) | xargs clang-format -style=file -output-replacements-xml | grep "<replacement " >/dev/null
	
	
iomanx.s:
	iomanx.s iomanx_irx
#filexio.s:
#	filexio.s filexio_irx
#usbd.s:
#	usbd.s usbd_irx
#usbhdfsd.s:
#	usbhdfsd.s usb_mass_irx
init.s:
	@echo "--------------- init ---------------"
	@$(MAKE) -C init 
	@echo "converting with bin2s"
	@bin2s init.irx init.s init_irx
chkesr.s:
	@echo "--------------- chkesr ---------------"
	@$(MAKE) -C chkesr 
	@echo "converting with bin2s"
	@bin2s chkesr.irx chkesr.s chkesr_irx
elf_loader.s:
	@echo "--------------- elf_loader ---------------"
	@$(MAKE) -C elf_loader 
	@echo "converting with bin2s"
	@bin2s elf_loader/elf_loader.elf elf_loader.s elf_loader

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal


