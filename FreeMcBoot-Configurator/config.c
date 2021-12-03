//---------------------------------------------------------------------------
// File name:   config.c
//---------------------------------------------------------------------------
///LANG TABLE
#define LANG_MENU_TITLE "Free McBoot Configurator 1.3b6 for FunTuna "



#include "launchelf.h"
#include <stdbool.h>
#include <libmc.h>
//#include "libmc-common.h"
enum {
	//	DEF_TIMEOUT = 10,
	//	DEF_HIDE_PATHS = TRUE,
	/**********************DEFAULT COLORS**********************/ /**
   *DEF_COLOR1 = GS_SETREG_RGBA(128,128,128,0), //Backgr 
   *DEF_COLOR2 = GS_SETREG_RGBA(64,64,64,0),    //Frame
   *DEF_COLOR3 = GS_SETREG_RGBA(96,0,0,0),      //Select
   *DEF_COLOR4 = GS_SETREG_RGBA(0,0,0,0),       //Text
   */
	DEF_COLOR1 = GS_SETREG_RGBA(0, 0, 0, 0),                     //Backgr
	DEF_COLOR2 = GS_SETREG_RGBA(160, 160, 160, 0),               //Frame
	DEF_COLOR3 = GS_SETREG_RGBA(255, 0, 0, 0),                   //Select
	DEF_COLOR4 = GS_SETREG_RGBA(255, 255, 255, 0),               //Text
	DEF_COLOR5 = GS_SETREG_RGBA(102, 0, 255, 0),                 //Messages
	                                                             //	DEF_COLOR6 = GS_SETREG_RGBA(0,0,255,0),     //Graph2
	                                                             //	DEF_COLOR7 = GS_SETREG_RGBA(0,0,0,0),       //Graph3
	                                                             //	DEF_COLOR8 = GS_SETREG_RGBA(0,0,0,0),       //Graph4
	                                                             //	DEF_DISCCONTROL = FALSE,
	DEF_INTERLACE = TRUE,
	DEF_MENU_FRAME = TRUE,
	//	DEF_MENU = TRUE,
	DEF_RESETIOP = TRUE,
	//	DEF_NUMCNF = 1,
	DEF_SWAPKEYS = FALSE,
	/*	DEF_HOSTWRITE = FALSE,
	DEF_BRIGHT = 50,
	DEF_POPUP_OPAQUE = FALSE,
	DEF_INIT_DELAY = 0,
	DEF_USBKBD_USED = 1,
	DEF_SHOW_TITLES = 1,
	DEF_PATHPAD_LOCK = 0,
	DEF_JPGVIEW_TIMER = 5,
	DEF_JPGVIEW_TRANS = 2,
	DEF_JPGVIEW_FULL = 0,
	DEF_PSU_HUGENAMES = 0,
	DEF_PSU_DATENAMES = 0,
	DEF_PSU_NOOVERWRITE = 0,
*/
	/*	DEFAULT=0,
	SHOW_TITLES=12,
	DISCCONTROL,
	FILENAME,
	SCREEN,
	SETTINGS,
	NETWORK,
	FMCBOOT,
	OK,
	CANCEL
*/
};

