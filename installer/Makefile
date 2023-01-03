EE_BIN = Installer.elf
EE_BIN_PACKED = FunTuna-Fork_Installer.ELF
EE_BIN_STRIPPED = stripped.elf
EE_OBJS = main.o gs.o pad.o  gs_asm.o ps2_asm.o dma_asm.o
EE_OBJS += PS2BBL_ELF.o opl_elf.o ule_elf.o apps_icn.o apps_sys.o boot_sys.o boot_icn.o exploit_sys.o exploit_bin.o OpenTuna_SLIMS.o OpenTuna_FAT-170.o OpenTuna_FAT-110-120-150-160.o opl_icn.o opl_sys.o opl_cfg.o opl_cfg1.o opl_dualshock.o PS2BBL_CFG.o PS2BBL_icn.o PS2BBL_sys.o
EE_SRC =   PS2BBL_ELF.s opl_elf.s ule_elf.s apps_icn.s apps_sys.s boot_sys.s boot_icn.s exploit_sys.s exploit_bin.s OpenTuna_SLIMS.s OpenTuna_FAT-170.s OpenTuna_FAT-110-120-150-160.s opl_icn.s opl_sys.s opl_cfg.s opl_cfg1.s opl_dualshock.s PS2BBL_CFG.s PS2BBL_icn.s PS2BBL_sys.s
EE_LIBS = -ldebug -lcdvd -lpatches -lpad -lmc

ifeq ($(BOOT_FUNTUNA_ON_SUCCESS),1)
	EE_CFLAGS += -DBOOT_FUNTUNA_ON_SUCCESS -DELF_LAUNCHER
endif


all:
	@echo Building FunTuna Fork Installer
	$(MAKE) $(EE_BIN_PACKED)

# BOOT
PS2BBL_ELF.s:
	@bin2s INSTALLER_CONTENTS/BOOT/BOOT.ELF PS2BBL_ELF.s PS2BBL_ELF

boot_icn.s:
	@bin2s INSTALLER_CONTENTS/BOOT/PS2BBL.icn boot_icn.s boot_icn

boot_sys.s:
	@bin2s INSTALLER_CONTENTS/BOOT/icon.sys boot_sys.s boot_sys

ule_elf.s:
	@bin2s INSTALLER_CONTENTS/BOOT/ULE.ELF ule_elf.s ule_elf
	
# /BOOT

#CONFIG
PS2BBL_CFG.s:
	@bin2s INSTALLER_CONTENTS/PS2BBL/CONFIG.INI PS2BBL_CFG.s PS2BBL_CFG
	
PS2BBL_icn.s:
	@bin2s INSTALLER_CONTENTS/PS2BBL/PS2BBL.icn PS2BBL_icn.s PS2BBL_icn

PS2BBL_sys.s:
	@bin2s INSTALLER_CONTENTS/PS2BBL/icon.sys PS2BBL_sys.s PS2BBL_sys
# / CONFIG

# APPS
apps_icn.s:
	@bin2s INSTALLER_CONTENTS/APPS/APPS.icn apps_icn.s apps_icn

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
