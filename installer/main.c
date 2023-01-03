//#define __DEBUG_PRINTF__

/*------------------------------------------------------------*/
#include "main.h"
#include <osd_config.h>
//*/
#ifdef GS_DEBUG_ISRA
#define DEBUG_PRINTF(arg...) scr_printf(arg...);
#else 
#define DEBUG_PRINTF(arg...)	
#endif
#define IMPORT_BIN(_n) \
extern u8 _n[]; \
extern int size_##_n

IMPORT_BIN(PS2BBL_CFG);
IMPORT_BIN(PS2BBL_icn);
IMPORT_BIN(PS2BBL_sys);
IMPORT_BIN(PS2BBL_ELF);
IMPORT_BIN(opl_icn);
IMPORT_BIN(opl_sys);
IMPORT_BIN(opl_cfg);
IMPORT_BIN(opl_cfg1);
IMPORT_BIN(opl_dualshock);
IMPORT_BIN(opentuna_slims);
IMPORT_BIN(opentuna_fats);
IMPORT_BIN(opentuna_fat170);
IMPORT_BIN(exploit_sys);
IMPORT_BIN(boot_icn);
IMPORT_BIN(boot_sys);
IMPORT_BIN(opl_elf);
IMPORT_BIN(ule_elf);
IMPORT_BIN(apps_icn);
IMPORT_BIN(apps_sys);

static int pad_inited = 0;

#define RBG2INT(R,G,B) ((0<<24) + (R<<16) + (G<<8) + B)
const char banner[] = {
    "\t\t__________  _________________   ____________________.____\n"
    "\t\t\\______   \\/   _____/\\_____  \\  \\______   \\______   \\    |\n"
    "\t\t |     ___/\\_____  \\  /  ____/   |    |  _/|    |  _/    |\n"
    "\t\t |    |    /        \\/       \\   |    |   \\|    |   \\    |___\n"
    "\t\t |____|   /_______  /\\_______ \\  |______  /|______  /_______ \\\n"
    "\t\t                  \\/         \\/         \\/        \\/        \\/\n"
};

enum OPENTUNA_VARIANTS
{
	SLIMS = 0,	  // fat 0x190 and every 0x2?? ROM
	FATS,		  // 0x110, 0x120, 0x150, 0x160
	FAT170,		  // 0x170
	PROTOKERNELS, // this corresponds to rom 0x100 and 0x101, parrado won´t make the icons, but i will leavi it here in case someone makes it faster than instatuna
	UNSUPPORTED,
	
	OPENTUNA_VARIANTS_AMMOUNT
};