/*char LK_ID[15][10]={
	"auto",
	"Circle",
	"Cross",
	"Square",
	"Triangle",
	"L1",
	"R1",
	"L2",
	"R2",
	"L3",
	"R3",
	"Start",
	"Select",  //Predefined for "CONFIG"
	"Left",    //Predefined for "LOAD CONFIG--"
	"Right"    //Predefined for "LOAD CONFIG++"
};
*/
//char PathPad[30][MAX_PATH];
char tmp[MAX_PATH];
SETTING *setting = NULL;
SETTING *tmpsetting;
//---------------------------------------------------------------------------
// End of declarations
// Start of functions
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
//---------------------------------------------------------------------------
int CheckMC(void)
{
	int dummy, ret;

	mcGetInfo(0, 0, &dummy, &dummy, &dummy);
	mcSync(0, NULL, &ret);

	if (-1 == ret || 0 == ret)
		return 0;

	mcGetInfo(1, 0, &dummy, &dummy, &dummy);
	mcSync(0, NULL, &ret);

	if (-1 == ret || 0 == ret)
		return 1;

	return -11;
}
//---------------------------------------------------------------------------
/*unsigned long hextoul(char *string)
{
	unsigned long value;
	char c;

	value = 0;
	while( !(((c=*string++)<'0')||(c>'F')||((c>'9')&&(c<'A'))) )
		value = value*16 + ((c>'9') ? (c-'A'+10) : (c-'0'));
	return value;
}*/
//---------------------------------------------------------------------------
//storeSkinCNF will save most cosmetic settings to a RAM area
//------------------------------
/*size_t storeSkinCNF(char *cnf_buf)
{
	size_t CNF_size;

	sprintf(cnf_buf,
		"GUI_Col_1_ABGR = %08lX\r\n"
		"GUI_Col_2_ABGR = %08lX\r\n"
		"GUI_Col_3_ABGR = %08lX\r\n"
		"GUI_Col_4_ABGR = %08lX\r\n"
		"GUI_Col_5_ABGR = %08lX\r\n"
		"GUI_Col_6_ABGR = %08lX\r\n"
		"GUI_Col_7_ABGR = %08lX\r\n"
		"GUI_Col_8_ABGR = %08lX\r\n"
		"SKIN_FILE = %s\r\n"
		"GUI_SKIN_FILE = %s\r\n"
		"SKIN_Brightness = %d\r\n"
		"TV_mode = %d\r\n"
		"Screen_Interlace = %d\r\n"
		"Screen_X = %d\r\n"
		"Screen_Y = %d\r\n"
		"Popup_Opaque = %d\r\n"
		"Menu_Frame = %d\r\n"
		"Show_Menu = %d\r\n"
		"%n",           // %n causes NO output, but only a measurement
		setting->color[0],   //Col_1
		setting->color[1],   //Col_2
		setting->color[2],   //Col_3
		setting->color[3],   //Col_4
		setting->color[4],   //Col_5
		setting->color[5],   //Col_6
		setting->color[6],   //Col_7
		setting->color[7],   //Col_8
		setting->skin,       //SKIN_FILE
		setting->GUI_skin,   //GUI_SKIN_FILE
		setting->Brightness, //SKIN_Brightness
		setting->TV_mode,    //TV_mode
		setting->interlace,  //Screen_Interlace
		setting->screen_x,   //Screen_X
		setting->screen_y,   //Screen_Y
		setting->Popup_Opaque, //Popup_Opaque
		setting->Menu_Frame, //Menu_Frame
		setting->Show_Menu,  //Show_Menu
		&CNF_size       // This variable measures the size of sprintf data
  );
  return CNF_size;
}*/
//------------------------------
//endfunc storeSkinCNF
//---------------------------------------------------------------------------
//saveSkinCNF will save most cosmetic settings to a skin CNF file
//------------------------------
/*int saveSkinCNF(char *CNF)
{
	int ret, fd;
	char tmp[26*MAX_PATH + 30*MAX_PATH];
	char cnf_path[MAX_PATH];
	size_t CNF_size;

	CNF_size = storeSkinCNF(tmp);

	ret = genFixPath(CNF, cnf_path);
	if((ret < 0) || ((fd=genOpen(cnf_path,O_CREAT|O_WRONLY|O_TRUNC)) < 0)){
		return -1; //Failed open
	}
	ret = genWrite(fd,&tmp,CNF_size);
	if(ret!=CNF_size)
		ret = -2; //Failed writing
	genClose(fd);

	return ret;
}*/
//-----------------------------
//endfunc saveSkinCNF
//---------------------------------------------------------------------------
//saveSkinBrowser will save most cosmetic settings to browsed skin CNF file
//------------------------------
/*void saveSkinBrowser(void)
{
	int  tst;
	char path[MAX_PATH];
	char mess[MAX_PATH];

	getFilePath(path, DIR_CNF);
	if(path[0] == '\0')
		goto abort;
	if(!strncmp(path, "cdfs", 4))
		goto abort;

	drawMsg(LNG(Enter_File_Name));

	tmp[0]=0;
	if(keyboard(tmp, 36)>0)
		strcat(path, tmp);
	else{
abort:
		tst = -3;
		goto test;
	}

	tst = saveSkinCNF(path);

test:
	switch(tst){
	case -1:
		sprintf(mess, "%s \"%s\".", LNG(Failed_To_Save), path);
		break;
	case -2:
		sprintf(mess, "%s \"%s\".", LNG(Failed_writing), path);
		break;
	case -3:
		sprintf(mess, "%s \"%s\".", LNG(Failed_Saving_File), path);
		break;
	default:
		sprintf(mess, "%s \"%s\".", LNG(Saved), path);
	}
	drawMsg(mess);
}*/
//-----------------------------
//endfunc saveSkinBrowser
//---------------------------------------------------------------------------
//preloadCNF loads an entire CNF file into RAM it allocates
//------------------------------
/*unsigned char *preloadCNF(char *path)
{
	int fd, tst;
	size_t CNF_size;
	char cnf_path[MAX_PATH];
	unsigned char *RAM_p;

	fd=-1;
	if((tst = genFixPath(path, cnf_path)) >= 0)
		fd = genOpen(cnf_path, O_RDONLY);
	if(fd<0){
failed_load:
		return NULL;
	}
	CNF_size = genLseek(fd, 0, SEEK_END);
	printf("CNF_size=%d\n", CNF_size);
	genLseek(fd, 0, SEEK_SET);
	RAM_p = (char*)malloc(CNF_size);
	if	(RAM_p==NULL)	{ genClose(fd); goto failed_load; }
	genRead(fd, RAM_p, CNF_size);  //Read CNF as one long string
	genClose(fd);
	RAM_p[CNF_size] = '\0';        //Terminate the CNF string
	return RAM_p;
}*/
//------------------------------
//endfunc preloadCNF
//---------------------------------------------------------------------------
//scanSkinCNF will check for most cosmetic variables of a CNF
//------------------------------
/*int scanSkinCNF(unsigned char *name, unsigned char *value)
{
	if(!strcmp(name,"GUI_Col_1_ABGR")) setting->color[0] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_2_ABGR")) setting->color[1] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_3_ABGR")) setting->color[2] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_4_ABGR")) setting->color[3] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_5_ABGR")) setting->color[4] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_6_ABGR")) setting->color[5] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_7_ABGR")) setting->color[6] = hextoul(value);
	else if(!strcmp(name,"GUI_Col_8_ABGR")) setting->color[7] = hextoul(value);
	//----------
	else if(!strcmp(name,"SKIN_FILE")) strcpy(setting->skin,value);
	else if(!strcmp(name,"GUI_SKIN_FILE")) strcpy(setting->GUI_skin,value);
	else if(!strcmp(name,"SKIN_Brightness")) setting->Brightness = atoi(value);
	//----------
	else if(!strcmp(name,"TV_mode")) setting->TV_mode = atoi(value);
	else if(!strcmp(name,"Screen_Interlace")) setting->interlace = atoi(value);
	else if(!strcmp(name,"Screen_X")) setting->screen_x = atoi(value);
	else if(!strcmp(name,"Screen_Y")) setting->screen_y = atoi(value);
	//----------
	else if(!strcmp(name,"Popup_Opaque")) setting->Popup_Opaque = atoi(value);
	else if(!strcmp(name,"Menu_Frame")) setting->Menu_Frame = atoi(value);
	else if(!strcmp(name,"Show_Menu")) setting->Show_Menu = atoi(value);
	else
		return 0; //when no skin variable
	return 1; //when skin variable found
}*/
//------------------------------
//endfunc scanSkinCNF
//---------------------------------------------------------------------------
//loadSkinCNF will load most cosmetic settings from CNF file
//------------------------------
/*int loadSkinCNF(char *path)
{
	int dummy, var_cnt;
	unsigned char *RAM_p, *CNF_p, *name, *value;

	if( !(RAM_p = preloadCNF(path)) )
		return -1;
	CNF_p = RAM_p;
	for(var_cnt = 0; get_CNF_string(&CNF_p, &name, &value); var_cnt++)
		dummy = scanSkinCNF(name, value);
	free(RAM_p);
	updateScreenMode(0);
	if(setting->skin)
		loadSkin(BACKGROUND_PIC, 0, 0);
	return 0;
}
//------------------------------
//endfunc loadSkinCNF
//---------------------------------------------------------------------------
//loadSkinBrowser will load most cosmetic settings from browsed skin CNF file
//------------------------------
void loadSkinBrowser(void)
{
	int tst;
	char path[MAX_PATH];
	char mess[MAX_PATH];

	getFilePath(path, TEXT_CNF); // No Filtering, Be Careful.
	tst = loadSkinCNF(path);
	if(tst<0)
		sprintf(mess, "%s \"%s\".", LNG(Failed_To_Load), path);
	else
		sprintf(mess, "%s \"%s\".", LNG(Loaded_Config), path);

	drawMsg(mess);
}*/
//------------------------------
//endfunc loadSkinBrowser
//---------------------------------------------------------------------------
// Save LAUNCHELF.CNF (or LAUNCHELFx.CNF with multiple pages)
// sincro: ADD save USBD_FILE string
// polo: ADD save SKIN_FILE string
// suloku: ADD save MAIN_SKIN string //dlanor: changed to GUI_SKIN_FILE
//---------------------------------------------------------------------------
/*void saveConfig(char *mainMsg, char *CNF)
{
	int i, ret, fd;
	char c[MAX_PATH], tmp[26*MAX_PATH + 30*MAX_PATH];
	char cnf_path[MAX_PATH];
	size_t CNF_size, CNF_step;

	sprintf(tmp, "CNF_version = 3\r\n%n", &CNF_size); //Start CNF with version header

	for(i=0; i<15; i++){	//Loop to save the ELF paths for launch keys
		if((i<12) || (setting->LK_Flag[i]!=0)){
			sprintf(tmp+CNF_size,
				"LK_%s_E1 = %s\r\n"
				"%n",           // %n causes NO output, but only a measurement
				LK_ID[i], setting->LK_Path[i],
				&CNF_step       // This variable measures the size of sprintf data
	  	);
			CNF_size += CNF_step;
		}
	}//ends for

	i = strlen(setting->Misc);
	sprintf(tmp+CNF_size,
		"Misc = %s\r\n"
		"Misc_PS2Disc = %s\r\n"
		"Misc_FileBrowser = %s\r\n"
		"Misc_PS2Browser = %s\r\n"
		"Misc_PS2Net = %s\r\n"
		"Misc_PS2PowerOff = %s\r\n"
		"Misc_HddManager = %s\r\n"
		"Misc_TextEditor = %s\r\n"
		"Misc_JpgViewer = %s\r\n"
		"Misc_Configure = %s\r\n"
		"Misc_Load_CNFprev = %s\r\n"
		"Misc_Load_CNFnext = %s\r\n"
		"Misc_Set_CNF_Path = %s\r\n"
		"Misc_Load_CNF = %s\r\n"
		"Misc_ShowFont = %s\r\n"
		"Misc_Debug_Info = %s\r\n"
		"%n",           // %n causes NO output, but only a measurement
		setting->Misc,
		setting->Misc_PS2Disc+i,
		setting->Misc_FileBrowser+i,
		setting->Misc_PS2Browser+i,
		setting->Misc_PS2Net+i,
		setting->Misc_PS2PowerOff+i,
		setting->Misc_HddManager+i,
		setting->Misc_TextEditor+i,
		setting->Misc_JpgViewer+i,
		setting->Misc_Configure+i,
		setting->Misc_Load_CNFprev+i,
		setting->Misc_Load_CNFnext+i,
		setting->Misc_Set_CNF_Path+i,
		setting->Misc_Load_CNF+i,
		setting->Misc_ShowFont+i,
		setting->Misc_Debug_Info+i,
		&CNF_step       // This variable measures the size of sprintf data
  );
	CNF_size += CNF_step;

	CNF_size += storeSkinCNF(tmp+CNF_size);

	sprintf(tmp+CNF_size,
		"LK_auto_Timer = %d\r\n"
		"Menu_Hide_Paths = %d\r\n"
		"Init_CDVD_Check = %d\r\n"
		"Init_Reset_IOP = %d\r\n"
		"Menu_Pages = %d\r\n"
		"GUI_Swap_Keys = %d\r\n"
		"USBD_FILE = %s\r\n"
		"NET_HOSTwrite = %d\r\n"
		"Menu_Title = %s\r\n"
		"Init_Delay = %d\r\n"
		"USBKBD_USED = %d\r\n"
		"USBKBD_FILE = %s\r\n"
		"KBDMAP_FILE = %s\r\n"
		"Menu_Show_Titles = %d\r\n"
		"PathPad_Lock = %d\r\n"
		"CNF_Path = %s\r\n"
		"USBMASS_FILE = %s\r\n"
		"LANG_FILE = %s\r\n"
		"FONT_FILE = %s\r\n"
		"JpgView_Timer = %d\r\n"
		"JpgView_Trans = %d\r\n"
		"JpgView_Full = %d\r\n"
		"PSU_HugeNames = %d\r\n"
		"PSU_DateNames = %d\r\n"
		"PSU_NoOverwrite = %d\r\n"
		"%n",           // %n causes NO output, but only a measurement
		setting->timeout,    //auto_Timer
		setting->Hide_Paths,   //Menu_Hide_Paths
		setting->discControl,  //Init_CDVD_Check
		setting->resetIOP,   //Init_Reset_IOP
		setting->numCNF,     //Menu_Pages
		setting->swapKeys,   //GUI_Swap_Keys
		setting->usbd_file,  //USBD_FILE
		setting->HOSTwrite,  //NET_HOST_write
		setting->Menu_Title, //Menu_Title
		setting->Init_Delay,   //Init_Delay
		setting->usbkbd_used,  //USBKBD_USED
		setting->usbkbd_file,  //USBKBD_FILE
		setting->kbdmap_file,  //KBDMAP_FILE
		setting->Show_Titles,  //Menu_Show_Titles
		setting->PathPad_Lock, //PathPad_Lock
		setting->CNF_Path,     //CNF_Path
		setting->usbmass_file,  //USBMASS_FILE
		setting->lang_file,     //LANG_FILE
		setting->font_file,     //FONT_FILE
		setting->JpgView_Timer, //JpgView_Timer
		setting->JpgView_Trans, //JpgView_Trans
		setting->JpgView_Full,  //JpgView_Full
		setting->PSU_HugeNames, //PSU_HugeNames
		setting->PSU_DateNames, //PSU_DateNames
		setting->PSU_NoOverwrite, //PSU_NoOverwrite
		&CNF_step       // This variable measures the size of sprintf data
  );
	CNF_size += CNF_step;

	for(i=0; i<15; i++){  //Loop to save user defined launch key titles
		if(setting->LK_Title[i][0]){  //Only save non-empty strings
			sprintf(tmp+CNF_size,
				"LK_%s_Title = %s\r\n"
				"%n",           // %n causes NO output, but only a measurement
				LK_ID[i], setting->LK_Title[i],
				&CNF_step       // This variable measures the size of sprintf data
		  );
			CNF_size += CNF_step;
		}//ends if
	}//ends for

	sprintf(tmp+CNF_size,
		"PathPad_Lock = %d\r\n"
		"%n",           // %n causes NO output, but only a measurement
		setting->PathPad_Lock, //PathPad_Lock
		&CNF_step       // This variable measures the size of sprintf data
  );
	CNF_size += CNF_step;

	for(i=0; i<30; i++){  //Loop to save non-empty PathPad entries
		if(PathPad[i][0]){  //Only save non-empty strings
			sprintf(tmp+CNF_size,
				"PathPad[%02d] = %s\r\n"
				"%n",           // %n causes NO output, but only a measurement
				i, PathPad[i],
				&CNF_step       // This variable measures the size of sprintf data
		  );
			CNF_size += CNF_step;
		}//ends if
	}//ends for

	strcpy(c, LaunchElfDir);
	strcat(c, CNF);
	ret = genFixPath(c, cnf_path);
	if((ret >= 0) && ((fd=genOpen(cnf_path, O_RDONLY)) >= 0))
		genClose(fd);
	else {  //Start of clause for failure to use LaunchElfDir
		if(setting->CNF_Path[0]==0) { //if NO CNF Path override defined
			if(!strncmp(LaunchElfDir, "mc", 2))
				sprintf(c, "mc%d:/SYS-CONF", LaunchElfDir[2]-'0');
			else
				sprintf(c, "mc%d:/SYS-CONF", CheckMC());
	
			if((fd=fioDopen(c)) >= 0){
				fioDclose(fd);
				char strtmp[MAX_PATH] = "/";
				strcat(c, strcat(strtmp, CNF));
			}else{
				strcpy(c, LaunchElfDir);
				strcat(c, CNF);
			}
		}
	}  //End of clause for failure to use LaunchElfDir

	ret = genFixPath(c, cnf_path);
	if((ret < 0) || ((fd=genOpen(cnf_path,O_CREAT|O_WRONLY|O_TRUNC)) < 0)){
		sprintf(c, "mc%d:/SYS-CONF", CheckMC());
		if((fd=fioDopen(c)) >= 0){
			fioDclose(fd);
			char strtmp[MAX_PATH] = "/";
			strcat(c, strcat(strtmp, CNF));
		}else{
			strcpy(c, LaunchElfDir);
			strcat(c, CNF);
		}
		ret = genFixPath(c, cnf_path);
		if((fd=genOpen(cnf_path,O_CREAT|O_WRONLY|O_TRUNC)) < 0){
			sprintf(mainMsg, "%s %s", LNG(Failed_To_Save), CNF);
			return;
		}
	}
	ret = genWrite(fd,&tmp,CNF_size);
	if(ret==CNF_size)
		sprintf(mainMsg, "%s (%s)", LNG(Saved_Config), c);
	else
		sprintf(mainMsg, "%s (%s)", LNG(Failed_writing), CNF);
	genClose(fd);
}*/
//---------------------------------------------------------------------------
void initConfig(void)
{
	//int i;

	if (setting != NULL)
		free(setting);
	setting = (SETTING *)malloc(sizeof(SETTING));

	/*	sprintf(setting->Misc, "%s/", LNG_DEF(MISC));
	sprintf(setting->Misc_PS2Disc, "%s/%s", LNG_DEF(MISC), LNG_DEF(PS2Disc));
	sprintf(setting->Misc_FileBrowser, "%s/%s", LNG_DEF(MISC), LNG_DEF(FileBrowser));
	sprintf(setting->Misc_PS2Browser, "%s/%s", LNG_DEF(MISC), LNG_DEF(PS2Browser));
	sprintf(setting->Misc_PS2Net, "%s/%s", LNG_DEF(MISC), LNG_DEF(PS2Net));
	sprintf(setting->Misc_PS2PowerOff, "%s/%s", LNG_DEF(MISC), LNG_DEF(PS2PowerOff));
	sprintf(setting->Misc_HddManager, "%s/%s", LNG_DEF(MISC), LNG_DEF(HddManager));
	sprintf(setting->Misc_TextEditor, "%s/%s", LNG_DEF(MISC), LNG_DEF(TextEditor));
	sprintf(setting->Misc_JpgViewer, "%s/%s", LNG_DEF(MISC), LNG_DEF(JpgViewer));
	sprintf(setting->Misc_Configure, "%s/%s", LNG_DEF(MISC), LNG_DEF(Configure));
	sprintf(setting->Misc_Load_CNFprev, "%s/%s", LNG_DEF(MISC), LNG_DEF(Load_CNFprev));
	sprintf(setting->Misc_Load_CNFnext, "%s/%s", LNG_DEF(MISC), LNG_DEF(Load_CNFnext));
	sprintf(setting->Misc_Set_CNF_Path, "%s/%s", LNG_DEF(MISC), LNG_DEF(Set_CNF_Path));
	sprintf(setting->Misc_Load_CNF, "%s/%s", LNG_DEF(MISC), LNG_DEF(Load_CNF));
	sprintf(setting->Misc_ShowFont, "%s/%s", LNG_DEF(MISC), LNG_DEF(ShowFont));
	sprintf(setting->Misc_Debug_Info, "%s/%s", LNG_DEF(MISC), LNG_DEF(Debug_Info));

	for(i=0; i<15; i++){
		setting->LK_Path[i][0]  = 0;
		setting->LK_Title[i][0] = 0;
		setting->LK_Flag[i]    = 0;
	}
	for(i=0; i<30; i++) PathPad[i][0] = 0;

	strcpy(setting->LK_Path[1], setting->Misc_FileBrowser);
	setting->LK_Flag[1] = 1;
	setting->usbd_file[0] = '\0';
	setting->usbmass_file[0] = '\0';
	setting->usbkbd_file[0] = '\0';
	setting->kbdmap_file[0] = '\0';
	setting->skin[0] = '\0';
	setting->GUI_skin[0] = '\0';
	setting->Menu_Title[0] = '\0';
	setting->CNF_Path[0] = '\0';
	setting->lang_file[0] = '\0';
	setting->font_file[0] = '\0';
	setting->timeout = DEF_TIMEOUT;
	setting->Hide_Paths = DEF_HIDE_PATHS;
*/
	setting->color[0] = DEF_COLOR1;                //background
	setting->color[1] = DEF_COLOR2;                //frame
	setting->color[2] = DEF_COLOR3;                //selected
	setting->color[3] = DEF_COLOR4;                //common text
	setting->color[4] = DEF_COLOR5;                //messages
	strcpy(setting->Menu_Title, LANG_MENU_TITLE);  //LANG_MENU_TITLE
	                                               /*	setting->color[4] = DEF_COLOR5;
	setting->color[5] = DEF_COLOR6;
	setting->color[6] = DEF_COLOR7;
	setting->color[7] = DEF_COLOR8;
*/
	setting->screen_x = SCREEN_X;
	setting->screen_y = SCREEN_Y;
	//	setting->discControl = DEF_DISCCONTROL;
	setting->interlace = DEF_INTERLACE;
	setting->Menu_Frame = DEF_MENU_FRAME;
	//	setting->Show_Menu = DEF_MENU;
	setting->resetIOP = DEF_RESETIOP;
	//	setting->numCNF = DEF_NUMCNF;
	setting->swapKeys = DEF_SWAPKEYS;
	/*	setting->HOSTwrite = DEF_HOSTWRITE;
	setting->Brightness = DEF_BRIGHT;
	setting->TV_mode = TV_mode_AUTO; //0==Console_auto, 1==NTSC, 2==PAL
	setting->Popup_Opaque = DEF_POPUP_OPAQUE;
	setting->Init_Delay = DEF_INIT_DELAY;
	setting->usbkbd_used = DEF_USBKBD_USED;
	setting->Show_Titles = DEF_SHOW_TITLES;
	setting->PathPad_Lock = DEF_PATHPAD_LOCK;
	setting->JpgView_Timer = -1; //only used to detect missing variable
	setting->JpgView_Trans = -1; //only used to detect missing variable
	setting->JpgView_Full = DEF_JPGVIEW_FULL;
	setting->PSU_HugeNames = DEF_PSU_HUGENAMES;
	setting->PSU_DateNames = DEF_PSU_DATENAMES;
	setting->PSU_NoOverwrite = DEF_PSU_NOOVERWRITE;
*/
}
//------------------------------
//endfunc initConfig
//---------------------------------------------------------------------------
// Load LAUNCHELF.CNF (or LAUNCHELFx.CNF with multiple pages)
// sincro: ADD load USBD_FILE string
// polo: ADD load SKIN_FILE string
// suloku: ADD load MAIN_SKIN string //dlanor: changed to GUI_SKIN_FILE
// dlanor: added error flag return value 0==OK, -1==failure
//---------------------------------------------------------------------------
/*int loadConfig(char *mainMsg, char *CNF)
{
	int i, fd, tst, len, mcport, var_cnt, CNF_version;
	char tsts[20];
	char path[MAX_PATH];
	char cnf_path[MAX_PATH];
	unsigned char *RAM_p, *CNF_p, *name, *value;

	initConfig();

	strcpy(path, LaunchElfDir);
	strcat(path, CNF);
	if(!strncmp(path, "cdrom", 5)) strcat(path, ";1");

	fd=-1;
	if((tst = genFixPath(path, cnf_path)) >= 0)
		fd = genOpen(cnf_path, O_RDONLY);
	if(fd<0) {
		char strtmp[MAX_PATH], *p;
		int  pos;

		p = strrchr(path, '.');  //make p point to extension
		if(*(p-1)!='F')          //is this an indexed CNF
			p--;                   //then make p point to index
		pos = (p-path);
		strcpy(strtmp, path);
		strcpy(strtmp+pos-9, "LNCHELF"); //Replace LAUNCHELF with LNCHELF (for CD)
		strcpy(strtmp+pos-2, path+pos); //Add index+extension too
		if((tst = genFixPath(strtmp, cnf_path)) >= 0)
			fd = genOpen(cnf_path, O_RDONLY);
		if(fd<0) {
			if(!strncmp(LaunchElfDir, "mc", 2))
				mcport = LaunchElfDir[2]-'0';
			else
				mcport = CheckMC();
			if(mcport==1 || mcport==0){
				sprintf(strtmp, "mc%d:/SYS-CONF/", mcport);
				strcpy(cnf_path, strtmp);
				strcat(cnf_path, CNF);
				fd = genOpen(cnf_path, O_RDONLY);
				if(fd>=0)
					strcpy(LaunchElfDir, strtmp);
			}
		}
	}
	if(fd<0) {
failed_load:
		sprintf(mainMsg, "%s %s", LNG(Failed_To_Load), CNF);
		return -1;
	}
	// This point is only reached after succefully opening CNF
	genClose(fd);

	if( (RAM_p = preloadCNF(cnf_path))==NULL )
		goto failed_load;
	CNF_p = RAM_p;

//RA NB: in the code below, the 'LK_' variables have been implemented such that
//       any _Ex suffix will be accepted, with identical results. This will need
//       to be modified when more execution methods are implemented.

  CNF_version = 0;  // The CNF version is still unidentified
	for(var_cnt = 0; get_CNF_string(&CNF_p, &name, &value); var_cnt++)
	{	// A variable was found, now we dispose of its value.
		if(!strcmp(name,"CNF_version")){
			CNF_version = atoi(value);
			continue;
		} else if(CNF_version == 0)
			goto failed_load;  // Refuse unidentified CNF

		if( scanSkinCNF(name, value) )
			continue;

		for(i=0; i<15; i++){
			sprintf(tsts, "LK_%s_E%n", LK_ID[i], &len);
			if(!strncmp(name, tsts, len)) {
				strcpy(setting->LK_Path[i], value);
				setting->LK_Flag[i] = 1;
				break;
			}
		}
		if(i<15) continue;
		//----------
		//In the next group, the Misc device must be defined before its subprograms
		//----------
		else if(!strcmp(name,"Misc")) sprintf(setting->Misc, "%s/", value);
		else if(!strcmp(name,"Misc_PS2Disc"))
			sprintf(setting->Misc_PS2Disc, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_FileBrowser"))
			sprintf(setting->Misc_FileBrowser, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_PS2Browser"))
			sprintf(setting->Misc_PS2Browser, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_PS2Net"))
			sprintf(setting->Misc_PS2Net, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_PS2PowerOff"))
			sprintf(setting->Misc_PS2PowerOff, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_HddManager"))
			sprintf(setting->Misc_HddManager, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_TextEditor"))
			sprintf(setting->Misc_TextEditor, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_JpgViewer"))
			sprintf(setting->Misc_JpgViewer, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_Configure"))
			sprintf(setting->Misc_Configure, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_Load_CNFprev"))
			sprintf(setting->Misc_Load_CNFprev, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_Load_CNFnext"))
			sprintf(setting->Misc_Load_CNFnext, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_Set_CNF_Path"))
			sprintf(setting->Misc_Set_CNF_Path, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_Load_CNF"))
			sprintf(setting->Misc_Load_CNF, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_ShowFont"))
			sprintf(setting->Misc_ShowFont, "%s%s", setting->Misc, value);
		else if(!strcmp(name,"Misc_Debug_Info"))
			sprintf(setting->Misc_Debug_Info, "%s%s", setting->Misc, value);
		//----------
		else if(!strcmp(name,"LK_auto_Timer")) setting->timeout = atoi(value);
		else if(!strcmp(name,"Menu_Hide_Paths")) setting->Hide_Paths = atoi(value);
		//---------- NB: color settings moved to scanSkinCNF
		else if(!strcmp(name,"Init_CDVD_Check")) setting->discControl = atoi(value);
		else if(!strcmp(name,"Init_Reset_IOP")) setting->resetIOP = atoi(value);
		else if(!strcmp(name,"Menu_Pages")) setting->numCNF = atoi(value);
		else if(!strcmp(name,"GUI_Swap_Keys")) setting->swapKeys = atoi(value);
		else if(!strcmp(name,"USBD_FILE")) strcpy(setting->usbd_file,value);
		else if(!strcmp(name,"NET_HOSTwrite")) setting->HOSTwrite = atoi(value);
		else if(!strcmp(name,"Menu_Title")){
			strncpy(setting->Menu_Title, value, MAX_MENU_TITLE);
			setting->Menu_Title[MAX_MENU_TITLE] = '\0';
		}
		else if(!strcmp(name,"Init_Delay")) setting->Init_Delay = atoi(value);
		else if(!strcmp(name,"USBKBD_USED")) setting->usbkbd_used = atoi(value);
		else if(!strcmp(name,"USBKBD_FILE")) strcpy(setting->usbkbd_file,value);
		else if(!strcmp(name,"KBDMAP_FILE")) strcpy(setting->kbdmap_file,value);
		else if(!strcmp(name,"Menu_Show_Titles")) setting->Show_Titles = atoi(value);
		else if(!strcmp(name,"PathPad_Lock")) setting->PathPad_Lock = atoi(value);
		else if(!strcmp(name,"CNF_Path")) strcpy(setting->CNF_Path,value);
		else if(!strcmp(name,"USBMASS_FILE")) strcpy(setting->usbmass_file,value);
		else if(!strcmp(name,"LANG_FILE")) strcpy(setting->lang_file,value);
		else if(!strcmp(name,"FONT_FILE")) strcpy(setting->font_file,value);
		//----------
		else if(!strcmp(name,"JpgView_Timer")) setting->JpgView_Timer = atoi(value);
		else if(!strcmp(name,"JpgView_Trans")) setting->JpgView_Trans = atoi(value);
		else if(!strcmp(name,"JpgView_Full")) setting->JpgView_Full = atoi(value);
		//----------
		else if(!strcmp(name,"PSU_HugeNames")) setting->PSU_HugeNames = atoi(value);
		else if(!strcmp(name,"PSU_DateNames")) setting->PSU_DateNames = atoi(value);
		else if(!strcmp(name,"PSU_NoOverwrite")) setting->PSU_NoOverwrite = atoi(value);
		//----------
		else {
			for(i=0; i<15; i++){
				sprintf(tsts, "LK_%s_Title", LK_ID[i]);
				if(!strcmp(name, tsts)) {
					strncpy(setting->LK_Title[i], value, MAX_ELF_TITLE-1);
					break;
				}
			}
			if(i<15) continue;
			else if(!strncmp(name,"PathPad[",8)){
				i = atoi(name+8);
				if(i < 30){
					strncpy(PathPad[i], value, MAX_PATH-1);
					PathPad[i][MAX_PATH-1] = '\0';
				}
			}
		}
	} //ends for
	for(i=0; i<15; i++) setting->LK_Title[i][MAX_ELF_TITLE-1] = 0;
	free(RAM_p);
	if(setting->JpgView_Timer < 0)
		setting->JpgView_Timer = DEF_JPGVIEW_TIMER;
	if((setting->JpgView_Trans < 1) || (setting->JpgView_Trans > 4))
		setting->JpgView_Trans = DEF_JPGVIEW_TRANS;
	sprintf(mainMsg, "%s (%s)", LNG(Loaded_Config), path);
	return 0;
}*/
//------------------------------
//endfunc loadConfig
//---------------------------------------------------------------------------
// Polo: ADD Skin Menu with Skin preview
// suloku: ADD Main skin selection
//---------------------------------------------------------------------------
/*void Config_Skin(void)
{
	int  s, max_s=7;
	int  x, y;
	int event, post_event=0;
	char c[MAX_PATH];
	char skinSave[MAX_PATH], GUI_Save[MAX_PATH];
	int  Brightness = setting->Brightness;
	int current_preview = 0;

	strcpy(skinSave, setting->skin);
	strcpy(GUI_Save, setting->GUI_skin);

	loadSkin(PREVIEW_PIC, 0, 0);
	current_preview = PREVIEW_PIC;

	s=1;
	event = 1;  //event = initial entry
	while(1)
	{
		//Pad response section
		waitPadReady(0, 0);
		if(readpad())
		{
			if(new_pad & PAD_UP)
			{
				event |= 2;  //event |= valid pad command
				if(s!=1) s--;
				else s=max_s;
			}
			else if(new_pad & PAD_DOWN)
			{
				event |= 2;  //event |= valid pad command
				if(s!=max_s) s++;
				else s=1;
			}
			else if(new_pad & PAD_LEFT)
			{
				event |= 2;  //event |= valid pad command
				if(s!=1) s=1;
				else s=max_s;
			}
			else if(new_pad & PAD_RIGHT)
			{
				event |= 2;  //event |= valid pad command
				if(s!=max_s) s=max_s;
				else s=1;
			}
			else if((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE) )
			{
				event |= 2;  //event |= valid pad command
				if(s==1) {                      //Command == Cancel Skin Path
					setting->skin[0] = '\0';
					loadSkin(PREVIEW_PIC, 0, 0);
					current_preview = PREVIEW_PIC;
				} else if(s==3) {               //Command == Decrease Brightness
					if((Brightness > 0)&&(testsetskin == 1)) {
						Brightness--;
					}
				} else if(s==4) {               //Command == Cancel GUI Skin Path
					setting->GUI_skin[0] = '\0';
					loadSkin(PREVIEW_GUI, 0, 0);
					current_preview = PREVIEW_GUI;
				}
			}
			else if((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE))
			{
				event |= 2;  //event |= valid pad command
				if(s==1) {                      //Command == Set Skin Path
					getFilePath(setting->skin, SKIN_CNF);
					loadSkin(PREVIEW_PIC, 0, 0);
					current_preview = PREVIEW_PIC;
				} else if(s==2) {               //Command == Apply New Skin
					GUI_active = 0;
					loadSkin(BACKGROUND_PIC, 0, 0);
					setting->Brightness = Brightness;
					strcpy(skinSave, setting->skin);
					loadSkin(PREVIEW_PIC, 0, 0);
					current_preview = PREVIEW_PIC;
			} else if(s==3) {               //Command == Increase Brightness
					if((Brightness < 100)&&(testsetskin == 1)) {
						Brightness++;
					}
				} else if(s==4) {               //Command == Set GUI Skin Path
					getFilePath(setting->GUI_skin, GUI_SKIN_CNF);
					loadSkin(PREVIEW_GUI, 0, 0);
					current_preview = PREVIEW_GUI;
				} else if(s==5) {               //Command == Apply GUI Skin
					strcpy(GUI_Save, setting->GUI_skin);
					loadSkin(PREVIEW_GUI, 0, 0);
					current_preview = PREVIEW_GUI;
				} else if(s==6) {               //Command == Show GUI Menu
					setting->Show_Menu = !setting->Show_Menu;
				} else if(s==7) {               //Command == RETURN
					setting->skin[0] = '\0';
					strcpy(setting->skin, skinSave);
					setting->GUI_skin[0] = '\0';
					strcpy(setting->GUI_skin, GUI_Save);
					return;
				}
			}
			else if(new_pad & PAD_TRIANGLE) {
				setting->skin[0] = '\0';
				strcpy(setting->skin, skinSave);
				setting->GUI_skin[0] = '\0';
				strcpy(setting->GUI_skin, GUI_Save);
				return;
			}
		} //end if(readpad())
		
		if(event||post_event){ //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			if ( testsetskin == 1 ) {
				setBrightness(Brightness);
				gsKit_prim_sprite_texture(gsGlobal, &TexPreview,
				 SCREEN_WIDTH/4, ( SCREEN_HEIGHT/4 )+60, 0, 0,
				 ( SCREEN_WIDTH/4 )*3, ( ( SCREEN_HEIGHT/4 )*3 )+60, SCREEN_WIDTH, SCREEN_HEIGHT,
				 0, BrightColor);
				setBrightness(50);
			} else {
				gsKit_prim_sprite(gsGlobal,
				 SCREEN_WIDTH/4, ( SCREEN_HEIGHT/4 )+60, ( SCREEN_WIDTH/4 )*3, ( ( SCREEN_HEIGHT/4 )*3 )+60,
				 0, setting->color[0]);
			}
			drawFrame( ( SCREEN_WIDTH/4 )-2, ( ( SCREEN_HEIGHT/4 )+60 )-1,
			 ( ( SCREEN_WIDTH/4 )*3 )+1, ( ( SCREEN_HEIGHT/4 )*3 )+60,
			  setting->color[1]);

			x = Menu_start_x;
			y = Menu_start_y;
		
			printXY(LNG(SKIN_SETTINGS), x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->skin)==0)
				sprintf(c, "  %s: %s", LNG(Skin_Path), LNG(NULL));
			else
				sprintf(c, "  %s: %s", LNG(Skin_Path), setting->skin);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s", LNG(Apply_New_Skin));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s: %d", LNG(Brightness), Brightness);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->GUI_skin)==0)
				sprintf(c, "  %s %s: %s", LNG(GUI), LNG(Skin_Path), LNG(NULL));
			else
				sprintf(c, "  %s %s: %s", LNG(GUI), LNG(Skin_Path), setting->GUI_skin);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s", LNG(Apply_GUI_Skin));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(setting->Show_Menu)
				sprintf(c, "  %s: %s", LNG(Show_Menu), LNG(ON));
			else
				sprintf(c, "  %s: %s", LNG(Show_Menu), LNG(OFF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s", LNG(RETURN));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(current_preview == PREVIEW_PIC)
				sprintf(c, "%s ", LNG(Normal));
			else
				sprintf(c, "%s ", LNG(GUI));
			strcat(c, LNG(Skin_Preview));
			printXY(c, SCREEN_WIDTH/4, (SCREEN_HEIGHT/4)+78-FONT_HEIGHT, setting->color[3], TRUE, 0);

			//Cursor positioning section
			y = Menu_start_y + s*(FONT_HEIGHT);
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if ((s == 1)||(s == 4)) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Edit), LNG(Clear));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Edit), LNG(Clear));
			} else if (s == 3) {  //if cursor at a colour component or a screen offset
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Add), LNG(Subtract));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Add), LNG(Subtract));
			} else if (s == 6) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(Change));
				else
					sprintf(c, "ÿ0:%s", LNG(Change));
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(OK));
				else
					sprintf(c, "ÿ0:%s", LNG(OK));
			}
			sprintf(tmp, " ÿ3:%s", LNG(Return));
			strcat(c, tmp);
			setScrTmp("", c);
		}//ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}//ends while
}//ends Config_Skin*/
//---------------------------------------------------------------------------
/*void Config_Screen(void)
{
	int i;
	int s, max_s=35;		//define cursor index and its max value
	int x, y;
	int event, post_event=0;
	u64 rgb[8][3];
	char c[MAX_PATH];
	int space=((SCREEN_WIDTH-SCREEN_MARGIN-4*FONT_WIDTH)-(Menu_start_x+2*FONT_WIDTH))/8;
	
	event = 1;	//event = initial entry

	for(i=0; i<8; i++) {
		rgb[i][0] = setting->color[i] & 0xFF;
		rgb[i][1] = setting->color[i] >> 8 & 0xFF;
		rgb[i][2] = setting->color[i] >> 16 & 0xFF;
	}
	
	s=0;
	while(1)
	{
		//Pad response section
		waitPadReady(0, 0);
		if(readpad())
		{
			if(new_pad & PAD_UP)
			{
				event |= 2;  //event |= valid pad command
				if(s==0)
					s=max_s;
				else if(s==24)
					s=2;
				else
					s--;
			}
			else if(new_pad & PAD_DOWN)
			{
				event |= 2;  //event |= valid pad command
				if((s<24)&&(s%3==2))
					s=24;
				else if(s==max_s)
					s=0;
				else
					s++;
			}
			else if(new_pad & PAD_LEFT)
			{
				event |= 2;  //event |= valid pad command
				if(s>=34) s=32;
				else if(s>=32) s=31;
				else if(s>=31) s=28;
				else if(s>=28) s=27;
				else if(s>=27) s=25;
				else if(s>=25) s=24; //at or 
				else if(s>=24) s=21; //if s beyond color settings
				else if(s>=3) s-=3;  //if s in a color beyond Color1 step to preceding color
			}
			else if(new_pad & PAD_RIGHT)
			{
				event |= 2;  //event |= valid pad command
				if(s>=32) s=34;
				else if(s>=31) s=32;
				else if(s>=28) s=31;
				else if(s>=27) s=28;
				else if(s>=25) s=27;
				else if(s>=24) s=25;
				else if(s>=21) s=24; //if s in Color8, move it to ScreenX
				else s+=3;           //if s in a color before Color8, step to next color
			}
			else if((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE) )
			{ //User pressed CANCEL=>Subtract/Clear
				event |= 2;  //event |= valid pad command
				if(s<24) {
					if(rgb[s/3][s%3] > 0) {
						rgb[s/3][s%3]--;
						setting->color[s/3] = 
							GS_SETREG_RGBA(rgb[s/3][0], rgb[s/3][1], rgb[s/3][2], 0);
					}
				} else if(s==25) {
					if(setting->screen_x > 0) {
						setting->screen_x--;
						updateScreenMode(0);
					}
				} else if(s==26) {
					if(setting->screen_y > 0) {
						setting->screen_y--;
						updateScreenMode(0);
					}
				} else if(s==31) {  //cursor is at Menu_Title
						setting->Menu_Title[0] = '\0';
				}
			}
			else if((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE))
			{ //User pressed OK=>Add/Ok/Edit
				event |= 2;  //event |= valid pad command
				if(s<24) {
					if(rgb[s/3][s%3] < 255) {
						rgb[s/3][s%3]++;
						setting->color[s/3] = 
							GS_SETREG_RGBA(rgb[s/3][0], rgb[s/3][1], rgb[s/3][2], 0);
					}
				}else if(s==24){
					setting->TV_mode = (setting->TV_mode+1)%3; //Change between 0,1,2
					updateScreenMode(1);
				} else if(s==25) {
					setting->screen_x++;
					updateScreenMode(0);
				} else if(s==26) {
					setting->screen_y++;
					updateScreenMode(0);
				} else if(s==27) {
					setting->interlace = !setting->interlace;
					updateScreenMode(1);
				} else if(s==28) {
					Config_Skin();
				} else if(s==29) {
					loadSkinBrowser();
				} else if(s==30) {
					saveSkinBrowser();
				} else if(s==31) {  //cursor is at Menu_Title
					char tmp[MAX_MENU_TITLE+1];
					strcpy(tmp, setting->Menu_Title);
					if(keyboard(tmp, 36)>=0)
						strcpy(setting->Menu_Title, tmp);
				} else if(s==32) {
					setting->Menu_Frame = !setting->Menu_Frame;
				} else if(s==33) {
					setting->Popup_Opaque = !setting->Popup_Opaque;
				} else if(s==max_s-1) { //Always put 'RETURN' next to last
					return;
				} else if(s==max_s) { //Always put 'DEFAULT SCREEN SETTINGS' last
					setting->skin[0] = '\0';
					setting->GUI_skin[0] = '\0';
					loadSkin(BACKGROUND_PIC, 0, 0);
					setting->color[0] = DEF_COLOR1;
					setting->color[1] = DEF_COLOR2;
					setting->color[2] = DEF_COLOR3;
					setting->color[3] = DEF_COLOR4;
					setting->color[4] = DEF_COLOR5;
					setting->color[5] = DEF_COLOR6;
					setting->color[6] = DEF_COLOR7;
					setting->color[7] = DEF_COLOR8;
					setting->TV_mode = TV_mode_AUTO;
					setting->screen_x = SCREEN_X;
					setting->screen_y = SCREEN_Y;
					setting->interlace = DEF_INTERLACE;
					setting->Menu_Frame = DEF_MENU_FRAME;
					setting->Show_Menu = DEF_MENU;
					setting->Brightness = DEF_BRIGHT;
					setting->Popup_Opaque = DEF_POPUP_OPAQUE;
					updateScreenMode(0);
					
					for(i=0; i<8; i++) {
						rgb[i][0] = setting->color[i] & 0xFF;
						rgb[i][1] = setting->color[i] >> 8 & 0xFF;
						rgb[i][2] = setting->color[i] >> 16 & 0xFF;
					}
				}
			}
			else if(new_pad & PAD_TRIANGLE)
				return;
		}

		if(event||post_event){ //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			x = Menu_start_x;

			for(i=0; i<8; i++){
				y = Menu_start_y;
				sprintf(c, "%s%d", LNG(Color), i+1);
				printXY(c, x+(space*(i+1))-(printXY(c, 0, 0, 0, FALSE, space-FONT_WIDTH/2)/2), y,
					setting->color[3], TRUE, space-FONT_WIDTH/2);
				if(i==0)
					sprintf(c, "%s", LNG(Backgr));
				else if(i==1)
					sprintf(c, "%s", LNG(Frames));
				else if(i==2)
					sprintf(c, "%s", LNG(Select));
				else if(i==3)
					sprintf(c, "%s", LNG(Normal));
				else if(i>=4)
					sprintf(c, "%s%d", LNG(Graph), i-3);
				printXY(c, x+(space*(i+1))-(printXY(c, 0, 0, 0, FALSE, space-FONT_WIDTH/2)/2), y+FONT_HEIGHT,
					setting->color[3], TRUE, space-FONT_WIDTH/2);
				y += FONT_HEIGHT*2;
				printXY("R:", x, y, setting->color[3], TRUE, 0);
				sprintf(c, "%02lX", rgb[i][0]);
				printXY(c, x+(space*(i+1))-FONT_WIDTH, y, setting->color[3], TRUE, 0);
				y += FONT_HEIGHT;
				printXY("G:", x, y, setting->color[3], TRUE, 0);
				sprintf(c, "%02lX", rgb[i][1]);
				printXY(c, x+(space*(i+1))-FONT_WIDTH, y, setting->color[3], TRUE, 0);
				y += FONT_HEIGHT;
				printXY("B:", x, y, setting->color[3], TRUE, 0);
				sprintf(c, "%02lX", rgb[i][2]);
				printXY(c, x+(space*(i+1))-FONT_WIDTH, y, setting->color[3], TRUE, 0);
				y += FONT_HEIGHT;
				sprintf(c, "ÿ4");
				printXY(c, x+(space*(i+1))-FONT_WIDTH, y, setting->color[i], TRUE, 0);
			} //ends loop for colour RGB values
			y += FONT_HEIGHT*2;
			sprintf(c, "  %s: ", LNG(TV_mode));
			if(setting->TV_mode==TV_mode_NTSC)
				strcat(c, "NTSC");
			else if(setting->TV_mode==TV_mode_PAL)
				strcat(c, "PAL");
			else
				strcat(c, "AUTO");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s: %d", LNG(Screen_X_offset), setting->screen_x);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s: %d", LNG(Screen_Y_offset), setting->screen_y);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			if(setting->interlace)
				sprintf(c, "  %s: %s", LNG(Interlace), LNG(ON));
			else
				sprintf(c, "  %s: %s", LNG(Interlace), LNG(OFF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s...", LNG(Skin_Settings));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s...", LNG(Load_Skin_CNF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s...", LNG(Save_Skin_CNF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			if(setting->Menu_Title[0]=='\0')
				sprintf(c, "  %s: %s", LNG(Menu_Title), LNG(NULL));
			else
				sprintf(c, "  %s: %s", LNG(Menu_Title),setting->Menu_Title);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			if(setting->Menu_Frame)
				sprintf(c, "  %s: %s", LNG(Menu_Frame), LNG(ON));
			else
				sprintf(c, "  %s: %s", LNG(Menu_Frame), LNG(OFF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(setting->Popup_Opaque)
				sprintf(c, "  %s: %s", LNG(Popups_Opaque), LNG(ON));
			else
				sprintf(c, "  %s: %s", LNG(Popups_Opaque), LNG(OFF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", LNG(RETURN));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", LNG(Use_Default_Screen_Settings));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			//Cursor positioning section
			x = Menu_start_x;
			y = Menu_start_y;

			if(s<24){  //if cursor indicates a colour component
				int colnum = s/3;
				int comnum = s-colnum*3;
				x += (space*(colnum+1))-(FONT_WIDTH*4);
				y += (2+comnum)*FONT_HEIGHT;
			} else {  //if cursor indicates anything after colour components
				y += (s-24+6)*FONT_HEIGHT+FONT_HEIGHT/2;  //adjust y for cursor beyond colours
				//Here y is almost correct, except for additional group spacing
				if(s>=24)            //if cursor at or beyond TV mode choice
					y+=FONT_HEIGHT/2;  //adjust for half-row space below colours
				if(s>=25)            //if cursor at or beyond screen offsets
					y+=FONT_HEIGHT/2;  //adjust for half-row space below TV mode choice
				if(s>=27)            //if cursor at or beyond interlace choice
					y+=FONT_HEIGHT/2;  //adjust for half-row space below screen offsets
				if(s>=28)            //if cursor at or beyond 'SKIN SETTINGS'
					y+=FONT_HEIGHT/2;  //adjust for half-row space below interlace choice
				if(s>=31)            //if cursor at or beyond 'Menu Title'
					y+=FONT_HEIGHT/2;  //adjust for half-row space below 'SKIN SETTINGS'
				if(s>=32)            //if cursor at or beyond 'Menu Frame'
					y+=FONT_HEIGHT/2;  //adjust for half-row space below 'Menu Title'
				if(s>=max_s-1)            //if cursor at or beyond 'RETURN'
					y+=FONT_HEIGHT/2;  //adjust for half-row space below 'Popups Opaque'
			}
			drawChar(LEFT_CUR, x, y, setting->color[3]);  //draw cursor

			//Tooltip section
			if (s<24||s==25||s==26) {  //if cursor at a colour component or a screen offset
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Add), LNG(Subtract));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Add), LNG(Subtract));
			} else if(s==24||s==27||s==32||s==33) {
				//if cursor at 'TV mode', 'INTERLACE', 'Menu Frame' or 'Popups Opaque'
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(Change));
				else
					sprintf(c, "ÿ0:%s", LNG(Change));
			} else if(s==28||s==29||s==30){  //if cursor at 'SKIN SETTINGS'
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(OK));
				else
					sprintf(c, "ÿ0:%s", LNG(OK));
			} else if(s==31){  //if cursor at Menu_Title
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Edit), LNG(Clear));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Edit), LNG(Clear));
			} else {  //if cursor at 'RETURN' or 'DEFAULT' options
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(OK));
				else
					sprintf(c, "ÿ0:%s", LNG(OK));
			}
			sprintf(tmp, " ÿ3:%s", LNG(Return));
			strcat(c, tmp);
			setScrTmp("", c);
		}//ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}//ends while
}//ends Config_Screen*/
//---------------------------------------------------------------------------
// Other settings by EP
// sincro: ADD USBD SELECTOR MENU
// dlanor: Add Menu_Title config
//---------------------------------------------------------------------------
/*void Config_Startup(void)
{
	int s, max_s=14;		//define cursor index and its max value
	int x, y;
	int event, post_event=0;
	char c[MAX_PATH];

	event = 1;	//event = initial entry
	s=1;
	while(1)
	{
		//Pad response section
		waitPadReady(0, 0);
		if(readpad())
		{
			if(new_pad & PAD_UP)
			{
				event |= 2;  //event |= valid pad command
				if(s!=1) s--;
				else s=max_s;
			}
			else if(new_pad & PAD_DOWN)
			{
				event |= 2;  //event |= valid pad command
				if(s!=max_s) s++;
				else s=1;
			}
			else if(new_pad & PAD_LEFT)
			{
				event |= 2;  //event |= valid pad command
				if(s!=max_s) s=max_s;
				else s=1;
			}
			else if(new_pad & PAD_RIGHT)
			{
				event |= 2;  //event |= valid pad command
				if(s!=max_s) s=max_s;
				else s=1;
			}
			else if((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE) )
			{
				event |= 2;  //event |= valid pad command
				if(s==2 && setting->numCNF>1) setting->numCNF--;
				else if(s==4) setting->usbd_file[0] = '\0';
				else if(s==5 && setting->Init_Delay>0) setting->Init_Delay--;
				else if(s==6 && setting->timeout>0) setting->timeout--;
				else if(s==8) setting->usbkbd_file[0] = '\0';
				else if(s==9) setting->kbdmap_file[0] = '\0';
				else if(s==10) setting->CNF_Path[0] = '\0';
				else if(s==11) setting->usbmass_file[0] = '\0';
				else if(s==12){
					setting->lang_file[0] = '\0';
					Load_External_Language();
				}
				else if(s==13){
					setting->font_file[0] = '\0';
					loadFont("");
				}
			}
			else if((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE))
			{
				event |= 2;  //event |= valid pad command
				if(s==1)
					setting->resetIOP = !setting->resetIOP;
				else if(s==2)
					setting->numCNF++;
				else if(s==3)
					setting->swapKeys = !setting->swapKeys;
				else if(s==4)
					getFilePath(setting->usbd_file, USBD_IRX_CNF);
				else if(s==5)
					setting->Init_Delay++;
				else if(s==6)
					setting->timeout++;
				else if(s==7)
					setting->usbkbd_used = !setting->usbkbd_used;
				else if(s==8)
					getFilePath(setting->usbkbd_file, USBKBD_IRX_CNF);
				else if(s==9)
					getFilePath(setting->kbdmap_file, KBDMAP_FILE_CNF);
				else if(s==10)
				{	char *tmp;

					getFilePath(setting->CNF_Path, CNF_PATH_CNF);
					if((tmp = strrchr(setting->CNF_Path, '/')))
						tmp[1] = '\0';
				}
				else if(s==11)
					getFilePath(setting->usbmass_file, USBMASS_IRX_CNF);
				else if(s==12){
					getFilePath(setting->lang_file, LANG_CNF);
					Load_External_Language();
				}else if(s==13){
					getFilePath(setting->font_file, FONT_CNF);
					if(loadFont(setting->font_file)==0)
						setting->font_file[0] = '\0';
				}else if(s==max_s)
					return;
			}
			else if(new_pad & PAD_TRIANGLE)
				return;
		}

		if(event||post_event){ //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			x = Menu_start_x;
			y = Menu_start_y;

			printXY(LNG(STARTUP_SETTINGS), x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			if(setting->resetIOP)
				sprintf(c, "  %s: %s", LNG(Reset_IOP), LNG(ON));
			else
				sprintf(c, "  %s: %s", LNG(Reset_IOP), LNG(OFF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s: %d", LNG(Number_of_CNFs), setting->numCNF);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(setting->swapKeys)
				sprintf(c, "  %s: ÿ1:%s ÿ0:%s", LNG(Pad_mapping), LNG(OK), LNG(CANCEL));
			else
				sprintf(c, "  %s: ÿ0:%s ÿ1:%s", LNG(Pad_mapping), LNG(OK), LNG(CANCEL));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->usbd_file)==0)
				sprintf(c, "  %s: %s", LNG(USBD_IRX), LNG(DEFAULT));
			else
				sprintf(c, "  %s: %s", LNG(USBD_IRX), setting->usbd_file);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s: %d", LNG(Initial_Delay), setting->Init_Delay);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s: %d", LNG(Default_Timeout), setting->timeout);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(setting->usbkbd_used)
				sprintf(c, "  %s: %s", LNG(USB_Keyboard_Used), LNG(ON));
			else
				sprintf(c, "  %s: %s", LNG(USB_Keyboard_Used), LNG(OFF));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->usbkbd_file)==0)
				sprintf(c, "  %s: %s", LNG(USB_Keyboard_IRX), LNG(DEFAULT));
			else
				sprintf(c, "  %s: %s", LNG(USB_Keyboard_IRX), setting->usbkbd_file);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->kbdmap_file)==0)
				sprintf(c, "  %s: %s", LNG(USB_Keyboard_Map), LNG(DEFAULT));
			else
				sprintf(c, "  %s: %s", LNG(USB_Keyboard_Map), setting->kbdmap_file);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->CNF_Path)==0)
				sprintf(c, "  %s: %s", LNG(CNF_Path_override), LNG(NONE));
			else
				sprintf(c, "  %s: %s", LNG(CNF_Path_override), setting->CNF_Path);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->usbmass_file)==0)
				sprintf(c, "  %s: %s", LNG(USB_Mass_IRX), LNG(DEFAULT));
			else
				sprintf(c, "  %s: %s", LNG(USB_Mass_IRX), setting->usbmass_file);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->lang_file)==0)
				sprintf(c, "  %s: %s", LNG(Language_File), LNG(DEFAULT));
			else
				sprintf(c, "  %s: %s", LNG(Language_File), setting->lang_file);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if(strlen(setting->font_file)==0)
				sprintf(c, "  %s: %s", LNG(Font_File), LNG(DEFAULT));
			else
				sprintf(c, "  %s: %s", LNG(Font_File), setting->font_file);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			y += FONT_HEIGHT / 2;
			sprintf(c, "  %s", LNG(RETURN));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			//Cursor positioning section
			y = Menu_start_y + s*FONT_HEIGHT + FONT_HEIGHT /2;

			if(s>=max_s) y+=FONT_HEIGHT/2;
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if ((s==1)||(s==3)||(s==7)) { //resetIOP || usbkbd_used
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(Change));
				else
					sprintf(c, "ÿ0:%s", LNG(Change));
			} else if ((s==2)||(s==5)||(s==6)) { //numCNF || Init_Delay || timeout
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Add), LNG(Subtract));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Add), LNG(Subtract));
			} else if((s==4)||(s==8)||(s==9)||(s==10)||(s==11)||(s==12)||(s==13)) {
			//usbd_file||usbkbd_file||kbdmap_file||CNF_Path||usbmass_file
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Browse), LNG(Clear));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Browse), LNG(Clear));
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(OK));
				else
					sprintf(c, "ÿ0:%s", LNG(OK));
			}
			sprintf(tmp, " ÿ3:%s", LNG(Return));
			strcat(c, tmp);
			setScrTmp("", c);
		}//ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}//ends while
}//ends Config_Startup*/
//---------------------------------------------------------------------------
// Network settings GUI by Slam-Tilt
//---------------------------------------------------------------------------
/*void saveNetworkSettings(char *Message)
{
	char firstline[50];
	extern char ip[16];
	extern char netmask[16];
	extern char gw[16];
    int out_fd,in_fd;
    int ret=0,i=0;
	int size,sizeleft=0;
	char *ipconfigfile=0;

	// Default message, will get updated if save is sucessfull
	sprintf(Message,"%s", LNG(Saved_Failed));

	sprintf(firstline,"%s %s %s\n\r",ip,netmask,gw);


	mcSync(0,NULL,NULL);
	mcMkDir(0, 0, "SYS-CONF");
	mcSync(0, NULL, &ret);

	// This block looks at the existing ipconfig.dat and works out if there is
	// already any data beyond the first line. If there is it will get appended to the output
	// to new file later.

	in_fd = fioOpen("mc0:/SYS-CONF/IPCONFIG.DAT", O_RDONLY);
	if (in_fd >= 0) {

		size = fioLseek(in_fd, 0, SEEK_END);
		printf("size of existing file is %ibytes\n\r",size);

		ipconfigfile = (char *)malloc(size);

		fioLseek(in_fd, 0, SEEK_SET);
		fioRead(in_fd, ipconfigfile, size);


		for (i=0; (ipconfigfile[i] != 0 && i<=size); i++)

		{
			// printf("%i-%c\n\r",i,ipconfigfile[i]);
		}

		sizeleft=size-i;

		fioClose(in_fd);
	}

	// Writing the data out

	out_fd=fioOpen("mc0:/SYS-CONF/IPCONFIG.DAT", O_WRONLY | O_TRUNC | O_CREAT);
	if(out_fd >=0)
	{
		mcSync(0, NULL, &ret);
		fioWrite(out_fd,firstline,strlen(firstline));
		mcSync(0, NULL, &ret);

		// If we have any extra data, spit that out too.
		if (sizeleft > 0)
		{
			mcSync(0, NULL, &ret);
			fioWrite(out_fd,&ipconfigfile[i],sizeleft);
			mcSync(0, NULL, &ret);
		}

		sprintf(Message,"%s mc0:/SYS-CONF/IPCONFIG.DAT", LNG(Saved));

		fioClose(out_fd);

	}
}*/
//---------------------------------------------------------------------------
// Convert IP string to numbers
//---------------------------------------------------------------------------
/*void ipStringToOctet(char *ip, int ip_octet[4])
{

	// This takes a string (ip) representing an IP address and converts it
	// into an array of ints (ip_octet)
	// Rewritten 22/10/05

	char oct_str[5];
	int oct_cnt,i;

	oct_cnt = 0;
	oct_str[0]=0;

	for (i=0; ((i<=strlen(ip)) && (oct_cnt<4)); i++)
	{
		if ((ip[i] == '.') | (i==strlen(ip)))
		{
			ip_octet[oct_cnt] = atoi(oct_str);
			oct_cnt++;
			oct_str[0]=0;
		} else
			sprintf(oct_str,"%s%c",oct_str,ip[i]);
	}
}*/
//---------------------------------------------------------------------------
/*data_ip_struct BuildOctets(char *ip, char *nm, char *gw)
{

	// Populate 3 arrays with the ip address (as ints)

	data_ip_struct iplist;

	ipStringToOctet(ip,iplist.ip);
	ipStringToOctet(nm,iplist.nm);
	ipStringToOctet(gw,iplist.gw);

	return(iplist);
}*/
//---------------------------------------------------------------------------
/*void Config_Network(void)
{
	// Menu System for Network Settings Page.

	int s,l;
	int x, y;
	int event, post_event=0;
	char c[MAX_PATH];
	extern char ip[16];
	extern char netmask[16];
	extern char gw[16];
	data_ip_struct ipdata;
	char NetMsg[MAX_PATH] = "";

	event = 1;	//event = initial entry
	s=1;
	l=1;
	ipdata = BuildOctets(ip,netmask,gw);

	while(1)
	{
		//Pad response section
		waitPadReady(0, 0);
		if(readpad())
		{
			if(new_pad & PAD_UP)
			{
				event |= 2;  //event |= valid pad command
				if(s!=1) s--;
				else {s=5; l=1; }
			}
			else if(new_pad & PAD_DOWN)
			{
				event |= 2;  //event |= valid pad command
				if(s!=5) s++;
				else s=1;
				if(s>3) l=1;
			}
			else if(new_pad & PAD_LEFT)
			{
				event |= 2;  //event |= valid pad command
				if(s<4)
					if(l>1)
						l--;
			}
			else if(new_pad & PAD_RIGHT)
			{
				event |= 2;  //event |= valid pad command
				if(s<4)
					if(l<5)
						l++;
			}
			else if((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE) )
			{
				event |= 2;  //event |= valid pad command
				if((s<4) && (l>1))
				{
					if (s == 1)
					{
							if (ipdata.ip[l-2] > 0)
							{
								ipdata.ip[l-2]--;
							}
					}
					else if (s == 2)
					{
							if (ipdata.nm[l-2] > 0)
							{
								ipdata.nm[l-2]--;
							}
					}
					else if (s == 3)
					{
							if (ipdata.gw[l-2] > 0)
							{
								ipdata.gw[l-2]--;
							}
					}

				}
			}
			else if((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE))
			{
				event |= 2;  //event |= valid pad command
				if((s<4) && (l>1))
				{
					if (s == 1)
					{
							if (ipdata.ip[l-2] < 255)
							{
								ipdata.ip[l-2]++;
							}
					}
					else if (s == 2)
					{
							if (ipdata.nm[l-2] < 255)
							{
								ipdata.nm[l-2]++;
							}
					}
					else if (s == 3)
					{
							if (ipdata.gw[l-2] < 255)
							{
								ipdata.gw[l-2]++;
							}
					}

				}

				else if(s==4)
				{
					sprintf(ip,"%i.%i.%i.%i",ipdata.ip[0],ipdata.ip[1],ipdata.ip[2],ipdata.ip[3]);
					sprintf(netmask,"%i.%i.%i.%i",ipdata.nm[0],ipdata.nm[1],ipdata.nm[2],ipdata.nm[3]);
					sprintf(gw,"%i.%i.%i.%i",ipdata.gw[0],ipdata.gw[1],ipdata.gw[2],ipdata.gw[3]);

					saveNetworkSettings(NetMsg);
				}
				else
					return;
			}
			else if(new_pad & PAD_TRIANGLE)
				return;
		}

		if(event||post_event){ //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			x = Menu_start_x;
			y = Menu_start_y;

			printXY(LNG(NETWORK_SETTINGS), x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			y += FONT_HEIGHT / 2;

			int len = (strlen(LNG(IP_Address))+5>strlen(LNG(Netmask))+5)?
				strlen(LNG(IP_Address))+5:strlen(LNG(Netmask))+5;
			len = (len>strlen(LNG(Gateway))+5)? len:strlen(LNG(Gateway))+5;
			sprintf(c, "%s:", LNG(IP_Address));
			printXY(c, x+2*FONT_WIDTH, y, setting->color[3], TRUE, 0);
			sprintf(c, "%.3i . %.3i . %.3i . %.3i", ipdata.ip[0],ipdata.ip[1],ipdata.ip[2],ipdata.ip[3]);
			printXY(c, x+len*FONT_WIDTH, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "%s:", LNG(Netmask));
			printXY(c, x+2*FONT_WIDTH, y, setting->color[3], TRUE, 0);
			sprintf(c, "%.3i . %.3i . %.3i . %.3i", ipdata.nm[0],ipdata.nm[1],ipdata.nm[2],ipdata.nm[3]);
			printXY(c, x+len*FONT_WIDTH, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "%s:", LNG(Gateway));
			printXY(c, x+2*FONT_WIDTH, y, setting->color[3], TRUE, 0);
			sprintf(c, "%.3i . %.3i . %.3i . %.3i", ipdata.gw[0],ipdata.gw[1],ipdata.gw[2],ipdata.gw[3]);
			printXY(c, x+len*FONT_WIDTH, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s \"mc0:/SYS-CONF/IPCONFIG.DAT\"", LNG(Save_to));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			y += FONT_HEIGHT / 2;
			sprintf(c, "  %s", LNG(RETURN));
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			//Cursor positioning section
			y = Menu_start_y + s*FONT_HEIGHT + FONT_HEIGHT/2;

			if(s>=4) y+=FONT_HEIGHT/2;
			if(s>=5) y+=FONT_HEIGHT/2;
			if (l > 1)
				x += l*48 + 15;
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if ((s <4) && (l==1)) {
					sprintf(c, "%s", LNG(Right_DPad_to_Edit));
			} else if (s < 4) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", LNG(Add), LNG(Subtract));
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", LNG(Add), LNG(Subtract));
			} else if( s== 4) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(Save));
				else
					sprintf(c, "ÿ0:%s", LNG(Save));
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", LNG(OK));
				else
					sprintf(c, "ÿ0:%s", LNG(OK));
			}
			sprintf(tmp, " ÿ3:%s", LNG(Return));
			strcat(c, tmp);
			setScrTmp(NetMsg, c);
		}//ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}//ends while
}//ends Config_Network*/
//---------------------------------------------------------------------------
// Configuration for Free McBoot
//---------------------------------------------------------------------------

