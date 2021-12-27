#ifndef LAUNCHELF_H
#define LAUNCHELF_H

#include <stdio.h>
#include <tamtypes.h>
#include <sifcmd.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <string.h>
#include <malloc.h>
#include <libhdd.h>
#include <libmc.h>
#include <libpad.h>
///#include <fileio.h>
#include <sys/stat.h>
#include <iopheap.h>
#include <errno.h>
//#include <fileXio_rpc.h>
#include <iopcontrol.h>
#include <stdarg.h>
#include <sbv_patches.h>
#include <slib.h>
#include <smem.h>
#include <smod.h>
///#include <sys/fcntl.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <debug.h>
#include <gsKit.h>
#include <dmaKit.h>
//#include <cdvd_rpc.h>
//#include "cd.h"
//#include "mass_rpc.h" //disused in switch to usbhdfsd
#include "iopmod_name.h"
//#include <libjpg.h>
//#include <libkbd.h>
///#include <floatlib.h>
//#include "hdl_rpc.h"

#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <io_common.h>

#define TRUE 1
#define FALSE 0

#define SCANRATE (gsKit_detect_signal() == GS_MODE_NTSC ? 60 : 50)

enum {                // cnfmode values for getFilePath in browsing for configurable file paths
	NON_CNF = 0,      // Normal browser mode, not configuration mode
	LK_ELF_CNF,       // Normal ELF choice for launch keys
	USBD_IRX_CNF,     // USBD.IRX choice for startup
	SKIN_CNF,         // Skin JPG choice
	GUI_SKIN_CNF,     // GUI Skin JPG choice
	USBKBD_IRX_CNF,   // USB keyboard IRX choice (only PS2SDK)
	KBDMAP_FILE_CNF,  // USB keyboard mapping table choice
	CNF_PATH_CNF,     // CNF Path override choice
	TEXT_CNF,         // No restriction choice
	DIR_CNF,          // Directory choice
	JPG_CNF,          // Jpg viewer choice
	USBMASS_IRX_CNF,  // USB_MASS.IRX choice for startup
	LANG_CNF,         // Language file choise
	FONT_CNF,         // Font file choice ( .fnt )
	CNFMODE_CNT       // Total number of cnfmode values defined
};

enum {
	SCREEN_MARGIN = 16,
	FONT_WIDTH = 8,
	FONT_HEIGHT = 16,
	LINE_THICKNESS = 3,

	MAX_NAME = 256,
	MAX_PATH = 1025,
	MAX_ENTRY = 2048,
	MAX_PARTITIONS = 500,
	MAX_MENU_TITLE = 40,
	MAX_ELF_TITLE = 72,
	MAX_TEXT_LINE = 80
};

typedef struct
{
	/*	char CNF_Path[MAX_PATH];
	char LK_Path[15][MAX_PATH];
	char LK_Title[15][MAX_ELF_TITLE];
	int  LK_Flag[15];
	char Misc[64];
	char Misc_PS2Disc[64];
	char Misc_FileBrowser[64];
	char Misc_PS2Browser[64];
	char Misc_PS2Net[64];
	char Misc_PS2PowerOff[64];
	char Misc_HddManager[64];
	char Misc_TextEditor[64];
	char Misc_JpgViewer[64];
	char Misc_Configure[64];
	char Misc_Load_CNFprev[64];
	char Misc_Load_CNFnext[64];
	char Misc_Set_CNF_Path[64];
	char Misc_Load_CNF[64];
	char Misc_ShowFont[64];
	char Misc_Debug_Info[64];
	char usbd_file[MAX_PATH];
	char usbkbd_file[MAX_PATH];
	char usbmass_file[MAX_PATH];
	char kbdmap_file[MAX_PATH];
	char skin[MAX_PATH];
	char GUI_skin[MAX_PATH];
*/
	char Menu_Title[MAX_MENU_TITLE + 1];
	//	char lang_file[MAX_PATH];
	//	char font_file[MAX_PATH];
	int Menu_Frame;
	//	int Show_Menu;
	//	int timeout;
	//	int Hide_Paths;
	u64 color[8];
	int screen_x;
	int screen_y;
	//	int discControl;
	int interlace;
	int resetIOP;
	//	int numCNF;
	int swapKeys;
	/*	int HOSTwrite;
	int Brightness;
	int TV_mode;
	int Popup_Opaque;
	int Init_Delay;
	int usbkbd_used;
	int Show_Titles;
	int PathPad_Lock;
	int JpgView_Timer;
	int JpgView_Trans;
	int JpgView_Full;
	int PSU_HugeNames;
	int PSU_DateNames;
	int PSU_NoOverwrite;
*/
} SETTING;

