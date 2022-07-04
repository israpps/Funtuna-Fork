//#define __DEBUG_PRINTF__

/*------------------------------------------------------------*/
#include "main.h"
//*/
#ifdef GS_DEBUG_ISRA
	#define DEBUG_PRINTF(arg...) scr_printf(arg);
#elif defined(USE_PRINTF)
	#define DEBUG_PRINTF(arg...) printf(arg);
#else
	#define DEBUG_PRINTF(arg...)
#endif

#include "embedded.h"

static int pad_inited = 0;
int key;
#include "BMPS/FUNTUNA_FORK_INSTALLER_BG.h"
#include "BMPS/FUNTUNA_FORK_INSTALLER_WELCOME.h"
#include "BMPS/FUNTUNA_FORK_INSTALLER_SUCESS.h"
#include "BMPS/FUNTUNA_FORK_INSTALLER_FAIL.h"

#include "BMPS/INSTALL_QUERY.h"
#include "BMPS/ICONQUERY.h"
#include "BMPS/ICONQUERY_FAT170.h"
#include "BMPS/ICONQUERY_FATS.h"
#include "BMPS/ICONQUERY_SLIMS.h"

#include "BMPS/FILE_CONFLICT.h"
#include "BMPS/NOT_ENOUGH_SPACE.h"
#include "BMPS/NO_MEMORY_CARD.h"
#include "BMPS/NO_PS2_MC.h"

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
	DEBUG_PRINTF("getting icon type...\n");
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
	DEBUG_PRINTF("[IOP-RESET]: START\n");
	//parrado
	SifInitRpc(0);
	while (!SifIopReset("", 0)) {};
	while (!SifIopSync()) {};
	SifInitRpc(0);
	DEBUG_PRINTF("[IOP-RESET]: END\n");
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
//--------------------------------------------------------------
int wait_key(int key)
{
	DEBUG_PRINTF("waiting for key...\n");
	int new_pad;
	while (1)
	{
		new_pad = ReadCombinedPadStatus();
		if (new_pad & key)
			return new_pad;
	}
}
//--------------------------------------------------------------
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
//--------------------------------------------------------------
static void InitPS2(void)
{
	DEBUG_PRINTF("[PS2-INIT]: start\n");
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	fioInit();
	
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();
	DEBUG_PRINTF("[PS2-INIT]: Loading IRX drivers...\n");
	SifExecModuleBuffer(SIO2MAN_irx, size_SIO2MAN_irx, 0, NULL, NULL);
	SifExecModuleBuffer(PADMAN_irx, size_PADMAN_irx, 0, NULL, NULL);
	SifExecModuleBuffer(MCMAN_irx, size_MCMAN_irx, 0, NULL, NULL);
	SifExecModuleBuffer(MCSERV_irx, size_MCSERV_irx, 0, NULL, NULL);
	DEBUG_PRINTF("[PS2-INIT]: sbv_patch_fileio()\n");
	sbv_patch_fileio();// THANKS fjtrujy
	DEBUG_PRINTF("PadInit\n");
	PadInitPads();
	DEBUG_PRINTF("mcInit (XMC)\n");
	mcInit(MC_TYPE_XMC);
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

DEBUG_PRINTF("embed file written: %s\n", target);
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

DEBUG_PRINTF("embed file written: %s\n", target);
	return 0;
}
//--------------------------------------------------------------
//return 0 = ok, return 1 = error
static int install(int mcport, int icon_variant)
{
	display_bmp(640, 448, BG);
	scr_printf("\n\n\n\n\t\tInstalling for memory card %u...\n",mcport);
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

	#ifndef REQUIRED_SPACE
	if (mc_Free < 1950){return NOT_ENOUGH_SPACE;}//Installation actually requires less than this (something like 1.6MB), but i left a larger size for space check since OPL will create settings and icon files on first launch... (and users will innevitally load more files)
	#else
	if (mc_Free < (REQUIRED_SPACE + 128)){return NOT_ENOUGH_SPACE;}
	#endif

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
	scr_printf("\t\tCreating Folders...\n");
	scr_printf("\t\t\tBOOT\n");
		ret = mcMkDir(mcport, 0, "BOOT");
		mcSync(0, NULL, &ret);
	scr_printf("\t\t\tAPPS\n");
		ret = mcMkDir(mcport, 0, "APPS");
		mcSync(0, NULL, &ret);
	scr_printf("\t\t\tOPL\n");
		ret = mcMkDir(mcport, 0, "OPL");
		mcSync(0, NULL, &ret);
	if (icon_variant != UNSUPPORTED){
	scr_printf("\t\t\tBXEXEC-OPENTUNA\n");
		ret = mcMkDir(mcport, 0, "BXEXEC-OPENTUNA");
		mcSync(0, NULL, &ret);
	}
	///FOLDERS
	scr_printf("\t\tWriting Files\n");
	//OPENTUNA
	if (icon_variant != UNSUPPORTED)
	{
		if (icon_variant == SLIMS)
		{
			retorno = write_embed(&opentuna_slims, size_opentuna_slims, "BXEXEC-OPENTUNA", "icon.icn", mcport);
			scr_printf("\t\t\tWriting Opentuna for SLIMS and SCPH-5XXXX with ROM 1.90\n");
		}
		else if (icon_variant == FATS)
		{
			retorno = write_embed(&opentuna_fats, size_opentuna_fats, "BXEXEC-OPENTUNA", "icon.icn", mcport);
			scr_printf("\t\t\tWriting Opentuna for FATS (from SCPH-18000 to SCPH-39XXX)\n");
		}
		else if (icon_variant == FAT170)
		{
			retorno = write_embed(&opentuna_fat170, size_opentuna_fat170, "BXEXEC-OPENTUNA", "icon.icn", mcport);
			scr_printf("\t\t\tWriting Opentuna for SCPH-5XXXX with ROM 1.70\n");
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
		scr_printf("\t\t\tManipulating Opentuna icon timestamp\n");
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

	} else {scr_printf("\t\t THIS PS2 IS INCOMPATIBLE WITH OPENTUNA\n\tSOPENTUNA FILES SKIPPED!\n");}
	///OPENTUNA

	//FUNTUNA&APPS
	scr_printf("\t\t\tuLaunchELF\n");
			retorno = write_embed(&ule_elf, size_ule_elf, "BOOT", "ULE.ELF",mcport);
		if (retorno < 0) {return 6;}
			retorno = write_embed(&ule_cnf, size_ule_cnf, "BOOT", "LAUNCHELF.CNF",mcport);
		if (retorno < 0) {return 6;}

	scr_printf("\t\t\tFreeMcBoot\n");
			retorno = write_embed(&FMCB_ELF, size_FMCB_ELF, "BOOT", "BOOT.ELF",mcport);
		if (retorno < 0) {return 6;}
			retorno = write_embed(&FMCB_CNF, size_FMCB_CNF, "BOOT", "FUNTUNA.CNF",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\t\tFreeMcBoot Configurator\n");
			retorno = write_embed(&CFG_ELF, size_CFG_ELF, "BOOT", "CFG.ELF",mcport);
		if (retorno < 0) {return 6;}

    scr_printf("\t\t\tFreeMcBoot USB driver\n");
			retorno = write_embed(&FUNTUNA_USBD, size_FUNTUNA_USBD, "BOOT", "USBD.IRX",mcport);
		if (retorno < 0) {return 6;}
    scr_printf("\t\t\tBDM driver\n");
			retorno = write_embed(&FUNTUNA_BDM, size_FUNTUNA_BDM, "BOOT", "BMD.IRX",mcport);
		if (retorno < 0) {return 6;}
    scr_printf("\t\t\tBDM Exfat driver\n");
			retorno = write_embed(&FUNTUNA_BDMFS_FATFS, size_FUNTUNA_BDMFS_FATFS, "BOOT", "BDMFS_FATFS.IRX",mcport);
		if (retorno < 0) {return 6;}
    scr_printf("\t\t\tBDM USB driver\n");
			retorno = write_embed(&FUNTUNA_USBMASS_BD, size_FUNTUNA_USBMASS_BD, "BOOT", "USBMASS_BD.IRX",mcport);
		if (retorno < 0) {return 6;}
/*
    scr_printf("\t\t\tFileXio IRX Driver\n");
			retorno = write_embed(&FILEXIO_IRX, size_FILEXIO_IRX, "BOOT", "FILEXIO.IRX",mcport);
		if (retorno < 0) {return 6;}*/

	scr_printf("\t\t\tBOOT icons\n");
			retorno = write_embed(&boot_icn, size_boot_icn, "BOOT", "SYSTEM.icn",mcport);
		if (retorno < 0) {return 6;}
			retorno = write_embed(&boot_sys, size_boot_sys, "BOOT", "icon.sys",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\t\tPoweroff utility\n");
			retorno = write_embed(&poweroff_elf, size_poweroff_elf, "BOOT", "POWEROFF.ELF",mcport);
		if (retorno < 0) {return 6;}
	///FUNTUNA&APPS
	scr_printf("\t\t\tAPPS folder icons\n");
			retorno = write_embed(&apps_sys, size_apps_sys, "APPS","icon.sys",mcport);
		if (retorno < 0) {return 6;}
			retorno = write_embed(&apps_icn, size_apps_icn, "APPS","funtuna_apps.icn",mcport);
		if (retorno < 0) {return 6;}
	scr_printf("\t\t\tOPL\n");
			retorno = write_embed(&opl_elf, size_opl_elf, "APPS","OPNPS2LD.ELF",mcport);
		if (retorno < 0) {return 6;}
	if (!file_exists(opl_settings_location))//If no OPL config file...
	{
		scr_printf("\t\t\tNo OPL settings found!\n\t\t Loading Preconfigured OPL Folder...\n");
		if (mcport==0)
			write_embed_replace(&opl_cfg, size_opl_cfg, "OPL", "conf_opl.cfg", mcport);//main config file has two variants, each of them has IGR Path assigned according to mcport value
		else write_embed_replace(&opl_cfg1, size_opl_cfg1, "OPL", "conf_opl.cfg", mcport);//

		write_embed_replace(&opl_icn, size_opl_icn, "OPL", "opl.icn", mcport);
		write_embed_replace(&opl_sys, size_opl_sys, "OPL", "icon.sys", mcport);
		write_embed_replace(&opl_dualshock, size_opl_dualshock, "OPL", "conf_game.cfg", mcport);
	}
	scr_printf("\t\tInstallation Finished\n");
	return 0;
}
//--------------------------------------------------------------

static void CleanUp(void) //trimmed from FMCB 1.9??? installer
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
//--------------------------------------------------------------
void tell_the_user_wtf_happened(int retval)
{
	scr_clear();
	switch (retval)
	{
		case NO_MEMORY_CARD:
			display_bmp(640, 448, _NO_MEMORY_CARD);
			break;

		case NO_PS2_MEMORY_CARD:
			display_bmp(640, 448, NO_PS2_MC);
			break;

		case NOT_ENOUGH_SPACE:
			display_bmp(640, 448, _NOT_ENOUGH_SPACE);
			break;

		case FORTUNA_FOUND:
		case OPENTUNA_FOUND:
		case FUNTUNA_FORK_FOUND:
		case BOOT_FOLDER_EXISTS:
		case APPS_FOLDER_EXISTS:
			display_bmp(640, 448, FILE_CONFLICT);
			break;

		default:
			display_bmp(640, 448, FAIL);
	}
	scr_printf("\n\n\n\nERROR CODE: [%d]",retval);
}
//--------------------------------------------------------------
int IconQuery(int default_icon_type)
{
	switch (default_icon_type)
	{
		case SLIMS:
			display_bmp(640, 448, ICONQUERY_SLIMS);
			break;
		case FATS:
			display_bmp(640, 448, ICONQUERY_FATS);
			break;
		case FAT170:
			display_bmp(640, 448, ICONQUERY_FAT170);
			break;
		default:
			display_bmp(640, 448, ICONQUERY);
	}

	while (1)
	{
		key = wait_key(-1);
		if (key & PAD_TRIANGLE)
			return SLIMS;
		if (key & PAD_CROSS)
			return FATS;
		if (key & PAD_SQUARE)
			return FAT170;
	}
}
int Choose_Install_Mode(int default_icon_type)
{
	display_bmp(640, 448, INSTALL_QUERY);
	while (1)
	{
		key = wait_key(-1);
		if (key & PAD_R2) 
			return default_icon_type;

		if (key & PAD_L2) 
			return IconQuery(default_icon_type);
	}
}
//--------------------------------------------------------------
//close program and go to browser
static void PS2_browser(void)
{
	CleanUp();
	LoadExecPS2("rom0:OSDSYS", 0, NULL);
}
enum STATE
{
	USERQUERY = 0,
	INSTALLING,
	INST_ERROR,
	INST_SUCCESS,
	
	STATE_COUNT
};
int main (int argc, char *argv[])
{
DEBUG_PRINTF("-------------------------{FunTuna Fork Installer}-------------------------\nhello world\n");
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
	icontype = GetIconType(ROM_VERSION);
	pad_inited = 1;

	int iz = 1, mcport;
	int state = USERQUERY;

	display_bmp(640, 448, WELCOME);
	while (1) {
		key = wait_key(-1);
		switch (state)
		{
			case USERQUERY:
				if (key & PAD_L1) 
				{
					mcport = 0;
					iz = install(mcport, Choose_Install_Mode(icontype));
					if(iz == 0)
					{
						state = INST_SUCCESS;
						display_bmp(640, 448, SUCESS);
					} else {
						state = INST_ERROR;
						tell_the_user_wtf_happened(iz);
					}
				}
				else if (key & PAD_R1) 
				{
					mcport = 1;
					iz = install(mcport, Choose_Install_Mode(icontype));
					if(iz == 0)
					{
						state = INST_SUCCESS;
						display_bmp(640, 448, SUCESS);
					} else {
						state = INST_ERROR;
						tell_the_user_wtf_happened(iz);
					}
				}
				continue;
			case INST_SUCCESS:
			case INST_ERROR:
				if (key & PAD_START) 
					PS2_browser();
				continue;
		}
	}

	return 0;
}