extern void icon_sys;
extern int size_icon_sys;
extern void icon_icn;
extern int size_icon_icn;

//Define Free McBoot paths where CNF is looked for
char cnf_path_fmcb[MAX_PATH] = "mc0:/BOOT/FUNTUNA.CNF";
char cnf_path_fmcb2[MAX_PATH] = "mc1:/BOOT/FUNTUNA.CNF";
char cnf_path_fmcb3[MAX_PATH] = "mass:/FUNTUNA.CNF";

char fmcbMsg[MAX_PATH] = "";

//CNF settings
typedef struct
{
	int hacked_OSDSYS;
	int OSDSYS_menu_x;
	int OSDSYS_menu_y;
	int OSDSYS_enter_x;
	int OSDSYS_enter_y;
	int OSDSYS_version_x;
	int OSDSYS_version_y;
	int Skip_MC;
	int Skip_HDD;
	int Skip_Disc;
	int Skip_Logo;
	int Inner_Browser;
	int OSD_scroll;
	int OSD_mvelocity;
	int OSD_accel;
	char OSD_cursor[2][11];     // 0 left, 1 right
	char OSD_delimiter[2][81];  // 0 top, 1 bottom
	int OSD_displayitems;
	char OSD_tvmode[5];
	int Debug;
	int Fastboot;
	float pad_delay;
	//	char selected_color[MAX_PATH];
	//	char unselected_color[MAX_PATH];
	char LK_E1_Path[17][MAX_PATH];
	char LK_E2_Path[17][MAX_PATH];
	char LK_E3_Path[17][MAX_PATH];
	char OSD_Name[100][MAX_PATH];
	char OSD_E1_Path[100][MAX_PATH];
	char OSD_E2_Path[100][MAX_PATH];
	char OSD_E3_Path[100][MAX_PATH];
	char ESR_Path[3][MAX_PATH];
} FMCB;

