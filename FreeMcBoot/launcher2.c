/*
  _____     ___ ____
   ____|   |    ____|      PS2 Open Source Project
  |     ___|   |____

---------------------------------------------------------------------------

    Copyright (C) 2008 - Neme & jimmikaelkael (www.psx-scene.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the Free McBoot License.

	This program and any related documentation is provided "as is"
	WITHOUT ANY WARRANTIES, either express or implied, including, but not
 	limited to, implied warranties of fitness for a particular purpose. The
 	entire risk arising out of use or performance of the software remains
 	with you.
   	In no event shall the author be liable for any damages whatsoever
 	(including, without limitation, damages to your hardware or equipment,
 	environmental damage, loss of health, or any kind of pecuniary loss)
 	arising out of the use of or inability to use this software or
 	documentation, even if the author has been advised of the possibility of
 	such damages.

    You should have received a copy of the Free McBoot License along with
    this program; if not, please report at psx-scene :
    http://psx-scene.com/forums/freevast/

---------------------------------------------------------------------------

    Init module for cvdvd init fix and multilanguage fix are from :
    ffgriever (www.psx-scene.com)

	Modifications for working just from mc0:/BOOT/ are from :
	Howling Wolf & Chelsea

---------------------------------------------------------------------------

*/

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <fileio.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <sbv_patches.h>
#include <loadfile.h>
#include <libpad.h>
#include <string.h>
#include <stdio.h>
#include <osd_config.h>
#include <libcdvd.h>
#include <malloc.h>
#include <debug.h>
#include "splash.h"
#include "loading.h"

#define MAX_PATH 260

extern void iomanx_irx;
extern int size_iomanx_irx;

extern void init_irx;
extern int size_init_irx;
extern void chkesr_irx;
extern int size_chkesr_irx;

extern void elf_loader;
extern int size_elf_loader;

#define DEBUG

#define RGBA(r, g, b, a) \
	((u64)(r) | ((u64)(g) << 8) | ((u64)(b) << 16) | ((u64)(a) << 24))

#ifdef DEBUG
#define SETBG(r, g, b) *(u64 *)0x120000e0 = RGBA(r, g, b, 0)
#else
#define SETBG(...)
#endif

// GS related
#define NTSC 2
#define PAL 3

int VMode;


// Externals:

// chkesr_rpc.c
extern int chkesr_rpc_Init(void);
extern int Check_ESR_Disc(void);

// timer.c
extern void TimerInit(void);
extern u64 Timer(void);
extern void TimerEnd(void);

// pad.c
extern u32 new_pad;
extern int readPad(void);
extern void waitAnyPadReady(void);
extern int setupPad(void);

// gs.c
typedef enum {
	PAL_640_512_32,
	NTSC_640_448_32
} gs_video_mode;
extern void gs_reset(void);
extern int gs_init(gs_video_mode mode);
extern void gs_set_fill_color(u8 r, u8 g, u8 b);
extern void gs_fill_rect(u16 x0, u16 y0, u16 x1, u16 y1);
extern u16 gs_get_max_x(void);
extern u16 gs_get_max_y(void);
extern void gs_print_bitmap(u16 x, u16 y, u16 w, u16 h, u32 *data);


// function prototypes
void wipeUserMem(void);
int file_exists(char *filepath);
int get_CNF_string(unsigned char **CNF_p_p, unsigned char **name_p_p, unsigned char **value_p_p);
int loadConfig(void);
int Read_SYSTEM_CNF(char *boot_path, char *ver);
u8 *find_bytes_with_mask(u8 *buf, u32 bufsize, u8 *bytes, u8 *mask, u32 len);
u8 *find_string(const u8 *string, u8 *buf, u32 bufsize);
const char *get_string_pointer(const char **strings, u32 index);
int handle_menu_selection(int selected);
void patch_menu(u8 *osd);
void draw_menu_item_selected(int X, int Y, u32 *color, int alpha, const char *string, int num);
void draw_menu_item_unselected(int X, int Y, u32 *color, int alpha, const char *string, int num);
void patch_draw_menu(u8 *osd);
void get_buttons_panel_type(int type);
void draw_nonselectable_item_left(int X, int Y, u32 *color, int alpha, const char *string);
void draw_nonselectable_item_right(int X, int Y, u32 *color, int alpha, const char *string);
void draw_icon_left(int type, int X, int Y, int alpha);
void draw_icon_right(int type, int X, int Y, int alpha);
void patch_button_panel(u8 *osd);
int deviated_exec_dvdv(void);
int deviated_exec_ps2dvd(void);
void patch_auto_launch(u8 *osd);
void patch_skip_disc(u8 *osd);
void patch_loop_menu(u8 *osd);
void patch_force_video_mode(u8 *osd);
void patch_skip_hdd(u8 *osd);
void patch_and_execute_osdsys(void *epc, void *gp);
void IOP_Reset(void);
void load_modules(void);
void load_chkesr_module(void);
void CleanUp(int iop_reset);
void OSDSYS_CleanUp(void);
void launch_osdsys(void);
void check_path(void);
void reload_osdsys(void);
void load_elf(char *elf_path);
void check_ESR_paths(void);
int FastBoot_Disc(void);
void check_exec_paths(void);
void Set_Default_Settings(void);
void SetOsdConfig(void);
void FMCB_loader_Init(void);


// region letter list
char *MG_region[8] = {"A", "E", "U", "J", "M", "O", "R", "C"};
u8 MG_REGION[1];

// Paths that can be changed with those from cnf.
char esr_path[3][MAX_PATH] = {
    "mass:/BOOT/ESR.ELF",
    "mc?:/BOOT/ESR.ELF",
    "mc?:/B?DATA-SYSTEM/ESR.ELF"};

// Hardcoded paths
char default_path[15][30] = {
    "mc?:/BOOT/ULE.ELF",  // Auto index : 0
    "mc?:/BOOT/BOOT2.ELF",
    "mc?:/APPS/ULE.ELF",
    "mass:/BOOT/BOOT4.ELF",  // L2   index : 3
    "mc?:/BOOT/BOOT4.ELF",
    "mc?:/B?DATA-SYSTEM/BOOT4.ELF",
    "mass:/BOOT/BOOT2.ELF",  // R2   index : 6
    "mc?:/BOOT/BOOT2.ELF",
    "mc?:/B?DATA-SYSTEM/BOOT2.ELF",
    "mass:/BOOT/BOOT3.ELF",  // L1   index : 9
    "mc?:/BOOT/BOOT3.ELF",
    "mc?:/B?DATA-SYSTEM/BOOT3.ELF",
    "mass:/RESCUE.ELF",  // R1   index : 12
    "mc?:/BOOT/BOOT1.ELF",
    "mc?:/B?DATA-SYSTEM/BOOT1.ELF"};

char rescue_path[30] = "mass:/RESCUE.ELF";
char cnf_path_usb[30] = "mass:/FUNTUNA.CNF";
char cnf_path[30] = "mc0:/BOOT/FUNTUNA.CNF";
/**********************USBD.IRX******************************/
char usbd_irx_path[30] = "mc0:/BOOT/USBD.IRX";
char usbd_irx_path_sysconf[30] = "mc0:/SYS-CONF/USBD.IRX";
/**********************USBHDFSD.IRX************************/
char usb_mass_irx_path[30] = "mc0:/BOOT/USBHDFSD.IRX";
char usb_mass_irx_path_sysconf[30] = "mc0:/SYS-CONF/USBHDFSD.IRX";

// DVD-Player Update path
char dvdpl_path[] = "mc0:/BREXEC-DVDPLAYER/dvdplayer.elf";

// Buttons ID must be kept in this order !
// They are readed in a loop depending on their libpad value
char LK_ID[17][10] = {
    "Auto",
    "Select",    // 0x0001
    "L3",        // 0x0002
    "R3",        // 0x0004
    "Start",     // 0x0008
    "Up",        // 0x0010
    "Right",     // 0x0020
    "Down",      // 0x0040
    "Left",      // 0x0080
    "L2",        // 0x0100
    "R2",        // 0x0200
    "L1",        // 0x0400
    "R1",        // 0x0800
    "Triangle",  // 0x1000
    "Circle",    // 0x2000
    "Cross",     // 0x4000
    "Square"     // 0x8000
};


#define NEWITEMS 100  // the number of max added menu items in osdsys

// OSDSYS settings struct, contains all configurable OSDSYS settings
// takes 1700 bytes with 100 items
typedef struct
{
	int hack_enabled;              // Enable/Disable OSDSYS hacking
	int skip_mc;                   // Enable/Disable MC Update check
	int skip_hdd;                  // Enable/Disable HDD Update check
	int skip_disc;                 // Enable/Disable disc boot while inserting them while OSDSYS is loaded
	int skip_logo;                 // Enable/Disable Sony Entertainment logo while loading OSDSYS
	int goto_inner_browser;        // Enable/Disable goes to inner_browser while loading OSDSYS
	char *video_mode;              // Set OSDSYS Video mode : AUTO, PAL or NTSC
	int scroll_menu;               // Enable/Disable scrolling menu
	int menu_x;                    // Set menu X coordinate (menu center)
	int menu_y;                    // Set menu Y coordinate (menu center), only for scroll menu
	int enter_x;                   // Set "Enter" button X coordinate (at main OSDSYS menu)
	int enter_y;                   // Set "Enter" button Y coordinate (at main OSDSYS menu)
	int version_x;                 // Set "Version" button X coordinate (at main OSDSYS menu)
	int version_y;                 // Set "Version" button Y coordinate (at main OSDSYS menu)
	int cursor_max_velocity;       // Set the cursors movement amplitude, only for scroll menu
	int cursor_acceleration;       // Set the cursors speed, only for scroll menu
	char *left_cursor;             // Set the left cursor text, only for scroll menu
	char *right_cursor;            // Set the right cursor text, only for scroll menu
	char *menu_top_delimiter;      // Set the top menu delimiter text, only for scroll menu
	char *menu_bottom_delimiter;   // Set the bottom menu delimiter text, only for scroll menu
	u32 selected_color[4];         // Set the menu items color when selected
	u32 unselected_color[4];       // Set the menu items color when not selected
	int num_displayed_items;       // Set the number of menu items displayed, only for scroll menu
	char *item_name[NEWITEMS];     // Set menu items text
	char *item_path[NEWITEMS][3];  // Set 3 paths for each menu items
} osdsys_settings;

// FMCB settings struct, loader settings
typedef struct
{
	int pad_delay;           // Set the total pad press delay
	int debug_screen;        // Enable/Disable green debug screen when all 3 paths are not valid
	int fastboot;            // Enable/Disable FastBoot for PS1, PS2, ESR, Video DVD, Audio CD Discs
	char *p_LK_Path[17][3];  // Paths pointers for pad
	char *p_ESR_Path[3];     // Paths pointers for ESR
	int autolaunch_patch;    // Enable/Disable autolaunch_patch (The one that allows ESR disc to be
} fmcb_settings;             // booted, and PS2 DVD fix on some older PS2), not user configureable

// variables to hold FMCB & OSDSYS settings
osdsys_settings OSDSYS;
fmcb_settings FMCB;

char romver_region_char[1];
char *p_ExecPath;
char *p_pad_ExecPath[3];
char cdboot_path[MAX_PATH];
char eromdrv_arg[MAX_PATH];
char dvdpl_arg[MAX_PATH];
int item_cnt = 0;  // Counter for OSDSYS menu printable items (existing and checked)
int pad_inited = 0;
int timer_inited = 0;
int cdvdrpc_inited = 0;
int fastboot_delay = 10;  // not user configurable, 10ms minimum
int old_dvdelf = 1;       // DVDELF version, Fat PS2 one by default
char *valid_ESR_path;     // Tested valid ESR path
u8 romver[16];
int call_from_osdsys = 0;  // flag to be set to 1 when our functions are called from OSDSYS
int loading_print = 0;     // flag to set to 1 if we want to print "loading" bitmap when load_elf func is called
u32 bios_version = 0;      // Bios revision (acquired from init.irx)
int isEarlyJap = 0;        // To determine if the ps2 is an early Jap
int dummy_memalloc = 1;
int boot_from_mc = 0;      // To determine from which mc FMCB has been booted
unsigned char *CNF_RAM_p;  // pointer to CNF file into memory

