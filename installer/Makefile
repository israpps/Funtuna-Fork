BINDIR = bin/
EE_BIN = $(BINDIR)Installer.elf
EE_BIN_PACKED = $(BINDIR)PS2BBL_OpenTuna_Installer.ELF
EE_BIN_STRIPPED = $(BINDIR)stripped.elf
EE_OBJS = main.o gs.o pad.o  gs_asm.o ps2_asm.o dma_asm.o
EE_OBJS += PS2BBL_ELF.o opl_elf.o ule_elf.o apps_icn.o apps_sys.o boot_sys.o boot_icn.o exploit_sys.o OpenTuna_SLIMS.o OpenTuna_FAT-170.o OpenTuna_FAT-110-120-150-160.o opl_icn.o opl_sys.o opl_cfg.o opl_cfg1.o opl_dualshock.o PS2BBL_CFG.o PS2BBL_icn.o PS2BBL_sys.o
EE_SRC =   PS2BBL_ELF.s opl_elf.s ule_elf.s apps_icn.s apps_sys.s boot_sys.s boot_icn.s exploit_sys.s OpenTuna_SLIMS.s OpenTuna_FAT-170.s OpenTuna_FAT-110-120-150-160.s opl_icn.s opl_sys.s opl_cfg.s opl_cfg1.s opl_dualshock.s PS2BBL_CFG.s PS2BBL_icn.s PS2BBL_sys.s
EE_LIBS = -ldebug -lcdvd -lpatches -lpad -lmc

ifeq ($(BOOT_FUNTUNA_ON_SUCCESS),1)
	EE_CFLAGS += -DBOOT_FUNTUNA_ON_SUCCESS -DELF_LAUNCHER
endif


all:
	@echo Building FunTuna Fork Installer
	$(MAKE) $(EE_BIN_PACKED)
	
rebuild: clean all
# BOOT
PS2BBL_ELF.c:
	@bin2c INSTALLER_CONTENTS/BOOT/BOOT.ELF PS2BBL_ELF.c PS2BBL_ELF

boot_icn.c:
	@bin2c INSTALLER_CONTENTS/BOOT/PS2BBL.icn boot_icn.c boot_icn

boot_sys.c:
	@bin2c INSTALLER_CONTENTS/BOOT/icon.sys boot_sys.c boot_sys

ule_elf.c:
	@bin2c INSTALLER_CONTENTS/BOOT/ULE.ELF ule_elf.c ule_elf
	
# /BOOT

#CONFIG
PS2BBL_CFG.c:
	@bin2c INSTALLER_CONTENTS/PS2BBL/CONFIG.INI PS2BBL_CFG.c PS2BBL_CFG
	
PS2BBL_icn.c:
	@bin2c INSTALLER_CONTENTS/PS2BBL/PS2BBL.icn PS2BBL_icn.c PS2BBL_icn

PS2BBL_sys.c:
	@bin2c INSTALLER_CONTENTS/PS2BBL/icon.sys PS2BBL_sys.c PS2BBL_sys
# / CONFIG

# APPS
apps_icn.c:
	@bin2c INSTALLER_CONTENTS/APPS/APPS.icn apps_icn.c apps_icn

apps_sys.c:
	@bin2c INSTALLER_CONTENTS/APPS/icon.sys apps_sys.c apps_sys
	
opl_elf.c:
	@bin2c INSTALLER_CONTENTS/APPS/OPNPS2LD.ELF opl_elf.c opl_elf
# /APPS

# OPL

opl_icn.c:
	@bin2c INSTALLER_CONTENTS/OPL/opl.icn opl_icn.c opl_icn
	
opl_sys.c:
	@bin2c INSTALLER_CONTENTS/OPL/icon.sys opl_sys.c opl_sys
	
opl_cfg.c:
	@bin2c INSTALLER_CONTENTS/OPL/conf_opl.cfg opl_cfg.c opl_cfg
	
opl_cfg1.c:
	@bin2c INSTALLER_CONTENTS/OPL/conf_opl1.cfg opl_cfg1.c opl_cfg1
	
opl_dualshock.c:
	@bin2c INSTALLER_CONTENTS/OPL/conf_game.cfg opl_dualshock.c opl_dualshock
	
# /OPL


# EXPLOIT
exploit_bin.c:
	@bin2c INSTALLER_CONTENTS/ENTRYPOINT/exploit.bin exploit_bin.c exploit_bin
	
OpenTuna_SLIMS.c:
	@bin2c INSTALLER_CONTENTS/ENTRYPOINT/slims.bin OpenTuna_SLIMS.c opentuna_slims
	
OpenTuna_FAT-110-120-150-160.c:
	@bin2c INSTALLER_CONTENTS/ENTRYPOINT/fats.bin OpenTuna_FAT-110-120-150-160.c opentuna_fats
	
OpenTuna_FAT-170.c:
	@bin2c INSTALLER_CONTENTS/ENTRYPOINT/170.bin OpenTuna_FAT-170.c opentuna_fat170


exploit_sys.c:
	@bin2c INSTALLER_CONTENTS/ENTRYPOINT/exploit.sys exploit_sys.c exploit_sys
# /EXPLOIT
clean:
	@echo cleaning...
	@echo ...objs and $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@rm -fr *.o $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@echo ...embedded resources
	@rm -fr FMCB_ELF.* FMCB_CNF.* CFG_ELF.* opl_elf.* ule_elf.* apps_icn.* apps_sys.* poweroff_elf.* FUNTUNA_USBHDFSD.* FUNTUNA_USBD.* ule_cnf.* boot_sys.* boot_icn.* OpenTuna_SLIMS.* OpenTuna_FAT-170.* OpenTuna_FAT-110-120-150-160.* opl_icn.* opl_sys.* opl_cfg.* opl_cfg1.* opl_dualshock.*
	@rm -fr PS2BBL*.s
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