u32 OSDSYS_selected_color[4];
u32 OSDSYS_unselected_color[4];

FMCB *fmcb = NULL;
FMCB *tmpfmcb;

//Launch Keys identifiers
char LK_ID_fmcb[17][10] = {
    "Auto",
    "Circle",
    "Cross",
    "Square",
    "Triangle",
    "L1",
    "R1",
    "L2",
    "R2",
    "L3",
    "R3",
    "Up",
    "Down",
    "Left",
    "Right",
    "Start",
    "Select"};
//---------------------------------------------------------------------------
//storeFmcbCNF will save Free McBoot settings to a RAM area
//------------------------------
size_t storeFmcbCNF(char *cnf_buf)
{
	size_t CNF_size, CNF_step;
	int i;

	sprintf(cnf_buf, "CNF_version = 1\r\n%n", &CNF_size);  //Start CNF with version header

	//fmcb core options
	sprintf(cnf_buf + CNF_size,

	        "Debug_Screen = %d\r\n"
	        "FastBoot = %d\r\n"
	        "ESR_Path_E1 = %s\r\n"
	        "ESR_Path_E2 = %s\r\n"
	        "ESR_Path_E3 = %s\r\n"
	        "pad_delay = %.0f\r\n"

	        "%n",            // %n causes NO output, but only a measurement
	        fmcb->Debug,     //Debug Screen
	        fmcb->Fastboot,  //Fastboot disc
	        fmcb->ESR_Path[0],
	        fmcb->ESR_Path[1],
	        fmcb->ESR_Path[2],
	        fmcb->pad_delay,

	        &CNF_step  // This variable measures the size of sprintf data
	);
	CNF_size += CNF_step;

	//Launch keys
	for (i = 0; i < 17; i++) {  //Loop to save the ELF paths for launch keys
		if (fmcb->LK_E1_Path[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "LK_%s_E1 = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        LK_ID_fmcb[i], fmcb->LK_E1_Path[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}

		if (fmcb->LK_E2_Path[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "LK_%s_E2 = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        LK_ID_fmcb[i], fmcb->LK_E2_Path[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}

		if (fmcb->LK_E2_Path[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "LK_%s_E3 = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        LK_ID_fmcb[i], fmcb->LK_E3_Path[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}
	}  //ends for*/

	//Hacked OSDSYS options
	sprintf(cnf_buf + CNF_size,
	        "hacked_OSDSYS = %d\r\n"
	        "OSDSYS_video_mode = %s\r\n"
	        "OSDSYS_Skip_Disc = %d\r\n"
	        "OSDSYS_Skip_Logo = %d\r\n"
	        "OSDSYS_Inner_Browser = %d\r\n"
	        "OSDSYS_selected_color = 0x%02X,0x%02X,0x%02X,0x%02X\r\n"
	        "OSDSYS_unselected_color = 0x%02X,0x%02X,0x%02X,0x%02X\r\n"
	        "OSDSYS_scroll_menu = %d\r\n"
	        "OSDSYS_menu_x = %d\r\n"
	        "OSDSYS_menu_y = %d\r\n"
	        "OSDSYS_enter_x = %d\r\n"
	        "OSDSYS_enter_y = %d\r\n"
	        "OSDSYS_version_x = %d\r\n"
	        "OSDSYS_version_y = %d\r\n"
	        "OSDSYS_cursor_max_velocity = %d\r\n"
	        "OSDSYS_cursor_acceleration = %d\r\n"
	        "OSDSYS_left_cursor = %s\r\n"
	        "OSDSYS_right_cursor = %s\r\n"
	        "OSDSYS_menu_top_delimiter = %s\r\n"
	        "OSDSYS_menu_bottom_delimiter = %s\r\n"
	        "OSDSYS_num_displayed_items = %d\r\n"
	        "OSDSYS_Skip_MC = %d\r\n"
	        "OSDSYS_Skip_HDD = %d\r\n"

	        "%n",  // %n causes NO output, but only a measurement
	        fmcb->hacked_OSDSYS,
	        fmcb->OSD_tvmode,
	        fmcb->Skip_Disc,
	        fmcb->Skip_Logo,
	        fmcb->Inner_Browser,
	        OSDSYS_selected_color[0],
	        OSDSYS_selected_color[1],
	        OSDSYS_selected_color[2],
	        OSDSYS_selected_color[3],
	        OSDSYS_unselected_color[0],
	        OSDSYS_unselected_color[1],
	        OSDSYS_unselected_color[2],
	        OSDSYS_unselected_color[3],
	        fmcb->OSD_scroll,
	        fmcb->OSDSYS_menu_x,
	        fmcb->OSDSYS_menu_y,
	        fmcb->OSDSYS_enter_x,
	        fmcb->OSDSYS_enter_y,
	        fmcb->OSDSYS_version_x,
	        fmcb->OSDSYS_version_y,
	        fmcb->OSD_mvelocity,
	        fmcb->OSD_accel,
	        fmcb->OSD_cursor[0],
	        fmcb->OSD_cursor[1],
	        fmcb->OSD_delimiter[0],
	        fmcb->OSD_delimiter[1],
	        fmcb->OSD_displayitems,
	        fmcb->Skip_MC,
	        fmcb->Skip_HDD,

	        &CNF_step  // This variable measures the size of sprintf data
	);
	CNF_size += CNF_step;

	//OSD names and keys
	for (i = 0; i < 100; i++) {  //Loop to save the OSD names
		if (fmcb->OSD_Name[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "name_OSDSYS_ITEM_%d = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        i + 1, fmcb->OSD_Name[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}

		if (fmcb->OSD_E1_Path[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "path1_OSDSYS_ITEM_%d = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        i + 1, fmcb->OSD_E1_Path[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}

		if (fmcb->OSD_E2_Path[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "path2_OSDSYS_ITEM_%d = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        i + 1, fmcb->OSD_E2_Path[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}

		if (fmcb->OSD_E3_Path[i][0] != 0) {
			sprintf(cnf_buf + CNF_size,
			        "path3_OSDSYS_ITEM_%d = %s\r\n"
			        "%n",  // %n causes NO output, but only a measurement
			        i + 1, fmcb->OSD_E3_Path[i],
			        &CNF_step  // This variable measures the size of sprintf data
			);

			CNF_size += CNF_step;
		}
	}  //ends for*/

	return CNF_size;
}  //endfunc storeFmcbCNF
//---------------------------------------------------------------------------
//saveFmcbCNF will save Free McBoot settings to a CNF file
//------------------------------
int saveFmcbCNF(char *fmcbMsg, char *CNF)
{
	int ret = 0, fd;
	char tmp[26 * MAX_PATH + 30 * MAX_PATH];
	char cnf_path[MAX_PATH], icon_path[MAX_PATH];
	size_t CNF_size;

	CNF_size = storeFmcbCNF(tmp);
	if (strncmp(CNF, "mass:", 5)) {  //if not the mass device
		mcSync(0, NULL, NULL);
		//mcMkDir(CNF[2]-'0', 0, "FMCB-CNF");
		mcMkDir(CNF[2] - '0', 0, "BOOT");
		mcSync(0, NULL, &ret);
		/*
	//icon.sys
	icon_path[0]=0;
	strncat(icon_path, CNF, 14);
	strcat(icon_path, "icon.sys");
	if (!exists(icon_path)){
		if ((fd=genOpen(icon_path,O_CREAT|O_WRONLY|O_TRUNC)) < 0){
			sprintf(fmcbMsg, "Failed to open %s", icon_path); 
			return -1; //Failed open
		}
		ret = genWrite(fd,&icon_sys,size_icon_sys);
		if(ret!=size_icon_sys){
			sprintf(fmcbMsg, "Failed to write %s", icon_path);
			ret = -2; //Failed writing
		}
		genClose(fd);
	
	//FMCB.icn
		icon_path[0]=0;
		strncat(icon_path, CNF, 14);
		//strcat(icon_path, "FMCB.icn");
		strcat(icon_path, "sysconf.icn");
		if (!exists(icon_path)){
			if ((fd=genOpen(icon_path,O_CREAT|O_WRONLY|O_TRUNC)) < 0){
				sprintf(fmcbMsg, "Failed to open %s", icon_path); 
				return -1; //Failed open
			}
			ret = genWrite(fd,&icon_icn,size_icon_icn);
			if(ret!=size_icon_icn){
				sprintf(fmcbMsg, "Failed to write %s", icon_path);
				ret = -2; //Failed writing
			}
			genClose(fd);
		}
	
	}*/
	}
	//FREEMCB.CNF
	ret = genFixPath(CNF, cnf_path);
	if ((ret < 0) || ((fd = genOpen(cnf_path, O_CREAT | O_WRONLY | O_TRUNC)) < 0)) {
		sprintf(fmcbMsg, "Failed to open %s", cnf_path);
		return -1;  //Failed open
	}
	ret = genWrite(fd, &tmp, CNF_size);
	if (ret != CNF_size) {
		sprintf(fmcbMsg, "Failed to write %s", cnf_path);
		ret = -2;  //Failed writing
	}
	genClose(fd);

	//------------------------------------------------------------------------------------------------------------------------------------------
/*#define ARRAY_ENTRIES 64

	///timestamp
	static sceMcTblGetDir mcDirAAA[64] __attribute__((aligned(64)));
	static sceMcStDateTime new_timestamp;  //Maxium Timestamp
	new_timestamp.Resv2 = 0;
	new_timestamp.Sec = 59;
	new_timestamp.Min = 59;
	new_timestamp.Hour = 23;
	new_timestamp.Day = 31;
	new_timestamp.Month = 12;
	new_timestamp.Year = 2099;
	mcDirAAA->_Modify = new_timestamp;
	mcDirAAA->_Create = new_timestamp;
	mcSetFileInfo(0, 0, "BXEXEC-FUNTUNA", mcDirAAA, 0x02);
	mcSync(0, NULL, NULL);
	///\timestamp


*/
	//------------------------------------------------------------------------------------------------------------------------------------------*/
	//*/
	sprintf(fmcbMsg, "Saved %s", CNF);
	return ret;
}  //endfunc saveFmcbCNF
//----------------------------------------------------------------
// Initialize Free McBoot CNF configuration
//----------------------------------------------------------------
void initConfig_fmcb(int mode)  //mode 0 = failed init, mode 1 = succesful init
{
	int i;

	if (fmcb != NULL)
		free(fmcb);
	fmcb = (FMCB *)malloc(sizeof(FMCB));

	fmcb->Fastboot = 1;
	fmcb->Debug = 0;
	fmcb->pad_delay = 0;

	//OSDSYS settings
	fmcb->hacked_OSDSYS = 1;
	strcpy(fmcb->OSD_tvmode, "AUTO");
	fmcb->Skip_MC = 1;
	fmcb->Skip_HDD = 1;
	fmcb->Skip_Disc = 1;
	fmcb->Skip_Logo = 1;
	fmcb->Inner_Browser = 0;
	fmcb->OSDSYS_menu_x = 320;
	fmcb->OSDSYS_menu_y = 110;
	fmcb->OSDSYS_enter_x = -1;
	fmcb->OSDSYS_enter_y = -1;
	fmcb->OSDSYS_version_x = -1;
	fmcb->OSDSYS_version_y = -1;
	fmcb->OSD_scroll = 1;
	fmcb->OSD_mvelocity = 1000;
	fmcb->OSD_accel = 100;
	fmcb->OSD_displayitems = 7;
	strcpy(fmcb->OSD_cursor[0], "o009");
	strcpy(fmcb->OSD_cursor[1], "o008");
	strcpy(fmcb->OSD_delimiter[0], "y-99r0.00FunTuna  r0.60c1[r0.60 %VER%r0.60]y-00");
	strcpy(fmcb->OSD_delimiter[1], "c0r0.60y+99 based on FreeMcBoot 1.8 y-00r0.00");
	//selected color
	OSDSYS_selected_color[0] = strtol("0x10", NULL, 16);    // hex base
	OSDSYS_selected_color[1] = strtol("0x80", NULL, 16);    // hex base
	OSDSYS_selected_color[2] = strtol("0xE0", NULL, 16);    // hex base
	OSDSYS_selected_color[3] = strtol("0x80", NULL, 16);    // hex base
	                                                        //selected color
	OSDSYS_unselected_color[0] = strtol("0x33", NULL, 16);  // hex base
	OSDSYS_unselected_color[1] = strtol("0x33", NULL, 16);  // hex base
	OSDSYS_unselected_color[2] = strtol("0x33", NULL, 16);  // hex base
	OSDSYS_unselected_color[3] = strtol("0x80", NULL, 16);  // hex base

	//OSD names and keys
	for (i = 0; i < 100; i++) {  //all clean by default
		fmcb->OSD_Name[i][0] = 0;
		fmcb->OSD_E1_Path[i][0] = 0;
		fmcb->OSD_E3_Path[i][0] = 0;
		fmcb->OSD_E2_Path[i][0] = 0;
	}  //ends for*/
	if (!mode) {
		strcpy(fmcb->OSD_Name[0], "uLaunchELF");  //Menu names
		strcpy(fmcb->OSD_Name[1], "ESR");
		strcpy(fmcb->OSD_Name[2], "OpenPS2Loader");
		strcpy(fmcb->OSD_Name[3], "Simple Media System");
		strcpy(fmcb->OSD_Name[99], "Free McBoot Configurator");

		strcpy(fmcb->OSD_E1_Path[0], "mass:/BOOT/ULE.ELF");  //Item 1
		strcpy(fmcb->OSD_E2_Path[0], "mc?:/BOOT/ULE.ELF");
		strcpy(fmcb->OSD_E3_Path[0], "mc?:/B?DATA-SYSTEM/ULE.ELF");

		strcpy(fmcb->OSD_E1_Path[1], "mass:/BOOT/ESR.ELF");  //Item 2
		strcpy(fmcb->OSD_E2_Path[1], "mc?:/BOOT/ESR.ELF");
		strcpy(fmcb->OSD_E3_Path[1], "mc?:/B?DATA-SYSTEM/ESR.ELF");

		strcpy(fmcb->OSD_E1_Path[2], "mass:/BOOT/OPNPS2LD.ELF");  //Item 3
		strcpy(fmcb->OSD_E2_Path[2], "mc?:/BOOT/OPNPS2LD.ELF");
		strcpy(fmcb->OSD_E3_Path[2], "mc?:/B?DATA-SYSTEM/OPNPS2LD.ELF");

		strcpy(fmcb->OSD_E1_Path[3], "mass:/BOOT/SMS.ELF");  //Item 4
		strcpy(fmcb->OSD_E2_Path[3], "mc?:/BOOT/SMS.ELF");
		strcpy(fmcb->OSD_E3_Path[3], "mc?:/B?DATA-SYSTEM/SMS.ELF");

		strcpy(fmcb->OSD_E1_Path[99], "mc?:/BOOT/FMCB_CFG.ELF");  //Item 5
		//strcpy(fmcb->OSD_E2_Path[99], "mc?:/BOOT/BOOT.ELF");
		//strcpy(fmcb->OSD_E3_Path[99], "mc?:/B?DATA-SYSTEM/BOOT.ELF");

		for (i = 0; i < 17; i++) {
			strcpy(fmcb->LK_E1_Path[i], "OSDSYS");  //By default any non L-R button press launches browser
			fmcb->LK_E2_Path[i][0] = 0;
			fmcb->LK_E3_Path[i][0] = 0;
		}
		//Auto
		/*	strcpy(fmcb->LK_E1_Path[0], "mass:/BOOT/BOOT.ELF");
	strcpy(fmcb->LK_E2_Path[0], "mc?:/BOOT/BOOT.ELF");
	strcpy(fmcb->LK_E3_Path[0], "mc?:/B?DATA-SYSTEM/BOOT.ELF");

	strcpy(fmcb->LK_E1_Path[5], "mass:/BOOT/BOOT3.ELF");//L1
	strcpy(fmcb->LK_E1_Path[6], "mass:/BOOT/BOOT1.ELF");//R1
	strcpy(fmcb->LK_E1_Path[7], "mass:/BOOT/BOOT4.ELF");//L2
	strcpy(fmcb->LK_E1_Path[8], "mass:/BOOT/BOOT2.ELF");//R2
	
	strcpy(fmcb->LK_E2_Path[5], "mc?:/BOOT/BOOT3.ELF");//L1
	strcpy(fmcb->LK_E2_Path[6], "mc?:/BOOT/BOOT1.ELF");//R1
	strcpy(fmcb->LK_E2_Path[7], "mc?:/BOOT/BOOT4.ELF");//L2
	strcpy(fmcb->LK_E2_Path[8], "mc?:/BOOT/BOOT2.ELF");//R2
	
	strcpy(fmcb->LK_E3_Path[5], "mc?:/B?DATA-SYSTEM/BOOT3.ELF");//L1
	strcpy(fmcb->LK_E3_Path[6], "mc?:/B?DATA-SYSTEM/BOOT1.ELF");//R1
	strcpy(fmcb->LK_E3_Path[7], "mc?:/B?DATA-SYSTEM/BOOT4.ELF");//L2
	strcpy(fmcb->LK_E3_Path[8], "mc?:/B?DATA-SYSTEM/BOOT2.ELF");//R2
*/
		strcpy(fmcb->LK_E2_Path[0], "OSDSYS");
		strcpy(fmcb->LK_E3_Path[0], "OSDSYS");

		strcpy(fmcb->LK_E1_Path[5], "OSDSYS");  //L1
		strcpy(fmcb->LK_E1_Path[6], "OSDSYS");  //R1
		strcpy(fmcb->LK_E1_Path[7], "OSDSYS");  //L2
		strcpy(fmcb->LK_E1_Path[8], "OSDSYS");  //R2

		strcpy(fmcb->LK_E2_Path[5], "OSDSYS");  //L1
		strcpy(fmcb->LK_E2_Path[6], "OSDSYS");  //R1
		strcpy(fmcb->LK_E2_Path[7], "OSDSYS");  //L2
		strcpy(fmcb->LK_E2_Path[8], "OSDSYS");  //R2

		strcpy(fmcb->LK_E3_Path[5], "OSDSYS");  //L1
		strcpy(fmcb->LK_E3_Path[6], "OSDSYS");  //R1
		strcpy(fmcb->LK_E3_Path[7], "OSDSYS");  //L2
		strcpy(fmcb->LK_E3_Path[8], "OSDSYS");  //R2
		                                        //start
		strcpy(fmcb->LK_E1_Path[15], "mc?:/BOOT/FMCB_CFG.ELF");
		strcpy(fmcb->LK_E2_Path[15], "OSDSYS");
	}

	//ESR paths

	strcpy(fmcb->ESR_Path[0], "mass:/BOOT/ESR.ELF");
	strcpy(fmcb->ESR_Path[1], "mc?:/BOOT/ESR.ELF");
	strcpy(fmcb->ESR_Path[2], "mc?:/B?DATA-SYSTEM/ESR.ELF");

}  // ends initConfig_fmcb
//----------------------------------------------------------------
// Load Free McBoot's CNF
//----------------------------------------------------------------
int loadConfig_fmcb(char *fmcbMsg, char *path)
{
	int i, j, fd, var_cnt, CNF_version;
	size_t CNF_size;
	char tsts[20];
	char hexvalue_buf[20];
	unsigned char *RAM_p, *CNF_p, *name, *value;

	//int test;//enable to test usbmodule loading

	initConfig_fmcb(1);
	if (!strncmp(path, "mass:", 5))
		//test = loadUsbModules();
		loadUsbModules();

	fd = -1;
	fd = open(path, O_RDONLY);
	/*if (fd < 0) {
		path[2] = '1';
		fd = open(path, O_RDONLY);
	}*/
	if (fd < 0) {
	failed_load:
		close(fd);
		initConfig_fmcb(0);
		//sprintf(fmcbMsg, "Failed to load %s %d", path, test);
		sprintf(fmcbMsg, "Failed to load %s", path);
		return -1;
	}  // This point is only reached after succefully opening CNF

	CNF_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	RAM_p = (char *)malloc(CNF_size);
	CNF_p = RAM_p;
	if (CNF_p == NULL) {
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
		if (!strcmp(name, "Debug_Screen")) {
			fmcb->Debug = atoi(value);
			continue;
		}
		if (!strcmp(name, "FastBoot")) {
			fmcb->Fastboot = atoi(value);
			continue;
		}
		if (!strcmp(name, "pad_delay")) {
			fmcb->pad_delay = atoi(value);
			continue;
		}
		//OSD settings
		if (!strcmp(name, "hacked_OSDSYS")) {
			fmcb->hacked_OSDSYS = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_video_mode")) {
			strcpy(fmcb->OSD_tvmode, value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_Disc")) {
			fmcb->Skip_Disc = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_Logo")) {
			fmcb->Skip_Logo = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Inner_Browser")) {
			fmcb->Inner_Browser = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_selected_color")) {
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					hexvalue_buf[j] = value[j];
				}
				OSDSYS_selected_color[i] = strtol(hexvalue_buf, NULL, 16);  // hex base
				value += 5;
			}
			continue;
		}
		if (!strcmp(name, "OSDSYS_unselected_color")) {
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					hexvalue_buf[j] = value[j];
				}
				OSDSYS_unselected_color[i] = strtol(hexvalue_buf, NULL, 16);  // hex base
				value += 5;
			}
			continue;
		}
		if (!strcmp(name, "OSDSYS_scroll_menu")) {
			fmcb->OSD_scroll = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_x")) {
			fmcb->OSDSYS_menu_x = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_y")) {
			fmcb->OSDSYS_menu_y = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_enter_x")) {
			fmcb->OSDSYS_enter_x = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_enter_y")) {
			fmcb->OSDSYS_enter_y = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_version_x")) {
			fmcb->OSDSYS_version_x = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_version_y")) {
			fmcb->OSDSYS_version_y = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_cursor_max_velocity")) {
			fmcb->OSD_mvelocity = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_cursor_acceleration")) {
			fmcb->OSD_accel = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_left_cursor")) {
			strcpy(fmcb->OSD_cursor[0], value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_right_cursor")) {
			strcpy(fmcb->OSD_cursor[1], value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_top_delimiter")) {
			strcpy(fmcb->OSD_delimiter[0], value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_menu_bottom_delimiter")) {
			strcpy(fmcb->OSD_delimiter[1], value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_num_displayed_items")) {
			fmcb->OSD_displayitems = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_MC")) {
			fmcb->Skip_MC = atoi(value);
			continue;
		}
		if (!strcmp(name, "OSDSYS_Skip_HDD")) {
			fmcb->Skip_HDD = atoi(value);
			continue;
		}
		//OSD names
		for (i = 0; i < 100; i++) {
			sprintf(tsts, "name_OSDSYS_ITEM_%d", i + 1);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->OSD_Name[i], value);
				//scr_printf("%s\n", LK_E1_Path[i]);
				break;
			}
		}
		//OSD item path1
		for (i = 0; i < 100; i++) {
			sprintf(tsts, "path1_OSDSYS_ITEM_%d", i + 1);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->OSD_E1_Path[i], value);
				//scr_printf("%s\n", LK_E1_Path[i]);
				break;
			}
		}
		//OSD item path2
		for (i = 0; i < 100; i++) {
			sprintf(tsts, "path2_OSDSYS_ITEM_%d", i + 1);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->OSD_E2_Path[i], value);
				//scr_printf("%s\n", LK_E1_Path[i]);
				break;
			}
		}
		//OSD item path3
		for (i = 0; i < 100; i++) {
			sprintf(tsts, "path3_OSDSYS_ITEM_%d", i + 1);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->OSD_E3_Path[i], value);
				//scr_printf("%s\n", LK_E1_Path[i]);
				break;
			}
		}

		//first launch key
		for (i = 0; i < 17; i++) {
			sprintf(tsts, "LK_%s_E1", LK_ID_fmcb[i]);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->LK_E1_Path[i], value);
				//scr_printf("%s\n", LK_E1_Path[i]);
				break;
			}
		}
		//second launch key
		for (i = 0; i < 17; i++) {
			sprintf(tsts, "LK_%s_E2", LK_ID_fmcb[i]);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->LK_E2_Path[i], value);
				//scr_printf("%s\n", LK_E2_Path[i]);
				break;
			}
		}
		//third launch key
		for (i = 0; i < 17; i++) {
			sprintf(tsts, "LK_%s_E3", LK_ID_fmcb[i]);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->LK_E3_Path[i], value);
				//scr_printf("%s\n", LK_E3_Path[i]);
				break;
			}
		}
		//ESR paths
		for (i = 0; i < 3; i++) {
			sprintf(tsts, "ESR_Path_E%d", i + 1);
			if (!strcmp(name, tsts)) {
				strcpy(fmcb->ESR_Path[i], value);
				//scr_printf("%s\n", LK_E1_Path[i]);
				break;
			}
		}
	}  // ends for
	free(RAM_p);
	sprintf(fmcbMsg, "Loaded %s", path);
	return 0;
}  //ends loadConfig_fmcb
void setLK(int LKN, int mode, int s)
{  //mode 1-> map file to mc? instead of mc0/1

	char c[MAX_PATH];

	if (LKN == 1) {
		getFilePath(fmcb->LK_E1_Path[s], TRUE);
		if (!strncmp(fmcb->LK_E1_Path[s], "mc", 2)) {
			if (!strncmp(fmcb->LK_E1_Path[s] + 7, "DATA-SYSTEM", 11))
				fmcb->LK_E1_Path[s][6] = '?';
			if (mode == 1) {
				sprintf(c, "mc?%s", &fmcb->LK_E1_Path[s][3]);
				strcpy(fmcb->LK_E1_Path[s], c);
			}
		}
/*			else if (strncmp(fmcb->LK_E1_Path[s], "mass", 4) != 0){
				if (strlen(fmcb->LK_E1_Path[s]) > 0){//if cancelled browsing, do not print the message
					sprintf(fmcbMsg, "Incompatible path selected");
					fmcb->LK_E1_Path[s][0]=0;
				}
			}
*/	}
	else if (LKN == 2){
	getFilePath(fmcb->LK_E2_Path[s], TRUE);
	if (!strncmp(fmcb->LK_E2_Path[s], "mc", 2)) {
		if (!strncmp(fmcb->LK_E2_Path[s] + 7, "DATA-SYSTEM", 11))
			fmcb->LK_E2_Path[s][6] = '?';
		if (mode == 1) {
			sprintf(c, "mc?%s", &fmcb->LK_E2_Path[s][3]);
			strcpy(fmcb->LK_E2_Path[s], c);
		}
	}
/*			else if (strncmp(fmcb->LK_E2_Path[s], "mass", 4) != 0){
				if (strlen(fmcb->LK_E2_Path[s]) > 0){//if cancelled browsing, do not print the message
					sprintf(fmcbMsg, "Incompatible path selected");
					fmcb->LK_E2_Path[s][0]=0;
				}
			}
*/	}
	else{
	getFilePath(fmcb->LK_E3_Path[s], TRUE);
	if (!strncmp(fmcb->LK_E3_Path[s], "mc", 2)) {
		if (!strncmp(fmcb->LK_E3_Path[s] + 7, "DATA-SYSTEM", 11))
			fmcb->LK_E3_Path[s][6] = '?';
		if (mode == 1) {
			sprintf(c, "mc?%s", &fmcb->LK_E3_Path[s][3]);
			strcpy(fmcb->LK_E3_Path[s], c);
		}
	}
/*			else if (strncmp(fmcb->LK_E3_Path[s], "mass", 4) != 0){
				if (strlen(fmcb->LK_E3_Path[s]) > 0){//if cancelled browsing, do not print the message
					sprintf(fmcbMsg, "Incompatible path selected");
					fmcb->LK_E3_Path[s][0]=0;
				}
			}
*/	}
}
//---------------------------------------------------------------------------
// Configuration menu for Free McBoot launch keys
//---------------------------------------------------------------------------
void Config_fmcb_key(int LKN)
{

	char c[MAX_PATH];
	//char fmcbMsg[MAX_PATH] = "";
	int i;
	int s;
	int MAX_S = 17;
	int x, y;
	int event, post_event = 0;

	tmpfmcb = fmcb;
	fmcb = (FMCB *)malloc(sizeof(FMCB));
	*fmcb = *tmpfmcb;

	event = 1;  //event = initial entry
	s = 0;
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad & PAD_UP) {
				event |= 2;  //event |= valid pad command
				if (s != 0)
					s--;
				else
					s = MAX_S;
			} else if (new_pad & PAD_DOWN) {
				event |= 2;  //event |= valid pad command
				if (s != MAX_S)
					s++;
				else
					s = 0;
			} else if (new_pad & PAD_LEFT) {
				event |= 2;  //event |= valid pad command
				s = s - 5;
				if (s <= 0)
					s = 0;
			} else if (new_pad & PAD_RIGHT) {
				event |= 2;  //event |= valid pad command
				s = s + 5;
				if (s >= MAX_S)
					s = MAX_S;
			} else if ((new_pad & PAD_SQUARE) && (s < 17)) {
				event |= 2;  //event |= valid pad command
				if (s < 17) {
					sprintf(fmcbMsg, " ");
					setLK(LKN, 1, s);
				}
			} else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s < 17) {
					if (LKN == 1)
						fmcb->LK_E1_Path[s][0] = 0;
					else if (LKN == 2)
						fmcb->LK_E2_Path[s][0] = 0;
					else
						fmcb->LK_E3_Path[s][0] = 0;
				}
			} else if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s < 17) {
					setLK(LKN, 0, s);
				} else if (s == MAX_S)
					goto return_fmcb;

			} else if (new_pad & PAD_TRIANGLE) {
			return_fmcb:
				free(tmpfmcb);
				free(fmcb);
				fmcbMsg[0] = 0;
				return;
			} else if (new_pad & PAD_START) {
				event |= 2;  //event |= valid pad command
				if (LKN == 1)
					RunElf(fmcbMsg, fmcb->LK_E1_Path[s]);
				else if (LKN == 2)
					RunElf(fmcbMsg, fmcb->LK_E2_Path[s]);
				else
					RunElf(fmcbMsg, fmcb->LK_E3_Path[s]);
			} else if (new_pad & PAD_SELECT) {
				event |= 2;  //event |= valid pad command
				if (LKN == 1) {
					if (!strcmp(fmcb->LK_E1_Path[s], "OSDSYS"))
						sprintf(fmcb->LK_E1_Path[s], "FASTBOOT");
					else if (!strcmp(fmcb->LK_E1_Path[s], "FASTBOOT"))
						sprintf(fmcb->LK_E1_Path[s], "OSDMENU");
					else
						sprintf(fmcb->LK_E1_Path[s], "OSDSYS");
				} else if (LKN == 2) {
					if (!strcmp(fmcb->LK_E2_Path[s], "OSDSYS"))
						sprintf(fmcb->LK_E2_Path[s], "FASTBOOT");
					else if (!strcmp(fmcb->LK_E2_Path[s], "FASTBOOT"))
						sprintf(fmcb->LK_E2_Path[s], "OSDMENU");
					else
						sprintf(fmcb->LK_E2_Path[s], "OSDSYS");
				} else if (!strcmp(fmcb->LK_E3_Path[s], "OSDSYS"))
					sprintf(fmcb->LK_E3_Path[s], "FASTBOOT");
				else if (!strcmp(fmcb->LK_E3_Path[s], "FASTBOOT"))
					sprintf(fmcb->LK_E3_Path[s], "OSDMENU");
				else
					sprintf(fmcb->LK_E3_Path[s], "OSDSYS");
			}
		}  //end if(readpad())

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			//if(s < SHOW_TITLES) localMsg = setting->LK_Title[s];
			//else                localMsg = "";

			x = Menu_start_x;
			y = Menu_start_y;
			sprintf(c, "E%d Button Settings", LKN);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			for (i = 0; i < 17; i++) {
				switch (i) {
					case 0:
						strcpy(c, "  Auto   : ");
						break;
					case 1:
						strcpy(c, "  ÿ0     : ");
						break;
					case 2:
						strcpy(c, "  ÿ1     : ");
						break;
					case 3:
						strcpy(c, "  ÿ2     : ");
						break;
					case 4:
						strcpy(c, "  ÿ3     : ");
						break;
					case 5:
						strcpy(c, "  L1     : ");
						break;
					case 6:
						strcpy(c, "  R1     : ");
						break;
					case 7:
						strcpy(c, "  L2     : ");
						break;
					case 8:
						strcpy(c, "  R2     : ");
						break;
					case 9:
						strcpy(c, "  L3     : ");
						break;
					case 10:
						strcpy(c, "  R3     : ");
						break;
					case 11:
						strcpy(c, "  UP     : ");
						break;
					case 12:
						strcpy(c, "  DOWN   : ");
						break;
					case 13:
						strcpy(c, "  LEFT   : ");
						break;
					case 14:
						strcpy(c, "  RIGHT  : ");
						break;
					case 15:
						strcpy(c, "  START  : ");
						break;
					case 16:
						strcpy(c, "  SELECT : ");
						break;
				}
				if (LKN == 1)
					strcat(c, fmcb->LK_E1_Path[i]);
				else if (LKN == 2)
					strcat(c, fmcb->LK_E2_Path[i]);
				else
					strcat(c, fmcb->LK_E3_Path[i]);
				printXY(c, x, y, setting->color[3], TRUE, 0);
				y += FONT_HEIGHT;
			}

			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Return");
			printXY(c, x, y, setting->color[3], TRUE, 0);

			//Cursor positioning section
			y = Menu_start_y + (s + 1) * FONT_HEIGHT;
			if (s > 16)
				y += FONT_HEIGHT / 2;
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if (s < 17) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s ÿ2:%s SELECT:Set special START:Run", "Browse", "Clear", "Map to any MC");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s ÿ2:%s SELECT:Set special START:Run", "Browse", "Clear", "Map to any MC");
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "OK");
				else
					sprintf(c, "ÿ0:%s", "OK");
			}
			sprintf(tmp, " ÿ3:%s", "Return");
			strcat(c, tmp);
			setScrTmp(fmcbMsg, c);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}  //ends while

}  //ends Config_fmcb_key
//---------------------------------------------------------------------------
// Configuration menu for Hacked OSDSYS
//---------------------------------------------------------------------------
void setOSD(int item, int mode, int epath)
{  //mode 1-> map file to mc? instead of mc0/1

	char c[MAX_PATH];

	if (epath == 1) {
		getFilePath(fmcb->OSD_E1_Path[item], TRUE);
		if (!strncmp(fmcb->OSD_E1_Path[item], "mc", 2)) {
			if (!strncmp(fmcb->OSD_E1_Path[item] + 7, "DATA-SYSTEM", 11))
				fmcb->OSD_E1_Path[item][6] = '?';
			if (mode == 1) {
				sprintf(c, "mc?%s", &fmcb->OSD_E1_Path[item][3]);
				strcpy(fmcb->OSD_E1_Path[item], c);
			}
		}
	} else if (epath == 2) {
		getFilePath(fmcb->OSD_E2_Path[item], TRUE);
		if (!strncmp(fmcb->OSD_E2_Path[item], "mc", 2)) {
			if (!strncmp(fmcb->OSD_E2_Path[item] + 7, "DATA-SYSTEM", 11))
				fmcb->OSD_E2_Path[item][6] = '?';
			if (mode == 1) {
				sprintf(c, "mc?%s", &fmcb->OSD_E2_Path[item][3]);
				strcpy(fmcb->OSD_E2_Path[item], c);
			}
		}
	} else if (epath == 3) {
		getFilePath(fmcb->OSD_E3_Path[item], TRUE);
		if (!strncmp(fmcb->OSD_E3_Path[item], "mc", 2)) {
			if (!strncmp(fmcb->OSD_E3_Path[item] + 7, "DATA-SYSTEM", 11))
				fmcb->OSD_E3_Path[item][6] = '?';
			if (mode == 1) {
				sprintf(c, "mc?%s", &fmcb->OSD_E3_Path[item][3]);
				strcpy(fmcb->OSD_E3_Path[item], c);
			}
		}
	}
}
void Config_fmcb_OSDSYS_item(int item)
{

	char c[MAX_PATH];
	//char fmcbMsg[MAX_PATH] = "";
	char title_tmp[MAX_ELF_TITLE];
	//int i;
	int s;
	int MAX_S = 4;
	int x, y;
	int event, post_event = 0;

	tmpfmcb = fmcb;
	fmcb = (FMCB *)malloc(sizeof(FMCB));
	*fmcb = *tmpfmcb;

	event = 1;  //event = initial entry
	s = 0;
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad & PAD_UP) {
				event |= 2;  //event |= valid pad command
				if (s != 0)
					s--;
				else
					s = MAX_S;
			} else if (new_pad & PAD_DOWN) {
				event |= 2;  //event |= valid pad command
				if (s != MAX_S)
					s++;
				else
					s = 0;
			} else if (new_pad & PAD_LEFT) {
				event |= 2;  //event |= valid pad command
				s = s - 5;
				if (s <= 0)
					s = 0;
			} else if (new_pad & PAD_RIGHT) {
				event |= 2;  //event |= valid pad command
				s = s + 5;
				if (s >= MAX_S)
					s = MAX_S;
			} else if (new_pad & PAD_SQUARE) {
				event |= 2;          //event |= valid pad command
				if (s > 0 && s < 4)  //s= 1-2-3 stands matches the 3 launch paths
				{
					setOSD(item, 1, s);  //get elf file for corresponding launch path
				}
			} else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s == 0)
					fmcb->OSD_Name[item][0] = 0;
				if (s == 1)
					fmcb->OSD_E1_Path[item][0] = 0;
				if (s == 2)
					fmcb->OSD_E2_Path[item][0] = 0;
				if (s == 3)
					fmcb->OSD_E3_Path[item][0] = 0;
			} else if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s == 0) {
					strcpy(title_tmp, fmcb->OSD_Name[item]);
					if (keyboard(title_tmp, MAX_ELF_TITLE) >= 0)
						strcpy(fmcb->OSD_Name[item], title_tmp);
				}

				if (s > 0 && s < 4)  //s= 1-2-3 stands matches the 3 launch paths
				{
					setOSD(item, 0, s);  //get elf file for corresponding launch path
				}

				else if (s == MAX_S)
					goto return_fmcb;

			} else if (new_pad & PAD_SELECT) {  //s= 1-2-3 stands matches the 3 launch paths
				event |= 2;                     //event |= valid pad command
				if (s == 1) {
					if (!strcmp(fmcb->OSD_E1_Path[item], "OSDSYS"))
						sprintf(fmcb->OSD_E1_Path[item], "FASTBOOT");
					else
						sprintf(fmcb->OSD_E1_Path[item], "OSDSYS");
				} else if (s == 2) {
					if (!strcmp(fmcb->OSD_E2_Path[item], "OSDSYS"))
						sprintf(fmcb->OSD_E2_Path[item], "FASTBOOT");
					else
						sprintf(fmcb->OSD_E2_Path[item], "OSDSYS");
				} else if (s == 3) {
					if (!strcmp(fmcb->OSD_E3_Path[item], "OSDSYS"))
						sprintf(fmcb->OSD_E3_Path[item], "FASTBOOT");
					else
						sprintf(fmcb->OSD_E3_Path[item], "OSDSYS");
				}
			} else if (new_pad & PAD_TRIANGLE) {
			return_fmcb:
				free(tmpfmcb);
				free(fmcb);
				fmcbMsg[0] = 0;
				return;
			}
		}  //end if(readpad())

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			//if(s < SHOW_TITLES) localMsg = setting->LK_Title[s];
			//else                localMsg = "";

			x = Menu_start_x;
			y = Menu_start_y;
			sprintf(title_tmp, "Item %d Settings", item + 1);
			printXY(title_tmp, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  Name: %s", fmcb->OSD_Name[item]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  Path1: %s", fmcb->OSD_E1_Path[item]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  Path2: %s", fmcb->OSD_E2_Path[item]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  Path3: %s", fmcb->OSD_E3_Path[item]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Return");
			printXY(c, x, y, setting->color[3], TRUE, 0);

			//Cursor positioning section
			y = Menu_start_y + (s + 1) * FONT_HEIGHT + FONT_HEIGHT / 2;
			if (s > 0)
				y += FONT_HEIGHT / 2;
			if (s > 3)
				y += FONT_HEIGHT / 2;
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if (s == 0) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", "Type", "Clear");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", "Type", "Clear");
			} else if (s > 0 && s < 4) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s ÿ2:%s SELECT:Set special", "Browse", "Clear", "Map to any MC");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s ÿ2:%s SELECT:Set special", "Browse", "Clear", "Map to any MC");
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "OK");
				else
					sprintf(c, "ÿ0:%s", "OK");
			}
			sprintf(tmp, " ÿ3:%s", "Return");
			strcat(c, tmp);
			setScrTmp(fmcbMsg, c);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}  //ends while

}  //ends Config_fmcb_OSDSYS_item
void Config_fmcb_OSDSYS_scroll()
{

	char c[MAX_PATH];
	//char fmcbMsg[MAX_PATH] = "";
	char title_tmp[80];
	//int i;
	int s;
	int MAX_S = 9;
	int x, y;
	int event, post_event = 0;

	tmpfmcb = fmcb;
	fmcb = (FMCB *)malloc(sizeof(FMCB));
	*fmcb = *tmpfmcb;

	event = 1;  //event = initial entry
	s = 0;
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad & PAD_UP) {
				event |= 2;  //event |= valid pad command
				if (s != 0)
					s--;
				else
					s = MAX_S;
			} else if (new_pad & PAD_DOWN) {
				event |= 2;  //event |= valid pad command
				if (s != MAX_S)
					s++;
				else
					s = 0;
			} else if (new_pad & PAD_LEFT) {
				event |= 2;  //event |= valid pad command
				s = s - 5;
				if (s <= 0)
					s = 0;
			} else if (new_pad & PAD_RIGHT) {
				event |= 2;  //event |= valid pad command
				s = s + 5;
				if (s >= MAX_S)
					s = MAX_S;
			} else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command

				if (s == 1) {
					fmcb->OSD_displayitems = fmcb->OSD_displayitems - 2;
					if (fmcb->OSD_displayitems % 2 == 0)  //prevent even numbers from appearing
						fmcb->OSD_displayitems = fmcb->OSD_displayitems++;
					if (fmcb->OSD_displayitems <= 0)
						fmcb->OSD_displayitems = 15;
				}

				if (s == 2) {
					fmcb->OSDSYS_menu_y = fmcb->OSDSYS_menu_y--;
					if (fmcb->OSDSYS_menu_y < 0)
						fmcb->OSDSYS_menu_y = 220;
				}
				if (s == 3) {
					fmcb->OSD_mvelocity = fmcb->OSD_mvelocity - 100;
					if (fmcb->OSD_mvelocity < 0)
						fmcb->OSD_mvelocity = 0;
				}
				if (s == 4) {
					fmcb->OSD_accel = fmcb->OSD_accel - 10;
					if (fmcb->OSD_accel < 0)
						fmcb->OSD_accel = 0;
				}
				if (s == 5)
					fmcb->OSD_cursor[0][0] = 0;
				if (s == 6)
					fmcb->OSD_cursor[1][0] = 0;
				if (s == 7)
					fmcb->OSD_delimiter[0][0] = 0;
				if (s == 8)
					fmcb->OSD_delimiter[1][0] = 0;
			} else if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s == 0)
					fmcb->OSD_scroll = !fmcb->OSD_scroll;
				if (s == 1) {
					fmcb->OSD_displayitems = fmcb->OSD_displayitems + 2;
					if (fmcb->OSD_displayitems % 2 == 0)  //prevent even numbers from appearing
						fmcb->OSD_displayitems = fmcb->OSD_displayitems--;
					if (fmcb->OSD_displayitems >= 16)
						fmcb->OSD_displayitems = 1;
				}
				if (s == 2) {
					fmcb->OSDSYS_menu_y = fmcb->OSDSYS_menu_y++;
					if (fmcb->OSDSYS_menu_y > 220)
						fmcb->OSDSYS_menu_y = 0;
				}
				if (s == 3)
					fmcb->OSD_mvelocity = fmcb->OSD_mvelocity + 100;
				/*if (fmcb->OSD_mvelocity >= 800)
						fmcb->OSD_mvelocity = 0;*/
				if (s == 4)
					fmcb->OSD_accel = fmcb->OSD_accel + 10;
				/*if (fmcb->OSD_accel >= 800)
						fmcb->OSD_accel = 0;*/
				if (s == 5) {
					strcpy(title_tmp, fmcb->OSD_cursor[0]);
					if (keyboard(title_tmp, 10) >= 0)
						strcpy(fmcb->OSD_cursor[0], title_tmp);
				}
				if (s == 6) {
					strcpy(title_tmp, fmcb->OSD_cursor[1]);
					if (keyboard(title_tmp, 10) >= 0)
						strcpy(fmcb->OSD_cursor[1], title_tmp);
				}
				if (s == 7) {
					strcpy(title_tmp, fmcb->OSD_delimiter[0]);
					if (keyboard(title_tmp, 80) >= 0)
						strcpy(fmcb->OSD_delimiter[0], title_tmp);
				}
				if (s == 8) {
					strcpy(title_tmp, fmcb->OSD_delimiter[1]);
					if (keyboard(title_tmp, 80) >= 0)
						strcpy(fmcb->OSD_delimiter[1], title_tmp);
				}

				else if (s == MAX_S)
					goto return_fmcb;

			} else if (new_pad & PAD_SELECT) {
				event |= 2;  //event |= valid pad command
				if (s == 1)
					fmcb->OSD_displayitems = 7;
				if (s == 2)
					fmcb->OSDSYS_menu_y = 110;
				if (s == 3)
					fmcb->OSD_mvelocity = 1000;
				if (s == 4)
					fmcb->OSD_accel = 100;
				if (s == 5)
					strcpy(fmcb->OSD_cursor[0], "o009");
				if (s == 6)
					strcpy(fmcb->OSD_cursor[1], "o008");
				if (s == 7)
					strcpy(fmcb->OSD_delimiter[0], "y-99r0.00FunTuna  r0.60c1[r0.60Free McBoot v1.8 for Fortunar0.60]y-00");
				if (s == 8)
					strcpy(fmcb->OSD_delimiter[1], "c0r0.60y+99Based on FreeMcBoot 1.8y-00r0.00");
			} else if (new_pad & PAD_TRIANGLE) {
			return_fmcb:
				free(tmpfmcb);
				free(fmcb);
				fmcbMsg[0] = 0;
				return;
			}
		}  //end if(readpad())

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			//if(s < SHOW_TITLES) localMsg = setting->LK_Title[s];
			//else                localMsg = "";

			x = Menu_start_x;
			y = Menu_start_y;
			sprintf(title_tmp, "Scroll Settings");
			printXY(title_tmp, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			/**/ if (fmcb->OSD_scroll)
				sprintf(c, "  %s: %s", "Scroll Menu", "ON");
			else
				sprintf(c, "  %s: %s", "Scroll Menu", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  %s: %d", "Displayed Items", fmcb->OSD_displayitems);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  %s: %d", "Menu y", fmcb->OSDSYS_menu_y);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  %s: %d", "Cursor Max Velocity", fmcb->OSD_mvelocity);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  %s: %d", "Cursor Acceleration", fmcb->OSD_accel);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			/**/ sprintf(c, "  Left Cursor: %s", fmcb->OSD_cursor[0]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  Rigth Cursor: %s", fmcb->OSD_cursor[1]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  Top Delimiter: %s", fmcb->OSD_delimiter[0]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  Bottom Delimiter: %s", fmcb->OSD_delimiter[1]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Return");
			printXY(c, x, y, setting->color[3], TRUE, 0);

			//Cursor positioning section
			y = Menu_start_y + (s + 1) * FONT_HEIGHT + FONT_HEIGHT / 2;
			if (s > 4)
				y += FONT_HEIGHT / 2;
			if (s > 8)
				y += FONT_HEIGHT / 2;
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if (s == 0) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "Change");
				else
					sprintf(c, "ÿ0:%s", "Change");
			} else if (s > 0 && s < 5) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s SELECT: set default", "Add", "Subtract");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s SELECT: set default", "Add", "Subtract");
			} else if (s > 4 && s < 9) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s SELECT: set default", "Type", "Clear");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s SELECT: set default", "Type", "Clear");
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "OK");
				else
					sprintf(c, "ÿ0:%s", "OK");
			}
			sprintf(tmp, " ÿ3:%s", "Return");
			strcat(c, tmp);
			setScrTmp(fmcbMsg, c);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}  //ends while

}  //ends Config_fmcb_OSDSYS_scroll
int OSD_overwrite(int item)
{
	int event, post_event = 0;
	char c[MAX_PATH];
	const int
	    KEY_W = LINE_THICKNESS + 12 + (13 * FONT_WIDTH + 12 * 20) + 12 + LINE_THICKNESS,
	    KEY_H = LINE_THICKNESS + 1 + FONT_HEIGHT + 1 + LINE_THICKNESS + 8 + (7 * FONT_HEIGHT) + 8 + LINE_THICKNESS,
	    KEY_X = ((SCREEN_WIDTH - KEY_W) / 2) & -2,
	    KEY_Y = ((SCREEN_HEIGHT - KEY_H) / 2) & -2;

	event = 1;  //event = initial entry
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				return 1;
			} else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {
				return 0;
			}
		}

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			drawPopSprite(setting->color[0], KEY_X, KEY_Y, KEY_X + KEY_W - 1, KEY_Y + KEY_H - 1);
			drawFrame(KEY_X, KEY_Y, KEY_X + KEY_W - 1, KEY_Y + KEY_H - 1, setting->color[1]);

			sprintf(c, "Overwrite item %d?", item + 1);
			printXY(c, KEY_X + FONT_WIDTH * 14, KEY_Y + FONT_HEIGHT, setting->color[3], TRUE, 0);

			printXY(fmcb->OSD_Name[item], KEY_X + FONT_WIDTH, KEY_Y + FONT_HEIGHT * 3, setting->color[3], TRUE, 0);
			printXY(fmcb->OSD_E1_Path[item], KEY_X + FONT_WIDTH, KEY_Y + FONT_HEIGHT * 4, setting->color[3], TRUE, 0);
			printXY(fmcb->OSD_E2_Path[item], KEY_X + FONT_WIDTH, KEY_Y + FONT_HEIGHT * 5, setting->color[3], TRUE, 0);
			printXY(fmcb->OSD_E3_Path[item], KEY_X + FONT_WIDTH, KEY_Y + FONT_HEIGHT * 6, setting->color[3], TRUE, 0);
			if (swapKeys)
				printXY("ÿ1: OK / ÿ0: CANCEL", KEY_X + FONT_WIDTH * 13, KEY_Y + FONT_HEIGHT * 8, setting->color[3], TRUE, 0);
			else
				printXY("ÿ0: OK / ÿ1: CANCEL", KEY_X + FONT_WIDTH * 13, KEY_Y + FONT_HEIGHT * 8, setting->color[3], TRUE, 0);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;
	}  //ends while
}  //end OSD_overwrite
void Config_fmcb_OSDSYS()
{

	char c[MAX_PATH];
	char tempitem[4][MAX_PATH];
	//char fmcbMsg[MAX_PATH] = "";
	//int i;
	int e, s;
	int MAX_S = 26;
	int SEL_COL = 9;
	int UNSEL_COL = 14;
	int ENTER = 20;
	int VERSION = 23;
	int x, y;
	int event, post_event = 0;
	int osd_item = 0;

	tmpfmcb = fmcb;
	fmcb = (FMCB *)malloc(sizeof(FMCB));
	*fmcb = *tmpfmcb;

	event = 1;  //event = initial entry
	s = 0;
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad & PAD_UP) {
				event |= 2;                            //event |= valid pad command
				if (s > SEL_COL - 1 && s < UNSEL_COL)  //
					s = SEL_COL - 1;
				else if (s > UNSEL_COL - 1 && s < UNSEL_COL + 5)
					s = SEL_COL;
				else if (s == UNSEL_COL + 5)
					s = UNSEL_COL;
				else if (s > ENTER - 1 && s < VERSION)
					s = ENTER - 1;
				else if (s > VERSION - 1 && s < VERSION + 3)
					s = ENTER;
				else if (s == VERSION + 3)
					s = VERSION;
				else {
					if (s != 0)
						s--;
					else
						s = MAX_S;
				}
			} else if (new_pad & PAD_DOWN) {
				event |= 2;  //event |= valid pad command
				if (s > SEL_COL - 1 && s < UNSEL_COL)
					s = UNSEL_COL;
				else if (s > UNSEL_COL - 1 && s < UNSEL_COL + 5)
					s = UNSEL_COL + 5;
				else if (s > ENTER - 1 && s < VERSION)
					s = VERSION;
				else if (s > VERSION - 1 && s < VERSION + 3)
					s = VERSION + 3;
				else {
					if (s != MAX_S)
						s++;
					else
						s = 0;
				}
			} else if (new_pad & PAD_LEFT) {
				event |= 2;  //event |= valid pad command
				if (s == 1) {
					osd_item = osd_item--;
					if (osd_item < 0)
						osd_item = 99;
				} else if (s > SEL_COL && s < UNSEL_COL)
					s--;
				else if (s > UNSEL_COL && s < UNSEL_COL + 5)
					s--;
				else if (s == SEL_COL)
					s = s - 5;
				else if (s == UNSEL_COL)
					s = s - 9;
				else if (s > ENTER && s < VERSION)
					s--;
				else if (s > VERSION && s < VERSION + 3)
					s--;
				else if (s == ENTER - 1)
					s = SEL_COL - 3;
				else if (s == ENTER)
					s = SEL_COL - 2;
				else if (s == VERSION)
					s = SEL_COL - 1;
				else {
					s = s - 5;
					if (s > SEL_COL && s < UNSEL_COL + 4) {
						if (s != UNSEL_COL)
							s = 5;
					}
					if (s > ENTER && s < VERSION + 2) {
						if (s != VERSION)
							s = 7;
					} else if (s <= 0)
						s = 0;
				}
			} else if (new_pad & PAD_RIGHT) {
				event |= 2;  //event |= valid pad command
				if (s == 1) {
					osd_item = osd_item++;
					if (osd_item > 99)
						osd_item = 0;
				} else if (s > SEL_COL - 1 && s < UNSEL_COL - 1)
					s++;
				else if (s > UNSEL_COL - 1 && s < UNSEL_COL + 4)
					s++;
				else if (s == UNSEL_COL - 1 || s == UNSEL_COL + 4) {
				}  //do nothing
				else if (s > ENTER - 1 && s < VERSION - 1)
					s++;
				else if (s > VERSION - 1 && s < VERSION + 2)
					s++;
				else if (s == VERSION - 1 || s == VERSION + 2) {
				}  //do nothing
				else {
					s = s + 5;
					if (s > SEL_COL - 1 && s < UNSEL_COL + 5) {
						if (s != UNSEL_COL)
							s = UNSEL_COL + 5;
					} else if (s > ENTER - 1 && s < VERSION + 3) {
						if (s != VERSION)
							s = VERSION + 3;
					} else if (s >= MAX_S)
						s = MAX_S;
				}
			} else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {  //User pressed CANCEL=>Subtract/Clear
				event |= 2;                                                                         //event |= valid pad command
				if (s == 1) {
					fmcb->OSD_Name[osd_item][0] = 0;
					fmcb->OSD_E1_Path[osd_item][0] = 0;
					fmcb->OSD_E2_Path[osd_item][0] = 0;
					fmcb->OSD_E3_Path[osd_item][0] = 0;
				} else if (s > SEL_COL && s < UNSEL_COL) {
					if (s == SEL_COL + 1)
						e = 0;
					if (s == SEL_COL + 2)
						e = 1;
					if (s == SEL_COL + 3)
						e = 2;
					if (s == SEL_COL + 4)
						e = 3;
					OSDSYS_selected_color[e]--;
					if (OSDSYS_selected_color[e] == -1)
						OSDSYS_selected_color[e] = 255;
				} else if (s > UNSEL_COL && s < UNSEL_COL + 5) {
					if (s == UNSEL_COL + 1)
						e = 0;
					if (s == UNSEL_COL + 2)
						e = 1;
					if (s == UNSEL_COL + 3)
						e = 2;
					if (s == UNSEL_COL + 4)
						e = 3;
					OSDSYS_unselected_color[e]--;
					if (OSDSYS_unselected_color[e] == -1)
						OSDSYS_unselected_color[e] = 255;
				} else if (s == ENTER - 1) {
					fmcb->OSDSYS_menu_x--;
					if (fmcb->OSDSYS_menu_x < 0)
						fmcb->OSDSYS_menu_x = 0;
				} else if (s == ENTER + 1) {
					fmcb->OSDSYS_enter_x--;
					if (fmcb->OSDSYS_enter_x < -1)
						fmcb->OSDSYS_enter_x = 640;
				} else if (s == ENTER + 2) {
					fmcb->OSDSYS_enter_y--;
					if (fmcb->OSDSYS_enter_y < -1)
						fmcb->OSDSYS_enter_y = 256;
				} else if (s == VERSION + 1) {
					fmcb->OSDSYS_version_x--;
					if (fmcb->OSDSYS_version_x < -1)
						fmcb->OSDSYS_version_x = 640;
				} else if (s == VERSION + 2) {
					fmcb->OSDSYS_version_y--;
					if (fmcb->OSDSYS_version_y < -1)
						fmcb->OSDSYS_version_y = 256;
				}
			} else if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s == 0)
					fmcb->hacked_OSDSYS = !fmcb->hacked_OSDSYS;
				if (s == 1)
					Config_fmcb_OSDSYS_item(osd_item);
				if (s == 2)
					Config_fmcb_OSDSYS_scroll();
				if (s == 3) {
					if (!strcmp(fmcb->OSD_tvmode, "AUTO"))
						strcpy(fmcb->OSD_tvmode, "NTSC");
					else if (!strcmp(fmcb->OSD_tvmode, "NTSC"))
						strcpy(fmcb->OSD_tvmode, "PAL");
					else
						strcpy(fmcb->OSD_tvmode, "AUTO");
				}
				if (s == 4)
					fmcb->Skip_MC = !fmcb->Skip_MC;
				if (s == 5)
					fmcb->Skip_HDD = !fmcb->Skip_HDD;
				if (s == 6)
					fmcb->Skip_Disc = !fmcb->Skip_Disc;
				if (s == 7)
					fmcb->Skip_Logo = !fmcb->Skip_Logo;
				if (s == 8)
					fmcb->Inner_Browser = !fmcb->Inner_Browser;
				else if (s > SEL_COL && s < UNSEL_COL) {
					if (s == SEL_COL + 1)
						e = 0;
					if (s == SEL_COL + 2)
						e = 1;
					if (s == SEL_COL + 3)
						e = 2;
					if (s == SEL_COL + 4)
						e = 3;
					OSDSYS_selected_color[e]++;
					if (OSDSYS_selected_color[e] > 255)
						OSDSYS_selected_color[e] = 0;
				} else if (s > UNSEL_COL && s < UNSEL_COL + 5) {
					if (s == UNSEL_COL + 1)
						e = 0;
					if (s == UNSEL_COL + 2)
						e = 1;
					if (s == UNSEL_COL + 3)
						e = 2;
					if (s == UNSEL_COL + 4)
						e = 3;
					OSDSYS_unselected_color[e]++;
					if (OSDSYS_unselected_color[e] > 255)
						OSDSYS_unselected_color[e] = 0;
				} else if (s == ENTER - 1) {
					fmcb->OSDSYS_menu_x++;
					if (fmcb->OSDSYS_menu_x > 640)
						fmcb->OSDSYS_menu_x = 640;
				} else if (s == ENTER + 1) {
					fmcb->OSDSYS_enter_x++;
					if (fmcb->OSDSYS_enter_x > 640)
						fmcb->OSDSYS_enter_x = -1;
				} else if (s == ENTER + 2) {
					fmcb->OSDSYS_enter_y++;
					if (fmcb->OSDSYS_enter_y > 256)
						fmcb->OSDSYS_enter_y = -1;
				} else if (s == VERSION + 1) {
					fmcb->OSDSYS_version_x++;
					if (fmcb->OSDSYS_version_x > 640)
						fmcb->OSDSYS_version_x = -1;
				} else if (s == VERSION + 2) {
					fmcb->OSDSYS_version_y++;
					if (fmcb->OSDSYS_version_y > 256)
						fmcb->OSDSYS_version_y = -1;
				} else if (s == MAX_S)
					goto return_fmcb;

			} else if (new_pad & PAD_L2) {
				event |= 2;  //event |= valid pad command
				if (s == 1) {
					strcpy(tempitem[0], fmcb->OSD_Name[osd_item]);
					strcpy(tempitem[1], fmcb->OSD_E1_Path[osd_item]);
					strcpy(tempitem[2], fmcb->OSD_E2_Path[osd_item]);
					strcpy(tempitem[3], fmcb->OSD_E3_Path[osd_item]);
				}
			} else if (new_pad & PAD_R2) {
				event |= 2;  //event |= valid pad command
				if (s == 1) {
					if (fmcb->OSD_Name[osd_item][0] == 0 && fmcb->OSD_E1_Path[osd_item][0] == 0 && fmcb->OSD_E2_Path[osd_item][0] == 0 && fmcb->OSD_E3_Path[osd_item][0] == 0) {
						strcpy(fmcb->OSD_Name[osd_item], tempitem[0]);
						strcpy(fmcb->OSD_E1_Path[osd_item], tempitem[1]);
						strcpy(fmcb->OSD_E2_Path[osd_item], tempitem[2]);
						strcpy(fmcb->OSD_E3_Path[osd_item], tempitem[3]);
					} else if (OSD_overwrite(osd_item)) {
						strcpy(fmcb->OSD_Name[osd_item], tempitem[0]);
						strcpy(fmcb->OSD_E1_Path[osd_item], tempitem[1]);
						strcpy(fmcb->OSD_E2_Path[osd_item], tempitem[2]);
						strcpy(fmcb->OSD_E3_Path[osd_item], tempitem[3]);
					}
				}
			} else if (new_pad & PAD_SELECT) {
				event |= 2;  //event |= valid pad command
				if (s == SEL_COL) {
					OSDSYS_selected_color[0] = strtol("0x10", NULL, 16);  // hex base
					OSDSYS_selected_color[1] = strtol("0x80", NULL, 16);  // hex base
					OSDSYS_selected_color[2] = strtol("0xE0", NULL, 16);  // hex base
					OSDSYS_selected_color[3] = strtol("0x80", NULL, 16);  // hex base
				} else if (s > SEL_COL && s < UNSEL_COL) {
					if (s == SEL_COL + 1)
						OSDSYS_selected_color[0] = strtol("0x10", NULL, 16);  // hex base
					if (s == SEL_COL + 2)
						OSDSYS_selected_color[1] = strtol("0x80", NULL, 16);  // hex base
					if (s == SEL_COL + 3)
						OSDSYS_selected_color[2] = strtol("0xE0", NULL, 16);  // hex base
					if (s == SEL_COL + 4)
						OSDSYS_selected_color[3] = strtol("0x80", NULL, 16);  // hex base
				} else if (s == UNSEL_COL) {
					OSDSYS_unselected_color[0] = strtol("0x33", NULL, 16);  // hex base
					OSDSYS_unselected_color[1] = strtol("0x33", NULL, 16);  // hex base
					OSDSYS_unselected_color[2] = strtol("0x33", NULL, 16);  // hex base
					OSDSYS_unselected_color[3] = strtol("0x80", NULL, 16);  // hex base
				} else if (s > UNSEL_COL && s < UNSEL_COL + 5) {
					if (s == UNSEL_COL + 1)
						OSDSYS_unselected_color[0] = strtol("0x33", NULL, 16);  // hex base
					if (s == UNSEL_COL + 2)
						OSDSYS_unselected_color[1] = strtol("0x33", NULL, 16);  // hex base
					if (s == UNSEL_COL + 3)
						OSDSYS_unselected_color[2] = strtol("0x33", NULL, 16);  // hex base
					if (s == UNSEL_COL + 4)
						OSDSYS_unselected_color[3] = strtol("0x80", NULL, 16);  // hex base
				} else if (s == ENTER - 1)
					fmcb->OSDSYS_menu_x = 320;
				else if (s == ENTER) {
					fmcb->OSDSYS_enter_x = -1;
					fmcb->OSDSYS_enter_y = -1;
				} else if (s == ENTER + 1) {
					fmcb->OSDSYS_enter_x = -1;
				} else if (s == ENTER + 2) {
					fmcb->OSDSYS_enter_y = -1;
				} else if (s == VERSION) {
					fmcb->OSDSYS_version_x = -1;
					fmcb->OSDSYS_version_y = -1;
				} else if (s == VERSION + 1) {
					fmcb->OSDSYS_version_x = -1;
				} else if (s == VERSION + 2) {
					fmcb->OSDSYS_version_y = -1;
				}
			} else if (new_pad & PAD_TRIANGLE) {
			return_fmcb:
				free(tmpfmcb);
				free(fmcb);
				fmcbMsg[0] = 0;
				return;
			}
		}  //end if(readpad())

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			//if(s < SHOW_TITLES) localMsg = setting->LK_Title[s];
			//else                localMsg = "";

			x = Menu_start_x;
			y = Menu_start_y;
			sprintf(c, "OSD Settings");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			/**/ if (fmcb->hacked_OSDSYS)
				sprintf(c, "  %s: %s", "Hacked OSDSYS", "ON");
			else
				sprintf(c, "  %s: %s", "Hacked OSDSYS", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  Configure Item %2d : %s", osd_item + 1, fmcb->OSD_Name[osd_item]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ sprintf(c, "  Configure Scrolling Options");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			/**/ if (!strcmp(fmcb->OSD_tvmode, "AUTO"))
				sprintf(c, "  %s: %s", "Video Mode", "AUTO");
			else if (!strcmp(fmcb->OSD_tvmode, "NTSC"))
				sprintf(c, "  %s: %s", "Video Mode", "NTSC");
			else
				sprintf(c, "  %s: %s", "Video Mode", "PAL");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ if (fmcb->Skip_MC)
				sprintf(c, "  %s: %s", "Skip MC update check", "ON");
			else
				sprintf(c, "  %s: %s", "Skip MC update check", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ if (fmcb->Skip_HDD)
				sprintf(c, "  %s: %s", "Skip HDD update check", "ON");
			else
				sprintf(c, "  %s: %s", "Skip HDD update check", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ if (fmcb->Skip_Disc)
				sprintf(c, "  %s: %s", "Skip Disc Boot", "ON");
			else
				sprintf(c, "  %s: %s", "Skip Disc Boot", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ if (fmcb->Skip_Logo)
				sprintf(c, "  %s: %s", "Skip Sony Logo", "ON");
			else
				sprintf(c, "  %s: %s", "Skip Sony Logo", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			/**/ if (fmcb->Inner_Browser)
				sprintf(c, "  %s: %s", "Go to Browser", "ON");
			else
				sprintf(c, "  %s: %s", "Go to Browser", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT * 2;

			sprintf(c, "R     G     B     A");
			printXY(c, x + FONT_WIDTH * 25, y - FONT_HEIGHT, setting->color[3], TRUE, 0);
			sprintf(c, "%03d   %03d   %03d   %03d", OSDSYS_selected_color[0], OSDSYS_selected_color[1], OSDSYS_selected_color[2], OSDSYS_selected_color[3]);
			printXY(c, x + FONT_WIDTH * 24, y, setting->color[3], TRUE, 0);
			sprintf(c, "%03d   %03d   %03d   %03d", OSDSYS_unselected_color[0], OSDSYS_unselected_color[1], OSDSYS_unselected_color[2], OSDSYS_unselected_color[3]);
			printXY(c, x + FONT_WIDTH * 24, y + FONT_HEIGHT, setting->color[3], TRUE, 0);

			sprintf(c, "  %s", "Selected Color");
			printXY(c, x, y, GS_SETREG_RGBA(OSDSYS_selected_color[0], OSDSYS_selected_color[1], OSDSYS_selected_color[2], OSDSYS_selected_color[3]), TRUE, 0);
			sprintf(c, "ÿ4");
			printXY(c, x + FONT_WIDTH * 46, y, GS_SETREG_RGBA(OSDSYS_selected_color[0], OSDSYS_selected_color[1], OSDSYS_selected_color[2], OSDSYS_selected_color[3]), TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Unselected Color");
			printXY(c, x, y, GS_SETREG_RGBA(OSDSYS_unselected_color[0], OSDSYS_unselected_color[1], OSDSYS_unselected_color[2], OSDSYS_unselected_color[3]), TRUE, 0);
			sprintf(c, "ÿ4");
			printXY(c, x + FONT_WIDTH * 46, y, GS_SETREG_RGBA(OSDSYS_unselected_color[0], OSDSYS_unselected_color[1], OSDSYS_unselected_color[2], OSDSYS_unselected_color[3]), TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s: %d", "Menu X", fmcb->OSDSYS_menu_x);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Enter");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			sprintf(c, "X: %03d   Y: %03d", fmcb->OSDSYS_enter_x, fmcb->OSDSYS_enter_y);
			printXY(c, x + FONT_WIDTH * 24, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Version");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			sprintf(c, "X: %03d   Y: %03d", fmcb->OSDSYS_version_x, fmcb->OSDSYS_version_y);
			printXY(c, x + FONT_WIDTH * 24, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Return");
			printXY(c, x, y, setting->color[3], TRUE, 0);

			//Cursor positioning section
			if (s > SEL_COL && s < UNSEL_COL) {
				y = Menu_start_y + (SEL_COL + 3) * FONT_HEIGHT;
				if (s == SEL_COL + 1)
					x += FONT_WIDTH * 22;
				if (s == SEL_COL + 2)
					x += FONT_WIDTH * 28;
				if (s == SEL_COL + 3)
					x += FONT_WIDTH * 34;
				if (s == SEL_COL + 4)
					x += FONT_WIDTH * 40;
			} else if (s > UNSEL_COL && s < UNSEL_COL + 5) {
				y = Menu_start_y + (SEL_COL + 4) * FONT_HEIGHT;
				if (s == UNSEL_COL + 1)
					x += FONT_WIDTH * 22;
				if (s == UNSEL_COL + 2)
					x += FONT_WIDTH * 28;
				if (s == UNSEL_COL + 3)
					x += FONT_WIDTH * 34;
				if (s == UNSEL_COL + 4)
					x += FONT_WIDTH * 40;
			} else if (s < SEL_COL + 1) {
				y = Menu_start_y + (s + 1) * FONT_HEIGHT + FONT_HEIGHT / 2;
				if (s > 2)
					y += FONT_HEIGHT / 2;
				if (s > 8)
					y += FONT_HEIGHT;
			} else if (s == UNSEL_COL) {
				y = Menu_start_y + (UNSEL_COL - 2) * FONT_HEIGHT + FONT_HEIGHT;
			} else if (s == ENTER - 1) {
				y = Menu_start_y + (UNSEL_COL - 1) * FONT_HEIGHT + FONT_HEIGHT;
			} else if (s > ENTER - 1 && s < VERSION) {
				y = Menu_start_y + (UNSEL_COL - 1) * FONT_HEIGHT + FONT_HEIGHT * 2;

				if (s == ENTER + 1)
					x += FONT_WIDTH * 22;
				if (s == ENTER + 2)
					x += FONT_WIDTH * 31;
			} else if (s > VERSION - 1 && s < VERSION + 3) {
				y = Menu_start_y + (UNSEL_COL - 1) * FONT_HEIGHT + FONT_HEIGHT * 3;

				if (s == VERSION + 1)
					x += FONT_WIDTH * 22;
				if (s == VERSION + 2)
					x += FONT_WIDTH * 31;
			} else if (s == MAX_S) {
				y = Menu_start_y + (UNSEL_COL)*FONT_HEIGHT + FONT_HEIGHT * 3 + FONT_HEIGHT / 2;
			}

			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if (s == 0 || (s > 2 && s < 9)) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "Change");
				else
					sprintf(c, "ÿ0:%s", "Change");
			} else if (s == 1)
				if (swapKeys)
					sprintf(c, "ÿ1:OK ÿ0:Clear ÿ<|ÿ::Change item L2:Copy item R2:Paste item");
				else
					sprintf(c, "ÿ0:OK ÿ1:Clear ÿ<|ÿ::Change item L2:Copy item R2:Paste item");
			else if (s == SEL_COL || s == UNSEL_COL || s == ENTER || s == VERSION)
				sprintf(c, "SELECT: set default");
			else if ((s > UNSEL_COL && s < UNSEL_COL + 5) || (s > SEL_COL && s < UNSEL_COL) || (s > ENTER && s < VERSION) || (s > VERSION && s < VERSION + 3) || s == ENTER - 1) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s SELECT: set default", "Add", "Subtract");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s SELECT: set default", "Add", "Subtract");
			}
			/*else if (s==ENTER||s==VERSION)
				c[0]=0;*/
			else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "OK");
				else
					sprintf(c, "ÿ0:%s", "OK");
			}
			sprintf(tmp, " ÿ3:%s", "Return");
			strcat(c, tmp);
			setScrTmp(fmcbMsg, c);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}  //ends while

}  //ends Config_fmcb_OSDSYS

void Config_ESR_path()
{

	char c[MAX_PATH];
	//char fmcbMsg[MAX_PATH] = "";
	char title_tmp[MAX_ELF_TITLE];
	//int i;
	int s;
	int MAX_S = 3;
	int x, y;
	int event, post_event = 0;

	tmpfmcb = fmcb;
	fmcb = (FMCB *)malloc(sizeof(FMCB));
	*fmcb = *tmpfmcb;

	event = 1;  //event = initial entry
	s = 0;
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad & PAD_UP) {
				event |= 2;  //event |= valid pad command
				if (s != 0)
					s--;
				else
					s = MAX_S;
			} else if (new_pad & PAD_DOWN) {
				event |= 2;  //event |= valid pad command
				if (s != MAX_S)
					s++;
				else
					s = 0;
			}
			/*		else if(new_pad & PAD_LEFT)
			{
				event |= 2;  //event |= valid pad command
				s = s-5;
				if(s<=0)
					s=0;
			}
			else if(new_pad & PAD_RIGHT)
			{
				event |= 2;  //event |= valid pad command
				s = s+5;
				if(s>=MAX_S)
					s=MAX_S;
			}
	*/
			else if (new_pad & PAD_SQUARE) {
				event |= 2;  //event |= valid pad command

				if (s < 3) {
					getFilePath(fmcb->ESR_Path[s], TRUE);
					if (!strncmp(fmcb->ESR_Path[s], "mc", 2)) {
						if (!strncmp(fmcb->ESR_Path[s] + 7, "DATA-SYSTEM", 11))
							fmcb->ESR_Path[s][6] = '?';
						sprintf(c, "mc?%s", &fmcb->ESR_Path[s][3]);
						strcpy(fmcb->ESR_Path[s], c);
					}
				}
			} else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command

				if (s < 3)
					fmcb->ESR_Path[s][0] = 0;

			} else if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command

				if (s < 3) {
					getFilePath(fmcb->ESR_Path[s], TRUE);
					if (!strncmp(fmcb->ESR_Path[s] + 7, "DATA-SYSTEM", 11))
						fmcb->ESR_Path[s][6] = '?';
				}

				else if (s == MAX_S)
					goto return_fmcb;

			} else if (new_pad & PAD_TRIANGLE) {
			return_fmcb:
				free(tmpfmcb);
				free(fmcb);
				fmcbMsg[0] = 0;
				return;
			}
		}  //end if(readpad())

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			//if(s < SHOW_TITLES) localMsg = setting->LK_Title[s];
			//else                localMsg = "";

			x = Menu_start_x;
			y = Menu_start_y;
			sprintf(title_tmp, "ESR Path");
			printXY(title_tmp, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  Path1: %s", fmcb->ESR_Path[0]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  Path2: %s", fmcb->ESR_Path[1]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  Path3: %s", fmcb->ESR_Path[2]);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Return");
			printXY(c, x, y, setting->color[3], TRUE, 0);

			//Cursor positioning section
			y = Menu_start_y + (s + 1) * FONT_HEIGHT + FONT_HEIGHT / 2;
			if (s > 2)
				y += FONT_HEIGHT / 2;
			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if (s < 3) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s ÿ2:%s", "Browse", "Clear", "Map to any MC");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s ÿ2:%s", "Browse", "Clear", "Map to any MC");
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "OK");
				else
					sprintf(c, "ÿ0:%s", "OK");
			}
			sprintf(tmp, " ÿ3:%s", "Return");
			strcat(c, tmp);
			setScrTmp(fmcbMsg, c);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}  //ends while

}  //ends Config_ESR_path