//--------------------------------------------------------------

void wipeUserMem(void)
{  // Clean user memory
	int i;
	for (i = 0x100000; i < 0x2000000; i += 64) {
		asm(
		    "\tsq $0, 0(%0) \n"
		    "\tsq $0, 16(%0) \n"
		    "\tsq $0, 32(%0) \n"
		    "\tsq $0, 48(%0) \n" ::"r"(i));
	}
}

//--------------------------------------------------------------
int file_exists(char *filepath)
{
	int fdn;

	fdn = open(filepath, O_RDONLY);
	if (fdn < 0)
		return 0;
	close(fdn);

	return 1;
}

//________________ From uLaunchELF ______________________
//---------------------------------------------------------------------------
// get_CNF_string is the main CNF parser called for each CNF variable in a
// CNF file. Input and output data is handled via its pointer parameters.
// The return value flags 'false' when no variable is found. (normal at EOF)
//---------------------------------------------------------------------------
int get_CNF_string(unsigned char **CNF_p_p,
                   unsigned char **name_p_p,
                   unsigned char **value_p_p)
{
	unsigned char *np, *vp, *tp = *CNF_p_p;

start_line:
	while ((*tp <= ' ') && (*tp > '\0'))
		tp += 1;  //Skip leading whitespace, if any
	if (*tp == '\0')
		return 0;   //but exit at EOF
	np = tp;        //Current pos is potential name
	if (*tp < 'A')  //but may be a comment line
	{               //We must skip a comment line
		while ((*tp != '\r') && (*tp != '\n') && (*tp > '\0'))
			tp += 1;      //Seek line end
		goto start_line;  //Go back to try next line
	}

	while ((*tp >= 'A') || ((*tp >= '0') && (*tp <= '9')))
		tp += 1;  //Seek name end
	if (*tp == '\0')
		return 0;  //but exit at EOF

	while ((*tp <= ' ') && (*tp > '\0'))
		*tp++ = '\0';  //zero&skip post-name whitespace
	if (*tp != '=')
		return 0;  //exit (syntax error) if '=' missing
	*tp++ = '\0';  //zero '=' (possibly terminating name)

	while ((*tp <= ' ') && (*tp > '\0')       //Skip pre-value whitespace, if any
	       && (*tp != '\r') && (*tp != '\n')  //but do not pass the end of the line
	       && (*tp != '\7')                   //allow ctrl-G (BEL) in value
	)
		tp += 1;
	if (*tp == '\0')
		return 0;  //but exit at EOF
	vp = tp;       //Current pos is potential value

	while ((*tp != '\r') && (*tp != '\n') && (*tp != '\0'))
		tp += 1;  //Seek line end
	if (*tp != '\0')
		*tp++ = '\0';  //terminate value (passing if not EOF)
	while ((*tp <= ' ') && (*tp > '\0'))
		tp += 1;  //Skip following whitespace, if any

	*CNF_p_p = tp;    //return new CNF file position
	*name_p_p = np;   //return found variable name
	*value_p_p = vp;  //return found variable value
	return 1;         //return control to caller
}  //Ends get_CNF_string

//----------------------------------------------------------------
// Load CNF
//----------------------------------------------------------------
int loadConfig(void)
{
	int i, j, fd, var_cnt, CNF_version;
	size_t CNF_size;
	char tsts[20];
	char path[MAX_PATH];
	unsigned char *CNF_p, *name, *value;
	char hexvalue_buf[4];
	u8 *dummy_p = NULL;
	u32 dummy_sz;

	strcpy(path, cnf_path_usb);
	fd = -1;
	fd = open(path, O_RDONLY);  // Try to open cnf from USB first
	if (fd < 0) {
		strcpy(path, cnf_path);
		fd = -1;
		if (boot_from_mc == 1)
			path[2] = '1';
		fd = open(path, O_RDONLY);  // Try to open cnf from mc0:
		if (fd < 0) {
			if (boot_from_mc == 1)
				path[2] = '0';
			else
				path[2] = '1';
			fd = open(path, O_RDONLY);  // Try to open cnf from mc1:
		}
		if (fd < 0) {
		failed_load:
			return 0;
		}
	}  // This point is only reached after succefully opening CNF

	CNF_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	//init_scr();
	//scr_clear();

	if (dummy_memalloc) {
		// This part ensures that CNF content loads at 0x01f00000 leaving sufficient space for big cnf files
		// If we don't do this, memory will be allocated just after the loader, something like 0x000ecb00
		// leading to leave unsufficient space for big cnf files since OSDSYS needs to load at 0x00100000
		//scr_printf("\n\t allocating dummy space\n");
		dummy_p = (char *)malloc(32);
		dummy_sz = 0x01f00000 - (u32)(dummy_p)-16;
		free(dummy_p);
		dummy_p = (char *)malloc(dummy_sz);
		//scr_printf("\t dummy_sz: %08x\n", (u32)dummy_sz);
		//scr_printf("\t dummy_p: %08x\n", (u32)dummy_p);
	}

	CNF_RAM_p = (char *)malloc(CNF_size);
	//scr_printf("\n\t CNF_RAM_p: %08x\n", (u32)CNF_RAM_p);

	if (dummy_p != NULL) {
		free(dummy_p);
		dummy_memalloc = 0;
	}

	CNF_p = CNF_RAM_p;
	if (CNF_p == NULL) {
		close(fd);
		goto failed_load;
	}

	read(fd, CNF_p, CNF_size);  // Read CNF as one long string
	close(fd);
	CNF_p[CNF_size] = '\0';  // Terminate the CNF string

	CNF_version = 0;  // The CNF version is still unidentified
	for (var_cnt = 0; get_CNF_string(&CNF_p, &name, &value); var_cnt++) {
		// A variable was found, now we dispose of its value.
		if (!strcmp(name, "CNF_version")) {
			CNF_version = atoi(value);
			continue;
		} else if (CNF_version == 0) {
			goto failed_load;  // Refuse unidentified CNF
		}
		if (!strcmp(name, "OSDSYS_video_mode")) {
			OSDSYS.video_mode = value;
			continue;
		}
		for (i = 0; i < 3; i++) {
			sprintf(tsts, "ESR_Path_E%d", i + 1);
			if (!strcmp(name, tsts)) {
				FMCB.p_ESR_Path[i] = value;
				continue;
			}
		}
		if (!strcmp(name, "pad_delay")) {
			FMCB.pad_delay = atoi(value);
			continue;
		}
		if (!strcmp(name, "hacked_OSDSYS")) {
			OSDSYS.hack_enabled = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_scroll_menu")) {
			OSDSYS.scroll_menu = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_x")) {
			OSDSYS.menu_x = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_y")) {
			OSDSYS.menu_y = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_enter_x")) {
			OSDSYS.enter_x = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_enter_y")) {
			OSDSYS.enter_y = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_version_x")) {
			OSDSYS.version_x = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_version_y")) {
			OSDSYS.version_y = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_cursor_max_velocity")) {
			OSDSYS.cursor_max_velocity = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_cursor_acceleration")) {
			OSDSYS.cursor_acceleration = atoi(value);
			continue;
		}  ///CNF READER
		if (!strcmp(name, "OSDSYS_left_cursor")) {
			OSDSYS.left_cursor = value;
			continue;
		}
		if (!strcmp(name, "OSDSYS_right_cursor")) {
			OSDSYS.right_cursor = value;
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_top_delimiter")) {
			OSDSYS.menu_top_delimiter = value;
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_bottom_delimiter")) {
			OSDSYS.menu_bottom_delimiter = value;
			continue;
		}
		if (!strcmp(name, "OSDSYS_num_displayed_items")) {
			OSDSYS.num_displayed_items = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_Disc")) {
			OSDSYS.skip_disc = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_Logo")) {
			OSDSYS.skip_logo = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Inner_Browser")) {
			OSDSYS.goto_inner_browser = atoi(value);
			continue;
		}
		if (!strcmp(name, "FastBoot")) {
			FMCB.fastboot = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_MC")) {
			OSDSYS.skip_mc = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_HDD")) {
			OSDSYS.skip_hdd = atoi(value);
			continue;
		}
		if (!strcmp(name, "Debug_Screen")) {
			FMCB.debug_screen = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_selected_color")) {
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					hexvalue_buf[j] = value[j];
				}
				OSDSYS.selected_color[i] = strtol(hexvalue_buf, NULL, 16);
				value += 5;
			}
			continue;
		}
		if (!strcmp(name, "OSDSYS_unselected_color")) {
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					hexvalue_buf[j] = value[j];
				}
				OSDSYS.unselected_color[i] = strtol(hexvalue_buf, NULL, 16);
				value += 5;
			}
			continue;
		}
		for (i = 0; i < NEWITEMS; i++) {
			sprintf(tsts, "name_OSDSYS_ITEM_%d", i + 1);
			if (!strcmp(name, tsts)) {
				OSDSYS.item_name[i] = value;
				item_cnt++;
				break;
			}
		}
		for (i = 0; i < NEWITEMS; i++) {
			for (j = 0; j < 3; j++) {
				sprintf(tsts, "path%1d_OSDSYS_ITEM_%d", j + 1, i + 1);
				if (!strcmp(name, tsts)) {
					OSDSYS.item_path[i][j] = value;
					break;
				}
			}
		}
		for (i = 0; i < 17; i++) {
			for (j = 0; j < 3; j++) {
				sprintf(tsts, "LK_%s_E%d", LK_ID[i], j + 1);
				if (!strcmp(name, tsts)) {
					FMCB.p_LK_Path[i][j] = value;
					break;
				}
			}
		}
	}  // ends for

	// We don't want to release CNF_RAM_p now as we have pointers to it.
	//free(CNF_RAM_p);

	return 1;
}

//----------------------------------------------------------------
int Read_SYSTEM_CNF(char *boot_path, char *ver)
{
	// Returns disc type : 0 = failed; 1 = PS1; 2 = PS2;
	int var_cnt;
	size_t CNF_size;
	unsigned char *RAM_p, *CNF_p, *name, *value;
	int fd = -1;
	int Disc_Type = -1;  // -1 = Internal : Not Tested;

	//place 3 question mark in ver string
	strcpy(ver, "???");

	fd = open("cdrom0:\\SYSTEM.CNF;1", O_RDONLY);
	if (fd < 0) {
	failed_load:
		if (Disc_Type == -1) {
			// Test PS1 special cases
			if (file_exists("cdrom0:\\PSXMYST\\MYST.CCS;1")) {
				strcpy(boot_path, "SLPS_000.24");
				Disc_Type = 1;
			} else if (file_exists("cdrom0:\\CDROM\\LASTPHOT\\ALL_C.NBN;1")) {
				strcpy(boot_path, "SLPS_000.65");
				Disc_Type = 1;
			} else if (file_exists("cdrom0:\\PSX.EXE;1")) {
				//place 3 question mark in pathname
				strcpy(boot_path, "???");
				Disc_Type = 1;
			}
		}
		if (Disc_Type == -1)
			Disc_Type = 0;
		return Disc_Type;
	}  // This point is only reached after succefully opening CNF

	Disc_Type = 0;

	CNF_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	RAM_p = (char *)malloc(CNF_size);
	CNF_p = RAM_p;
	if (CNF_p == NULL) {
		close(fd);
		goto failed_load;
	}
	read(fd, CNF_p, CNF_size);  // Read CNF as one long string
	close(fd);
	CNF_p[CNF_size] = '\0';  // Terminate the CNF string

	strcpy(boot_path, "???");  //place 3 question mark in boot path

	for (var_cnt = 0; get_CNF_string(&CNF_p, &name, &value); var_cnt++) {
		// A variable was found, now we dispose of its value.
		if (!strcmp(name, "BOOT2")) {  // check for BOOT2 entry
			strcpy(boot_path, value);
			Disc_Type = 2;  // If found, PS2 disc type
			break;
		}
		if (!strcmp(name, "BOOT")) {  // check for BOOT entry
			strcpy(boot_path, value);
			Disc_Type = 1;  // If found, PS1 disc type
			continue;
		}
		if (!strcmp(name, "VER")) {  // check for VER entry
			strcpy(ver, value);
			continue;
		}
	}  // ends for
	free(RAM_p);
	return Disc_Type;
}