enum INSTALL_ERRORS
{
	INST_SUCESS = 0,
	NO_MEMORY_CARD,
	NO_PS2_MEMORY_CARD,
	NOT_ENOUGH_SPACE,
	FORTUNA_FOUND,
	OPENTUNA_FOUND,
	FUNTUNA_FORK_FOUND,
	BOOT_FOLDER_EXISTS,
	APPS_FOLDER_EXISTS,
	
	
	TOTAL_ERRORS
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
static void ResetIOP(void)
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
#ifdef ELF_LAUNCHER
	void LoadElf(char *filename, char *party)
	{
	
		char *args[1];
		t_ExecData exec;
		SifLoadElf(filename, &exec);
	
		if (exec.epc > 0)
		{
			ResetIOP();
	
			if (party != 0)
			{
				args[0] = party;
				ExecPS2((void *)exec.epc, (void *)exec.gp, 1, args);
			}
			else
			{
				ExecPS2((void *)exec.epc, (void *)exec.gp, 0, NULL);
			}
		}
	}
#endif
//--------------------------------------------------------------
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
/// DeleteFolder(); function was SP193's FreeMcBoot installer.
//thanks to SP193 for all his work
static int DeleteFolder(const char *folder)
{
	DIR *d = opendir(folder);
	size_t path_len = strlen(folder);
	int r = -1;

	if (d)
	{
		scr_printf("Detected [%s], deleting...\n",folder);
		struct dirent *p;

		r = 0;
		while (!r && (p = readdir(d)))
		{
			int r2 = -1;
			char *buf;
			size_t len;

			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);

			if (buf)
			{
				struct stat statbuf;

				snprintf(buf, len, "%s/%s", folder, p->d_name);
				if (!stat(buf, &statbuf))
				{
					if (S_ISDIR(statbuf.st_mode))
						r2 = DeleteFolder(buf);
					else
						r2 = unlink(buf);
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}

	if (!r)
		r = rmdir(folder);

	return r;
}
//=============================================================
static void InitPS2(void)
{
	printf("%s: start\n", __FUNCTION__);
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	fioInit();
	sbv_patch_disable_prefix_check();
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	sbv_patch_fileio();// THANKS fjtrujy

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
	scr_clear();
	scr_setfontcolor(0x00ffff);
	scr_printf("\n\n\nInstalling for memory card %u...\n",mcport);
	scr_setfontcolor(0xffffff);
	char version_manifest_path[32];
	char opl_settings_location[32];
	char opl_daily_bulshit_cnf[32];
	char temp_path[32];
	sprintf(opl_settings_location, "mc%u:/OPL/conf_opl.cfg", mcport);
	sprintf(version_manifest_path, "mc%u:/BXEXEC-OPENTUNA/icon.cnf", mcport);
	int ret,
		fd, 
		retorno, 
		must_kill_opl_folder = 0;
	static int  mc_Type, 
				mc_Free, 
				mc_Format;

	mcGetInfo( mcport, 0, &mc_Type, &mc_Free, &mc_Format);
	mcSync(0, NULL, &ret);
	if (ret != -1){return NO_MEMORY_CARD;}
	if (mc_Type != 2){return NO_PS2_MEMORY_CARD;}
	//  CLEANUP
	sprintf(temp_path,"mc%u:BOOT", mcport);
		DeleteFolder(temp_path);
	sprintf(temp_path,"mc%u:APPS", mcport);
		DeleteFolder(temp_path);
	sprintf(temp_path,"mc%u:BXEXEC-FUNTUNA", mcport);
		DeleteFolder(temp_path);
	sprintf(temp_path,"mc%u:FORTUNA", mcport);
		DeleteFolder(temp_path);
	sprintf(temp_path,"mc%u:OPENTUNA", mcport);
		DeleteFolder(temp_path);
	sprintf(temp_path,"mc%u:PS2BBL", mcport);
		DeleteFolder(temp_path);
	sprintf(temp_path,"mc%u:BXEXEC-OPENTUNA", mcport);
		DeleteFolder(temp_path);
	
	sprintf(opl_daily_bulshit_cnf, "mc%u:/OPL/conf_elm.cfg", mcport);// if OPL DB settings file found delete OPL directory to avoid problems when Official OPL reads the cfg files
	if (file_exists(opl_daily_bulshit_cnf)) {must_kill_opl_folder = 1;}
	sprintf(opl_daily_bulshit_cnf, "mc%u:/OPL/conf_elmz.cfg", mcport);// same as above
	if (file_exists(opl_daily_bulshit_cnf)) {must_kill_opl_folder = 1;}
	
	sprintf(opl_daily_bulshit_cnf, "mc%u:/OPL/conf_game.cfg", mcport);//This isn't related to OPL DB, but the absensce of this file could indicate that OPL folder containts old settings (pre 1.1.0), delete to avoid famous black screen shit
	if (!file_exists(opl_daily_bulshit_cnf)) {must_kill_opl_folder = 1;}
	
	if (must_kill_opl_folder == 1)
	{
		sprintf(temp_path,"mc%u:OPL", mcport);
		DeleteFolder(temp_path);
	}
	// /CLEANUP
	//If there's no free space, we have an error:
	if (mc_Free < 2000){return NOT_ENOUGH_SPACE;}//Installation actually requires less than this (something like 1.6MB), but i left a larger size for space check since OPL will create settings and icon files on first launch... (and users will innevitally load more files)

	//If the files exists, we have an error:
	if (mcport == 0) {
	if (file_exists("mc0:/BXEXEC-OPENTUNA/icon.icn")) {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc0:/BXEXEC-OPENTUNA/icon.sys")) {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc0:/BXEXEC-FUNTUNA/icon.icn"))  {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc0:/BXEXEC-FUNTUNA/icon.sys"))  {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc0:/OPENTUNA/icon.sys"))        {return OPENTUNA_FOUND;}
	if (file_exists("mc0:/FORTUNA/icon.sys"))         {return FORTUNA_FOUND;}
	} else {
	if (file_exists("mc1:/BXEXEC-OPENTUNA/icon.icn")) {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc1:/BXEXEC-OPENTUNA/icon.sys")) {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc1:/BXEXEC-FUNTUNA/icon.icn"))  {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc1:/BXEXEC-FUNTUNA/icon.sys"))  {return FUNTUNA_FORK_FOUND;}
	if (file_exists("mc1:/OPENTUNA/icon.sys"))        {return OPENTUNA_FOUND;}
	if (file_exists("mc1:/FORTUNA/icon.sys"))         {return FORTUNA_FOUND;}
	}
	//FOLDERS
	scr_setfontcolor(0x00ffff);
	scr_printf("\tCreating Folders...\n");
	scr_setfontcolor(0xffffff);
	scr_printf("\t\tBOOT\n");
		ret = mcMkDir(mcport, 0, "BOOT");
		mcSync(0, NULL, &ret);
	scr_printf("\t\tAPPS\n");
		ret = mcMkDir(mcport, 0, "APPS");
		mcSync(0, NULL, &ret);
	scr_printf("\t\tPS2BBL\n");
		ret = mcMkDir(mcport, 0, "PS2BBL");
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
	scr_setfontcolor(0x00ffff);
	scr_printf("\tWriting Files\n");
	scr_setfontcolor(0xffffff);
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
		scr_setfontcolor(0x00ffff);
		scr_printf("\t\tManipulating Opentuna icon timestamp\n");
		scr_setfontcolor(0xffffff);
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
				
	} else {
	scr_setfontcolor(0x0000ff);
	scr_printf("\t THIS PS2 IS INCOMPATIBLE WITH OPENTUNA\n\tSKIPPING OPENTUNA FILES!\n");DeleteFolder(temp_path);
	scr_setfontcolor(0xffffff);
	}
	///OPENTUNA
	