/*typedef struct
{
	int ip[4];
	int nm[4];
	int gw[4];
} data_ip_struct;
*/
extern char LaunchElfDir[MAX_PATH], LastDir[MAX_NAME];

/* main.c */
extern int TV_mode;
extern int swapKeys;
//extern int GUI_active;// Skin and Main Skin switch
void RunElf(char *fmcbMsg, char *pathin);
int exists(char *path);

//void	load_ps2host(void);
//void loadCdModules(void);
int loadUsbModules(void);
//void loadHddModules(void);
//void loadHdlInfoModule(void);
//int uLE_related(char *pathout, char *pathin);

/* elf.c */
int checkELFheader(char *filename);
void RunLoaderElf(char *filename, char *);

/* draw.c */
#define BACKGROUND_PIC 0
#define PREVIEW_PIC 1
#define JPG_PIC 2
#define THUMB_PIC 3
#define PREVIEW_GUI 4

#define FOLDER 0
#define WARNING 1

//unsigned char icon_folder[1024];
//unsigned char icon_warning[1024];

extern GSGLOBAL *gsGlobal;
extern GSTEXTURE TexSkin, TexPreview, TexPicture, TexThumb[MAX_ENTRY], TexIcon[2];
extern int testskin, testsetskin, testjpg, testthumb;
extern float PicWidth, PicHeight, PicW, PicH, PicCoeff;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int SCREEN_X;
extern int SCREEN_Y;
extern int Menu_start_x;
extern int Menu_title_y;
extern int Menu_message_y;
extern int Frame_start_y;
extern int Menu_start_y;
extern int Menu_end_y;
extern int Frame_end_y;
extern int Menu_tooltip_y;
extern u64 BrightColor;
extern int PicRotate, FullScreen;
extern u8 *FontBuffer;

void setScrTmp(const char *msg0, const char *msg1);
void drawSprite( u64 color, int x1, int y1, int x2, int y2 );
void drawPopSprite(u64 color, int x1, int y1, int x2, int y2);
void drawOpSprite(u64 color, int x1, int y1, int x2, int y2);
void drawMsg(const char *msg);
//void drawLastMsg(void);
void setupGS(int gs_vmode);
//void updateScreenMode(int adapt_XY);
void clrScr(u64 color);
//void setBrightness(int Brightness);
//void loadSkin(int Picture, char *Path, int ThumbNum);
void drawScr(void);
void drawFrame(int x1, int y1, int x2, int y2, u64 color);
void drawChar(unsigned int c, int x, int y, u64 colour);
int printXY(const unsigned char *s, int x, int y, u64 colour, int draw, int space);
//int printXY_sjis(const unsigned char *s, int x, int y, u64 colour, int);
//u8 *transcpy_sjis(u8 *d, u8 *s);
//void loadIcon(void);
int loadFont(char *path_arg);
//Comment out WriteFont_C when not used (also requires patch in draw.c)
//int	WriteFont_C(char *pathname);

/* pad.c */
#define PAD_R3_V0 0x010000
#define PAD_R3_V1 0x020000
#define PAD_R3_H0 0x040000
#define PAD_R3_H1 0x080000
#define PAD_L3_V0 0x100000
#define PAD_L3_V1 0x200000
#define PAD_L3_H0 0x400000
#define PAD_L3_H1 0x800000

extern u32 joy_value;
extern u32 new_pad;
int setupPad(void);
int readpad(void);
//int readpad_no_KB(void);
//int readpad_noRepeat(void);
void waitPadReady(int port, int slot);
void waitAnyPadReady(void);

/* config.c */
#define TV_mode_AUTO 0
#define TV_mode_NTSC 1
#define TV_mode_PAL 2