//________________ Neme's OSDSYS loading & hacking code ______________________

#define OSD_MAGIC 0x39390000  // arbitrary number to identify added menu items

// Define the new items here.
// Then define their behaviour in the handle_menu_selection() function.
static char *menuitems[NEWITEMS];
//static int menuitem_index[NEWITEMS];
static char *menuitem_path[NEWITEMS];

static u32 osdmenu[4 + NEWITEMS * 2];

struct osd_menu_info
{
	u32 unknown1;
	u32 *menu_ptr;
	u32 num_entries;
	u32 unknown2;
	u32 curr_selection;
};

static struct osd_menu_info *menuinfo = NULL;

static u32 execps2_code[] = {
    0x24030007,  // li v1, 7
    0x0000000c,  // syscall
    0x03e00008,  // jr ra
    0x00000000   // nop
};
static u32 execps2_mask[] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff};

static u32 pattern1[] = {
    0x00000001,  // unknown
    0x00000000,  // pointer to osdmenu
    0x00000002,  // number of entries
    0x00000003,  // unknown
    0x00000000   // current selection
};
static u32 pattern1_mask[] = {
    0xffffffff,
    0xff800000,
    0xffffffff,
    0xffffffff,
    0xffffffff};

static u32 pattern2[] = {
    // Search pattern in the osd string function:
    0x10000005,  //     beq	zero, zero, L2
    0x8c620000,  //     lw	v0, $xxxx(v1)
    0x00101880,  // L1: sll	v1, s0, 2	# string index * 4
    0x8c440000,  //     lw	a0, $xxxx(v0)	# osd string pointer array
    0x00641821,  //     addu	v1, v1, a0	# byte offset into array
    0x8c620000,  //     lw	v0, $0000(v1)	# pointer to string
    0xdfbf0010   // L2: ld	ra, $0010(sp)
};
static u32 pattern2_mask[] = {
    0xffffffff,
    0xffff0000,
    0xffffffff,
    0xffff0000,
    0xffffffff,
    0xffffffff,
    0xffffffff};

static u32 pattern3[] = {
    // Search pattern in the user input handling function:
    0x1000000e,  //     beq	zero, zero, exit
    0xdfbf0010,  //     ld	ra, $0010(sp)
    0x24040001,  // L1: li	a0, 1		# the 2nd menu item (sys conf)
    0x8c430000,  //     lw	v1, $xxxx(v0)	# current selection
    0x1464000a,  //     bne	v1, a0, exit	# sys config not selected?
    0xdfbf0010,  //     ld	ra, $0010(sp)
    0x0c000000,  //     jal	StartSysConfig
    0x00000000,  //     nop
    0x10000006,  //     beq	zero, zero, exit
    0xdfbf0010   //     ld	ra, $0010(sp)
};
static u32 pattern3_mask[] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffff0000,
    0xffffffff,
    0xffffffff,
    0xfc000000,
    0xffffffff,
    0xffffffff,
    0xffffffff};


//--------------------------------------------------------------
u8 *find_bytes_with_mask(u8 *buf, u32 bufsize, u8 *bytes, u8 *mask, u32 len)
{
	u32 i, j;

	for (i = 0; i < bufsize - len; i++) {
		for (j = 0; j < len; j++) {
			if ((buf[i + j] & mask[j]) != bytes[j])
				break;
		}
		if (j == len)
			return &buf[i];
	}
	return NULL;
}
//--------------------------------------------------------------
u8 *find_string(const u8 *string, u8 *buf, u32 bufsize)
{
	u32 i;
	const u8 *s, *p;

	for (i = 0; i < bufsize; i++) {
		s = string;
		for (p = buf + i; *s && *s == *p; s++, p++)
			;
		if (!*s)
			return (buf + i);
	}
	return NULL;
}
//--------------------------------------------------------------
const char *get_string_pointer(const char **strings, u32 index)
{
	if ((index & 0xffff0000) == OSD_MAGIC)
		return menuitems[index & 0xffff];

	return strings[index];
}

//--------------------------------------------------------------
int handle_menu_selection(int selected)
{

	if (selected == 1)
		return 1;

	//if (selected >= 2 + NEWITEMS)
	if (selected >= 2 + item_cnt)
		return 0;

	if ((selected - 2) >= 0) {

		call_from_osdsys = 1;
		loading_print = 0;
		load_elf(menuitem_path[selected - 2]);
	}
	return 0;
}
//--------------------------------------------------------------
void patch_menu(u8 *osd)
{
	u8 *ptr;
	u32 tmp, menu_addr, p2_addr, p3_addr, i;

	//------------------------------------------------------------------------
	// Search for all patterns and return if one of them not found
	//------------------------------------------------------------------------
	for (tmp = 0; tmp < 0x100000; tmp = (u32)(ptr - osd + 4)) {
		ptr = find_bytes_with_mask(osd + tmp, 0x100000 - tmp, (u8 *)pattern1, (u8 *)pattern1_mask, sizeof(pattern1));
		if (!ptr)
			return;
		if (_lw((u32)ptr + 4) == (u32)ptr - 4 * 4)
			break;
	}
	menu_addr = (u32)ptr;

	menuinfo = (struct osd_menu_info *)menu_addr;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern2, (u8 *)pattern2_mask, sizeof(pattern2));
	if (!ptr)
		return;
	p2_addr = (u32)ptr;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern3, (u8 *)pattern3_mask, sizeof(pattern3));
	if (!ptr)
		return;
	p3_addr = (u32)ptr;


	//------------------------------------------------------------------------
	// Patch the osd string function
	//------------------------------------------------------------------------
	tmp = 0x0c000000;
	tmp |= ((u32)get_string_pointer >> 2);
	_sw(0x0200282d, p2_addr + 2 * 4);  // daddu  a1, s0, zero
	                                   // lw     a0, $xxxx(v0)
	_sw(tmp, p2_addr + 4 * 4);         // jal    get_string_pointer
	_sw(0x00000000, p2_addr + 5 * 4);  // nop


	//------------------------------------------------------------------------
	// Patch the user input handling function
	//------------------------------------------------------------------------
	tmp = 0x0c000000;
	tmp |= ((u32)handle_menu_selection >> 2);
	_sw(tmp, p3_addr + 2 * 4);  // jal    handle_menu_selection
	tmp = _lw(p3_addr + 3 * 4) & 0xffff;
	tmp |= 0x8c440000;
	_sw(tmp, p3_addr + 3 * 4);         // lw     a0, $xxxx(v0)
	_sw(0x1040000a, p3_addr + 4 * 4);  // beq    v0, zero, exit


	//------------------------------------------------------------------------
	// Build the osd menu
	//------------------------------------------------------------------------
	osdmenu[0] = _lw(menu_addr - 4 * 4);  // "Browser"
	osdmenu[1] = _lw(menu_addr - 3 * 4);
	osdmenu[2] = _lw(menu_addr - 2 * 4);  // "System Configuration"
	osdmenu[3] = _lw(menu_addr - 1 * 4);

	//for (i = 0; i < NEWITEMS; i++) {
	for (i = 0; i < item_cnt; i++) {
		osdmenu[4 + i * 2] = OSD_MAGIC + i;
		osdmenu[5 + i * 2] = 0;
	}

	menuinfo->menu_ptr = osdmenu;          // store menu pointer
	menuinfo->num_entries = 2 + item_cnt;  // store number of menu items
	                                       //menuinfo->num_entries = 2 + NEWITEMS;
}


//=========================================================================
// The draw menu item hack :-)
//
// You can change menu items' color and position for selected and unselected
// items separately in the following two functions:
//
// draw_menu_item_selected()
// draw_menu_item_unselected()
//
// Be careful what you write in these functions as they get called every
// frame for every menu item!  For positioning the menu, update both
// functions with the same calculations, using the X/Y variables.
//
// Default values of the variables in V12 OSDSYS:
// X = 430 (this is the center of the menu)
// Y = 110 (this is the Y of the first item)
// alpha = 128 (smaller value is more transparency)
//=========================================================================

static u32 pattern4[] = {
    // Search pattern in the drawmenu function:
    0x001010c0,  //     sll	v0, s0, 3	# selection multiplied by 8 (offset into menu)
    0x00431021,  //     addu	v0, v0, v1	# pointer to string index
    0x0c000000,  //     jal	GetOSDString
    0x8c440000,  //     lw	a0, $0000(v0)	# get string index
    0x0040402d,  //     daddu	t0, v0, zero	# arg4: pointer to string
    0x240401ae,  //     li	a0, $01ae	# arg0: X coord = 430
    0x0220282d,  //     daddu	a1, s1, zero	# arg1: Y coord
    0x26000000,  //     addiu	a2, ??, $xxxx	# arg2: ptr to color components
    0x0c000000,  //     jal	DrawMenuItem
    0x0260382d   //     daddu	a3, s3, zero	# arg3: alpha
};
static u32 pattern4_mask[] = {
    0xffffffff,
    0xffffffff,
    0xfc000000,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xff000000,
    0xfc000000,
    0xffffffff};

static u32 selected_color[4] __attribute__((aligned(16)));
static u32 unselected_color[4] __attribute__((aligned(16)));

static void (*DrawMenuItem)(int X, int Y, u32 *color, int alpha, const char *string);
//-----------------------------------------------------------------------------------

static int dx = 0;
static int vel, acc;
static int offsY = 0;
static int font_height = 16;


void draw_menu_item_selected(int X, int Y, u32 *color, int alpha, const char *string, int num)
{
	int i;

	for (i = 0; i < 4; i++)
		selected_color[i] = OSDSYS.selected_color[i];

	if (!OSDSYS.scroll_menu) {  //Old style menu
		(*DrawMenuItem)(OSDSYS.menu_x, Y - item_cnt * 10, selected_color, alpha, string);
	} else {  //New style menu
		if (num == 0) {
			int amount;
			if (offsY < 0) {
				amount = -offsY >> 2;
				offsY += (amount > 0 ? amount : 1);
			} else if (offsY > 0) {
				amount = offsY >> 2;
				offsY -= (amount > 0 ? amount : 1);
			}
		}
		Y = (num << 1) - offsY;
		if ((Y < ((OSDSYS.num_displayed_items + 1) * (font_height / 2))) && (Y > -((OSDSYS.num_displayed_items + 1) * (font_height / 2)))) {
			vel -= acc;
			if (vel < -OSDSYS.cursor_max_velocity || vel > OSDSYS.cursor_max_velocity)
				acc = -acc;
			dx += vel;
			(*DrawMenuItem)(OSDSYS.menu_x, OSDSYS.menu_y + Y, selected_color, alpha, string);
			(*DrawMenuItem)(OSDSYS.menu_x - 220 + (dx >> 8), OSDSYS.menu_y + Y, selected_color, alpha, OSDSYS.left_cursor);
			(*DrawMenuItem)(OSDSYS.menu_x + 220 - (dx >> 8), OSDSYS.menu_y + Y, selected_color, alpha, OSDSYS.right_cursor);
		}
		(*DrawMenuItem)(OSDSYS.menu_x, OSDSYS.menu_y - ((OSDSYS.num_displayed_items + 1) * (font_height / 2)), selected_color, 128, OSDSYS.menu_top_delimiter);
		(*DrawMenuItem)(OSDSYS.menu_x, OSDSYS.menu_y + ((OSDSYS.num_displayed_items + 1) * (font_height / 2)), selected_color, 128, OSDSYS.menu_bottom_delimiter);
	}
}

