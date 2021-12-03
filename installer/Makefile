EE_BIN = Installer.elf
EE_BIN_PACKED = FunTuna-Fork_Installer.ELF
EE_BIN_STRIPPED = stripped.elf
EE_OBJS = main.o gs.o pad.o  gs_asm.o ps2_asm.o dma_asm.o
EE_OBJS += FMCB_ELF.o CFG_ELF.o opl_elf.o ule_elf.o apps_icn.o apps_sys.o poweroff_elf.o FUNTUNA_USBHDFSD.o FUNTUNA_USBD.o ule_cnf.o boot_sys.o boot_icn.o exploit_sys.o exploit_bin.o FMCB_CNF.o OpenTuna_SLIMS.o OpenTuna_FAT-170.o OpenTuna_FAT-110-120-150-160.o opl_icn.o opl_sys.o opl_cfg.o opl_cfg1.o opl_dualshock.o
EE_SRC =   FMCB_ELF.s CFG_ELF.s opl_elf.s ule_elf.s apps_icn.s apps_sys.s poweroff_elf.s FUNTUNA_USBHDFSD.s FUNTUNA_USBD.s ule_cnf.s boot_sys.s boot_icn.s exploit_sys.s exploit_bin.s FMCB_CNF.s OpenTuna_SLIMS.s OpenTuna_FAT-170.s OpenTuna_FAT-110-120-150-160.s opl_icn.s opl_sys.s opl_cfg.s opl_cfg1.s opl_dualshock.s
EE_LIBS = -ldebug -lcdvd -lpatches -lpad -lmc

ifeq ($(BOOT_FUNTUNA_ON_SUCCESS),1)
	EE_CFLAGS += -DBOOT_FUNTUNA_ON_SUCCESS -DELF_LAUNCHER
endif


all:
	@echo Building FunTuna Fork Installer
	$(MAKE) $(EE_BIN_PACKED)
#FREEMCBOOT:
#	test -f ../FreeMcBoot/BOOT.ELF || make -C ../FreeMcBoot clean all
#	cp ../FreeMcBoot/BOOT.ELF INSTALLER_CONTENTS/BOOT/BOOT.ELF
#FREEMCBOOT_CONFIGURATOR:
#	test -f ../FreeMcBoot-Configurator/CFG.ELF || make -C ../FreeMcBoot-Configurator clean all
#	cp ../FreeMcBoot-Configurator/CFG.ELF INSTALLER_CONTENTS/BOOT/CFG.ELF

# BOOT
FMCB_ELF.s:
	@bin2s INSTALLER_CONTENTS/BOOT/BOOT.ELF FMCB_ELF.s FMCB_ELF
	
FMCB_CNF.s:
	@bin2s INSTALLER_CONTENTS/BOOT/FUNTUNA.CNF FMCB_CNF.s FMCB_CNF

CFG_ELF.s:
	@bin2s INSTALLER_CONTENTS/BOOT/CFG.ELF CFG_ELF.s CFG_ELF
	
boot_icn.s:
	@bin2s INSTALLER_CONTENTS/BOOT/SYSTEM.icn boot_icn.s boot_icn

boot_sys.s:
	@bin2s INSTALLER_CONTENTS/BOOT/icon.sys boot_sys.s boot_sys

ule_elf.s:
	@bin2s INSTALLER_CONTENTS/BOOT/ULE.ELF ule_elf.s ule_elf

ule_cnf.s:
	@bin2s INSTALLER_CONTENTS/BOOT/LAUNCHELF.CNF ule_cnf.s ule_cnf

FUNTUNA_USBD.s:
	@bin2s INSTALLER_CONTENTS/BOOT/USBD.IRX FUNTUNA_USBD.s FUNTUNA_USBD
	
FUNTUNA_USBHDFSD.s:
	@bin2s INSTALLER_CONTENTS/BOOT/USBHDFSD.IRX FUNTUNA_USBHDFSD.s FUNTUNA_USBHDFSD

poweroff_elf.s:
	@bin2s INSTALLER_CONTENTS/BOOT/POWEROFF.ELF poweroff_elf.s poweroff_elf
	
# /BOOT
# APPS
apps_icn.s:
	@bin2s INSTALLER_CONTENTS/APPS/funtuna_apps.icn apps_icn.s apps_icn

apps_sys.s:
	@bin2s INSTALLER_CONTENTS/APPS/icon.sys apps_sys.s apps_sys
	
opl_elf.s:
	@bin2s INSTALLER_CONTENTS/APPS/OPNPS2LD.ELF opl_elf.s opl_elf
# /APPS

# OPL

opl_icn.s:
	@bin2s INSTALLER_CONTENTS/OPL/opl.icn opl_icn.s opl_icn
	
opl_sys.s:
	@bin2s INSTALLER_CONTENTS/OPL/icon.sys opl_sys.s opl_sys
	
opl_cfg.s:
	@bin2s INSTALLER_CONTENTS/OPL/conf_opl.cfg opl_cfg.s opl_cfg
	
opl_cfg1.s:
	@bin2s INSTALLER_CONTENTS/OPL/conf_opl1.cfg opl_cfg1.s opl_cfg1
	
opl_dualshock.s:
	@bin2s INSTALLER_CONTENTS/OPL/conf_game.cfg opl_dualshock.s opl_dualshock
	
# /OPL


# EXPLOIT
exploit_bin.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/exploit.bin exploit_bin.s exploit_bin
	
OpenTuna_SLIMS.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/slims.bin OpenTuna_SLIMS.s opentuna_slims
	
OpenTuna_FAT-110-120-150-160.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/fats.bin OpenTuna_FAT-110-120-150-160.s opentuna_fats
	
OpenTuna_FAT-170.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/170.bin OpenTuna_FAT-170.s opentuna_fat170


exploit_sys.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/exploit.sys exploit_sys.s exploit_sys
# /EXPLOIT
clean:
	@echo cleaning...
	@echo ...objs and $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@rm -fr *.o $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@echo ...embedded resources
	@rm -fr FMCB_ELF.* FMCB_CNF.* CFG_ELF.* opl_elf.* ule_elf.* apps_icn.* apps_sys.* poweroff_elf.* FUNTUNA_USBHDFSD.* FUNTUNA_USBD.* ule_cnf.* boot_sys.* boot_icn.* OpenTuna_SLIMS.* OpenTuna_FAT-170.* OpenTuna_FAT-110-120-150-160.* opl_icn.* opl_sys.* opl_cfg.* opl_cfg1.* opl_dualshock.*

#clean-fmcb:
#	make -C ../FreeMcBoot-Configurator clean
#	make -C ../FreeMcBoot clean

clean-bin:
	@rm $(EE_BIN_PACKED) 
	@rm $(EE_BIN_STRIPPED) 
	@rm $(EE_BIN)

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer $< $@ > /dev/null

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