extern char PathPad[30][MAX_PATH];
extern SETTING *setting;
void initConfig(void);
//int loadConfig(char *, char *);  //0==OK, -1==load failed
//void config(char *, char *);
void Config_fmcb(char *fmcbMsg);
//void initConfig_fmcb(void);
int loadConfig_fmcb(char *fmcbMsg, char *path);
extern char fmcbMsg[MAX_PATH];
extern char cnf_path_fmcb[MAX_PATH];
extern char cnf_path_fmcb2[MAX_PATH];
extern char cnf_path_fmcb3[MAX_PATH];

/* filer.c */
typedef struct
{
	char name[MAX_NAME];
	char title[32 * 2 + 1];
	mcTable stats;
} FILEINFO;

#define MOUNT_LIMIT 2
//extern char mountedParty[MOUNT_LIMIT][MAX_NAME];
//extern int latestMount;
//extern int nparties; //Clearing this causes FileBrowser to refresh party list
extern unsigned char *elisaFnt;
//char *PathPad_menu(const char *path);
void getFilePath(char *out, const int cnfmode);
//void	initHOST(void);
//char *makeHostPath(char *dp, char*sp);
//int ynDialog(const char *message);
//void nonDialog(char *message);
int keyboard(char *out, int max);
void genInit(void);
//void genLimObjName(char *uLE_path, int reserve);
int genFixPath(char *uLE_path, char *gen_path);
int genOpen(char *path, int mode);
int genLseek(int fd, int where, int how);
int genRead(int fd, void *buf, int size);
int genWrite(int fd, void *buf, int size);
int genClose(int fd);
int genDopen(char *path);
int genDclose(int fd);
//int genRemove(char *path);
//int genRmdir(char *path);
//int mountParty(const char *party);
//void unmountParty(int party_ix);
//int setFileList(const char *path, const char *ext, FILEINFO *files, int cnfmode);

/* hdd.c */
//void DebugDisp(char *Message);
//void hddManager(void);

/* editor.c */
//void TextEditor(void);

/* timer.c */
extern u64 WaitTime;

void TimerInit(void);
u64 Timer(void);
void TimerEnd(void);

/* jpgviewer.c */
//void JpgViewer(void);

/* lang.c */
/*typedef struct Language{
	u8* String;
} Language;

enum {
#define lang(id, name, value) LANG_##name,
#include "lang.h"
#undef lang
	LANG_COUNT
};

#define LNG(name) Lang_String[LANG_##name].String
#define LNG_DEF(name) Lang_Default[LANG_##name].String

extern Language Lang_String[];
extern Language Lang_Default[];
extern Language *External_Lang_Buffer;

void Init_Default_Language(void);
void Load_External_Language(void);
*/
/* font_uLE.c */

extern unsigned char font_uLE[];
enum {
	//0x100-0x109 are 5 double width characters for D-Pad buttons, which are accessed as:
	//"�0"==Circle  "�1"==Cross  "�2"==Square  "�3"==Triangle  "�4"==filled Square
	RIGHT_CUR = 0x10A,  //Triangle pointing left, for use to the right of an item
	LEFT_CUR = 0x10B,   //Triangle pointing right, for use to the left of an item
	UP_ARROW = 0x10C,   //Arrow pointing up
	DN_ARROW = 0x10D,   //Arrow pointing up
	LT_ARROW = 0x10E,   //Arrow pointing up
	RT_ARROW = 0x10F,   //Arrow pointing up
	TEXT_CUR = 0x110,   //Vertical bar, for use between two text characters
	UL_ARROW = 0x111,   //Arrow pointing up and to the left, from a vertical start.
	BR_SPLIT = 0x112,   //Splits rectangle from BL to TR with BR portion filled
	BL_SPLIT = 0x113,   //Splits rectangle from TL to BR with BL portion filled
	                    //0x114-0x11B are 4 double width characters for D-Pad buttons, which are accessed as:
	                    //"�:"==Right  "�;"==Down  "�<"==Left  "�="==Up
	FONT_COUNT = 0x11C  //Total number of characters in font
};

/* makeicon.c */
//int make_icon(char* icontext,char* filename);
//int make_iconsys(char* title,char* iconname, char* filename);

#endif
