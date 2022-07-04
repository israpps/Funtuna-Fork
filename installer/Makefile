EE_BIN = Installer.elf
EE_BIN_PACKED = FunTuna-Fork_Installer.ELF
EE_BIN_STRIPPED = stripped.elf
EE_OBJS = main.o gs.o pad.o  gs_asm.o ps2_asm.o dma_asm.o 

EMBEDDED_FILES_OBJ = FMCB_ELF.o CFG_ELF.o opl_elf.o ule_elf.o apps_icn.o apps_sys.o poweroff_elf.o FUNTUNA_USBD.o ule_cnf.o boot_sys.o boot_icn.o exploit_sys.o FMCB_CNF.o OpenTuna_SLIMS.o OpenTuna_FAT-170.o OpenTuna_FAT-110-120-150-160.o opl_icn.o opl_sys.o opl_cfg.o opl_cfg1.o opl_dualshock.o PADMAN_irx.o SIO2MAN_irx.o MCMAN_irx.o MCSERV_irx.o FUNTUNA_USBMASS_BD.o FUNTUNA_BDMFS_FATFS.o FUNTUNA_BDM.o
EMBEDDED_FILES_SRC = FMCB_ELF.s CFG_ELF.s opl_elf.s ule_elf.s apps_icn.s apps_sys.s poweroff_elf.s FUNTUNA_USBD.s ule_cnf.s boot_sys.s boot_icn.s exploit_sys.s FMCB_CNF.s OpenTuna_SLIMS.s OpenTuna_FAT-170.s OpenTuna_FAT-110-120-150-160.s opl_icn.s opl_sys.s opl_cfg.s opl_cfg1.s opl_dualshock.s PADMAN_irx.c SIO2MAN_irx.c MCMAN_irx.c MCSERV_irx.c FUNTUNA_USBMASS_BD.s FUNTUNA_BDMFS_FATFS.s FUNTUNA_BDM.s

EE_OBJS += $(EMBEDDED_FILES_OBJ)
EE_SRC = $(EMBEDDED_FILES_SRC)
EE_LIBS = -ldebug -lcdvd -lpatches -lpadx -lmc

PACKAGE_SIZE := $(shell du -s INSTALLER_CONTENTS/ | cut -f1)

ifeq ($(BOOT_FUNTUNA_ON_SUCCESS),1)
	EE_CFLAGS += -DBOOT_FUNTUNA_ON_SUCCESS -DELF_LAUNCHER
endif
ifeq ($(USE_PRINTF),1)
	EE_CFLAGS += -DUSE_PRINTF
endif

all:
	@echo Building FunTuna Fork Installer
	@echo package size is \[$(PACKAGE_SIZE)\]
	$(MAKE) $(EE_BIN_PACKED)
#FREEMCBOOT:
#	test -f ../FreeMcBoot/BOOT.ELF || make -C ../FreeMcBoot clean all
#	cp ../FreeMcBoot/BOOT.ELF INSTALLER_CONTENTS/BOOT/BOOT.ELF
#FREEMCBOOT_CONFIGURATOR:
#	test -f ../FreeMcBoot-Configurator/CFG.ELF || make -C ../FreeMcBoot-Configurator clean all
#	cp ../FreeMcBoot-Configurator/CFG.ELF INSTALLER_CONTENTS/BOOT/CFG.ELF



clean:
	@echo cleaning...
	@echo ...objs and $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@rm -fr *.o $(EE_BIN_PACKED) $(EE_BIN_STRIPPED) $(EE_BIN)
	@echo ...embedded resources
	@rm -fr $(EMBEDDED_FILES_OBJ) $(EMBEDDED_FILES_SRC)

clean-bin:
	@rm $(EE_BIN_PACKED)
	@rm $(EE_BIN_STRIPPED)
	@rm $(EE_BIN)

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer -v $< $@

package-size:
	@echo package size is \[$(PACKAGE_SIZE)\]

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
include embeds.make