//-----------------------------------------------------------------------------------
void draw_menu_item_unselected(int X, int Y, u32 *color, int alpha, const char *string, int num)
{
	int i;

	for (i = 0; i < 4; i++)
		unselected_color[i] = OSDSYS.unselected_color[i];

	if (!OSDSYS.scroll_menu) {  //Old style menu
		(*DrawMenuItem)(OSDSYS.menu_x, Y - item_cnt * 10, unselected_color, alpha, string);
	} else {  //New style menu
		if (num == 0) {
			int amount, destY = menuinfo->curr_selection << 4;
			if (offsY < destY) {
				amount = (destY - offsY) >> 2;
				offsY += (amount > 0 ? amount : 1);
			} else if (offsY > destY) {
				amount = (offsY - destY) >> 2;
				offsY -= (amount > 0 ? amount : 1);
			}
		}
		Y = (num << 1) - offsY;
		if ((Y < ((OSDSYS.num_displayed_items + 1) * (font_height / 2))) && (Y > -((OSDSYS.num_displayed_items + 1) * (font_height / 2)))) {
			if (Y < 0)
				alpha = 128 + (Y * (128 / ((OSDSYS.num_displayed_items + 1) * (font_height / 2))));
			else
				alpha = 128 - (Y * (128 / ((OSDSYS.num_displayed_items + 1) * (font_height / 2))));
			if (alpha < 0)
				alpha = 0;

			(*DrawMenuItem)(OSDSYS.menu_x, OSDSYS.menu_y + Y, unselected_color, alpha, string);
		}
	}
}

//-----------------------------------------------------------------------------------
void patch_draw_menu(u8 *osd)
{
	u8 *ptr;
	u32 tmp, addr1, addr2;

	vel = OSDSYS.cursor_max_velocity;
	acc = OSDSYS.cursor_acceleration;

	OSDSYS.num_displayed_items |= 1;  //must be odd value
	if (OSDSYS.num_displayed_items < 1)
		OSDSYS.num_displayed_items = 1;
	if (OSDSYS.num_displayed_items > 15)
		OSDSYS.num_displayed_items = 15;

	if (!menuinfo)
		return;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern4, (u8 *)pattern4_mask, sizeof(pattern4));
	if (!ptr)
		return;
	addr1 = (u32)ptr;  // code for selected menu item

	ptr = find_bytes_with_mask(ptr + 4, 256, (u8 *)pattern4, (u8 *)pattern4_mask, sizeof(pattern4));
	if (ptr != (u8 *)(addr1 + 48))
		return;
	addr2 = (u32)ptr;  // code for unselected menu item

	tmp = _lw(addr1 + 32);  // get the OSD's DrawMenuItem function pointer
	tmp &= 0x03ffffff;
	tmp <<= 2;
	DrawMenuItem = (void *)tmp;

	tmp = 0x0c000000;
	tmp |= ((u32)draw_menu_item_selected >> 2);
	_sw(tmp, addr1 + 32);  // overwrite the function call for selected item

	tmp = 0x0c000000;
	tmp |= ((u32)draw_menu_item_unselected >> 2);
	_sw(tmp, addr2 + 32);  // overwrite the function call for unselected item

	_sw(0x001048c0, addr1);      // make menu item's number the sixth param
	_sw(0x01231021, addr1 + 4);  // by loading it into t1 (multiplied by 8):
	_sw(0x001048c0, addr2);      // sll   t1, s0, 3
	_sw(0x01231021, addr2 + 4);  // addu  v0, t1, v1
}


//=========================================================================
// The DrawButtonPanel Hack :
//
// draw_icon_left() is called for all button icons less the last
// draw_icon_right() is called for only for last button icon (Options or Version)
// draw_nonselectable_item_left() is called for all items less the last
// draw_nonselectable_item_right() is called only for last item (Options or Version)
//
// get_buttons_panel_type() is called prior to functions above and determine panel type :
// 	- Main menu type : 1
// 	- Sys conf screen type : 8
//
// OSDSYS defaults are
// 	- Enter icon X : 188
// 	- Enter icon Y : 230 on PAL
// 	- Enter text X : 216
// 	- Enter text Y : 230 on PAL
// 	- Version icon X : 501
// 	- Version icon Y : 230 on PAL
// 	- Version text X : 529
// 	- Version text Y : 230 on PAL
//
//=========================================================================

static u32 pattern4_1[] = {
    // Search for draw_button_panel function start:
    0x00c0002d,  //     daddu XX, a2, zero
    0xff000000,  //     sd	 XX, 0x00XX(sp)
    0x0080802d,  //     daddu s0, a0, zero
    0xff000000,  //     sd	 XX, 0x00XX(sp)
    0xff000000,  //     sd	 XX, 0x00XX(sp)
    0xffb70000,  //     sd	 s7, 0x00XX(sp)
    0xffb60000,  //     sd	 s6, 0x00XX(sp)
    0xff000000,  //     sd	 XX, 0x00XX(sp)
    0x0c000000,  //     jal 	 unknown
    0xff000000   //     sd	 XX, 0x00XX(sp)
};

static u32 pattern4_1_mask[] = {
    0xffff00ff,
    0xff00ff00,
    0xffffffff,
    0xff00ff00,
    0xff00ff00,
    0xffffff00,
    0xffffff00,
    0xff00ff00,
    0xfc000000,
    0xff00ff00};

static u32 pattern4_2[] = {
    // Search pattern in the draw_button_panel function:
    0x3c020000,  //     lui 	 v0, 0x00XX
    0x0200302d,  //     daddu a2, XX, zero    # arg2 : Y
    0x24420000,  //     addiu v0, v0, 0xXXXX
    0x00b12823,  //     subu  a1, a1, s1
    0x8c44000c,  //     lw 	 a0, 0x000c(v0)  # arg0 : Icon type
    0x02a0382d,  //     daddu a3, s5, zero    # arg3 : alpha
    0x0c000000,  //     jal 	 DrawIcon
    0x24a5ffe4   //     addiu a1, a1, 0xffe4  # arg1 : X
};

static u32 pattern4_2_mask[] = {
    0xffffff00,
    0xff00ffff,
    0xffff0000,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xfc000000,
    0xffffffff};

static u32 pattern4_3[] = {
    // Search pattern in the draw_button_panel function:
    0x0040402d,  //     daddu t0, v0, zero 	 # arg4 : pointer to string
    0x0200202d,  //     daddu a0, s0, zero 	 # arg0 : X
    0x3c020000,  //     lui	 v0, 0x00XX
    0x0200282d,  //     daddu a1, XX, zero 	 # arg1 : Y
    0x24460000,  //     addiu a2, v0, 0xXXXX  # arg2 : pointer to color struct
    0x0c000000,  //     jal 	 DrawNonSelectableItem
    0x02a0382d   //     daddu a3, s5, zero 	 # arg3 : alpha
};

static u32 pattern4_3_mask[] = {
    0xffffffff,
    0xffffffff,
    0xffffff00,
    0xff00ffff,
    0xffff0000,
    0xfc000000,
    0xffffffff};

static void (*DrawNonSelectableItem)(int X, int Y, u32 *color, int alpha, const char *string);
static void (*DrawIcon)(int type, int X, int Y, int alpha);
static void (*DrawButtonPanel_1stfunc)(void);

int ButtonsPanel_Type = 0;

#define MAINMENU_PANEL 1
#define SYSCONF_PANEL 8

//-----------------------------------------------------------------------------------

void get_buttons_panel_type(int type)
{
	// Get Buttons Panel type (catch it in a0)
	ButtonsPanel_Type = type;

	// Call original function that was overwritted
	(*DrawButtonPanel_1stfunc)();
}

void draw_nonselectable_item_left(int X, int Y, u32 *color, int alpha, const char *string)
{
	if (ButtonsPanel_Type == MAINMENU_PANEL) {
		if (OSDSYS.enter_x == -1)
			OSDSYS.enter_x = X - 28;

		if (OSDSYS.enter_y == -1)
			OSDSYS.enter_y = Y;

		(*DrawNonSelectableItem)(OSDSYS.enter_x + 28, OSDSYS.enter_y, color, alpha, string);
	} else
		(*DrawNonSelectableItem)(X, Y, color, alpha, string);
}

void draw_nonselectable_item_right(int X, int Y, u32 *color, int alpha, const char *string)
{
	if (ButtonsPanel_Type == MAINMENU_PANEL) {
		if (OSDSYS.version_x == -1)
			OSDSYS.version_x = X - 28;

		if (OSDSYS.version_y == -1)
			OSDSYS.version_y = Y;

		(*DrawNonSelectableItem)(OSDSYS.version_x + 28, OSDSYS.version_y, color, alpha, string);
	} else
		(*DrawNonSelectableItem)(X, Y, color, alpha, string);
}

void draw_icon_left(int type, int X, int Y, int alpha)
{
	if (ButtonsPanel_Type == MAINMENU_PANEL) {
		if (OSDSYS.enter_x == -1)
			OSDSYS.enter_x = X;

		if (OSDSYS.enter_y == -1)
			OSDSYS.enter_y = Y;

		(*DrawIcon)(type, OSDSYS.enter_x, OSDSYS.enter_y, alpha);
	} else
		(*DrawIcon)(type, X, Y, alpha);
}

void draw_icon_right(int type, int X, int Y, int alpha)
{
	if (ButtonsPanel_Type == MAINMENU_PANEL) {
		if (OSDSYS.version_x == -1)
			OSDSYS.version_x = X;

		if (OSDSYS.version_y == -1)
			OSDSYS.version_y = Y;

		(*DrawIcon)(type, OSDSYS.version_x, OSDSYS.version_y, alpha);
	} else
		(*DrawIcon)(type, X, Y, alpha);
}

//-----------------------------------------------------------------------------------

void patch_button_panel(u8 *osd)
{
	u8 *ptr;
	u8 *first_ptr;
	u32 tmp;
	u32 addr, addr1, addr2, addr3, addr4, addr5;
	u32 pattern[1];
	u32 mask[1];

	// Search and overwrite 1st function call in DrawButtonPanel function
	first_ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern4_1, (u8 *)pattern4_1_mask, sizeof(pattern4_1));
	if (!first_ptr)
		return;
	addr1 = (u32)first_ptr;

	tmp = _lw(addr1 + 32);
	tmp &= 0x03ffffff;
	tmp <<= 2;
	DrawButtonPanel_1stfunc = (void *)tmp;  // get original function call

	tmp = 0x0c000000;
	tmp |= ((u32)get_buttons_panel_type >> 2);
	_sw(tmp, addr1 + 32);  // overwrite the function call

	// Search and overwrite 1st DrawIcon function call in DrawButtonPanel function
	ptr = find_bytes_with_mask(first_ptr, 0x1000, (u8 *)pattern4_2, (u8 *)pattern4_2_mask, sizeof(pattern4_2));
	if (!ptr)
		return;
	addr2 = (u32)ptr;  // code for bottom right icon

	tmp = _lw(addr2 + 24);
	addr = tmp;  // save function call code
	tmp &= 0x03ffffff;
	tmp <<= 2;
	DrawIcon = (void *)tmp;

	tmp = 0x0c000000;
	tmp |= ((u32)draw_icon_right >> 2);
	_sw(tmp, addr2 + 24);  // overwrite the function call for bottom right icon

	// Make pattern with function call code saved above
	pattern[0] = addr;
	mask[0] = 0xffffffff;

	// Search and overwrite 2nd DrawIcon function call in DrawButtonPanel function
	ptr = find_bytes_with_mask(ptr + 28, 0x1000, (u8 *)pattern, (u8 *)mask, sizeof(pattern));
	if (!ptr)
		return;
	addr3 = (u32)ptr;  // code for bottom left icons

	tmp = 0x0c000000;
	tmp |= ((u32)draw_icon_left >> 2);
	_sw(tmp, addr3);  // overwrite the function call for bottom left icons

	// Search and overwrite 1st DrawNonSelectableItem function call in DrawButtonPanel function
	ptr = find_bytes_with_mask(first_ptr, 0x1000, (u8 *)pattern4_3, (u8 *)pattern4_3_mask, sizeof(pattern4_3));
	if (!ptr)
		return;
	addr4 = (u32)ptr;  // code for bottom right item

	tmp = _lw(addr4 + 20);  // get the OSD's DrawNonSelectableItem function pointer
	addr = tmp;             // save function call code
	tmp &= 0x03ffffff;
	tmp <<= 2;
	DrawNonSelectableItem = (void *)tmp;

	tmp = 0x0c000000;
	tmp |= ((u32)draw_nonselectable_item_right >> 2);
	_sw(tmp, addr4 + 20);  // overwrite the function call for bottom right item

	// Make pattern with function call code saved above
	pattern[0] = addr;
	mask[0] = 0xffffffff;

	// Search and overwrite 2nd DrawNonSelectableItem function call in DrawButtonPanel function
	ptr = find_bytes_with_mask(ptr + 24, 0x1000, (u8 *)pattern, (u8 *)mask, sizeof(pattern));
	if (!ptr)
		return;
	addr5 = (u32)ptr;  // code for bottom left item

	tmp = 0x0c000000;
	tmp |= ((u32)draw_nonselectable_item_left >> 2);
	_sw(tmp, addr5);  // overwrite the function call for bottom left item
}



