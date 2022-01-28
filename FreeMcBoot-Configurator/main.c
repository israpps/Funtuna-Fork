//--------------------------------------------------------------
//File name:   main.c
//--------------------------------------------------------------
#include "launchelf.h"

//dlanor: I'm correcting all these erroneous 'u8 *name' declarations
//dlanor: They are not pointers at all, but pure block addresses
//dlanor: Thus they should be declared as 'void name'
/*extern void iomanx_irx;
extern int  size_iomanx_irx;
extern void filexio_irx;
extern int  size_filexio_irx;
extern void ps2dev9_irx;
extern int  size_ps2dev9_irx;
extern void ps2ip_irx;
extern int  size_ps2ip_irx;
extern void ps2smap_irx;
extern int  size_ps2smap_irx;
extern void smsutils_irx;
extern int  size_smsutils_irx;
extern void ps2host_irx;
extern int  size_ps2host_irx;
extern void ps2ftpd_irx;
extern int  size_ps2ftpd_irx;
extern void ps2atad_irx;
extern int  size_ps2atad_irx;
extern void ps2hdd_irx;
extern int  size_ps2hdd_irx;
extern void ps2fs_irx;
extern int  size_ps2fs_irx;
extern void poweroff_irx;
extern int  size_poweroff_irx;
extern void loader_elf;
extern int  size_loader_elf;
extern void ps2netfs_irx;
extern int  size_ps2netfs_irx;
extern void iopmod_irx;
extern int  size_iopmod_irx;
extern void usbd_irx;
extern int  size_usbd_irx;
extern void usb_mass_irx;
extern int  size_usb_mass_irx;
extern void cdvd_irx;
extern int  size_cdvd_irx;
extern void ps2kbd_irx;
extern int  size_ps2kbd_irx;
extern void hdl_info_irx;
extern int  size_hdl_info_irx;
*/
extern unsigned char iomanx_irx[];
extern unsigned int size_iomanx_irx;
#ifdef EXTERNAL_FILEXIO
extern unsigned char filexio_irx[];
extern unsigned int size_filexio_irx;
#endif
//#define DEBUG
#ifdef DEBUG
#define dbgprintf(args...) scr_printf(args)
#define dbginit_scr() init_scr()
#else
#define dbgprintf(args...) \
	do {                   \
	} while (0)
#define dbginit_scr() \
	do {              \
	} while (0)
#endif

enum {
	BUTTON,
	DPAD
};

void Reset();

int TV_mode;
int trayopen = FALSE;
int selected = 0;
int timeout = 0;
int init_delay = 0;
int poweroff_delay = 0;  //Set only when calling hddPowerOff
int mode = BUTTON;
int user_acted = 0; /* Set when commands given, to break timeout */
char LaunchElfDir[MAX_PATH], mainMsg[MAX_PATH];
char CNF[MAX_NAME];
int numCNF = 0;
int maxCNF;
int swapKeys;
int GUI_active;

u64 WaitTime;

#define IPCONF_MAX_LEN (3 * 16)
char if_conf[IPCONF_MAX_LEN];
int if_conf_len;

char ip[16] = "192.168.0.10";
char netmask[16] = "255.255.255.0";
char gw[16] = "192.168.0.1";

char netConfig[IPCONF_MAX_LEN + 64];  //Adjust size as needed

//State of module collections
int have_NetModules = 0;
int have_HDD_modules = 0;
//State of sbv_patches
int have_sbv_patches = 0;
//Old State of Checkable Modules (valid header)
int old_sio2man = 0;
int old_mcman = 0;
int old_mcserv = 0;
int old_padman = 0;
int old_fakehost = 0;
int old_poweroff = 0;
int old_iomanx = 0;
int old_filexio = 0;
int old_ps2dev9 = 0;
int old_ps2ip = 0;
int old_ps2atad = 0;
int old_ps2hdd = 0;
int old_ps2fs = 0;
int old_ps2netfs = 0;
//State of Uncheckable Modules (invalid header)
int have_cdvd = 0;
int have_usbd = 0;
int have_usb_mass = 0;
int have_ps2smap = 0;
int have_ps2host = 0;
int have_ps2ftpd = 0;
int have_ps2kbd = 0;
int have_hdl_info = 0;
//State of Checkable Modules (valid header)
int have_urgent = 0;  //flags presence of urgently needed modules
int have_sio2man = 0;
int have_mcman = 0;
int have_mcserv = 0;
int have_padman = 0;
int have_fakehost = 0;
int have_poweroff = 0;
int have_iomanx = 0;
int have_filexio = 0;
int have_ps2dev9 = 0;
int have_ps2ip = 0;
int have_ps2atad = 0;
int have_ps2hdd = 0;
int have_ps2fs = 0;
int have_ps2netfs = 0;

int force_IOP = 0;  //flags presence of incompatible drivers, so we must reset IOP

int done_setupPowerOff = 0;
int ps2kbd_opened = 0;

int boot_argc;
char *boot_argv[8];
char boot_path[MAX_PATH];

//--------------------------------------------------------------
//executable code
//--------------------------------------------------------------
//Function to print a text row to the 'gs' screen
//------------------------------

void CheckModules(void)
{
	smod_mod_info_t mod_t;

	old_sio2man = (have_sio2man = smod_get_mod_by_name(IOPMOD_NAME_SIO2MAN, &mod_t));
	old_mcman = (have_mcman = smod_get_mod_by_name(IOPMOD_NAME_MCMAN, &mod_t));
	old_mcserv = (have_mcserv = smod_get_mod_by_name(IOPMOD_NAME_MCSERV, &mod_t));
	old_padman = (have_padman = smod_get_mod_by_name(IOPMOD_NAME_PADMAN, &mod_t));
	/*	old_fakehost = (have_fakehost = smod_get_mod_by_name(IOPMOD_NAME_FAKEHOST, &mod_t));
	old_poweroff = (have_poweroff = smod_get_mod_by_name(IOPMOD_NAME_POWEROFF, &mod_t));
	old_iomanx   = (have_iomanx = smod_get_mod_by_name(IOPMOD_NAME_IOMANX, &mod_t));
	old_filexio  = (have_filexio = smod_get_mod_by_name(IOPMOD_NAME_FILEXIO, &mod_t));
	old_ps2dev9  = (have_ps2dev9 = smod_get_mod_by_name(IOPMOD_NAME_PS2DEV9, &mod_t));
	old_ps2ip    = (have_ps2ip = smod_get_mod_by_name(IOPMOD_NAME_PS2IP, &mod_t));
	old_ps2atad  = (have_ps2atad = smod_get_mod_by_name(IOPMOD_NAME_PS2ATAD, &mod_t));
	old_ps2hdd   = (have_ps2hdd = smod_get_mod_by_name(IOPMOD_NAME_PS2HDD, &mod_t));
	old_ps2fs    = (have_ps2fs = smod_get_mod_by_name(IOPMOD_NAME_PS2FS, &mod_t));
	old_ps2netfs = (have_ps2netfs= smod_get_mod_by_name(IOPMOD_NAME_PS2NETFS, &mod_t));
*/
}

