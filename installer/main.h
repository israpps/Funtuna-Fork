#ifndef _MAIN_H_
#define _MAIN_H_
#include <libmc.h>
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <sbv_patches.h>
#include <loadfile.h>
#include <libpad.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
//#include <osd_config.h>
//#include <libcdvd.h>
#include <debug.h>
#include "enums.h"

#define NEWLIB_PORT_AWARE
#include <fileio.h>


#define MAX_PATH 260

/*     gs.c     */

// ntsc_pal
#define NTSC			2
#define PAL				3

extern int VMode;

typedef enum {
	PAL_640_512_32,
	NTSC_640_448_32
} gs_video_mode;

void gs_reset(void);
int gs_detect_signal(void);
int gs_init(gs_video_mode mode);
void gs_set_fill_color(u8 r, u8 g, u8 b);
void gs_fill_rect(u16 x0, u16 y0, u16 x1, u16 y1);
u16 gs_get_max_x(void);
u16 gs_get_max_y(void);
void gs_print_bitmap(u16 x, u16 y, u16 w, u16 h, u32 *data);

/* pad.c */

extern u32 new_pad;

int readPad(void);
void waitAnyPadReady(void);
int setupPad(void);



#endif
