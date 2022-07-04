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

FUNTUNA_BDM.s:
	@bin2s INSTALLER_CONTENTS/BOOT/BDM.IRX FUNTUNA_BDM.s FUNTUNA_BDM

FUNTUNA_BDMFS_FATFS.s:
	@bin2s INSTALLER_CONTENTS/BOOT/BDMFS_FATFS.IRX FUNTUNA_BDMFS_FATFS.s FUNTUNA_BDMFS_FATFS

FUNTUNA_USBMASS_BD.s:
	@bin2s INSTALLER_CONTENTS/BOOT/USBMASS_BD.IRX FUNTUNA_USBMASS_BD.s FUNTUNA_USBMASS_BD

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

# OPENTUNA

OpenTuna_SLIMS.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/slims.bin OpenTuna_SLIMS.s opentuna_slims

OpenTuna_FAT-110-120-150-160.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/fats.bin OpenTuna_FAT-110-120-150-160.s opentuna_fats

OpenTuna_FAT-170.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/170.bin OpenTuna_FAT-170.s opentuna_fat170

exploit_sys.s:
	@bin2s INSTALLER_CONTENTS/ENTRYPOINT/exploit.sys exploit_sys.s exploit_sys
# /OPENTUNA

# IRX
SIO2MAN_irx.c: $(PS2SDK)/iop/irx/freesio2.irx
	bin2c $(PS2SDK)/iop/irx/freesio2.irx SIO2MAN_irx.c SIO2MAN_irx

MCMAN_irx.c: $(PS2SDK)/iop/irx/mcman.irx
	bin2c $(PS2SDK)/iop/irx/mcman.irx MCMAN_irx.c MCMAN_irx

MCSERV_irx.c: $(PS2SDK)/iop/irx/mcserv.irx
	bin2c $(PS2SDK)/iop/irx/mcserv.irx MCSERV_irx.c MCSERV_irx

PADMAN_irx.c: $(PS2SDK)/iop/irx/freepad.irx
	bin2c $(PS2SDK)/iop/irx/freepad.irx PADMAN_irx.c PADMAN_irx

# /IRX