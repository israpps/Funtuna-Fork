#ifndef EMBEDDED_INCLUDED
#define EMBEDDED_INCLUDED

extern u8 opl_icn[];
extern int size_opl_icn;
//----------------------------------------//
extern u8 opl_sys[];
extern int size_opl_sys;
//----------------------------------------//
extern u8 opl_cfg[];
extern int size_opl_cfg;
//----------------------------------------//
extern u8 opl_cfg1[];
extern int size_opl_cfg1;
//----------------------------------------//
extern u8 opl_dualshock[];
extern int size_opl_dualshock;
//----------------------------------------//
extern u8  opentuna_slims[];
extern int size_opentuna_slims;
//----------------------------------------//
extern u8 opentuna_fats[];
extern int size_opentuna_fats;
//----------------------------------------//
extern u8 opentuna_fat170[];
extern int size_opentuna_fat170;
//----------------------------------------//
extern u8 exploit_sys[];
extern int size_exploit_sys;
//----------------------------------------//
extern u8 FMCB_ELF[];
extern int size_FMCB_ELF;
//----------------------------------------//
extern u8 FMCB_CNF[];
extern int size_FMCB_CNF;
//----------------------------------------//
extern u8 CFG_ELF[];
extern int size_CFG_ELF;
//----------------------------------------//
extern u8 boot_icn[];
extern int size_boot_icn;
//----------------------------------------//
extern u8 boot_sys[];
extern int size_boot_sys;
//----------------------------------------//
extern u8 ule_cnf[];
extern int size_ule_cnf;
//----------------------------------------//
extern u8 FUNTUNA_USBD[];
extern int size_FUNTUNA_USBD;
//----------------------------------------//
extern u8 FUNTUNA_BDM[];
extern int size_FUNTUNA_BDM;
//----------------------------------------//
extern u8 FUNTUNA_BDMFS_FATFS[];
extern int size_FUNTUNA_BDMFS_FATFS;
//----------------------------------------//
extern u8 FUNTUNA_USBMASS_BD[];
extern int size_FUNTUNA_USBMASS_BD;
//----------------------------------------//
extern u8 poweroff_elf[];
extern int size_poweroff_elf;
//----------------------------------------//
extern u8 opl_elf[];
extern int size_opl_elf;
//----------------------------------------//
extern u8 ule_elf[];
extern int size_ule_elf;
//----------------------------------------//
extern u8 apps_icn[];
extern int size_apps_icn;
//----------------------------------------//
extern u8 apps_sys[];
extern int size_apps_sys;

// Embedded IOP drivers
extern unsigned char SIO2MAN_irx[];
extern unsigned int size_SIO2MAN_irx;
//----------------------------------------//
extern unsigned char PADMAN_irx[];
extern unsigned int size_PADMAN_irx;
//----------------------------------------//
extern unsigned char MCMAN_irx[];
extern unsigned int size_MCMAN_irx;
//----------------------------------------//
extern unsigned char MCSERV_irx[];
extern unsigned int size_MCSERV_irx;

#endif //EMBEDDED_INCLUDED