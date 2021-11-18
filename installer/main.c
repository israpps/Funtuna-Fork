//#define __DEBUG_PRINTF__

/*------------------------------------------------------------*/
#include "main.h"
//*/
#ifdef GS_DEBUG_ISRA
#define DEBUG_PRINTF(arg...) scr_printf(arg...);
#else 
#define DEBUG_PRINTF(arg...)	
#endif

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
//
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
extern u8 FUNTUNA_USBHDFSD[];
extern int size_FUNTUNA_USBHDFSD;
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

static int pad_inited = 0;
#include "FUNTUNA_FORK_INSTALLER_BG.h"
#include "FUNTUNA_FORK_INSTALLER_WELCOME.h"
#include "FUNTUNA_FORK_INSTALLER_SUCESS.h"
#include "FUNTUNA_FORK_INSTALLER_FAIL.h"

enum OPENTUNA_VARIANTS
{
	SLIMS = 0,	  // fat 0x190 and every 0x2?? ROM
	FATS,		  // 0x110, 0x120, 0x150, 0x160
	FAT170,		  // 0x170
	PROTOKERNELS, // this corresponds to rom 0x100 and 0x101, parrado won´t make the icons, but i will leavi it here in case someone makes it faster than instatuna
	UNSUPPORTED,
	
	OPENTUNA_VARIANTS_AMMOUNT
};
char* ICONTYPE_ALIAS[4] = {"190+","110+","170 ","100 "};

int GetIconType(unsigned long int ROMVERSION)
{
	int icontype = UNSUPPORTED;

	if (ROMVERSION >= 0x190)
		icontype = SLIMS;

	if ((ROMVERSION < 0x190) && (ROMVERSION >= 0x110))
		icontype = FATS;

	if (ROMVERSION == 0x170)
		icontype = FAT170;

	return icontype;
}

//--------------------------------------------------------------
static int file_exists(char *filepath)
{
	int fdn;

	fdn = open(filepath, O_RDONLY);
	if (fdn < 0)
		return 0;

	close(fdn);

	return 1;
}

//--------------------------------------------------------------
static void Reset_IOP(void)
{
	//parrado
	SifInitRpc(0);
	while (!SifIopReset("", 0))
	{
	};
	while (!SifIopSync())
	{
	};
	SifInitRpc(0);
}
//=============================================================
static void display_bmp(u16 W, u16 H, u32 *data)
{
	gs_print_bitmap(
	(gs_get_max_x() - W) / 2,   //x
	(gs_get_max_y() - H) / 2,   //y
	W,                          //w
	H,                          //h
	data                        //array
	);
}
//=============================================================

static void InitPS2(void)
{
	Reset_IOP();
	SifInitIopHeap();
	SifLoadFileInit();
	fioInit();

	sbv_patch_disable_prefix_check();
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);

	//Faltaba iniciar la MC (alexparrado)
	mcInit(MC_TYPE_MC);

	setupPad();
	waitAnyPadReady();
}