	//FUNTUNA&APPS
	scr_printf("\t\tPS2BBL\n");
		retorno = write_embed(&PS2BBL_ELF, size_PS2BBL_ELF, "BOOT", "BOOT.ELF",mcport);
		if (retorno < 0) {return 6;}

	scr_printf("\t\tPS2BBL Configuration\n");
		retorno = write_embed(&PS2BBL_CFG, size_PS2BBL_CFG, "PS2BBL", "CONFIG.INI",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&PS2BBL_icn, size_PS2BBL_icn, "PS2BBL", "PS2BBL.icn",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&PS2BBL_sys, size_PS2BBL_sys, "PS2BBL", "icon.sys",mcport);
		if (retorno < 0) {return 6;}

	scr_printf("\t\tuLaunchELF\n");
		retorno = write_embed(&ule_elf, size_ule_elf, "BOOT", "ULE.ELF",mcport);
		if (retorno < 0) {return 6;}

	scr_printf("\t\tBOOT icons\n");
		retorno = write_embed(&boot_icn, size_boot_icn, "BOOT", "PS2BBL.icn",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&boot_sys, size_boot_sys, "BOOT", "icon.sys",mcport);
		if (retorno < 0) {return 6;}
	///FUNTUNA&APPS
	scr_printf("\t\tAPPS icons\n");
		retorno = write_embed(&apps_sys, size_apps_sys, "APPS","icon.sys",mcport);
		if (retorno < 0) {return 6;}
		retorno = write_embed(&apps_icn, size_apps_icn, "APPS","APPS.icn",mcport);
		if (retorno < 0) {return 6;}