void delay(int count)
{
	int i;
	int ret;
	for (i = 0; i < count; i++) {
		ret = 0x01000000;
		while (ret--)
			asm("nop\nnop\nnop\nnop");
	}
}
//------------------------------
//endfunc delay
//--------------------------------------------------------------
void initsbv_patches(void)
{
	if (!have_sbv_patches) {
		dbgprintf("Init MrBrown sbv_patches\n");
		sbv_patch_enable_lmb();
		sbv_patch_disable_prefix_check();
		sbv_patch_fileio();
		have_sbv_patches = 1;
	}
}
//------------------------------
//endfunc initsbv_patches
//--------------------------------------------------------------
void	load_iomanx(void)
{
	int ret;

	if	(!have_iomanx)
	{	SifExecModuleBuffer(&iomanx_irx, size_iomanx_irx, 0, NULL, &ret);
		have_iomanx = 1;
	}
}
//------------------------------
//endfunc load_iomanx
//--------------------------------------------------------------
void	load_filexio(void)
{
	int dummy;
	if	(have_filexio) return;
#ifdef EXTERNAL_FILEXIO
	char filePath[MAX_PATH];
	void *fileBase;
	int fileSize, i;
	FILE *File;

	fileBase = NULL;
	fileSize = 0;
		strcpy(filePath, "mc0:/BOOT/FILEXIO.IRX");
		
		if (!exists(filePath))
			filePath[2] ^= 1;
		
		if (!exists(filePath))
			return;
		
		File = fopen(filePath, "r");
		if (File != NULL) {
			fseek(File, 0, SEEK_END);
			fileSize = ftell(File);
			fseek(File, 0, SEEK_SET);
			if (fileSize) {
				if ((fileBase = malloc(fileSize)) > 0) {
					fread(fileBase, 1, fileSize, File);
				} else
					fileSize = 0;
			}
			fclose(File);
		} else {return;}
		
		SifExecModuleBuffer(fileBase, fileSize, 0, NULL, &dummy);
		free(fileBase);
#else
	SifExecModuleBuffer(&filexio_irx, size_filexio_irx, 0, NULL, &dummy);
#endif
		have_filexio = 1;
}

void loadBasicModules(void)
{
	if (!have_sio2man) {
		SifLoadModule("rom0:SIO2MAN", 0, NULL);
		have_sio2man = 1;
	}
	if (!have_mcman) {
		SifLoadModule("rom0:MCMAN", 0, NULL);
		have_mcman = 1;
	}
	if (!have_mcserv) {
		SifLoadModule("rom0:MCSERV", 0, NULL);
		have_mcserv = 1;
	}
	if (!have_padman) {
		SifLoadModule("rom0:PADMAN", 0, NULL);
		have_padman = 1;
	}
}
//------------------------------
//endfunc loadBasicModules
//--------------------------------------------------------------

int loadUsbModules(void)
{  //function used for loading usb modules from fmcb default install path
	char filePath[MAX_PATH];
	void *fileBase;
	int fileSize, dummy, error = 0, i;
	FILE *File;

	fileBase = NULL;
	fileSize = 0;

	if (have_usbd && have_usb_mass)
		return 3;

	for (i = 1; i < 3; i++) {
		if (i == 1)
			strcpy(filePath, "mc0:/BOOT/USBD.IRX");
		else if (i == 2)
			strcpy(filePath, "mc0:/BOOT/USBHDFSD.IRX");

		if (!exists(filePath))
			filePath[2] ^= 1;
		if (!exists(filePath)) {
			error = i;  // 1 = error usbd, 2 = error usbhdfsd
			break;
		}

		File = fopen(filePath, "r");
		if (File != NULL) {
			fseek(File, 0, SEEK_END);
			fileSize = ftell(File);
			fseek(File, 0, SEEK_SET);
			if (fileSize) {
				if ((fileBase = malloc(fileSize)) > 0) {
					fread(fileBase, 1, fileSize, File);
				} else
					fileSize = 0;
			}
			fclose(File);
		}
		
		if ((SifExecModuleBuffer(fileBase, fileSize, 0, NULL, &dummy)) < 0) {
			error = -i;  // -1 = error usbd, -2 = error usbhdfsd
			break;
		}
		free(fileBase);
	}
	//free(fileBase);
	if (!error) {
		have_usbd = 1;
		have_usb_mass = 1;
	}
	
	return error;
}
//------------------------------
//endfunc loadUsbModules
//--------------------------------------------------------------
/*void loadKbdModules(void)
{
	loadUsbDModule();
	if( (have_usbd && !have_ps2kbd)
		&&(loadExternalModule(setting->usbkbd_file, &ps2kbd_irx, size_ps2kbd_irx))
	)	have_ps2kbd = 1;
}*/
//------------------------------
//endfunc loadKbdModules
//--------------------------------------------------------------
/*void loadHdlInfoModule(void)
{
	int ret;

	if(!have_hdl_info){
		drawMsg(LNG(Loading_HDL_Info_Module));
		SifExecModuleBuffer(&hdl_info_irx, size_hdl_info_irx, 0, NULL, &ret);
		ret = Hdl_Info_BindRpc();
		have_hdl_info = 1;
	}
}*/
//------------------------------
//endfunc loadHdlInfoModule
//--------------------------------------------------------------
/*void poweroffHandler(int i)
{
	hddPowerOff();
}*/
//------------------------------
//endfunc poweroffHandler
//--------------------------------------------------------------
/*void setupPowerOff(void) {
	int ret;

	if(!done_setupPowerOff) {
		hddPreparePoweroff();
		hddSetUserPoweroffCallback((void *)poweroffHandler, NULL);
		if	(!have_poweroff) {
			SifExecModuleBuffer(&poweroff_irx, size_poweroff_irx, 0, NULL, &ret);
			have_poweroff = 1;
		}
		load_iomanx();
		load_filexio();
		load_ps2dev9();
		done_setupPowerOff = 1;
	}
}*/
//------------------------------
//endfunc setupPowerOff
//--------------------------------------------------------------
/*void loadHddModules(void)
{
	if(!have_HDD_modules) {
		drawMsg(LNG(Loading_HDD_Modules));
		setupPowerOff();
		load_ps2atad(); //also loads ps2hdd & ps2fs
		have_HDD_modules = TRUE;
	}
}*/
//------------------------------
//endfunc loadHddModules
//--------------------------------------------------------------
// Load Network modules by EP (modified by RA)
//------------------------------
/*void loadNetModules(void)
{
	if(!have_NetModules){
		loadHddModules();
		loadUsbModules();
		drawMsg(LNG(Loading_NetFS_and_FTP_Server_Modules));
		
		// getIpConfig(); //RA NB: I always get that info, early in init
		// Also, my module checking makes some other tests redundant
		load_ps2netfs(); // loads ps2netfs from internal buffer
		load_ps2ftpd();  // loads ps2dftpd from internal buffer
		have_NetModules = 1;
	}
	strcpy(mainMsg, netConfig);
	if (setting->GUI_skin[0]) {
		GUI_active = 1;
		loadSkin(BACKGROUND_PIC, 0, 0);
	}
}*/
//------------------------------
//endfunc loadNetModules
//--------------------------------------------------------------
/*void startKbd(void)
{
	int kbd_fd;
	void *mapBase;
	int   mapSize;

	printf("Entering startKbd()\r\n");
	if(setting->usbkbd_used) {
		loadKbdModules();
		PS2KbdInit();
		ps2kbd_opened = 1;
		if(setting->kbdmap_file[0]) {
			if((kbd_fd = fioOpen(PS2KBD_DEVFILE, O_RDONLY)) >= 0) {
				printf("kbd_fd=%d; Loading Kbd map file \"%s\"\r\n",kbd_fd,setting->kbdmap_file);
				if(loadExternalFile(setting->kbdmap_file, &mapBase, &mapSize)) {
					if(mapSize == 0x600) {
						fioIoctl(kbd_fd, PS2KBD_IOCTL_SETKEYMAP, mapBase);
						fioIoctl(kbd_fd, PS2KBD_IOCTL_SETSPECIALMAP, mapBase+0x300);
						fioIoctl(kbd_fd, PS2KBD_IOCTL_SETCTRLMAP, mapBase+0x400);
						fioIoctl(kbd_fd, PS2KBD_IOCTL_SETALTMAP, mapBase+0x500);
					}
					printf("Freeing buffer after setting Kbd maps\r\n");
					free(mapBase);
				}
				fioClose(kbd_fd);
			}
		}
	}
}*/
//------------------------------
//endfunc startKbd
//--------------------------------------------------------------
// Read SYSTEM.CNF for MISC/PS2Disc launch command
/*int ReadCNF(char *LK_Path)
{
	char *systemcnf;
	int fd;
	int size;
	int n;
	int i;
	
	loadCdModules();*/