//=========================================================================
//
// Disc Launch patches
//
// patch_auto_launch function patch original disc launch functions.
// patch_skip_disc function is for disc skip under OSDSYS main menu
//
//=========================================================================

static u32 pattern5[] = {
    // ExecuteDisc function
    0x27bdfff0,  //    addiu	sp, sp, $fff0
    0x3c03001f,  //    lui	v1, $001f
    0xffbf0000,  //    sd	ra, $0000(sp)
    0x0080302d,  //    daddu	a2, a0, zero
    0x8c620014,  //    lw	v0, $0014(v1)
    0x2c420007,  //    sltiu	v0, v0, 7
    0x10400000,  //    beq	v0, zero, quit
    0x00000000,  //    li	v0, 1		# or ld ra,(sp)
    0x3c02001f,  //    lui	v0, $001f
    0x8c420014,  //    lw	v0, $0014(v0)
    0x3c030000,  //    lui	v1, xxxx
    0x24630000   //    addiu	v1, v1, xxxx
};

static u32 pattern5_mask[] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffff00,
    0x00000000,
    0xffffffff,
    0xffffffff,
    0xffffff00,
    0xffff0000};

static u32 pattern5_2[] = {
    // ExecuteDisc function on early jap
    0x27bdfff0,  //    addiu	sp, sp, $fff0
    0x8c430000,  //    lw	v1, $XXXX(v0)
    0xffbf0000,  //    sd	ra, $0000(sp)
    0x8c640000,  //    lw	a0, $XXXX(v1)
    0x24830002,  //    addiu	v1, a0, 2
    0x2c620006,  //    sltiu	v0, v1, 6
    0x10400000,  //    beq	v0, zero, quit
    0x3c020000,  //    lui	v0, xxxx
    0x00031880,  //	  sll	v1, v1, 2
    0x24420000   //    addiu	v0, v0, xxxx
};

static u32 pattern5_2_mask[] = {
    0xffffffff,
    0xffff0000,
    0xffffffff,
    0xffff0000,
    0xffffffff,
    0xffffffff,
    0xffff0000,
    0xffff0000,
    0xffffffff,
    0xffff0000};

static u32 pattern6[] = {
    // Code around main menu disc detection part1
    0xac220cec,  //    sw	v0, $0cec(at)
    0x0c000000,  //    jal	xxxx
    0x00000000,  //    nop
    0x0c000000,  //    jal	xxxx
    0x00000000,  //    nop
    0x3c02001f,  //    lui	v0, $001f
    0x26440000,  //    addiu	a0, s2, $xxxx
    0x8c430c44,  //    lw	v1, $0c44(v0)
    0x0c000000,  //    jal	xxxx
    0xac830008,  //    sw	v1, $0008(a0)
    0x10000000,  //    beq	zero, zero, xxxx
    0x26420000   //    addiu	v0, s2, $xxxx
};

static u32 pattern6_mask[] = {
    0xffffffff,
    0xfc000000,
    0xffffffff,
    0xfc000000,
    0xffffffff,
    0xffffffff,
    0xffff0000,
    0xffffffff,
    0xfc000000,
    0xffffffff,
    0xffff0000,
    0xffff0000};

static u32 pattern7[] = {
    // Code around main menu disc detection part2
    0x3c02001f,  //    lui	v0, $001f
    0x24030003,  //    li	v1, 3
    0xac4305e8,  //    sw	v1, $05e8(v0)
    0x24040002,  //    li	a0, 2
    0xac4405ec,  //    sw	a0, $05ec(v0)
    0x3c10001f,  //    lui	s0, $001f
    0x24020001,  //    li	v0, 1
    0xae0205e4,  //    sw	v0, $05e4(s0)
    0x0c000000   //    jal	WaitVblankStart?
};

static u32 pattern7_mask[] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xfc000000};

// An array that stores what function to call for each disc type.
// It looks something like this:
// u32 *exec_disc_func_array[7] = {
//    exec_ps2_game_disc,		// ps2 game dvd
//    exec_ps2_game_disc,		// ps2 game cd
//    exec_ps1_game_disc,		// ps1 game cd
//    exec_dvdv_disc,			// dvd video
//    do_nothing,				// none (return 1)
//    do_nothing,				// none (return 1)
//    exec_hdd_stuff			// hddload
//}
//
// On early jap it looks like this:
// u32 *exec_disc_func_array[6] = {
//    reboot,					// perform LoadExecPS2(NULL, 0, NULL)
//    do_nothing,				// none (return 1)
//    exec_ps2_game_disc,		// ps2 game dvd
//    exec_ps2_game_disc,		// ps2 game cd
//    exec_ps1_game_disc,		// ps1 game cd
//    exec_dvdv_disc			// dvd video
//}

static u32 *exec_disc_func_array = NULL;

//--------------------------------------------------------------
// Here we redefine dvdv launch behaviour
int deviated_exec_dvdv(void)
{
	call_from_osdsys = 1;
	load_elf("DVDV_CHECK");

	return 0;
}

//--------------------------------------------------------------
// Here we redefine ps2 dvd launch behaviour
int deviated_exec_ps2dvd(void)
{
	char *args[4];
	char ver[MAX_PATH];

	if (Read_SYSTEM_CNF(cdboot_path, ver) == 2) {
		OSDSYS_CleanUp();  // CleanUp needed before launching game
		if (CNF_RAM_p != NULL)
			free(CNF_RAM_p);
		args[0] = cdboot_path;
		LoadExecPS2("rom0:PS2LOGO", 1, args);  // Launch PS2 Game with rom0:PS2LOGO
	}

	// second try, for failing case when PS2DVD was inserted before ps2 startup on v9, v10
	call_from_osdsys = 1;
	load_elf("PS2DVD");

	return 0;
}

//--------------------------------------------------------------
void patch_auto_launch(u8 *osd)
{
	u8 *ptr;
	u32 tmp, addr1;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern5, (u8 *)pattern5_mask, sizeof(pattern5));
	if (ptr) {

		addr1 = (u32)ptr;  // address of the ExecuteDisc function

		tmp = _lw(addr1 + 40) << 16;
		tmp += (signed short)(_lw(addr1 + 44) & 0xffff);
		exec_disc_func_array = (u32 *)tmp;

		exec_disc_func_array[3] = (u32)deviated_exec_dvdv;    // overwrite dvdv function pointer
		exec_disc_func_array[0] = (u32)deviated_exec_ps2dvd;  // overwrite ps2 dvd function pointer
	} else {
		ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern5_2, (u8 *)pattern5_2_mask, sizeof(pattern5_2));
		if (!ptr)
			return;

		addr1 = (u32)ptr;  // address of the ExecuteDisc function on early jap

		tmp = _lw(addr1 + 28) << 16;
		tmp += (signed short)(_lw(addr1 + 36) & 0xffff);
		exec_disc_func_array = (u32 *)tmp;

		exec_disc_func_array[5] = (u32)deviated_exec_dvdv;    // overwrite early jap dvdv function pointer
		exec_disc_func_array[2] = (u32)deviated_exec_ps2dvd;  // overwrite early jap ps2dvd function pointer
	}
}

//--------------------------------------------------------------
void patch_skip_disc(u8 *osd)
{
	u8 *ptr;
	u32 tmp, addr2, addr3, dist;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern6, (u8 *)pattern6_mask, sizeof(pattern6));
	if (!ptr)
		return;
	addr2 = (u32)ptr;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern7, (u8 *)pattern7_mask, sizeof(pattern7));
	if (!ptr)
		return;
	addr3 = (u32)ptr;

	tmp = addr2 + 48;  // patch start
	dist = ((addr3 - tmp) >> 2) - 1;
	if (dist > 0x40)
		return;

	_sw(0x10000000 + dist, tmp);  // branch to addr3
	_sw(0, tmp + 4);              // nop
}

//=========================================================================
//  Loop menu Patch

static u32 pattern8[] = {
    0x30621000,
    0x10400007,
    0x2604ffe8,
    0x8c830010,
    0x2462ffff,
    0x0441000e,
    0xac820010};

static u32 pattern8_mask[] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff};

static u32 menu_loop_patch1[7] = {
    0x8e04fff8,
    0x8e05fff0,
    0x0004102a,
    0x0082280b,
    0x20a3ffff,
    0x1000000c,
    0xae03fff8};

static u32 menu_loop_patch2[9] = {
    0x1040000e,
    0x30620020,
    0x8e05fff8,
    0x8e02fff0,
    0x24a30001,
    0x0062102a,
    0x0002180a,
    0x00000000,
    0xae03fff8,
};

//-----------------------------------------------------------------------------------
void patch_loop_menu(u8 *osd)
{
	int i;
	u8 *ptr;
	u32 *addr, *src, *dst;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern8, (u8 *)pattern8_mask, sizeof(pattern8));
	if (!ptr)
		return;
	addr = (u32 *)ptr;

	if (addr[9] == 0x30624000 && addr[20] == 0x24045200) {
		src = menu_loop_patch1;
		dst = addr + 2;
		for (i = 0; i < 7; i++)
			*(dst++) = *(src++);

		src = menu_loop_patch2;
		dst = addr + 11;
		for (i = 0; i < 9; i++)
			*(dst++) = *(src++);

		addr[10] = addr[-1];  // reload normal pad variable
		addr[-1] += 8;        // get key repeat variable
	}
}

//=========================================================================
//  Force video mode Patch

static u32 pattern9[] = {
    0xffbf0000,
    0x0c000000,
    0x00000000,
    0x38420002,
    0xdfbf0000,
    0x2c420001};
static u32 pattern9_mask[] = {
    0xffffffff,
    0xfc000000,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff};

//--------------------------------------------------------------
void patch_force_video_mode(u8 *osd)
{
	u8 *ptr;

	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern9, (u8 *)pattern9_mask, sizeof(pattern9));
	if (!ptr)
		return;

	if (!strcmp(OSDSYS.video_mode, "NTSC"))      // NTSC:
		_sw(0x0000102d, (u32)ptr + 20);          // set return value to 0
	else if (!strcmp(OSDSYS.video_mode, "PAL"))  // PAL:
		_sw(0x24020001, (u32)ptr + 20);          // set return value to 1
}

//=========================================================================
//  SkipHdd patch for v3, v4 (those not supporting "SkipHdd" arg)

static u32 pattern10[] = {
    // Code near MC Update & HDD load
    0x0c000000,  // jal 	 CheckMcUpdate
    0x0220282d,  // daddu a1, s1, zero
    0x3c04002a,  // lui	 a0, 0x002a         #SkipHdd jump must be here
    0x0000282d,  // daddu a1, zero, zero 	#arg1: 0
    0x24840000,  // addiu a0, a0, 0xXXXX  	#arg0: "rom0:ATAD"
    0x0c000000,  // jal 	 LoadModule
    0x0000302d,  // daduu a2, zero, zero		#arg2: 0
    0x04400000   // bltz  v0, Exit_HddLoad
};
static u32 pattern10_mask[] = {
    0xfc000000,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffff0000,
    0xfc000000,
    0xffffffff,
    0xffff0000};