	scr_printf("\t\tOPL\n");
		retorno = write_embed(&opl_elf, size_opl_elf, "APPS","OPNPS2LD.ELF",mcport);
		if (retorno < 0) {return 6;}

	if (!file_exists(opl_settings_location))//If no OPL config file...
	{
		scr_setfontcolor(0x00ffff);
		scr_printf("\t\tNo OPL settings found!\n\t\t Loading Preconfigured OPL Folder...\n");
		scr_setfontcolor(0xffffff);
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

	ResetIOP();

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
void tell_the_user_wtf_happened(int retval)
{
	scr_clear();
	scr_setfontcolor(0x0000ff);
	scr_printf("\n\n\n\t\tERROR!\n\n\n");
	scr_setfontcolor(0xffffff);
	switch (retval)
	{
		case NO_MEMORY_CARD:
			scr_printf("\tNo memory card detected!\n");
			break;
			
		case NO_PS2_MEMORY_CARD:
			scr_printf("\tNo PS2 memory card detected!\n");
			break;
			
		case NOT_ENOUGH_SPACE:
			scr_printf("\tNot enough space on card!\n");
			break;
			
		case FORTUNA_FOUND:
		case OPENTUNA_FOUND:
		case FUNTUNA_FORK_FOUND:
		case BOOT_FOLDER_EXISTS:
		case APPS_FOLDER_EXISTS:
			scr_printf("\tI/O error, conflictive folder could not be wiped.\n");
			break;
			
		default:
		scr_setfontcolor(0x00ffff);
			scr_printf("\tUnknown error!\n");
		scr_setfontcolor(0xffffff);
	}
	scr_printf("\t\nERROR CODE: [%d]",retval);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int IconQuery(int default_icon_type)
{
	scr_clear();
	scr_setfontcolor(0x00ffff);
	scr_printf("\n\n\n\t\t\tChoose Icon variant\n\n\t(the one compatible with this ps2 will have a green tint)\n\n");
	scr_setfontcolor(0xffffff);
	if (default_icon_type == SLIMS)
		scr_setfontcolor(0x00ff00);

	scr_printf("\tTRIANGLE: All Slims and SCPH-500xx with 1.90 ROM\n");
	scr_setfontcolor(0xffffff);

	if (default_icon_type == FATS)
		scr_setfontcolor(0x00ff00);

	scr_printf("\tCROSS: FATs from SCPH-18000 to SCPH-390xx (ROM 1.10 to 1.60)\n");
	scr_setfontcolor(0xffffff);

	if (default_icon_type == FAT170)
		scr_setfontcolor(0x00ff00);

	scr_printf("\tSQUARE: SCPH-500xx with 1.70 ROM\n");
	scr_setfontcolor(0xffffff);

	while (1)
	{
		readPad();
		if (new_pad & PAD_TRIANGLE)     return SLIMS;
		if (new_pad & PAD_CROSS)  		return FATS;
		if (new_pad & PAD_SQUARE) 		return FAT170;
	}
}
int Choose_Install_Mode(int default_icon_type)
{
	scr_clear();
	scr_printf("\n\n\n\tChoose installation mode:\n");
	scr_printf("\tL2: Manual install\n");
	scr_printf("\tR2: Automatic install\n");
	while (1)
	{
		readPad();
		if (new_pad & PAD_R2) return default_icon_type;
		
		if (new_pad & PAD_L2) return IconQuery(default_icon_type);
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//close program and go to browser
static void PS2_browser(void)
{
	CleanUp();
	LoadExecPS2("rom0:OSDSYS", 0, NULL);
}

void inst_success(void)
{
	scr_clear();
	scr_setfontcolor(0x00ff00);
	scr_printf("\n\n\n\t\tInstallation succeeded!\n\n\tPress start to quit\n\n");
	scr_setfontcolor(0xffffff);

	if (configGetLanguage() == 0)
	{
		scr_setfontcolor(0x0000ff);
		scr_printf("CONSOLE LANGUAGE IS IN JAPANESE\n\tOPENTUNA CRASHES IF JAPANESE TEXT IS BEING DISPLAYED\n\t PLEASE CHANGE TO ANOTHER LANGUAGE BEFORE RUNNING OPENTUNA\n");
		scr_setfontcolor(0xffffff);
	}

}
void inst_err(void)
{
	scr_clear();
	scr_setfontcolor(0x0000ff);
	scr_printf("\n\n\n\t\tInstallation Failed!\n\n\tPress start to quit\n");
	scr_setfontcolor(0xffffff);
}

int main(int argc, char *argv[])
{
	init_scr();
	printf("PS2BBL+OpenTuna Installer starts!\n");
	int fdn, icontype;
	unsigned long int ROM_VERSION;
	char romver[5];
	VMode = NTSC;

	// Loads Needed modules
	InitPS2();
	printf("Reading ROMVER\n");
	//gs_reset(); // Reset GS
	if ((fdn = open("rom0:ROMVER", O_RDONLY)) > 0) // Reading ROMVER
	{
		read(fdn, romver, 4);
		close(fdn);

		if (romver[4] == 'E')
			VMode = PAL;
		romver[4] = '\0';
		ROM_VERSION = strtoul(romver, NULL, 16); //convert ROM version to unsigned long int for further use on automatic Install
	}
	/*
	if (VMode == PAL)
		gs_init(PAL_640_512_32);
	else
		gs_init(NTSC_640_448_32);*/
	printf("calculating required icon type\n");
	icontype = GetIconType(ROM_VERSION);
	waitAnyPadReady();
	pad_inited = 1;

	int iz = 1, mcport;
	int menuactual = 101;//101: Initial Menu, 102: Installing (not needed), 103: Error, 104: Done
	//printf("FunTuna Fork Installer\n\nPress L1 to install on memory card 1 | Press R1 to install on memory card 2");

	scr_printf("\n\n\n\n%s\n\n\t\t\t\tPlayStation Basic BootLoader + OpenTuna Installer\n\t\tPS2BBL By Matias Israelson\n\t\tOpenTuna by Alex Parrado\n\n", banner);
	scr_printf("\tL1: Install on Memory Card 1\n");
	scr_printf("\tR1: Install on Memory Card 2\n");
	while (1) {
		readPad();

		if ( (new_pad & PAD_L1) && (menuactual == 101) ) {
			menuactual = 102;
			mcport = 0;
			//iz = install(mcport, icontype);
			iz = install(mcport, Choose_Install_Mode(icontype));
			if(iz == 0){
				menuactual = 104;
				inst_success();
			} else {
				menuactual = 103;//error
				tell_the_user_wtf_happened(iz);
			}
		}
		else if ( (new_pad & PAD_R1) && (menuactual == 101) ) {
			menuactual = 102;
			mcport = 1;
			//iz = install(mcport, icontype);
			iz = install(mcport, Choose_Install_Mode(icontype));
			if(iz == 0){
				menuactual = 104;
				inst_success();
			} else {
				menuactual = 103;//error
				tell_the_user_wtf_happened(iz);
			}
		}

		else if ((new_pad & PAD_START) && (menuactual == 103)) {PS2_browser();}
		else if ((new_pad & PAD_START) && (menuactual == 104)) 
		{
#ifdef BOOT_FUNTUNA_ON_SUCCESS
			if (mcport==0){
				if (file_exists("mc0:/BOOT/BOOT.ELF")) 
					LoadElf("mc0:/BOOT/BOOT.ELF", "mc0:/BOOT/");
			} else {
				if (file_exists("mc1:/BOOT/BOOT.ELF")) 
					LoadElf("mc1:/BOOT/BOOT.ELF", "mc1:/BOOT/");
			}
#endif
			PS2_browser();
		}
	}

	return 0;
}