//---------------------------------------------------------------------------
// Configuration menu for Free McBoot
//---------------------------------------------------------------------------
void select_swapkeys()
{
	int event, post_event = 0;
	const int
	    KEY_W = LINE_THICKNESS + 12 + (13 * FONT_WIDTH + 12 * 12) + 12 + LINE_THICKNESS,
	    KEY_H = LINE_THICKNESS + 1 + FONT_HEIGHT + 1 + LINE_THICKNESS + 8 + (6 * FONT_HEIGHT) + 8 + LINE_THICKNESS,
	    KEY_X = ((SCREEN_WIDTH - KEY_W) / 2) & -2,
	    KEY_Y = ((SCREEN_HEIGHT - KEY_H) / 2) & -2;

	const int
	    KEY_W2 = LINE_THICKNESS + 12 + (55 * FONT_WIDTH + 12 * 12) + 12 + LINE_THICKNESS,
	    KEY_H2 = LINE_THICKNESS + 1 + FONT_HEIGHT + 1 + LINE_THICKNESS + 8 + (2 * FONT_HEIGHT) + 8 + LINE_THICKNESS,
	    KEY_X2 = SCREEN_WIDTH - KEY_W2 - 8,
	    KEY_Y2 = SCREEN_HEIGHT - KEY_H2 - 2;

	event = 1;  //event = initial entry
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad)
				event |= 2;  //event |= pad command
			if (new_pad & PAD_CROSS) {
				swapKeys = TRUE;
				return;
			} else if (new_pad & PAD_CIRCLE)
				return;
		}

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			drawPopSprite(setting->color[0], KEY_X, KEY_Y, KEY_X + KEY_W - 1, KEY_Y + KEY_H - 1);
			drawFrame(KEY_X, KEY_Y, KEY_X + KEY_W - 1, KEY_Y + KEY_H - 1, setting->color[1]);

			printXY("Select buttons layout", KEY_X + FONT_HEIGHT * 3, KEY_Y + FONT_HEIGHT, setting->color[3], TRUE, 0);
			printXY("ÿ0: ÿ1 Cancel / ÿ0 OK", KEY_X + FONT_HEIGHT * 3, KEY_Y + FONT_HEIGHT * 4, setting->color[3], TRUE, 0);
			printXY("ÿ1: ÿ0 Cancel / ÿ1 OK", KEY_X + FONT_HEIGHT * 3, KEY_Y + FONT_HEIGHT * 6, setting->color[3], TRUE, 0);

			drawPopSprite(setting->color[0], KEY_X2, KEY_Y2, KEY_X2 + KEY_W2 - 1, KEY_Y2 + KEY_H2 - 1);
			drawFrame(KEY_X2, KEY_Y2, KEY_X2 + KEY_W2 - 1, KEY_Y2 + KEY_H2 - 1, setting->color[1]);
			printXY("FMCB Configurator by suloku. mod by HWNJ, modified by Matias Israelson", KEY_X2 + FONT_WIDTH * 2, KEY_Y2 + FONT_HEIGHT, setting->color[3], TRUE, 0);
			printXY("Based on uLaunchELF by E P and Dlanor, originally coded by Mirakichi.", KEY_X2 + FONT_WIDTH * 2, KEY_Y2 + FONT_HEIGHT * 2, setting->color[3], TRUE, 0);
			printXY("Free McBoot by Jimmikaelkael and Neme 2008. Special thanks to uyjulian.", KEY_X2 + FONT_WIDTH * 2, KEY_Y2 + FONT_HEIGHT * 3, setting->color[3], TRUE, 0);

		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;
	}  //ends while
}  //end selec_swapkeys
void Config_fmcb(char *fmcbMsg)
{
	char c[MAX_PATH];
	//char fmcbMsg[MAX_PATH] = "";
	int s;
	int MAX_S = 17;
	int x, y;
	int event, post_event = 0;

	event = 1;  //event = initial entry
	s = 0;
	select_swapkeys();
	while (1) {
		//Pad response section
		waitPadReady(0, 0);
		if (readpad()) {
			if (new_pad & PAD_UP) {
				event |= 2;  //event |= valid pad command
				if (s != 0)
					s--;
				else
					s = MAX_S;
			} else if (new_pad & PAD_DOWN) {
				event |= 2;  //event |= valid pad command
				if (s != MAX_S)
					s++;
				else
					s = 0;
			} else if (new_pad & PAD_LEFT) {
				event |= 2;  //event |= valid pad command
				s = s - 4;
				if (s <= 0)
					s = 0;
			} else if (new_pad & PAD_RIGHT) {
				event |= 2;  //event |= valid pad command
				s = s + 4;
				if (s >= MAX_S)
					s = MAX_S;
			}
			/*else if((new_pad & PAD_SQUARE) && (s<SHOW_TITLES)){
				event |= 2;  //event |= valid pad command
				strcpy(title_tmp, setting->LK_Title[s]);
				if(keyboard(title_tmp, MAX_ELF_TITLE)>=0)
					strcpy(setting->LK_Title[s], title_tmp);
			}*/
			else if ((!swapKeys && new_pad & PAD_CROSS) || (swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s == 10) {
					fmcb->pad_delay = fmcb->pad_delay - 100;
				}
				if (fmcb->pad_delay < 0)
					fmcb->pad_delay = 10000;
			} else if ((swapKeys && new_pad & PAD_CROSS) || (!swapKeys && new_pad & PAD_CIRCLE)) {
				event |= 2;  //event |= valid pad command
				if (s == 0) {
					loadConfig_fmcb(fmcbMsg, cnf_path_fmcb);
				}
				if (s == 1) {
					loadConfig_fmcb(fmcbMsg, cnf_path_fmcb2);
				}
				if (s == 2) {
					loadConfig_fmcb(fmcbMsg, cnf_path_fmcb3);
				}

				if (s == 3) {
					sprintf(fmcbMsg, " ");
					Config_fmcb_key(1);
				}
				if (s == 4) {
					sprintf(fmcbMsg, " ");
					Config_fmcb_key(2);
				}
				if (s == 5) {
					sprintf(fmcbMsg, " ");
					Config_fmcb_key(3);
				}
				if (s == 6) {
					sprintf(fmcbMsg, " ");
					Config_fmcb_OSDSYS();
				}
				if (s == 7) {
					sprintf(fmcbMsg, " ");
					Config_ESR_path();
				}
				if (s == 8) {
					fmcb->Fastboot = !fmcb->Fastboot;
				}
				if (s == 9) {
					fmcb->Debug = !fmcb->Debug;
				}
				if (s == 10) {
					fmcb->pad_delay = fmcb->pad_delay + 100;
					if (fmcb->pad_delay > 10000)
						fmcb->pad_delay = 0;
				}

				else if (s == 11)
					saveFmcbCNF(fmcbMsg, cnf_path_fmcb);
				else if (s == 12)
					saveFmcbCNF(fmcbMsg, cnf_path_fmcb2);
				else if (s == 13)
					saveFmcbCNF(fmcbMsg, cnf_path_fmcb3);
				else if (s == 14) {
					RunElf(fmcbMsg, "Loader");
				} else if (s == 15) {
					RunElf(fmcbMsg, "mc?:/BOOT/ULE.ELF");
				} else if (s == 16) {
					RunElf(fmcbMsg, "mc?:/BOOT/POWEROFF.ELF");
				}
				 else if (s == MAX_S)
					__asm__ __volatile__(
					    "	li $3, 0x04;"
					    "	syscall;"
					    "	nop;");
			} else if (new_pad & PAD_SELECT) {
				event |= 2;  //event |= valid pad command
				if (s >= 0 && s < 3) {
					initConfig_fmcb(0);
					strcpy(fmcbMsg, "All set to default");
				}
			}
			//else if(new_pad & PAD_TRIANGLE) {}
		}  //end if(readpad())

		if (event || post_event) {  //NB: We need to update two frame buffers per event

			//Display section
			clrScr(setting->color[0]);

			x = Menu_start_x;
			y = Menu_start_y;
			printXY("Free McBoot Settings", x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Load CNF from MC0");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Load CNF from MC1");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Load CNF from Mass");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s...", "Configure E1 launch keys");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s...", "Configure E2 launch keys");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s...", "Configure E3 launch keys");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s...", "Configure OSDSYS options");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s...", "Configure ESR Path");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			if (fmcb->Fastboot)
				sprintf(c, "  %s: %s", "FastBoot", "ON");
			else
				sprintf(c, "  %s: %s", "FastBoot", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			if (fmcb->Debug)
				sprintf(c, "  %s: %s", "Debug Screen", "ON");
			else
				sprintf(c, "  %s: %s", "Debug Screen", "OFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;

			sprintf(c, "  %s: %.1f", "Pad Delay", fmcb->pad_delay / 1000);
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Save CNF to MC0");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Save CNF to MC1");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Save CNF to Mass");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			y += FONT_HEIGHT / 2;

			sprintf(c, "  %s", "Return to FunTuna");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "Return to uLaunchELF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "POWEROFF");
			printXY(c, x, y, setting->color[3], TRUE, 0);
			y += FONT_HEIGHT;
			sprintf(c, "  %s", "PS2 Browser");
			printXY(c, x, y, setting->color[3], TRUE, 0);

			//Cursor positioning section
			y = Menu_start_y + (s + 1) * FONT_HEIGHT + FONT_HEIGHT / 2;
			if (s > 2)
				y += FONT_HEIGHT / 2;
			if (s > 7)
				y += FONT_HEIGHT / 2;
			if (s > 10)
				y += FONT_HEIGHT / 2;
			if (s > 13)
				y += FONT_HEIGHT / 2;

			drawChar(LEFT_CUR, x, y, setting->color[3]);

			//Tooltip section
			if (s >= 0 && s < 3) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s SELECT:Set all default", "OK");
				else
					sprintf(c, "ÿ0:%s SELECT:Set all default", "OK");
			} else if (s > 7 && s < 10) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "Change");
				else
					sprintf(c, "ÿ0:%s", "Change");
			} else if (s == 10) {
				if (swapKeys)
					sprintf(c, "ÿ1:%s ÿ0:%s", "Add", "Subtract");
				else
					sprintf(c, "ÿ0:%s ÿ1:%s", "Add", "Subtract");
			} else {
				if (swapKeys)
					sprintf(c, "ÿ1:%s", "OK");
				else
					sprintf(c, "ÿ0:%s", "OK");
			}
			//sprintf(tmp, " ÿ3:%s", "Return");
			//strcat(c, tmp);
			setScrTmp(fmcbMsg, c);
		}  //ends if(event||post_event)
		drawScr();
		post_event = event;
		event = 0;

	}  //ends while
}  //ends config_fmcb

//---------------------------------------------------------------------------
// End of file: config.c
//---------------------------------------------------------------------------