/*
	CDVD_FlushCache();
	CDVD_DiskReady(0);
	*/
/*	i = 0x10000;
	while(i--) asm("nop\nnop\nnop\nnop");
	fd = fioOpen("cdrom0:\\SYSTEM.CNF;1",1);
	if(fd>=0) {
		size = fioLseek(fd,0,SEEK_END);
		fioLseek(fd,0,SEEK_SET);
		systemcnf = (char*)malloc(size+1);
		fioRead(fd, systemcnf, size);
		systemcnf[size+1]=0;
		for(n=0; systemcnf[n]!=0; n++){
			if(!strncmp(&systemcnf[n], "BOOT2", 5)) {
				n+=5;
				break;
			}
		}
		while(systemcnf[n]!=0 && systemcnf[n]==' ') n++;
		if(systemcnf[n]!=0 ) n++; // salta '='
		while(systemcnf[n]!=0 && systemcnf[n]==' ') n++;
		if(systemcnf[n]==0){
			free(systemcnf);
			return 0;
		}
		
		for(i=0; systemcnf[n+i]!=0; i++) {
			LK_Path[i] = systemcnf[n+i];
			if(i>2)
				if(!strncmp(&LK_Path[i-1], ";1", 2)) {
					LK_Path[i+1]=0;
					break;
				}
		}
		fioClose(fd);
		free(systemcnf);
		return 1;
	}
	return 0;
}*/
//------------------------------
//endfunc ReadCNF
//--------------------------------------------------------------
/*void	ShowFont(void)
{
	int test_type=0;
	int test_types=2;  //Patch test_types for number of test loops
	int	i, j, event, post_event=0;
	char Hex[18] = "0123456789ABCDEF";
	int ch_x_stp = 1+FONT_WIDTH+1+LINE_THICKNESS;
	int ch_y_stp = 2+FONT_HEIGHT+1+LINE_THICKNESS;
	int	mat_w = LINE_THICKNESS+17*ch_x_stp;
	int mat_h = LINE_THICKNESS+17*ch_y_stp;
	int mat_x = (((SCREEN_WIDTH-mat_w)/2) & -2);
	int mat_y = (((SCREEN_HEIGHT-mat_h)/2) & -2);
	int ch_x  = mat_x+LINE_THICKNESS+1;
//	int	ch_y  = mat_y+LINE_THICKNESS+2;
	int px, ly, cy;
	u64 col_0=setting->color[0], col_1=setting->color[1], col_3=setting->color[3];

//The next line is a patch to save font, if/when needed (needs patch in draw.c too)
//	WriteFont_C("mc0:/SYS-CONF/font_uLE.c");

	event = 1;   //event = initial entry
	//----- Start of event loop -----
	while(1) {
		//Display section
		if(event||post_event) { //NB: We need to update two frame buffers per event
			drawOpSprite(col_0, mat_x, mat_y, mat_x+mat_w-1, mat_y+mat_h-1);
		*/