//write &embed_file to path
// returns:
// -1 fail to open | -2 failed to write | 0 succes
static int write_embed(void *embed_file, const int embed_size, char* folder, char* filename, int mcport)
{
	int fd, ret;
	char target[MAX_PATH];
	sprintf(target, "mc%u:/%s/%s", mcport, folder, filename);
	if ((ret = open(target, O_RDONLY)) < 0)  //if not exist
	{
		if ((fd = open(target, O_CREAT | O_WRONLY | O_TRUNC)) < 0) {
			return -1;
		}
		ret = write(fd, embed_file, embed_size);
		if (ret != embed_size) {
			return -2;
		}
		close(fd);
	}

#ifdef __DEBUG_PRINTF__
	printf("embed file written: %s\n", target);
#endif
	return 0;
}
static int write_embed_replace(void *embed_file, const int embed_size, char* folder, char* filename, int mcport)
{
	int fd, ret;
	char target[MAX_PATH];
	sprintf(target, "mc%u:/%s/%s", mcport, folder, filename);
		if ((fd = open(target, O_CREAT | O_WRONLY | O_TRUNC)) < 0) {
			return -1;
		}
		ret = write(fd, embed_file, embed_size);
		if (ret != embed_size) {
			return -2;
		}
		close(fd);

#ifdef __DEBUG_PRINTF__
	printf("embed file written: %s\n", target);
#endif
	return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//return 0 = ok, return 1 = error
static int install(int mcport, int icon_variant)
{
	display_bmp(640, 448, BG);
	scr_printf("Installing for memory card %u...\n",mcport);
	char version_manifest_path[31];
	char opl_settings_location[25];
	sprintf(opl_settings_location, "mc%u:/OPL/conf_opl.cfg", mcport);
	sprintf(version_manifest_path, "mc%u:/BXEXEC-OPENTUNA/icon.cnf", mcport); 
	int ret, fd, retorno;
	static int mc_Type, mc_Free, mc_Format;

	mcGetInfo( mcport, 0, &mc_Type, &mc_Free, &mc_Format);
	mcSync(0, NULL, &ret);

	//If there's no MC, we have an error:
	if (ret != -1){return 1;}

	//If it is not a PS2 MC, we have an error:
	if (mc_Type != 2){return 2;}

	//If there's no free space, we have an error:
	if (mc_Free < 2000){return 3;}//Installation actually requires less than this (something like 1.6MB), but i left a larger size for space check since OPL will create settings and icon files on first launch... (and users will innevitally load more files)

	//If the files exists, we have an error:
	if (mcport == 0) {
	if (file_exists("mc0:/BXEXEC-OPENTUNA/icon.icn")) {return 4;}
	if (file_exists("mc0:/BXEXEC-OPENTUNA/icon.sys")) {return 4;}
	if (file_exists("mc0:/BXEXEC-FUNTUNA/icon.icn")) {return 4;}
	if (file_exists("mc0:/BXEXEC-FUNTUNA/icon.sys")) {return 4;}
	} else {
	if (file_exists("mc1:/BXEXEC-OPENTUNA/icon.icn")) {return 4;}
	if (file_exists("mc1:/BXEXEC-OPENTUNA/icon.sys")) {return 4;}
	if (file_exists("mc1:/BXEXEC-FUNTUNA/icon.icn")) {return 4;}
	if (file_exists("mc1:/BXEXEC-FUNTUNA/icon.sys")) {return 4;}
	}
	//FOLDERS
	scr_printf("\tCreating Folders...\n");
	scr_printf("\t\tBOOT\n");
		ret = mcMkDir(mcport, 0, "BOOT");
		mcSync(0, NULL, &ret);
	scr_printf("\t\tAPPS\n");
		ret = mcMkDir(mcport, 0, "APPS");
		mcSync(0, NULL, &ret);
	scr_printf("\t\tOPL\n");
		ret = mcMkDir(mcport, 0, "OPL");
		mcSync(0, NULL, &ret);
	if (icon_variant != UNSUPPORTED){
	scr_printf("\t\tBXEXEC-OPENTUNA\n");
		ret = mcMkDir(mcport, 0, "BXEXEC-OPENTUNA");
		mcSync(0, NULL, &ret);	
	}
	///FOLDERS
	scr_printf("\tWriting Files\n");
	//OPENTUNA
	if (icon_variant != UNSUPPORTED) 
	{
		if (icon_variant == SLIMS)
		{
			retorno = write_embed(&opentuna_slims, size_opentuna_slims, "BXEXEC-OPENTUNA", "icon.icn", mcport);
			scr_printf("\t\tWriting Opentuna for SLIMS and SCPH-5XXXX with ROM 1.90\n");
		}
		else if (icon_variant == FATS)
		{
			retorno = write_embed(&opentuna_fats, size_opentuna_fats, "BXEXEC-OPENTUNA", "icon.icn", mcport);
			scr_printf("\t\tWriting Opentuna for FATS (from SCPH-18000 to SCPH-39XXX)\n");
		}
		else if (icon_variant == FAT170)
		{
			retorno = write_embed(&opentuna_fat170, size_opentuna_fat170, "BXEXEC-OPENTUNA", "icon.icn", mcport);
			scr_printf("\t\tWriting Opentuna for SCPH-5XXXX with ROM 1.70\n");
		}
		
		if (retorno < 0)
		{
			return 6;
		}
			retorno = write_embed(&exploit_sys, size_exploit_sys, "BXEXEC-OPENTUNA","icon.sys",mcport);
			if (retorno < 0) {return 6;}
			
				if (! ((fd = open(version_manifest_path, O_CREAT | O_WRONLY | O_TRUNC)) < 0)){
				ret = write(fd, ICONTYPE_ALIAS[icon_variant], 4);
				close(fd);
				}
		scr_printf("\t\tManipulating Opentuna icon timestamp\n");
		static sceMcTblGetDir mcDirAAA[64] __attribute__((aligned(64)));
		static sceMcStDateTime maximahora; //Maxium Timestamp, for the ones who does not speak Spanish
	
		maximahora.Resv2 = 0;
		maximahora.Sec = 59;
		maximahora.Min = 59;
		maximahora.Hour = 23;
		maximahora.Day = 31;
		maximahora.Month = 12;
		maximahora.Year = 2099;
		mcDirAAA->_Modify = maximahora;
		mcDirAAA->_Create = maximahora;
		mcSetFileInfo(mcport, 0, "BXEXEC-OPENTUNA", mcDirAAA, 0x02);
		mcSync(0, NULL, &ret);		
				
	} else {scr_printf("\t THIS PS2 IS NOT COMPATIBLE WITH OPENTUNA\n\tSKIPPING OPENTUNA FILES!\n");}
	///OPENTUNA
	
	//FUNTUNA&APPS
	scr_printf("\t\tAPPS folder icons\n");
		retorno = write_embed(&apps_sys, size_apps_sys, "APPS","icon.sys",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&apps_icn, size_apps_icn, "APPS","funtuna_apps.icn",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\tOPL\n");
		retorno = write_embed(&opl_elf, size_opl_elf, "APPS","OPNPS2LD.ELF",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\tFreeMcBoot\n");
		retorno = write_embed(&FMCB_ELF, size_FMCB_ELF, "BOOT", "BOOT.ELF",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&FMCB_CNF, size_FMCB_CNF, "BOOT", "FUNTUNA.CNF",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\tFreeMcBoot Configurator\n");
		retorno = write_embed(&CFG_ELF, size_CFG_ELF, "BOOT", "CFG.ELF",mcport);
		if (retorno < 0) {return 6;}
        
        scr_printf("\t\tFreeMcBoot USB drivers\n");
		retorno = write_embed(&FUNTUNA_USBD, size_FUNTUNA_USBD, "BOOT", "USBD.IRX",mcport);
		if (retorno < 0) {return 6;}
                retorno = write_embed(&FUNTUNA_USBHDFSD, size_FUNTUNA_USBHDFSD, "BOOT", "USBHDFSD.IRX",mcport);
		if (retorno < 0) {return 6;}

	scr_printf("\t\tBOOT icons\n");
		retorno = write_embed(&boot_icn, size_boot_icn, "BOOT", "SYSTEM.icn",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&boot_sys, size_boot_sys, "BOOT", "icon.sys",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\tuLaunchELF\n");
		retorno = write_embed(&ule_elf, size_ule_elf, "BOOT", "ULE.ELF",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&ule_cnf, size_ule_cnf, "BOOT", "LAUNCHELF.CNF",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\tPoweroff utility\n");
		retorno = write_embed(&poweroff_elf, size_poweroff_elf, "BOOT", "POWEROFF.ELF",mcport);
		if (retorno < 0) {return 6;}
	///FUNTUNA&APPS
	if (!file_exists(opl_settings_location))//If no OPL config file...
	{
		scr_printf("\t\tNo OPL settings found!\n\t\t Loading Preconfigured OPL Folder...\n");
		if (mcport==0)
			write_embed_replace(&opl_cfg, size_opl_cfg, "OPL", "conf_opl.cfg", mcport);//main config file has two variants, each of them has IGR Path assigned according to mcport value
		else write_embed_replace(&opl_cfg1, size_opl_cfg1, "OPL", "conf_opl.cfg", mcport);//
		
		write_embed_replace(&opl_icn, size_opl_icn, "OPL", "opl.icn", mcport);
		write_embed_replace(&opl_sys, size_opl_sys, "OPL", "icon.sys", mcport);
		write_embed_replace(&opl_dualshock, size_opl_dualshock, "OPL", "conf_game.cfg", mcport);
	}
	scr_printf("Installation Finished\n");
	return 0;
}
//--------------------------------------------------------------

static void CleanUp(void) //trimmed from FMCB
{
	if (pad_inited) {
		padPortClose(0,0);
		padPortClose(1,0);
		padEnd(); 
	}

	Reset_IOP();

	// Reloads common modules
	SifLoadFileInit();
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();

	SifLoadModule("rom0:SIO2MAN", 0, 0);
	SifLoadModule("rom0:CDVDFSV", 0, 0);
	SifLoadModule("rom0:CDVDMAN", 0, 0);
	SifLoadModule("rom0:MCMAN", 0, 0);
	SifLoadModule("rom0:MCSERV", 0, 0);
	SifLoadModule("rom0:PADMAN", 0, 0);

	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();

	FlushCache(0);
	FlushCache(2);

	// clear the screen
	gs_set_fill_color(0, 0, 0);
	gs_fill_rect(0, 0, gs_get_max_x(), gs_get_max_y());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//close program and go to browser
static void PS2_browser(void)
{
	CleanUp();
	LoadExecPS2("rom0:OSDSYS", 0, NULL);
}

int main (int argc, char *argv[])
{
	
	int fdn, icontype;
	unsigned long int ROM_VERSION;
	char romver[5];
	VMode = NTSC;

	// Loads Needed modules
	InitPS2();

	gs_reset(); // Reset GS
	if ((fdn = open("rom0:ROMVER", O_RDONLY)) > 0) // Reading ROMVER
	{
		read(fdn, romver, 4);
		close(fdn);

		if (romver[4] == 'E')
			VMode = PAL;
		romver[4] = '\0';
		ROM_VERSION = strtoul(romver, NULL, 16); //convert ROM version to unsigned long int for further use on automatic Install
	}

	if (VMode == PAL)
		gs_init(PAL_640_512_32);
	else
		gs_init(NTSC_640_448_32);

	//display_bmp(640, 448, inst);
	//display_bmp(640, 448, BG);
	icontype = GetIconType(ROM_VERSION);
	waitAnyPadReady();
	pad_inited = 1;

	int iz = 1;
	int menuactual = 101;//101: Initial Menu, 102: Installing (not needed), 103: Error, 104: Done
	//printf("FunTuna Fork Installer\n\nPress L1 to install on memory card 1 | Press R1 to install on memory card 2");

	display_bmp(640, 448, WELCOME);//Again, just in case of an old japanese console
	while (1) {
		readPad();

		if ( (new_pad & PAD_L1) && (menuactual == 101) ) {
			menuactual = 102;
			//display_bmp(640, 448, wait);
			iz = install(0,icontype);
			if(iz == 0){
				menuactual = 104;
				display_bmp(640, 448, SUCESS);
			}
			else {
				menuactual = 103;
				display_bmp(640,448,FAIL);
				//error
			}
		}
		else if ( (new_pad & PAD_R1) && (menuactual == 101) ) {
			menuactual = 102;
			//display_bmp(640, 448, wait);
			iz = install(1,icontype);
			if(iz == 0){
				menuactual = 104;
				display_bmp(640, 448, SUCESS);
			}
			else {
				menuactual = 103;
				display_bmp(640,448,FAIL);
				//error
			}
		}

		else if ((new_pad & PAD_START) && (menuactual == 103)) {PS2_browser();}
		else if ((new_pad & PAD_START) && (menuactual == 104)) {PS2_browser();}
	}

	return 0;
}