//--------------------------------------------------------------
void patch_skip_hdd(u8 *osd)
{
	u8 *ptr;
	u32 addr;

	// Search code near MC Update & HDD load
	ptr = find_bytes_with_mask(osd, 0x00100000, (u8 *)pattern10, (u8 *)pattern10_mask, sizeof(pattern10));
	if (!ptr)
		return;
	addr = (u32)ptr;

	// Place "beq zero, zero, Exit_HddLoad" just after CheckMcUpdate() call
	_sw(0x10000000 + ((signed short)(_lw(addr + 28) & 0xffff) + 5), addr + 8);
}

//--------------------------------------------------------------
void patch_and_execute_osdsys(void *epc, void *gp)
{
	// Apply all OSDSYS patches.

	int n = 0;
	char *args[5], *ptr;

	args[n++] = "rom0:";

	if (OSDSYS.hack_enabled) {
		// If Hacked OSDSYS ON then apply menu patch
		patch_menu((u8 *)epc);
		patch_draw_menu((u8 *)epc);
		patch_loop_menu((u8 *)epc);
		patch_button_panel((u8 *)epc);

		// Try video-mode patch only if different from AUTO
		if (strcmp(OSDSYS.video_mode, "AUTO"))
			patch_force_video_mode((u8 *)epc);

		// Skip disc patch applied if OSDSYS_skip_disc is ON
		if (OSDSYS.skip_disc)
			patch_skip_disc((u8 *)epc);

		if (_lw(0x202d78) == 0x0c080898 &&
		    _lw(0x202b40) == 0x0c080934 &&
		    _lw(0x20ffa0) == 0x0c080934) {
			_sw(0x00000000, 0x202d78);
			_sw(0x24020000, 0x202b40);
			_sw(0x24020000, 0x20ffa0);
		}

		if (OSDSYS.goto_inner_browser)
			args[n++] = "BootBrowser";  // triggers BootBrowser to reach internal mc browser
		else if ((OSDSYS.skip_disc) || (OSDSYS.skip_logo))
			args[n++] = "BootClock";  // triggers BootClock arg to skip OSDSYS intro

		//if (OSDSYS.skip_mc)
		if (find_string("SkipMc", (u8 *)epc, 0x100000))  // triggers SkipMc arg
			args[n++] = "SkipMc";                        // Skip mc?:/BREXEC-SYSTEM/osdxxx.elf update on v5 and above

		if (OSDSYS.skip_hdd) {
			if (find_string("SkipHdd", (u8 *)epc, 0x100000))  // triggers SkipHdd arg
				args[n++] = "SkipHdd";                        // Skip Hddload on v5 and above
			else
				patch_skip_hdd((u8 *)epc);  // SkipHdd patch for v3 & v4
		}
	}

	if (FMCB.autolaunch_patch)
		patch_auto_launch((u8 *)epc);  // Apply auto launch patch to catch ESR disc & failing PS2 dvd game launch on some PS2

	// To avoid loop in OSDSYS (Handle those models not supporting SkipMc arg) :
	while ((ptr = find_string("EXEC-SYSTEM", (u8 *)epc, 0x100000)))
		strncpy(ptr, "EXEC-OSDSYS", 11);

	FlushCache(0);
	FlushCache(2);

	ExecPS2(epc, gp, n, args);
}

//--------------------------------------------------------------
void IOP_Reset(void)
{
	// resets IOP and update with EELOADCNF

	while (!SifIopReset("rom0:UDNL rom0:EELOADCNF", 0))
		;
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
}

//--------------------------------------------------------------
void load_modules(void)
{
	// loads FMCB needed modules

	int ret;

	// Apply loadmodulebuffer and prefix check patch
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();

	SifLoadModule("rom0:SIO2MAN", 0, 0);
	SifLoadModule("rom0:MCMAN", 0, 0);
	SifLoadModule("rom0:MCSERV", 0, 0);
	SifLoadModule("rom0:PADMAN", 0, 0);

	SifExecModuleBuffer(&iomanx_irx, size_iomanx_irx, 0, NULL, &ret);

	///try to load usb modules from...
	/* **************************USBD.IRX****************************************** */
	if (SifLoadModule(usbd_irx_path, 0, NULL) < 0) /**mc0:/BOOT/USBD.IRX*/
	{
		usbd_irx_path[2]++;
		if (SifLoadModule(usbd_irx_path, 0, NULL) < 0) /**mc1:/BOOT/USBD.IRX*/
		{
			if (SifLoadModule(usbd_irx_path_sysconf, 0, NULL) < 0) /**mc0:/SYS-CONF/USBD.IRX*/
			{
				usbd_irx_path_sysconf[2]++;
				SifLoadModule(usbd_irx_path_sysconf, 0, NULL); /**mc1:/SYS-CONF/USBD.IRX*/
			}
		}
	}
	/* **************************USBHDFSD.IRX************************************** */
	if (SifLoadModule(usb_mass_irx_path, 0, NULL) < 0) /**mc0:/BOOT/USBHDFSD.IRX*/
	{
		usb_mass_irx_path[2]++;
		if (SifLoadModule(usb_mass_irx_path, 0, NULL) < 0) /**mc1:/BOOT/USBHDFSD.IRX*/
		{
			if (SifLoadModule(usb_mass_irx_path_sysconf, 0, NULL) < 0) /**mc0:/SYS-CONF/USBHDFSD.IRX*/
			{
				usb_mass_irx_path_sysconf[2]++;
				SifLoadModule(usb_mass_irx_path_sysconf, 0, NULL); /**mc1:/SYS-CONF/USBHDFSD.IRX*/
			}
		}
	}
}
//--------------------------------------------------------------
void load_chkesr_module(void)
{
	// load chkesr and other modules (EROMDRV) needed to read DVDV

	int ret;
	int i;
	char eromdrvpath[MAX_PATH];

	SifExecModuleBuffer(&chkesr_irx, size_chkesr_irx, 0, NULL, &ret);  // ESR Disc checker
	chkesr_rpc_Init();

	SifLoadModule("rom0:ADDDRV", 0, NULL);

	// Needed by chkesr module : EROMDRV must be loaded to access DVDV.
	if ((ret = SifLoadModuleEncrypted("rom1:EROMDRV", 0, NULL)) < 0) {  // first try for Fat PS2 EROMDRV
		for (i = 0; i < 8; i++) {                                       // 8 tries for slims PS2 EROMDRVx, to find the valid MG region
			sprintf(eromdrvpath, "%s%c", "rom1:EROMDRV", MG_region[i][0]);
			ret = SifLoadModuleEncrypted(eromdrvpath, 0, NULL);
			if (ret >= 0) {
				old_dvdelf = 0;
				MG_REGION[0] = MG_region[i][0];
				break;
			}
		}
	}
}
//--------------------------------------------------------------
void CleanUp(int iop_reset)
{
	// de-Init cdvd, pads and Timer
	if (iop_reset) {
		if (!isEarlyJap) {
			cdInit(CDVD_INIT_EXIT);
			cdvdrpc_inited = 0;
		}
	}

	if (pad_inited) {
		// Early jap doesn't support those pad de-inits
		if (!isEarlyJap) {
			padPortClose(0, 0);
			padPortClose(1, 0);
			padEnd();
			padReset();
			pad_inited = 0;
		}
	}

	if (timer_inited) {
		TimerEnd();
		timer_inited = 0;
	}

	if (iop_reset) {
		IOP_Reset();

		SifLoadFileInit();
		load_modules();
	}

	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();

	FlushCache(0);
	FlushCache(2);
}

//--------------------------------------------------------------
void OSDSYS_CleanUp(void)
{
	SifInitRpc(0);

	DisableIntc(3);
	DisableIntc(2);

	SifLoadFileInit();
	SifLoadModule("rom0:CLEARSPU", 0, 0);

	IOP_Reset();

	SifLoadFileInit();
	load_modules();

	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();

	FlushCache(0);
	FlushCache(2);
}

//--------------------------------------------------------------
void launch_osdsys(void)  // Run OSDSYS
{
	u8 *ptr;
	t_ExecData exec;
	int i, j, r;

	if (OSDSYS.hack_enabled) {
		r = 0;
		for (i = 0; i < NEWITEMS; i++) {                                                  // Ckeck in all OSDSYS item array
			for (j = 0; j < 3; j++) {                                                     // Ckeck all 3 possible path
				if ((OSDSYS.item_path[i][j] != NULL) && (OSDSYS.item_name[i] != NULL)) {  // If name and path not null and empty
					if ((strlen(OSDSYS.item_path[i][j]) > 0) && (strlen(OSDSYS.item_name[i]) > 0)) {
						p_ExecPath = OSDSYS.item_path[i][j];
						//
						if (!strncmp(p_ExecPath + 5, "B?DATA-SYSTEM", 13))
							memset(p_ExecPath + 6, romver_region_char[0], 1);  // Check path start with mc?:/B?DATA-SYSTEM
						if (!strncmp(p_ExecPath, "mc?:", 4))                   // Check if path start with mc?: in this case search for the file in 2 slots.
						{
							memset(p_ExecPath + 2, 0x30, 1);  // mc?: -> mc0:
							if (!file_exists(p_ExecPath))
								memset(p_ExecPath + 2, 0x31, 1);  // mc0: -> mc1:
						}

						if ((!strcmp(p_ExecPath, "OSDSYS")) || (!strcmp(p_ExecPath, "FASTBOOT"))) {
							// if path is set to OSDSYS or FASTBOOT,
							menuitems[r] = OSDSYS.item_name[i];         // copy name to osdsys item name
							menuitem_path[r] = OSDSYS.item_path[i][j];  // Copy index to an array
							r++;
							break;

						} else {  // If path isn't set to OSDSYS, check paths
							if (!file_exists(p_ExecPath))
								continue;  // If files doesn't exists, check next path
							else {
								menuitems[r] = OSDSYS.item_name[i];         // File found, copy name to osdsys item name
								menuitem_path[r] = OSDSYS.item_path[i][j];  // Copy file path
								r++;
								break;
							}
						}
					}
				}
			}
		}
		item_cnt = r;
	}

	SifLoadElf("rom0:OSDSYS", &exec);

	if (exec.epc > 0) {
		// If it loaded to 0x200000 it's probably not packed (old osdsys like jap v1).
		// In this case just patch and execute it.
		if ((exec.epc & 0xfff00000) == 0x00200000) {
			CleanUp(1);
			patch_and_execute_osdsys((void *)exec.epc, (void *)exec.gp);
		}

		// Find the ExecPS2 function in the unpacker starting from 0x100000.
		ptr = find_bytes_with_mask((u8 *)0x100000, 0x1000, (u8 *)execps2_code, (u8 *)execps2_mask, sizeof(execps2_code));

		// If found replace it with a call to our patch_and_execute_osdsys() function.
		if (ptr) {
			u32 instr = 0x0c000000;
			instr |= ((u32)patch_and_execute_osdsys >> 2);
			*(u32 *)ptr = instr;
			*(u32 *)&ptr[4] = 0;
		}

		CleanUp(1);

		// Execute the osd unpacker. If the above patching was successful it will
		// call the patch_and_execute_osdsys() function after unpacking.
		ExecPS2((void *)exec.epc, (void *)exec.gp, 0, NULL);
		while (1) {
			;
		}
	}
}

//--------------------------------------------------------------

void check_path(void)  // check if a path contains FASTBOOT, OSDSYS, B?DATA, or mc?
{
	if (!strcmp(p_ExecPath, "FASTBOOT"))  // if path is set to FASTBOOT then
		if (FastBoot_Disc() >= 0)         // try to Boot the disc
			while (1) {
				;
			}  // If disc was booted...
	if (!strcmp(p_ExecPath, "OSDSYS"))
		launch_osdsys();                   // if path is set to OSDSYS then launch browser
	if (!strcmp(p_ExecPath, "OSDMENU")) {  // if path is set to OSDMENU then skip disc boot
		OSDSYS.skip_disc = 1;
		launch_osdsys();
	}
	if (!strncmp(p_ExecPath + 5, "B?DATA-SYSTEM", 13))
		memset(p_ExecPath + 6, romver_region_char[0], 1);  // Check path start with mc?:/B?DATA-SYSTEM
	if (!strncmp(p_ExecPath, "mc?:", 4))                   // Check if path start with mc?: in this case search for the file in 2 slots.
	{
		memset(p_ExecPath + 2, 0x30, 1);  // mc?: -> mc0:
		if (!file_exists(p_ExecPath))
			memset(p_ExecPath + 2, 0x31, 1);  // mc0: -> mc1:
	}
}

