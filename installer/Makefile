EE_BIN = Installer.elf
EE_BIN_PACKED = FunTuna-Fork_Installer.ELF
EE_BIN_STRIPPED = stripped.elf
EE_OBJS = main.o gs.o pad.o  gs_asm.o ps2_asm.o dma_asm.o
EE_OBJS += FMCB_ELF.o CFG_ELF.o opl_elf.o ule_elf.o apps_icn.o apps_sys.o poweroff_elf.o FUNTUNA_USBHDFSD.o FUNTUNA_USBD.o ule_cnf.o boot_sys.o boot_icn.o exploit_sys.o exploit_bin.o FMCB_CNF.o
EE_SRC =   FMCB_ELF.s CFG_ELF.s opl_elf.s ule_elf.s apps_icn.s apps_sys.s poweroff_elf.s FUNTUNA_USBHDFSD.s FUNTUNA_USBD.s ule_cnf.s boot_sys.s boot_icn.s exploit_sys.s exploit_bin.s FMCB_CNF.s
EE_LIBS = -ldebug -lcdvd -lpatches -lpad -lmc

all:
	$(MAKE) $(EE_BIN_PACKED)

# BOOT
FMCB_ELF.s:
	bin2s INSTALLER_CONTENTS/BOOT/BOOT.ELF FMCB_ELF.s FMCB_ELF
	
FMCB_CNF.s:
	bin2s INSTALLER_CONTENTS/BOOT/FUNTUNA.CNF FMCB_CNF.s FMCB_CNF

CFG_ELF.s:
	bin2s INSTALLER_CONTENTS/BOOT/CFG.ELF CFG_ELF.s CFG_ELF
	
boot_icn.s:
	bin2s INSTALLER_CONTENTS/BOOT/SYSTEM.icn boot_icn.s boot_icn

boot_sys.s:
	bin2s INSTALLER_CONTENTS/BOOT/icon.sys boot_sys.s boot_sys

ule_elf.s:
	bin2s INSTALLER_CONTENTS/BOOT/ULE.ELF ule_elf.s ule_elf

ule_cnf.s:
	bin2s INSTALLER_CONTENTS/BOOT/LAUNCHELF.CNF ule_cnf.s ule_cnf

FUNTUNA_USBD.s:
	bin2s INSTALLER_CONTENTS/BOOT/USBD.IRX FUNTUNA_USBD.s FUNTUNA_USBD
	
FUNTUNA_USBHDFSD.s:
	bin2s INSTALLER_CONTENTS/BOOT/USBHDFSD.IRX FUNTUNA_USBHDFSD.s FUNTUNA_USBHDFSD

poweroff_elf.s:
	bin2s INSTALLER_CONTENTS/BOOT/POWEROFF.ELF poweroff_elf.s poweroff_elf
	
# /BOOT
# APPS
apps_icn.s:
	bin2s INSTALLER_CONTENTS/APPS/funtuna_apps.icn apps_icn.s apps_icn

apps_sys.s:
	bin2s INSTALLER_CONTENTS/APPS/icon.sys apps_sys.s apps_sys
	
opl_elf.s:
	bin2s INSTALLER_CONTENTS/APPS/OPNPS2LD.ELF opl_elf.s opl_elf
# /APPS

# EXPLOIT
exploit_bin.s:
	bin2s INSTALLER_CONTENTS/ENTRYPOINT/exploit.bin exploit_bin.s exploit_bin
exploit_sys.s:
	bin2s INSTALLER_CONTENTS/ENTRYPOINT/exploit.sys exploit_sys.s exploit_sys
# /EXPLOIT
clean:
	@echo cleaning...
	@echo ...objs and *.elf
	@rm -fr *.o $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@echo ...embedded resources
	@rm -fr FMCB_ELF.* FMCB_CNF.* CFG_ELF.* opl_elf.* ule_elf.* apps_icn.* apps_sys.* poweroff_elf.* FUNTUNA_USBHDFSD.* FUNTUNA_USBD.* ule_cnf.* boot_sys.* boot_icn.*

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer $< $@ > /dev/null

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