//Here the background rectangle has been prepared
/* //Start of commented out section //Move this line as needed for tests
			//Start of gsKit test section
			if(test_type > 1) goto done_test;
			gsKit_prim_point(gsGlobal, mat_x+16, mat_y+16, 1, col_3);
			gsKit_prim_point(gsGlobal, mat_x+33, mat_y+16, 1, col_3);
			gsKit_prim_point(gsGlobal, mat_x+33, mat_y+33, 1, col_3);
			gsKit_prim_point(gsGlobal, mat_x+16, mat_y+33, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+48, mat_y+48, mat_x+65, mat_y+48, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+65, mat_y+48, mat_x+65, mat_y+65, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+65, mat_y+65, mat_x+48, mat_y+65, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+48, mat_y+65, mat_x+48, mat_y+48, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+80, mat_y+80, mat_x+97, mat_y+81, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+97, mat_y+80, mat_x+96, mat_y+97, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+97, mat_y+97, mat_x+80, mat_y+96, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+80, mat_y+97, mat_x+81, mat_y+80, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+80, mat_y+16, mat_x+81, mat_y+16, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+97, mat_y+16, mat_x+97, mat_y+17, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+97, mat_y+33, mat_x+96, mat_y+33, 1, col_3);
			gsKit_prim_line(gsGlobal, mat_x+80, mat_y+33, mat_x+80, mat_y+32, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+16, mat_y+80, mat_x+17, mat_y+81, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+33, mat_y+80, mat_x+32, mat_y+81, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+33, mat_y+97, mat_x+32, mat_y+96, 1, col_3);
			gsKit_prim_sprite(gsGlobal, mat_x+16, mat_y+97, mat_x+17, mat_y+96, 1, col_3);
			goto end_display;
done_test:
			//End of gsKit test section
*/
//End of commented out section  //Move this line as needed for tests
//Start of font display section
//Now we start to draw all vertical frame lines
/*			px=mat_x;
			drawOpSprite(col_1, px, mat_y, px+LINE_THICKNESS-1, mat_y+mat_h-1);
			for(j=0; j<17; j++) { //for each font column, plus the row_index column
				px += ch_x_stp;
				drawOpSprite(col_1, px, mat_y, px+LINE_THICKNESS-1, mat_y+mat_h-1);
			} //ends for each font column, plus the row_index column
			//Here all the vertical frame lines have been drawn
			//Next we draw the top horizontal line
			drawOpSprite(col_1, mat_x, mat_y, mat_x+mat_w-1, mat_y+LINE_THICKNESS-1);
			cy = mat_y+LINE_THICKNESS+2;
			ly = mat_y;
			for(i=0; i<17; i++) { //for each font row
				px=ch_x;
				if(!i) { //if top row (which holds the column indexes)
					drawChar('\\', px, cy, col_3);     //Display '\' at index crosspoint
				} else { //else a real font row
					drawChar(Hex[i-1], px, cy, col_3); //Display row index
				}
				for(j=0; j<16; j++) { //for each font column
					px += ch_x_stp;
					if(!i) { //if top row (which holds the column indexes)
						drawChar(Hex[j], px, cy, col_3); //Display Column index
					} else {
						drawChar((i-1)*16+j, px, cy, col_3); //Display font character
					}
				} //ends for each font column
				ly += ch_y_stp;
				drawOpSprite(col_1, mat_x, ly, mat_x+mat_w-1, ly+LINE_THICKNESS-1);
				cy += ch_y_stp;
			} //ends for each font row
			//End of font display section
		} //ends if(event||post_event)
//end_display:
		drawScr();
		post_event = event;
		event = 0;

		//Pad response section
		waitAnyPadReady();
		if(readpad() && new_pad){
			event |= 2;
			if((++test_type) < test_types){
				mat_y++;
				continue;
			}
			if (setting->GUI_skin[0]) {
				GUI_active = 1;
				loadSkin(BACKGROUND_PIC, 0, 0);
			}
			break;
		}
	} //ends while
	//----- End of event loop -----
}*/
//------------------------------
//endfunc ShowFont
//--------------------------------------------------------------
/*void triggerPowerOff(void)
{
	char filepath[MAX_PATH] = "xyz:/imaginary/hypothetical/doesn't.exist";
	FILE *File;

	File = fopen( filepath, "r" );
//	sprintf(mainMsg, "%s => %08X.", filepath, File);
//	drawMsg(mainMsg);
	if( File != NULL ) {
		fclose( File );
	} // end if( File != NULL )
}*/
//------------------------------
//endfunc triggerPowerOff
//--------------------------------------------------------------
/*void Validate_CNF_Path(void)
{
	char cnf_path[MAX_PATH];

	if(setting->CNF_Path[0] != '\0') {
		if(genFixPath(setting->CNF_Path, cnf_path) >= 0)
			strcpy(LaunchElfDir, setting->CNF_Path);
	}
}*/
//------------------------------
//endfunc Validate_CNF_Path
//--------------------------------------------------------------
/*void Set_CNF_Path(void)
{
	char	*tmp;

	getFilePath(setting->CNF_Path, CNF_PATH_CNF);
	if((tmp = strrchr(setting->CNF_Path, '/')))
	tmp[1] = '\0';
	Validate_CNF_Path();

	if(!strcmp(setting->CNF_Path, LaunchElfDir))
		sprintf(mainMsg, "%s ", LNG(Valid));
	else
		sprintf(mainMsg, "%s ", LNG(Bogus));
	sprintf(mainMsg+6, "%s = \"%s\"", LNG(CNF_Path), setting->CNF_Path);

}*/
//------------------------------
//endfunc Set_CNF_Path
//--------------------------------------------------------------
//Reload CNF, possibly after a path change
/*int reloadConfig()
{
	char tmp[MAX_PATH];
	int CNF_error = -1;

	if (numCNF == 0)
		strcpy(CNF, "LAUNCHELF.CNF");
	else
		sprintf(CNF, "LAUNCHELF%i.CNF", numCNF);

	CNF_error = loadConfig(mainMsg, CNF);
	Validate_CNF_Path();
	updateScreenMode(0);
	if (setting->GUI_skin[0]) GUI_active = 1;
	else GUI_active= 0;
	loadSkin(BACKGROUND_PIC, 0, 0);

	if(CNF_error<0)
		strcpy(tmp, mainMsg+strlen(LNG(Failed_To_Load)));
	else
		strcpy(tmp, mainMsg+strlen(LNG(Loaded_Config)));

	Load_External_Language();
	loadFont(setting->font_file);

	if(CNF_error<0)
		sprintf(mainMsg, "%s%s", LNG(Failed_To_Load), tmp);
	else
		sprintf(mainMsg, "%s%s", LNG(Loaded_Config), tmp);

	timeout = (setting->timeout+1)*SCANRATE;
	if(setting->discControl)
		loadCdModules();

	return CNF_error;
}*/
//------------------------------
//endfunc reloadConfig
//--------------------------------------------------------------
// Config Cycle Left  (--) by EP
/*void decConfig()
{
	if (numCNF > 0)
		numCNF--;
	else
		numCNF = maxCNF-1;
	
	reloadConfig();
}*/
//------------------------------
//endfunc decConfig
//--------------------------------------------------------------
// Config Cycle Right (++) by EP
/*void incConfig()
{
	if (numCNF < maxCNF-1)
		numCNF++;
	else
		numCNF = 0;
	
	reloadConfig();
}*/
//------------------------------
//endfunc incConfig
//--------------------------------------------------------------
//exists.  Tests if a file exists or not
//------------------------------
int exists(char *path)
{
	int fd;

	fd = genOpen(path, O_RDONLY);
	if (fd < 0)
		return 0;
	genClose(fd);
	return 1;
}
//------------------------------
//endfunc exists
//--------------------------------------------------------------
//uLE_related.  Tests if an uLE_related file exists or not
// Returns:
//  1 == uLE related path with file present
//  0 == uLE related path with file missing
// -1 == Not uLE related path
//------------------------------
/*int uLE_related(char *pathout, char *pathin)
{
	int ret;

	if(!strncmp(pathin, "uLE:/", 5)){
		sprintf(pathout, "%s%s", LaunchElfDir, pathin+5);
		if( exists(pathout) )
			return 1;
		sprintf(pathout, "%s%s", "mc0:/SYS-CONF/", pathin+5);
		if( !strncmp(LaunchElfDir, "mc1", 3) )
			pathout[2] = '1';
		if( exists(pathout) )
			return 1;
		pathout[2] ^= 1;  //switch between mc0 and mc1
		if( exists(pathout) )
			return 1;
		ret = 0;
	} else
		ret = -1;
	strcpy(pathout, pathin);
	return ret;
}*/
//------------------------------
//endfunc uLE_related
//--------------------------------------------------------------
// Run ELF. The generic ELF launcher.
//------------------------------
void RunElf(char *fmcbMsg, char *pathin)
{
	//char tmp[MAX_PATH];
	static char path[MAX_PATH];
	static char fullpath[MAX_PATH];
	static char party[40];
	//char *p;

	if (pathin[0] == 0)
		return;

	//if( !uLE_related(path, pathin) ) //1==uLE_rel 0==missing, -1==other dev
	//	return;

	strcpy(path, pathin);
	if (!strncmp(path, "OSDSYS", 6) || !strncmp(path, "FASTBOOT", 8) || !strncmp(path, "OSDMENU", 7))
		goto ELFnotFound;

	if (!strncmp(path + 5, "B?DATA", 6))
		goto Brdata;


	if (!strncmp(path, "Loader", 6)) {
		strcpy(path, "mass:/BOOT/BOOT.ELF");
		if (exists(path))
			goto finish;
		strcpy(path, "mc0:/BOOT/BOOT.ELF");
		if (!exists(path)) {
			path[2] = '1';
			if (exists(path))
				goto finish;
		} else
			goto finish;
	Brdata:
		strcpy(path, "mc0:/BEDATA-SYSTEM/BOOT.ELF");
		if (!exists(path)) {
			path[2] = '1';
			if (exists(path))
				goto finish;
		} else
			goto finish;
		strcpy(path, "mc0:/BADATA-SYSTEM/BOOT.ELF");
		if (!exists(path)) {
			path[2] = '1';
			if (exists(path))
				goto finish;
		} else
			goto finish;
		strcpy(path, "mc0:/BIDATA-SYSTEM/BOOT.ELF");
		if (!exists(path)) {
			path[2] = '1';
			if (exists(path))
				goto finish;
		} else
			goto finish;

		goto ELFnotFound;  //comming here means no-one of loader files was found
	}
finish:
	if (!strncmp(path, "mc", 2)) {
		if (!strncmp(path, "mc?", 3)) {
			path[2] = '0';
			if (!exists(path))
				path[2] = '1';
		}
		strcpy(fullpath, path);
		if (checkELFheader(fullpath) <= 0) {
		ELFnotFound:
			sprintf(fmcbMsg, "%s %s.", path, "Elf file not found");
			return;
		}
	}
	/*	if(!strncmp(path, "mc", 2)){
		party[0] = 0;
		if(path[2]==':'){
			strcpy(fullpath, "mc0:");
			strcat(fullpath, path+3);
			if(checkELFheader(fullpath)<=0){
				fullpath[2]='1';
				if(checkELFheader(fullpath)<=0){
ELFnotFound:
					sprintf(mainMsg, "%s %s.", path, LNG(is_Not_Found));
				}
			} //coming here means the ELF on unspecified MC is fine
		} else {
			strcpy(fullpath, path);
			if(checkELFheader(fullpath)<=0)
				goto ELFnotFound;
			//coming here means the ELF on specified MC is fine
		} //coming here means the ELF on  MC is fine
	}else if(!strncmp(path, "hdd0:/", 6)){
		loadHddModules();
		if(checkELFheader(path)<=0)
			goto ELFnotFound;
		//coming here means the ELF is fine
		sprintf(party, "hdd0:%s", path+6);
		p = strchr(party, '/');
		sprintf(fullpath, "pfs0:%s", p);
		*p = 0;
	}*/
	else if (!strncmp(path, "mass:", 5)) {
		loadUsbModules();
		if (checkELFheader(path) <= 0)
			goto ELFnotFound;
		//coming here means the ELF is fine
		party[0] = 0;
		strcpy(fullpath, "mass:");
		if (path[5] == '/')
			strcat(fullpath, path + 6);
		else
			strcat(fullpath, path + 5);
	} /*else if(!strncmp(path, "host:", 5)){
		initHOST();
		party[0] = 0;
		strcpy(fullpath, "host:");
		if(path[5] == '/')
			strcat(fullpath, path+6);
		else
			strcat(fullpath, path+5);
		makeHostPath(fullpath, fullpath);
		if(checkELFheader(fullpath)<=0)
				goto ELFnotFound;
		//coming here means the ELF is fine
	}else if(!stricmp(path, setting->Misc_PS2Disc)){
		drawMsg(LNG(Reading_SYSTEMCNF));
		strcpy(mainMsg, LNG(Failed));
		party[0]=0;
		trayopen=FALSE;
		if(!ReadCNF(fullpath)) return;
		//strcpy(mainMsg, "Success!"); return;
	}else if(!stricmp(path, setting->Misc_FileBrowser)){
		if (setting->GUI_skin[0]) {
			GUI_active = 0;
			loadSkin(BACKGROUND_PIC, 0, 0);
		}
		mainMsg[0] = 0;
		tmp[0] = 0;
		LastDir[0] = 0;
		getFilePath(tmp, FALSE);
		if(tmp[0])
			RunElf(tmp);
		return;
	}else if(!stricmp(path, setting->Misc_PS2Browser)){
		__asm__ __volatile__(
			"	li $3, 0x04;"
			"	syscall;"
			"	nop;"
		);
		//There has been a major change in the code for calling PS2Browser
		//The method above is borrowed from PS2MP3. It's independent of ELF loader
		//The method below was used earlier, but causes reset with new ELF loader
		//party[0]=0;
		//strcpy(fullpath,"rom0:OSDSYS");
	}else if(!stricmp(path, setting->Misc_PS2Net)){
		mainMsg[0] = 0;
		loadNetModules();
		return;
	}else if(!stricmp(path, setting->Misc_PS2PowerOff)){
		mainMsg[0] = 0;
		drawMsg(LNG(Powering_Off_Console));
		setupPowerOff();
		hddPowerOff();
		poweroff_delay = SCANRATE/4; //trigger delay for those without net adapter
		return;
	}else if(!stricmp(path, setting->Misc_HddManager)){
		if (setting->GUI_skin[0]) {
			GUI_active = 0;
			loadSkin(BACKGROUND_PIC, 0, 0);
		}
		hddManager();
		return;
	}else if(!stricmp(path, setting->Misc_TextEditor)){
		if (setting->GUI_skin[0]) {
			GUI_active = 0;
			loadSkin(BACKGROUND_PIC, 0, 0);
		}
		TextEditor();
		return;
	}else if(!stricmp(path, setting->Misc_JpgViewer)){
		if (setting->GUI_skin[0]) {
			GUI_active = 0;
			loadSkin(BACKGROUND_PIC, 0, 0);
		}
		JpgViewer();
		return;
	}else if(!stricmp(path, setting->Misc_Configure)){
		if (setting->GUI_skin[0]) {
			GUI_active = 0;
			loadSkin(BACKGROUND_PIC, 0, 0);
		}
		config(mainMsg, CNF);
		return;
	}else if(!stricmp(path, setting->Misc_Load_CNFprev)){
		decConfig();
		return;
	}else if(!stricmp(path, setting->Misc_Load_CNFnext)){
		incConfig();
		return;
	}else if(!stricmp(path, setting->Misc_Set_CNF_Path)){
		Set_CNF_Path();
		return;
	}else if(!stricmp(path, setting->Misc_Load_CNF)){
		reloadConfig();
		return;
//Next clause is for an optional font test routine
	}else if(!stricmp(path, setting->Misc_ShowFont)){
		ShowFont();
		return;
	}else if(!stricmp(path, setting->Misc_Debug_Info)){
		if (setting->GUI_skin[0]) {
			GUI_active = 0;
			loadSkin(BACKGROUND_PIC, 0, 0);
		}
		ShowDebugInfo();
		return;
	}else if(!strncmp(path, "cdfs", 4)){
		loadCdModules();
		CDVD_FlushCache();
		CDVD_DiskReady(0);
		party[0] = 0;
		strcpy(fullpath, path);
	}*/
	else if (!strncmp(path, "rom", 3)) {
		party[0] = 0;
		strcpy(fullpath, path);
	}

	clrScr(GS_SETREG_RGBA(0x00, 0x00, 0x00, 0));
	drawScr();
	clrScr(GS_SETREG_RGBA(0x00, 0x00, 0x00, 0));
	drawScr();
	free(setting);
	free(elisaFnt);
	//free(External_Lang_Buffer);
	free(FontBuffer);
	padPortClose(1, 0);
	padPortClose(0, 0);
	//if(ps2kbd_opened) PS2KbdClose();
	TimerEnd();
	RunLoaderElf(fullpath, party);
}
//------------------------------
//endfunc RunElf
//--------------------------------------------------------------
/*void RunSelectedElf(void)
{
	int n=0;
	int i;
	
	for(i=0; i<12; i++){
		if(setting->LK_Path[i][0] && n++==selected){
			RunElf(setting->LK_Path[i]);
			break;
		}
	}
}*/
//------------------------------
//endfunc RunSelectedElf
//--------------------------------------------------------------
// reboot IOP (original source by Hermes in BOOT.c - cogswaploader)
// dlanor: but changed now, as the original was badly bugged
void Reset()
{
	SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
	while (!SifIopSync())
		;
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();

	SifInitRpc(0);
	FlushCache(0);
	FlushCache(2);

	have_cdvd = 0;
	have_usbd = 0;
	have_usb_mass = 0;
	have_ps2smap = 0;
	have_ps2host = 0;
	have_ps2ftpd = 0;
	have_ps2kbd = 0;
	have_NetModules = 0;
	have_HDD_modules = 0;
	have_sbv_patches = 0;

	CheckModules();
	loadBasicModules();
	mcReset();
	mcInit(MC_TYPE_MC);
	//	padReset();
	//	setupPad();
}
//------------------------------
//endfunc Reset
//--------------------------------------------------------------
int uLE_detect_TV_mode()
{
	int ROMVER_fd;
	char ROMVER_file[20];  //16 byte file

	ROMVER_fd = genOpen("rom0:ROMVER", O_RDONLY);
	if (ROMVER_fd < 0)
		return TV_mode_NTSC;  //NTSC is default mode for unidentified console
	genRead(ROMVER_fd, ROMVER_file, 16);
	genClose(ROMVER_fd);
	if (ROMVER_file[4] == 'E')
		return TV_mode_PAL;  //PAL mode is identified by 'E' for Europe
	return TV_mode_NTSC;     //All other cases need NTSC
}
//------------------------------
//endfunc uLE_detect_TV_mode
//--------------------------------------------------------------
int main(int argc, char *argv[])
{
	char *p;  //, CNF_pathname[MAX_PATH];
	//int event, post_event=0;
	//int RunELF_index, nElfs=0;
	//CdvdDiscType_t cdmode, old_cdmode;  //used for disc change detection
	//int hdd_booted = 0;
	//int host_or_hdd_booted = 0;
	int mass_booted = 0;  //flags boot made with compatible mass drivers
	int mass_needed = 0;  //flags need to load compatible mass drivers
	int mc_booted = 0;
	//int cdvd_booted = 0;
	//int	host_booted = 0;
	int gs_vmode;
	int CNF_error = -1;  //assume error until CNF correctly loaded
	int i;

	boot_argc = argc;
	for (i = 0; (i < argc) && (i < 8); i++)
		boot_argv[i] = argv[i];

	SifInitRpc(0);
	CheckModules();
	loadBasicModules();
	mcInit(MC_TYPE_MC);
	genInit();
	//Init_Default_Language();

	force_IOP = 0;
	LaunchElfDir[0] = 0;
	if ((argc > 0) && argv[0]) {
		strcpy(LaunchElfDir, argv[0]);
		if (!strncmp(argv[0], "mass", 4)) {
			if (!strncmp(argv[0], "mass0:\\", 7)) {  //SwapMagic boot path for usb_mass
				//Transform the boot path to homebrew standards
				LaunchElfDir[4] = ':';
				strcpy(&LaunchElfDir[5], &LaunchElfDir[7]);
				for (i = 0; LaunchElfDir[i] != 0; i++) {
					if (LaunchElfDir[i] == '\\')
						LaunchElfDir[i] = '/';
				}
				force_IOP = 1;    //Note incompatible drivers present (as yet ignored)
				mass_needed = 1;  //Note need to load compatible mass: drivers
			} else {              //else we booted with normal homebrew mass: drivers
				mass_booted = 1;  //Note presence of compatible mass: drivers
			}
		} else if (!strncmp(argv[0], "mc", 2))
			mc_booted = 1;
		/*else if	(!strncmp(argv[0], "cd", 2))
			cdvd_booted = 1;
		else if	((!strncmp(argv[0], "hdd", 3)) || (!strncmp(argv[0], "pfs", 3)))
			hdd_booted = 1;  //Modify this section later to cover Dev2 needs !!!
		*/
	}
	strcpy(boot_path, LaunchElfDir);

	/*
	if(!strncmp(LaunchElfDir, "host",4)) {
		host_or_hdd_booted = 1;
		if	(have_fakehost)
			hdd_booted = 1;
		else
			host_booted = 1;
	}

	if	(host_booted)	//Fix untestable modules for host booting
	{	have_ps2smap = 1;
		have_ps2host	= 1;
	}
*/

	if (mass_booted)  //Fix untestable module for USB_mass booting
	{
		have_usbd = 1;
		have_usb_mass = 1;
	}

	if (((p = strrchr(LaunchElfDir, '/')) == NULL) && ((p = strrchr(LaunchElfDir, '\\')) == NULL))
		p = strrchr(LaunchElfDir, ':');
	if (p != NULL)
		*(p + 1) = 0;
	//The above cuts away the ELF filename from LaunchElfDir, leaving a pure path

	/*if(hdd_booted && !strncmp(LaunchElfDir, "hdd", 3)){;
		//Patch DMS4 Dev2 booting here, when we learn more about how it works
		//Trying to mount that partition for loading CNF simply crashes.
		//We may need a new IOP reset method for this.
	}*/

	LastDir[0] = 0;

	if (uLE_detect_TV_mode() == TV_mode_PAL) {  //Test console TV mode
		SCREEN_X = 652;
		SCREEN_Y = 72;
	} else {
		SCREEN_X = 632;
		SCREEN_Y = 50;
	}

	//RA NB: loadConfig needs  SCREEN_X and SCREEN_Y to be defaults matching TV mode
	/*CNF_error = loadConfig(mainMsg, strcpy(CNF, "LAUNCHELF.CNF"));
	if(CNF_error<0)
		strcpy(CNF_pathname, mainMsg+strlen(LNG(Failed_To_Load)));
	else
		strcpy(CNF_pathname, mainMsg+strlen(LNG(Loaded_Config)));
	*/
	initConfig();
	//init_delay = setting->Init_Delay*SCANRATE;

	//TV_mode = setting->TV_mode;
	//if((TV_mode!=TV_mode_NTSC)&&(TV_mode!=TV_mode_PAL)){ //If no forced request
	TV_mode = uLE_detect_TV_mode();  //Let console region decide TV_mode
	//}

	if (TV_mode == TV_mode_PAL) {  //Use PAL mode if chosen (forced or auto)
		gs_vmode = GS_MODE_PAL;
		SCREEN_WIDTH = 640;
		SCREEN_HEIGHT = 512;
		SCREEN_X = 652;
		SCREEN_Y = 72;
		Menu_end_y = Menu_start_y + 26 * FONT_HEIGHT;
	} else {  //else use NTSC mode (forced or auto)
		gs_vmode = GS_MODE_NTSC;
		SCREEN_WIDTH = 640;
		SCREEN_HEIGHT = 448;
		SCREEN_X = 632;
		SCREEN_Y = 50;
		Menu_end_y = Menu_start_y + 22 * FONT_HEIGHT;
	} /* end else */
	Frame_end_y = Menu_end_y + 4;
	Menu_tooltip_y = Frame_end_y + LINE_THICKNESS + 2;

	//maxCNF = setting->numCNF;
	swapKeys = setting->swapKeys;
	if (setting->resetIOP) {
		Reset();
		if (!strncmp(LaunchElfDir, "mass:", 5)) {
			initsbv_patches();
			loadUsbModules();
		}
		/*else if(!strncmp(LaunchElfDir, "host:", 5))
		{	getIpConfig();
			initsbv_patches();
			initHOST();
		}*/
	}
	//Here IOP reset (if done) has been completed, so it's time to load and init drivers
	//getIpConfig();
	setupPad();  //Comment out this line when using early setupPad above
	initsbv_patches();

	//if(setting->discControl)
	//	loadCdModules();

	//Last chance to look at bootup screen, so allow braking here
	/*
	if(readpad() && (new_pad && PAD_UP))
	{ scr_printf("________ Boot paused. Press 'Circle' to continue.\n");
		while(1)
		{	if(new_pad & PAD_CIRCLE)
				break;
			while(!readpad());
		}
	}
*/
	setupGS(gs_vmode);
	gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00, 0x00, 0x00, 0x00, 0x00));

	//	startKbd();
	TimerInit();
	//	WaitTime=Timer();

	//	loadFont("");  //Some font must be loaded before loading some device modules
	//	Load_External_Language();
	//	loadFont(setting->font_file);
	loadFont(0);
	//	if (setting->GUI_skin[0]) {GUI_active = 1;}
	//	loadSkin(BACKGROUND_PIC, 0, 0);

	gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00, 0x00, 0x00, 0x00, 0x00));

	/*	if(CNF_error<0)
		sprintf(mainMsg, "%s%s", LNG(Failed_To_Load), CNF_pathname);
	else
		sprintf(mainMsg, "%s%s", LNG(Loaded_Config), CNF_pathname);
*/
	//Here nearly everything is ready for the main menu event loop
	//But before we start that, we need to validate CNF_Path
	//	Validate_CNF_Path();

	//	timeout = (setting->timeout+1)*SCANRATE;
	//	cdmode = -1; //flag unchecked cdmode state
	//	event = 1;   //event = initial entry
	//----- Start of main menu event loop -----

	CNF_error = loadConfig_fmcb(fmcbMsg, cnf_path_fmcb3);  //check mass
	if (CNF_error < 0) {
		CNF_error = loadConfig_fmcb(fmcbMsg, cnf_path_fmcb);  //check mc0
		if (CNF_error < 0) {
			CNF_error = loadConfig_fmcb(fmcbMsg, cnf_path_fmcb2);  //check mc1
			if (CNF_error < 0)
				strcpy(fmcbMsg, "Failed to load configuration file");
		}
	}

	Config_fmcb(fmcbMsg);

	/*while(1){
		//Background event section
		if(setting->discControl){
			CDVD_Stop();
			old_cdmode = cdmode;
			cdmode = cdGetDiscType();
			if(cdmode!=old_cdmode)
				event |= 4;  //event |= disc change detection
			if(cdmode==CDVD_TYPE_NODISK){
				trayopen = TRUE;
				strcpy(mainMsg, LNG(No_Disc));
			}else if(cdmode>=0x01 && cdmode<=0x04){
				strcpy(mainMsg, LNG(Detecting_Disc));
			}else if(trayopen==TRUE){
				trayopen=FALSE;
				strcpy(mainMsg, LNG(Stop_Disc));
			}
		}

		if(poweroff_delay) {
			poweroff_delay--;
			if(!poweroff_delay)
				triggerPowerOff();
		}

		if(init_delay){
			init_delay--;
			if(init_delay%SCANRATE==SCANRATE-1)
				event |= 8;  //event |= visible delay change
		}
		else if(timeout && !user_acted){
			timeout--;
			if(timeout%SCANRATE==SCANRATE-1)
				event |= 8;  //event |= visible timeout change
		}

		//Display section
		if(event||post_event){  //NB: We need to update two frame buffers per event
			if (!(setting->GUI_skin[0]))
				nElfs = drawMainScreen(); //Display pure text GUI on generic background
			else if (!setting->Show_Menu) {
				setLaunchKeys(); //Display only GUI jpg
				clrScr(setting->color[0]);
			}
			else {
				if (TV_mode == TV_mode_NTSC)
					nElfs = drawMainScreen2ntsc(); //NTSC Display GUI jpg plus button texts
				else
					nElfs = drawMainScreen2pal(); //PAL
			}
		}
		drawScr();
		post_event = event;
		event = 0;

		//Pad response section
		if(!init_delay && (waitAnyPadReady(), readpad())){
			if(new_pad){
				event |= 2;  //event |= pad command
			}
			RunELF_index = -1;
			switch(mode){
			case BUTTON:
				if(new_pad & PAD_CIRCLE)        RunELF_index = 1;
				else if(new_pad & PAD_CROSS)    RunELF_index = 2;
				else if(new_pad & PAD_SQUARE)   RunELF_index = 3;
				else if(new_pad & PAD_TRIANGLE) RunELF_index = 4;
				else if(new_pad & PAD_L1)       RunELF_index = 5;
				else if(new_pad & PAD_R1)       RunELF_index = 6;
				else if(new_pad & PAD_L2)       RunELF_index = 7;
				else if(new_pad & PAD_R2)       RunELF_index = 8;
				else if(new_pad & PAD_L3)       RunELF_index = 9;
				else if(new_pad & PAD_R3)       RunELF_index = 10;
				else if(new_pad & PAD_START)    RunELF_index = 11;
				else if(new_pad & PAD_SELECT)   RunELF_index = 12;
				else if(maxCNF > 1 && new_pad & PAD_LEFT)  RunELF_index = 13;
				else if(maxCNF > 1 && new_pad & PAD_RIGHT) RunELF_index = 14;
				else if(new_pad & PAD_UP || new_pad & PAD_DOWN){
					user_acted = 1;
					if (!setting->Show_Menu && setting->GUI_skin[0]){} //GUI Menu: disabled when there's no text on menu screen
					else{
						selected=0;
						mode=DPAD;
					}
				}
				if(RunELF_index >= 0 && setting->LK_Path[RunELF_index][0]){
					user_acted = 1;
					RunElf(setting->LK_Path[RunELF_index]);
				}
				break;
			
			case DPAD:
				if(new_pad & PAD_UP){
					selected--;
					if(selected<0)
						selected=nElfs-1;
				}else if(new_pad & PAD_DOWN){
					selected++;
					if(selected>=nElfs)
						selected=0;
				}else if((!swapKeys && new_pad & PAD_CROSS)
				      || (swapKeys && new_pad & PAD_CIRCLE) ){
					mode=BUTTON;
				}else if((swapKeys && new_pad & PAD_CROSS)
				      || (!swapKeys && new_pad & PAD_CIRCLE) ){
					if(maxCNF > 1 && selected==nElfs-1){
						mode=BUTTON;
						incConfig();
					}else if(maxCNF > 1 && selected==nElfs-2){
						mode=BUTTON;
						decConfig();
					}else if((maxCNF > 1 && selected==nElfs-3) || (selected==nElfs-1)){
						mode=BUTTON;
						if (setting->GUI_skin[0]) {
							GUI_active = 0;
							loadSkin(BACKGROUND_PIC, 0, 0);
						}
						config(mainMsg, CNF);
					}else
						RunSelectedElf();
				}
				break;
			}
		}//ends Pad response section

		if(timeout/SCANRATE==0 && setting->LK_Path[0][0] && mode==BUTTON && !user_acted){
			RunElf(setting->LK_Path[0]);
			user_acted = 1; //Halt timeout after default action, just as for user action
			event |= 8;  //event |= visible timeout change
		}
	}*/
	//----- End of main menu event loop -----
}
#ifndef SKIP_FTRUJY_ELF_REDUCTIONS
   void _ps2sdk_timezone_update() {}
   void _ps2sdk_libc_init() {}
   void _ps2sdk_libc_deinit() {}
   void _ps2sdk_args_parse(int argc, char ** argv) {}
#endif
//*/
//------------------------------
//endfunc main
//--------------------------------------------------------------
//End of file: main.c
//--------------------------------------------------------------