//--------------------------------------------------------------

void reload_osdsys(void)
{
	int i, config_loaded;

	if (CNF_RAM_p != NULL)
		free(CNF_RAM_p);

	// Reset to defaults
	Set_Default_Settings();

	// Read config before to check args for an elf to load
	config_loaded = loadConfig();

	// Adjust some vars if cnf is readed
	if (config_loaded) {
		for (i = 0; i < 3; i++)
			strcpy(esr_path[i], FMCB.p_ESR_Path[i]);  // copy 3 ESR paths from CNF
	}

	// setting ESR valid path
	check_ESR_paths();

	launch_osdsys();
}

//--------------------------------------------------------------
// ELF-header structures and identifiers
#define ELF_MAGIC 0x464c457f
#define ELF_PT_LOAD 1

//--------------------------------------------------------------
typedef struct
{
	u8 ident[16];
	u16 type;
	u16 machine;
	u32 version;
	u32 entry;
	u32 phoff;
	u32 shoff;
	u32 flags;
	u16 ehsize;
	u16 phentsize;
	u16 phnum;
	u16 shentsize;
	u16 shnum;
	u16 shstrndx;
} elf_header_t;
//--------------------------------------------------------------
typedef struct
{
	u32 type;
	u32 offset;
	void *vaddr;
	u32 paddr;
	u32 filesz;
	u32 memsz;
	u32 flags;
	u32 align;
} elf_pheader_t;
//--------------------------------------------------------------

void load_elf(char *elf_path)
{
	u8 *boot_elf;
	elf_header_t *boot_header;
	elf_pheader_t *boot_pheader;
	int i;
	char *args[6];
	char arg[1024];
	char elfpath[1024];
	//int n = 0;
	int ret;
	char eromdrv_arg[128];
	char dvdpl_arg[128];
	char ver[128];
	static int loading_y = 214;
	int dvdpl_update = 0;

	if (call_from_osdsys) {
		// clear the screen
		VMode = NTSC;  // Setting Vmode to NTSC by default, just a security
		if (romver[4] == 'E')
			VMode = PAL;  // Set Video mode
		gs_reset();       // Reset GS
		if (VMode == PAL) {
			gs_init(PAL_640_512_32);
			loading_y = 276;
		}  // set video mode
		else {
			gs_init(NTSC_640_448_32);
			loading_y = 214;
		}
		gs_set_fill_color(0x00, 0x00, 0x00);
		gs_set_fill_color(0x00, 0x00, 0x00);
		gs_fill_rect(0, 0, gs_get_max_x(), gs_get_max_y());
	}

	// Print loading bitmap if needed
	if (loading_print)
		gs_print_bitmap((gs_get_max_x() - loading_w) / 2, loading_y, loading_w, loading_h, loading);  // print centered loading bitmap


	// Perform needed cleanUp
	if (call_from_osdsys)
		OSDSYS_CleanUp();
	else
		CleanUp(0);

	SifInitRpc(0);
	SifLoadFileInit();
	load_chkesr_module();
	SifLoadFileExit();

	if (isEarlyJap) {
		cdInit(CDVD_INIT_INIT);
		cdvdrpc_inited = 1;
	}

	// If here for DVDV check, wait disk is ready
	if (!strcmp(elf_path, "DVDV_CHECK"))
		cdDiskReady(0);

	ret = Check_ESR_Disc();

	// Clear Screen
	gs_set_fill_color(0, 0, 0);
	gs_fill_rect(0, 0, gs_get_max_x(), gs_get_max_y());

	// OSDSYS launch handling
	if (!strcmp(elf_path, "OSDSYS"))
		reload_osdsys();

	// FASTBOOT launch handling
	if (!strcmp(elf_path, "FASTBOOT")) {
		if (FastBoot_Disc() >= 0)  // try to Boot the disc
			while (1) {
				;
			}  // If disc was booted...
		else
			reload_osdsys();
	}

	// PS2DVD launch handling
	if (!strcmp(elf_path, "PS2DVD")) {
		cdDiskReady(0);
		if (CNF_RAM_p != NULL)
			free(CNF_RAM_p);
		Read_SYSTEM_CNF(cdboot_path, ver);
		args[0] = cdboot_path;
		cdInit(CDVD_INIT_EXIT);
		SifExitRpc();                          //some programs need it to be here
		LoadExecPS2("rom0:PS2LOGO", 1, args);  // Launch PS2 Game with rom0:PS2LOGO
	}

	// DVDV launch handling
	if (!strcmp(elf_path, "DVDV_CHECK")) {
		if (ret == 0) {

			// Check for DVDPlayer Update on MC if skip_mc = 0 or Early jap
			if ((!OSDSYS.skip_mc) || (isEarlyJap)) {
				dvdpl_path[6] = romver_region_char[0];  // adjust region
				if (boot_from_mc == 1)
					dvdpl_path[2]++;  // adjust path to mc1:
				if (!file_exists(dvdpl_path)) {
					if (boot_from_mc == 1)
						dvdpl_path[2]--;  // adjust path to mc0:
					else
						dvdpl_path[2]++;  // adjust path to mc1:
					if (file_exists(dvdpl_path)) {
						dvdpl_update = 1;
					}
				} else {
					dvdpl_update = 1;
				}

				if (dvdpl_update) {
					if (CNF_RAM_p != NULL)
						free(CNF_RAM_p);
					// Launch DVD player from memory card
					args[0] = "-m rom0:SIO2MAN";
					args[1] = "-m rom0:MCMAN";
					args[2] = "-m rom0:MCSERV";
					sprintf(arg, "-x %s", dvdpl_path);  // -x :elf is encrypted for mc
					args[3] = arg;
					cdInit(CDVD_INIT_EXIT);
					SifExitRpc();
					LoadExecPS2("moduleload", 4, args);
				}
			}

			if (old_dvdelf) {                 // from v2 to v11
				args[0] = "-k rom1:EROMDRV";  // -k means to decrypt & load encrypted driver
				args[1] = "-m erom0:UDFIO";   // -m means to load driver
				args[2] = "-x erom0:DVDELF";  // -x means to decrypt & execute encrypted elf
			} else {                          // from v12 to v16
				sprintf(eromdrv_arg, "-k rom1:EROMDRV%c", MG_REGION[0]);
				sprintf(dvdpl_arg, "-x erom0:DVDPL%c", MG_REGION[0]);
				args[0] = eromdrv_arg;
				args[1] = "-m erom0:UDFIO";
				args[2] = dvdpl_arg;
			}
			if (CNF_RAM_p != NULL)
				free(CNF_RAM_p);
			cdInit(CDVD_INIT_EXIT);
			SifExitRpc();
			LoadExecPS2("moduleload2 rom1:UDNL rom1:DVDCNF", 3, args);
		} else {
			strcpy(elfpath, valid_ESR_path);
			args[0] = elfpath;
		}

	} else {
		strcpy(elfpath, elf_path);
		args[0] = elfpath;
	}

	// Load & execute embedded loader from here
	boot_elf = (u8 *)&elf_loader;

	// Get Elf header
	boot_header = (elf_header_t *)boot_elf;

	// Check elf magic
	if ((*(u32 *)boot_header->ident) != ELF_MAGIC)
		return;

	// Get program headers
	boot_pheader = (elf_pheader_t *)(boot_elf + boot_header->phoff);

	// Scan through the ELF's program headers and copy them into apropriate RAM
	// section, then padd with zeros if needed.
	for (i = 0; i < boot_header->phnum; i++) {

		if (boot_pheader[i].type != ELF_PT_LOAD)
			continue;

		memcpy(boot_pheader[i].vaddr, boot_elf + boot_pheader[i].offset, boot_pheader[i].filesz);

		if (boot_pheader[i].memsz > boot_pheader[i].filesz)
			memset(boot_pheader[i].vaddr + boot_pheader[i].filesz, 0, boot_pheader[i].memsz - boot_pheader[i].filesz);
	}

	if (CNF_RAM_p != NULL)
		free(CNF_RAM_p);
	cdInit(CDVD_INIT_EXIT);
	SifExitRpc();

	// Execute Elf Loader
	ExecPS2((void *)boot_header->entry, 0, 1, args);
}

//--------------------------------------------------------------
void check_ESR_paths(void)
{
	// This function checks for a valid ESR path

	int j;

	p_ExecPath = NULL;
	for (j = 0; j < 3; j++) {  // Try all 3 possible ESR paths
		if (!p_ExecPath)
			p_ExecPath = esr_path[j];
		if (p_ExecPath != NULL) {
			check_path();
			if (!file_exists(p_ExecPath))
				p_ExecPath = NULL;
			else {
				valid_ESR_path = p_ExecPath;
				return;
			}
		}
	}
	if (!p_ExecPath)
		valid_ESR_path = NULL;  // If no valid ESR path found
}

//--------------------------------------------------------------
int FastBoot_Disc(void)
{
	// Boot an identified disc in the tray
	CdvdDiscType_t cdmode;
	char *args[3];
	char ver[MAX_PATH];
	int i;

	i = 0x20000;
	while (i--)
		asm("nop\nnop\nnop\nnop");  // small delay for cdStatus

	if (cdStatus() == CDVD_STAT_OPEN)
		return -1;  // If tray is open, no fastboot, return

	while (cdGetDiscType() == CDVD_TYPE_DETECT) {
		;
	}  // Trick : if tray is open before
	if (cdGetDiscType() == CDVD_TYPE_NODISK)
		return -1;  // startup it detects it as closed...

	cdDiskReady(0);
	cdmode = cdGetDiscType();  // If tray is closed, get disk type

	if (cdmode == CDVD_TYPE_NODISK) {
		return -1;
	} else if ((cdmode == CDVD_TYPE_PS1CD) || (cdmode == CDVD_TYPE_PS1CDDA)) {
		if (Read_SYSTEM_CNF(cdboot_path, ver) == 1) {  // double check PS1 disc type & read CNF
			args[0] = cdboot_path;                     // 1st arg is main elf path
			args[1] = ver;                             // 2nd arg is ver
			CleanUp(0);                                // Cleanup needed
			if (CNF_RAM_p != NULL)
				free(CNF_RAM_p);
			LoadExecPS2("rom0:PS1DRV", 2, args);  // Run PS1 game
		} else
			launch_osdsys();  // launch OSDSYS is failed to identify PS1 disc
	} else if ((cdmode == CDVD_TYPE_PS2DVD) || (cdmode == CDVD_TYPE_PS2CD) || (cdmode == CDVD_TYPE_PS2CDDA)) {
		if (Read_SYSTEM_CNF(cdboot_path, ver) == 2) {  // double check PS2 disc type & read CNF
			CleanUp(0);                                // Cleanup needed
			if (CNF_RAM_p != NULL)
				free(CNF_RAM_p);
			LoadExecPS2(cdboot_path, 0, NULL);  // Execute main elf
		} else
			launch_osdsys();  // launch OSDSYS is failed to identify PS2 disc
	} else if (cdmode == CDVD_TYPE_DVDVIDEO) {
		load_elf("DVDV_CHECK");
	} else {
		return -1;
	}

	return 0;
}

//--------------------------------------------------------------
void check_exec_paths(void)
{
	int j;

	// 3 Paths are acquired from pad press or not, check them
	p_ExecPath = NULL;
	for (j = 0; j < 3; j++) {  // Try all 3 possible elf paths
		if (!p_ExecPath)
			p_ExecPath = p_pad_ExecPath[j];
		if (p_ExecPath != NULL) {
			check_path();
			if (!file_exists(p_ExecPath))
				p_ExecPath = NULL;
			else {
				load_elf(p_ExecPath);  // if elf is valid then excecute it
			}
		}
	}
	if (!p_ExecPath) {  // If no valid elf found
		if (!(FMCB.debug_screen))
			launch_osdsys();  //If Debug_Screen is set to 0 and no boot file found then launch browser
		SETBG(0, 0xff, 0);    // File not found, Green screen
		while (1) {
			;
		}
	}
}

//--------------------------------------------------------------
void Set_Default_Settings(void)
{
	// Loads Needed OSDSYS and FMCB defaults
	int i, j;

	OSDSYS.hack_enabled = 0;
	OSDSYS.skip_mc = 0;
	OSDSYS.skip_hdd = 0;
	OSDSYS.skip_disc = 0;
	OSDSYS.skip_logo = 1;
	OSDSYS.goto_inner_browser = 0;
	OSDSYS.video_mode = "AUTO";
	OSDSYS.scroll_menu = 1;
	OSDSYS.menu_x = 320;
	OSDSYS.menu_y = 110;
	OSDSYS.enter_x = 30;
	OSDSYS.enter_y = -1;
	OSDSYS.version_x = -1;
	OSDSYS.version_y = -1;
	OSDSYS.cursor_max_velocity = 1000;
	OSDSYS.cursor_acceleration = 100;
	OSDSYS.left_cursor = ">>";
	OSDSYS.right_cursor = "<<";
	OSDSYS.menu_top_delimiter = "------=/\\=------";
	OSDSYS.menu_bottom_delimiter = "------=\\/=------";
	OSDSYS.selected_color[0] = 0x10;
	OSDSYS.selected_color[1] = 0x80;
	OSDSYS.selected_color[2] = 0xe0;
	OSDSYS.selected_color[3] = 0x80;
	OSDSYS.unselected_color[0] = 0x33;
	OSDSYS.unselected_color[1] = 0x33;
	OSDSYS.unselected_color[2] = 0x33;
	OSDSYS.unselected_color[3] = 0x80;
	OSDSYS.num_displayed_items = 7;
	for (i = 0; i < NEWITEMS; i++) {
		OSDSYS.item_name[i] = NULL;
		for (j = 0; j < 3; j++)
			OSDSYS.item_path[i][j] = NULL;
	}

	FMCB.pad_delay = 1000;
	FMCB.debug_screen = 0;
	FMCB.fastboot = 0;
	FMCB.autolaunch_patch = 1;
	for (i = 0; i < 17; i++)
		for (j = 0; j < 3; j++)
			FMCB.p_LK_Path[i][j] = NULL;
	for (i = 0; i < 3; i++)
		FMCB.p_ESR_Path[i] = NULL;
}

//--------------------------------------------------------------
void SetOsdConfig(void)
{
	// Restore osd settings with values readed from init.irx
	// Must be called after init.irx is loaded

	u32 OSDconfig_c1, OSDconfig_c2, OSDconfig_c3, OSDconfig_c4;
	u32 OSD_config = 0;
	u32 OSD_config2 = 0;
	u32 region;
	u8 romver_region = 0;

	// Get config values from init.irx
	OSDconfig_c1 = *(vu32 *)0x20100000;
	OSDconfig_c2 = *(vu32 *)0x20100004;
	OSDconfig_c3 = *(vu32 *)0x20100008;
	OSDconfig_c4 = *(vu32 *)0x2010000c;
	bios_version = *(vu32 *)0x20100010;
	romver_region = (u8)(*(vu32 *)0x20100014);

	region = (romver_region == 'E' ? 2 :  // Getting region char
	              (romver_region == 'J' ? 0 :
	                                      (romver_region == 'H' ? 1 :
	                                                              (romver_region == 'U' ? 1 : 1))));  //Default NTSC U

	// scph-10000, scph-15000 are early jap: bios 100 and 101
	// thanks to l_oliveira for sharing knowledge about this ;)
	if ((romver_region == 'J') && (bios_version <= 120))
		isEarlyJap = 1;

	// Determine SetOsdConfigParam value from NVRAM values returned by init.irx
	OSD_config |= OSDconfig_c1 & 1;                // spdifMode, digital Out: Enabled(0), Disabled(1)
	OSD_config |= ((OSDconfig_c1 >> 1) & 3) << 1;  // screenType: 4/3(0), fullscreen(1), 16/9(2)
	OSD_config |= ((OSDconfig_c1 >> 3) & 1) << 3;  // videoOutput: SCART(0), Component(1)
	OSD_config |= ((OSDconfig_c1 >> 4) & 1) << 4;  // japLanguage: Japanese(0), non-Japanese(1)
	OSD_config |= region << 13;                    // region : 2 is PAL, 1 seems to be NTSC-U, 0 seems to be NTSC-J
	if (isEarlyJap) {
		OSD_config |= (*(vu32 *)(0x001c9ba4)) << 5;  // unknown scph-10000 & 15000 read it at 0x001c9ba4
		OSD_config |= (OSDconfig_c2 & 0x1) << 4;     // language early jap: language = (config17 & 0x1) << 4;
		OSD_config |= 0x21C << 21;                   // timezoneOffset, early jap: return 0x21C
	} else {
		OSD_config |= (*(vu32 *)(0x001f1224)) << 5;                          // unknown: From v3 to v18 read it at 0x1f1224
		OSD_config |= (OSDconfig_c2 & 0x1F) << 16;                           // language: 0 to 7
		OSD_config |= (((OSDconfig_c3 << 8) | OSDconfig_c4) & 0x7FF) << 21;  // timezoneOffset
	}

	// If failed to retrieve values from init.irx
	// Default to English, PAL, spdifMode disabled, fullscreen, SCART, non-Japanese, GMT+1
	if ((!bios_version) && (!romver_region))
		OSD_config = 0x07814233;

	// Set config
	SetOsdConfigParam(&OSD_config);

	// Determine SetOsdConfigParam2 value from NVRAM values returned by init.irx
	if (!isEarlyJap) {                                                  // Skip SetOsdConfigParam2 for early jap
		OSD_config2 |= ((((OSDconfig_c3 ^ 0x80) << 1) >> 4) & 1) << 4;  // daylightSaving
		OSD_config2 |= ((((OSDconfig_c3 ^ 0x80) << 1) >> 5) & 1) << 5;  // timeFormat
		OSD_config2 |= ((((OSDconfig_c3 ^ 0x80) << 1) >> 6) & 3) << 6;  // dateFormat
		// Set config2
		SetOsdConfigParam2(&OSD_config2, 1, 1);
	}
}

//--------------------------------------------------------------
void FMCB_loader_Init(void)
{
	int fdn = 0;
	int splash_y = 185;
	int ret;

	VMode = NTSC;  // Setting Vmode to NTSC by default, just a security

	// Set FMCB & OSDSYS default settings for configureable items
	Set_Default_Settings();

	gs_reset();  // Reset GS

	if ((fdn = open("rom0:ROMVER", O_RDONLY)) > 0) {  // Reading ROMVER
		read(fdn, romver, sizeof romver);
		close(fdn);
	}

	// Getting region char
	romver_region_char[0] = (romver[4] == 'E' ? 'E' :
	                                            (romver[4] == 'J' ? 'I' :
	                                                                (romver[4] == 'H' ? 'A' :
	                                                                                    (romver[4] == 'U' ? 'A' : romver[4]))));

	if (romver[4] == 'E')
		VMode = PAL;  // Set Video mode

	// Init GS with the good vmode
	if (VMode == PAL) {
		gs_init(PAL_640_512_32);
		splash_y = 247;
	} else {
		gs_init(NTSC_640_448_32);
		splash_y = 0;
	}

	// clear the screen
	gs_set_fill_color(0x00, 0x00, 0x0);
	gs_fill_rect(0, 0, gs_get_max_x(), gs_get_max_y());

	// print bitmap
	gs_print_bitmap((gs_get_max_x() - splash_w) / 2, (gs_get_max_y() - splash_h) / 2, splash_w, splash_h, splash);  // print centered splash bitmap

	IOP_Reset();
	IOP_Reset();  //twice, some in-hdloader hack

	// Load needed modules
	SifLoadFileInit();
	load_modules();
	SifExecModuleBuffer(&init_irx, size_init_irx, 0, NULL, &ret);  // CDVD init fix
	SifLoadFileExit();

	// Restore osd settings with values readed from init.irx
	SetOsdConfig();
}

//--------------------------------------------------------------
// MAIN FUNC
//--------------------------------------------------------------

int main(int argc, char *argv[])
{
	int i, j;
	u64 WaitTime;
	static int pad_press = 0;
	static int config_loaded = 0;
	int button;
	static int pad_button = 0x0100;  // first pad button is L2
	static int num_buttons = 4;      // buttons to check;

	// Perform Init
	wipeUserMem();  //clearing mem, so better not to have anything valuable on stack
	SifInitRpc(0);
	FMCB_loader_Init();

	// Test here for rescue boot files if there are no additional args
	p_ExecPath = rescue_path;
	if (file_exists(p_ExecPath)) {
		load_elf(p_ExecPath);
	}

	// Determine from which mc slot FMCB was booted
	if (!strncmp(argv[0], "mc0", 3))
		boot_from_mc = 0;
	else if (!strncmp(argv[0], "mc1", 3))
		boot_from_mc = 1;

	// Read config before to check args for an elf to load
	config_loaded = loadConfig();

	// Adjust some vars if cnf is readed
	if (config_loaded) {
		for (i = 0; i < 3; i++)
			strcpy(esr_path[i], FMCB.p_ESR_Path[i]);  // copy 3 ESR paths from CNF
		pad_button = 0x0001;                          // first pad button is Select
		num_buttons = 16;                             // buttons to check
	} else {                                          // if no CNF found, fills p_LK_Path array with default boot paths
		for (i = 0; i < 5; i++)
			for (j = 0; j < 3; j++)
				FMCB.p_LK_Path[i][j] = default_path[3 * i + j];
	}

	// setting Auto paths
	for (i = 0; i < 3; i++)
		p_pad_ExecPath[i] = FMCB.p_LK_Path[0][i];

	// setting ESR valid path
	check_ESR_paths();

	// Init Pads and Timer
	setupPad();
	waitAnyPadReady();
	pad_inited = 1;

	TimerInit();
	timer_inited = 1;


	// 1st Loop that wait for pad press until fastboot_delay is reached
	if (bios_version >= 160)
		fastboot_delay = 1000;  // For fixing problem on slim, 1s :(
	WaitTime = Timer();
	while (Timer() <= (WaitTime + fastboot_delay)) {  // Wait fastboot_delay for pad press
		waitAnyPadReady();
		if (readPad()) {
			button = pad_button;
			for (i = 0; i < num_buttons; i++) {  // check all pad buttons
				if (new_pad & button) {
					// if button detected , copy path to corresponding index
					for (j = 0; j < 3; j++)
						p_pad_ExecPath[j] = FMCB.p_LK_Path[i + 1][j];
					check_exec_paths();
					return 0;
				}
				button = button << 1;  // sll of 1 cleared bit to move to next pad button
			}
		}
	}

	// FastBoot delay has been consumed so adjust pad_delay.
	if ((FMCB.pad_delay < 1000) && (bios_version >= 160))
		FMCB.pad_delay = 1000;  // For fixing problem on slim, 1s :(
	FMCB.pad_delay -= fastboot_delay;
	if (FMCB.pad_delay < 0)
		FMCB.pad_delay = 0;

	// Here, no key has been pressed, so if FastBoot option is ON, try FastBoot
	if ((FMCB.fastboot) || (isEarlyJap)) {
		if (FastBoot_Disc() >= 0)  // try to Boot the disc
			while (1) {
				;
			}  // If disc was booted...
	}

	// 2nd Loop that wait for pad press until pad_delay-fastboot_delay is reached
	WaitTime = Timer();
	while ((Timer() <= (WaitTime + FMCB.pad_delay)) && (!pad_press)) {
		waitAnyPadReady();
		if (readPad()) {
			button = pad_button;
			for (i = 0; i < num_buttons; i++) {  // check all pad buttons
				if (new_pad & button) {
					// if button detected, copy path to corresponding index
					for (j = 0; j < 3; j++)
						p_pad_ExecPath[j] = FMCB.p_LK_Path[i + 1][j];
					pad_press = 1;
					break;
				}
				button = button << 1;  // sll of 1 cleared bit to move to next pad button
			}
		}
	}

	// Checking pad press
	check_exec_paths();

	// We don't want to come here
	return 0;
}
