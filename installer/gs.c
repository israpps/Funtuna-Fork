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

    Author of this GS code:	Tony Saveski, t_saveski@yahoo.com

*/

#include "main.h"

//dma_asm.s
extern void dma_reset(void);

//gs_asm.s
extern void gs_set_imr(void);
extern void gs_set_crtc(u8 int_mode, u8 ntsc_pal, u8 field_mode);

//ps2_asm.s
extern void ps2_flush_cache(int blah);

typedef struct
{
	u16 ntsc_pal;
	u16 width;
	u16 height;
	u16 psm;
	u16 bpp;
	u16 magh;
} __attribute__((aligned(16))) vmode_t;

vmode_t vmodes[] = {
	{PAL,  640, 512, 0, 32, 4},
	{NTSC, 640, 448, 0, 32, 4}
};

static vmode_t *cur_mode;


static u16	gs_max_x=0;		// current resolution max coordinates
static u16	gs_max_y=0;

static u8	gs_fill_r=0;	// current fill color
static u8	gs_fill_g=0;
static u8	gs_fill_b=0;

static u16	gs_view_x0=0;	// current viewport coordinates
static u16	gs_view_x1=1;
static u16	gs_view_y0=0;
static u16	gs_view_y1=1;

int VMode;

//---------------------------------------------------------------------------

// Playstation2 GS Privileged Registers
#define pmode		0x12000000	// Setup CRT Controller
#define smode2		0x12000020	// CRTC Video Settings: PAL/NTCS, Interlace, etc.
#define dispfb1		0x12000070	// Setup the CRTC's Read Circuit 1 data source settings
#define display1	0x12000080	// RC1 display output settings
#define dispfb2		0x12000090	// Setup the CRTC's Read Circuit 2 data source settings
#define display2	0x120000a0	// RC2 display output settings
#define extbuf		0x120000b0	// ...
#define extdata		0x120000c0	// ...
#define extwrite	0x120000d0	// ...
#define bgcolor		0x120000e0	// Set CRTC background color
#define csr			0x12001000	// System status and reset
#define imr			0x12001010	// Interrupt Mask Register
#define busdir		0x12001040	// ...
#define siglblid	0x12001080	// ...

// Playstation2 GS General Purpose Registers
#define prim		0x00	// Select and configure current drawing primitive
#define rgbaq		0x01	// Setup current vertex color
#define st			0x02	// ...
#define uv			0x03	// ...
#define xyzf2		0x04	// Set vertex coordinate
#define xyz2		0x05	// Set vertex coordinate and 'kick' drawing
#define tex0_1		0x06	// ...
#define tex0_2		0x07	// ...
#define clamp_1		0x08	// ...
#define clamp_2		0x09	// ...
#define fog			0x0a	// ...
#define xyzf3		0x0c	// ...
#define xyz3		0x0d	// ...
#define tex1_1		0x14	// ...
#define tex1_2		0x15	// ...
#define tex2_1		0x16	// ...
#define tex2_2		0x17	// ...
#define xyoffset_1	0x18	// Mapping from Primitive to Window coordinate system (Context 1)
#define xyoffset_2	0x19	// Mapping from Primitive to Window coordinate system (Context 2)
#define prmodecont	0x1a	// ...
#define prmode		0x1b	// ...
#define texclut		0x1c	// ...
#define scanmsk		0x22	// ...
#define miptbp1_1	0x34	// ...
#define miptbp1_2	0x35	// ...
#define miptbp2_1	0x36	// ...
#define miptbp2_2	0x37	// ...
#define texa		0x3b	// ...
#define fogcol		0x3d	// ...
#define texflush	0x3f	// ...
#define scissor_1	0x40	// Setup clipping rectangle (Context 1)
#define scissor_2	0x41	// Setup clipping rectangle (Context 2)
#define alpha_1		0x42	// ...
#define alpha_2		0x43	// ...
#define dimx		0x44	// ...
#define dthe		0x45	// ...
#define colclamp	0x46	// ...
#define test_1		0x47	// ...
#define test_2		0x48	// ...
#define pabe		0x49	// ...
#define fba_1		0x4a	// ...
#define fba_2		0x4b	// ...
#define frame_1		0x4c	// Frame buffer settings (Context 1)
#define frame_2		0x4d	// Frame buffer settings (Context 2)
#define zbuf_1		0x4e	// ...
#define zbuf_2		0x4f	// ...
#define bitbltbuf	0x50	// Setup Image Transfer Between EE and GS
#define trxpos		0x51	// Setup Image Transfer Coordinates
#define trxreg		0x52	// Setup Image Transfer Size
#define trxdir		0x53	// Set Image Transfer Directon + Start Transfer
#define hwreg		0x54	// ...
#define signal		0x60	// ...
#define finish		0x61	// ...
#define label		0x62	// ...

// Playstation2 DMA Channel Registers
#define gif_chcr	0x1000a000	// GIF Channel Control Register
#define gif_madr	0x1000a010	// Transfer Address Register
#define gif_qwc		0x1000a020	// Transfer Size Register (in qwords)
#define gif_tadr	0x1000a030	// ...

//===========================================================================
// Privileged Register Macros
//===========================================================================

//---------------------------------------------------------------------------
// CSR Register
//---------------------------------------------------------------------------
#define CSR			((volatile u64 *)(csr))

#define GS_RESET() \
	*CSR = ((u64)(1)	<< 9)

//---------------------------------------------------------------------------
// PMODE Register
//---------------------------------------------------------------------------
#define PMODE		((volatile u64 *)(pmode))
#define GS_SET_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
	*PMODE = \
	((u64)(EN1) << 0) 	| \
	((u64)(EN2) << 1) 	| \
	((u64)(001)	<< 2) 	| \
	((u64)(MMOD)<< 5) 	| \
	((u64)(AMOD)<< 6) 	| \
	((u64)(SLBG)<< 7) 	| \
	((u64)(ALP) << 8)
//---------------------------------------------------------------------------
// DISPFP2 Register
//---------------------------------------------------------------------------
#define DISPFB2		((volatile u64 *)(dispfb2))
#define GS_SET_DISPFB2(FBP,FBW,PSM,DBX,DBY) \
	*DISPFB2 = \
	((u64)(FBP)	<< 0)	| \
	((u64)(FBW)	<< 9)	| \
	((u64)(PSM)	<< 15)	| \
	((u64)(DBX)	<< 32)	| \
	((u64)(DBY)	<< 43)
//---------------------------------------------------------------------------
// DISPLAY2 Register
//---------------------------------------------------------------------------
#define DISPLAY2	((volatile u64 *)(display2))
#define GS_SET_DISPLAY2(DX,DY,MAGH,MAGV,DW,DH) \
	*DISPLAY2 = \
	((u64)(DX)	<< 0)	| \
	((u64)(DY)	<< 12)	| \
	((u64)(MAGH)<< 23)	| \
	((u64)(MAGV)<< 27)	| \
	((u64)(DW)	<< 32)	| \
	((u64)(DH)	<< 44)
//---------------------------------------------------------------------------
// BGCOLOR Register
//---------------------------------------------------------------------------
#define BGCOLOR		((volatile u64 *)(bgcolor))
#define GS_SET_BGCOLOR(R,G,B) \
	*BGCOLOR = \
	((u64)(R)	<< 0)		| \
	((u64)(G)	<< 8)		| \
	((u64)(B)	<< 16)


//===========================================================================
// General Purpose Register Macros
//===========================================================================

//---------------------------------------------------------------------------
// BITBLTBUF Register - Setup Image Transfer Between EE and GS
//   SBP  - Source buffer address (Address/64)
//   SBW  - Source buffer width (Pixels/64)
//   SPSM - Source pixel format (0 = 32bit RGBA)
//   DBP  - Destination buffer address (Address/64)
//   DBW  - Destination buffer width (Pixels/64)
//   DPSM - Destination pixel format (0 = 32bit RGBA)
//
// - When transferring from EE to GS, only the Detination fields
//   need to be set. (Only Source fields for GS->EE, and all for GS->GS).
//---------------------------------------------------------------------------
#define GS_BITBLTBUF(SBP,SBW,SPSM,DBP,DBW,DPSM) \
	(((u64)(SBP)	<< 0)		| \
	 ((u64)(SBW)	<< 16)		| \
	 ((u64)(SPSM)	<< 24)		| \
	 ((u64)(DBP)	<< 32)		| \
	 ((u64)(DBW)	<< 48)		| \
	 ((u64)(DPSM)	<< 56))

//---------------------------------------------------------------------------
// FRAME_x Register
//---------------------------------------------------------------------------
#define GS_FRAME(FBP,FBW,PSM,FBMSK) \
	(((u64)(FBP)	<< 0)		| \
	 ((u64)(FBW)	<< 16)		| \
	 ((u64)(PSM)	<< 24)		| \
	 ((u64)(FBMSK)	<< 32))

//---------------------------------------------------------------------------
// PRIM Register
//---------------------------------------------------------------------------
#define PRIM_POINT			0
#define PRIM_LINE			1
#define PRIM_LINE_STRIP		2
#define PRIM_TRI			3
#define PRIM_TRI_STRIP		4
#define PRIM_TRI_FAN		5
#define PRIM_SPRITE			6

#define GS_PRIM(PRI,IIP,TME,FGE,ABE,AA1,FST,CTXT,FIX) \
	(((u64)(PRI)	<< 0)		| \
	 ((u64)(IIP)	<< 3)		| \
	 ((u64)(TME)	<< 4)		| \
	 ((u64)(FGE)	<< 5)		| \
	 ((u64)(ABE)	<< 6)		| \
	 ((u64)(AA1)	<< 7)		| \
	 ((u64)(FST)	<< 8)		| \
	 ((u64)(CTXT)	<< 9)		| \
	 ((u64)(FIX)	<< 10))

//---------------------------------------------------------------------------
// RGBAQ Register
//---------------------------------------------------------------------------
#define GS_RGBAQ(R,G,B,A,Q) \
	(((u64)(R)		<< 0)		| \
	 ((u64)(G)		<< 8)		| \
	 ((u64)(B)		<< 16)		| \
	 ((u64)(A)		<< 24)		| \
	 ((u64)(Q)		<< 32))

//---------------------------------------------------------------------------
// XYOFFSET_x Register
//---------------------------------------------------------------------------
#define GS_XYOFFSET(OFX,OFY)	\
	(((u64)(OFX)		<< 0)		| \
	 ((u64)(OFY)		<< 32))

//---------------------------------------------------------------------------
// SCISSOR_x Register
//---------------------------------------------------------------------------
#define GS_SCISSOR(X0,X1,Y0,Y1) \
	(((u64)(X0)		<< 0)		| \
	 ((u64)(X1)		<< 16)		| \
	 ((u64)(Y0)		<< 32)		| \
	 ((u64)(Y1)		<< 48))

//---------------------------------------------------------------------------
// TRXDIR Register - Set Image Transfer Directon, and Start Transfer
//   XDIR - (0=EE->GS, 1=GS->EE, 2=GS-GS, 3=Transmission is deactivated)
//---------------------------------------------------------------------------
#define XDIR_EE_GS			0
#define XDIR_GS_EE			1
#define XDIR_GS_GS			2
#define XDIR_DEACTIVATE		3

#define GS_TRXDIR(XDIR)	\
	((u64)(XDIR))

//---------------------------------------------------------------------------
// TRXPOS Register - Setup Image Transfer Coordinates
//   SSAX - Source Upper Left X
//   SSAY - Source Upper Left Y
//   DSAX - Destionation Upper Left X
//   DSAY - Destionation Upper Left Y
//   DIR  - Pixel Transmission Order (00 = top left -> bottom right)
//
// - When transferring from EE to GS, only the Detination fields
//   need to be set. (Only Source fields for GS->EE, and all for GS->GS).
//---------------------------------------------------------------------------
#define GS_TRXPOS(SSAX,SSAY,DSAX,DSAY,DIR)	\
	(((u64)(SSAX)	<< 0)		| \
	 ((u64)(SSAY)	<< 16)		| \
	 ((u64)(DSAX)	<< 32)		| \
	 ((u64)(DSAY)	<< 48)		| \
	 ((u64)(DIR)	<< 59))

//---------------------------------------------------------------------------
// TRXREG Register - Setup Image Transfer Size
//   RRW - Image Width
//   RRH - Image Height
//---------------------------------------------------------------------------
#define GS_TRXREG(RRW,RRH)	\
	(((u64)(RRW)	<< 0)		| \
	 ((u64)(RRH)	<< 32))

//---------------------------------------------------------------------------
// XYZ2 Register
//---------------------------------------------------------------------------
#define GS_XYZ2(X,Y,Z)	\
	(((u64)(X)		<< 0)		| \
	 ((u64)(Y)		<< 16)		| \
	 ((u64)(Z)		<< 32))


//---------------------------------------------------------------------------
#define GIF_AD		0x0e
#define GIF_NOP		0x0f

//---------------------------------------------------------------------------
// GS_PACKET macros
//---------------------------------------------------------------------------

#define DECLARE_GS_PACKET(NAME,ITEMS) \
	u64 __attribute__((aligned(64))) NAME[ITEMS*2+2]; \
	int NAME##_cur; \
	int NAME##_dma_size

#define BEGIN_GS_PACKET(NAME) \
	NAME##_cur = 0

#define GIF_TAG(NAME,NLOOP,EOP,PRE,PRIM,FLG,NREG,REGS) \
	NAME##_dma_size = NLOOP+1; \
	NAME[NAME##_cur++] = \
		((u64)(NLOOP)<< 0)		| \
		((u64)(EOP)	 << 15)		| \
		((u64)(PRE)	 << 46)		| \
		((u64)(PRIM) << 47)		| \
		((u64)(FLG)	 << 58)		| \
		((u64)(NREG) << 60);		\
	NAME[NAME##_cur++] = (u64)REGS

#define GIF_TAG_AD(NAME,NLOOP,EOP,PRE,PRIM,FLG) \
	GIF_TAG(NAME,NLOOP,EOP,PRE,PRIM,FLG,1,GIF_AD)

#define GIF_TAG_IMG(NAME,QSIZE) \
	GIF_TAG(NAME,(QSIZE),1,0,0,2,0,0); \
	NAME##_dma_size = 1 \

#define GIF_DATA_AD(NAME,REG,DAT) \
	NAME[NAME##_cur++] = (u64)DAT; \
	NAME[NAME##_cur++] = (u64)REG

#define SEND_GS_PACKET(NAME) \
	ps2_flush_cache(0);							\
	SET_QWC(GIF_QWC, NAME##_dma_size);			\
	SET_MADR(GIF_MADR, NAME, 0);				\
	SET_CHCR(GIF_CHCR, 1, 0, 0, 0, 0, 1, 0);	\
	DMA_WAIT(GIF_CHCR)



//---------------------------------------------------------------------------
// CHCR Register - Channel Control Register
//---------------------------------------------------------------------------
#define GIF_CHCR		((volatile u32 *)(gif_chcr))

#define SET_CHCR(WHICH,DIR,MOD,ASP,TTE,TIE,STR,TAG) \
	*WHICH = \
	((u32)(DIR)	<< 0)		| \
	((u32)(MOD)	<< 2)		| \
	((u32)(ASP)	<< 4)		| \
	((u32)(TTE)	<< 6)		| \
	((u32)(TIE)	<< 7)		| \
	((u32)(STR)	<< 8)		| \
	((u32)(TAG)	<< 16)

#define DMA_WAIT(WHICH) \
	while((*WHICH) & (1<<8))

//---------------------------------------------------------------------------
// MADR Register - Transfer Address Register
//---------------------------------------------------------------------------
#define GIF_MADR		((volatile u32 *)(gif_madr))

#define SET_MADR(WHICH,ADDR,SPR) \
	*WHICH = \
	((u32)(ADDR)<< 0)		| \
	((u32)(SPR)	<< 31)

//---------------------------------------------------------------------------
// TADR Register - Tag Address Register
//---------------------------------------------------------------------------
#define GIF_TADR		((volatile u32 *)(gif_tadr))

#define SET_TADR(WHICH,ADDR,SPR) \
	*WHICH = \
	((u32)(ADDR)	<< 0)		| \
	((u32)(SPR)	<< 31)

//---------------------------------------------------------------------------
// QWC Register - Transfer Data Size Register
//---------------------------------------------------------------------------
#define GIF_QWC		((volatile u32 *)(gif_qwc))

#define SET_QWC(WHICH,SIZE) \
	*WHICH = (u32)(SIZE)


//---------------------------------------------------------------------------
DECLARE_GS_PACKET(gs_dma_buf,50);

//------------------------------------------------------------------------------------------------------------------------
int gs_init(gs_video_mode mode)
{
vmode_t *v;

	v = &(vmodes[mode]);
	cur_mode = v;

	gs_max_x = v->width - 1;
	gs_max_y = v->height - 1;

	gs_view_x0 = 0;
	gs_view_y0 = 0;
	gs_view_x1 = gs_max_x;
	gs_view_y1 = gs_max_y;

	// - Initialize the DMA.
	// - Writes a 0 to most of the DMA registers.
	dma_reset();

	// - Sets the RESET bit if the GS CSR register.
	GS_RESET();

	__asm__("sync.p; nop;");

	// - Sets up the GS IMR register.
	// - The IMR register is used to mask and unmask certain interrupts.
	gs_set_imr();

	// - Use syscall 0x02 to setup some video mode stuff.
	gs_set_crtc(1, v->ntsc_pal, 0); // Interlaced, Field mode

	GS_SET_PMODE(
		0,		// ReadCircuit1 OFF
		1,		// ReadCircuit2 ON
		1,		// Use ALP register for Alpha Blending
		1,		// Alpha Value of ReadCircuit2 for output selection
		0,		// Blend Alpha with the output of ReadCircuit2
		0xFF	// Alpha Value = 1.0
	);

	GS_SET_DISPFB2(
		0,				// Frame Buffer base pointer = 0 (Address/2048)
		v->width/64,	// Buffer Width (Address/64)
		v->psm,			// Pixel Storage Format
		0,				// Upper Left X in Buffer = 0
		0				// Upper Left Y in Buffer = 0
	);

	GS_SET_DISPLAY2(
		656,		// X position in the display area (in VCK units)
		36,			// Y position in the display area (in Raster units)
		v->magh-1,	// Horizontal Magnification - 1
		0,						// Vertical Magnification = 1x
		v->width*v->magh-1,		// Display area width  - 1 (in VCK units) (Width*HMag-1)
		v->height-1				// Display area height - 1 (in pixels)	  (Height-1)
	);

	GS_SET_BGCOLOR(0, 0, 0);

	BEGIN_GS_PACKET(gs_dma_buf);

	GIF_TAG_AD(gs_dma_buf, 3, 1, 0, 0, 0);
	GIF_DATA_AD(gs_dma_buf, frame_1,
		GS_FRAME(
			0,					// FrameBuffer base pointer = 0 (Address/2048)
			v->width/64,		// Frame buffer width (Pixels/64)
			v->psm,				// Pixel Storage Format
			0));

	// No displacement between Primitive and Window coordinate systems.
	GIF_DATA_AD(gs_dma_buf, xyoffset_1, GS_XYOFFSET(0x0, 0x0));
	// Clip to frame buffer.
	GIF_DATA_AD(gs_dma_buf, scissor_1, GS_SCISSOR(0, gs_max_x, 0, gs_max_y));

	SEND_GS_PACKET(gs_dma_buf);

	return 1;
}

//---------------------------------------------------------------------------
//
#define MAX_TRANSFER	16384

void gs_print_bitmap(u16 x, u16 y, u16 w, u16 h, u32 *data)
{
u32 i;			// DMA buffer loop counter
u32 frac;		// flag for whether to run a fractional buffer or not
u32 current;	// number of pixels to transfer in current DMA
u32 qtotal;		// total number of qwords of data to transfer

	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);
	GIF_DATA_AD(gs_dma_buf, bitbltbuf,
		GS_BITBLTBUF(0, 0, 0,
			0,						// frame buffer address
			(gs_max_x+1)/64,		// frame buffer width
			0));
	GIF_DATA_AD(gs_dma_buf, trxpos,
		GS_TRXPOS(0, 0,	x, y, 0));	// left to right/top to bottom
	GIF_DATA_AD(gs_dma_buf, trxreg, GS_TRXREG(w, h));
	GIF_DATA_AD(gs_dma_buf, trxdir, GS_TRXDIR(XDIR_EE_GS));
	SEND_GS_PACKET(gs_dma_buf);

	qtotal = w*h/4;					// total number of quadwords to transfer.
	current = qtotal % MAX_TRANSFER;// work out if a partial buffer transfer is needed.
	frac=1;							// assume yes.
	if(!current)					// if there is no need for partial buffer
	{
		current = MAX_TRANSFER;		// start with a full buffer
		frac=0;						// and don't do extra partial buffer first
	}
	for(i=0; i<(qtotal/MAX_TRANSFER)+frac; i++)
	{
		BEGIN_GS_PACKET(gs_dma_buf);
		GIF_TAG_IMG(gs_dma_buf, current);
		SEND_GS_PACKET(gs_dma_buf);

		SET_QWC(GIF_QWC, current);
		SET_MADR(GIF_MADR, data, 0);
		SET_CHCR(GIF_CHCR, 1, 0, 0, 0, 0, 1, 0);
		DMA_WAIT(GIF_CHCR);

		data += current*4;
		current = MAX_TRANSFER;		// after the first one, all are full buffers
	}
}

//---------------------------------------------------------------------------
void gs_reset(void)
{
	GS_RESET();
}
//---------------------------------------------------------------------------
u16 gs_get_max_x(void)
{
	return(gs_max_x);
}

//---------------------------------------------------------------------------
u16 gs_get_max_y(void)
{
	return(gs_max_y);
}
//---------------------------------------------------------------------------
void gs_set_fill_color(u8 r, u8 g, u8 b)
{
	gs_fill_r = r;
	gs_fill_g = g;
	gs_fill_b = b;
}
//---------------------------------------------------------------------------
void gs_fill_rect(u16 x0, u16 y0, u16 x1, u16 y1)
{
	BEGIN_GS_PACKET(gs_dma_buf);

	GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);
	GIF_DATA_AD(gs_dma_buf, prim, GS_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
	GIF_DATA_AD(gs_dma_buf, rgbaq, GS_RGBAQ(gs_fill_r, gs_fill_g, gs_fill_b, 0, 0));
	// The XYZ coordinates are actually floating point numbers between
	// 0 and 4096 represented as unsigned integers where the lowest order
	// four bits are the fractional point. That's why all coordinates are
	// shifted left 4 bits.
	GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2(x0<<4, y0<<4, 0));
	// It looks like the default operation for the SPRITE primitive is to
	// not draw the right and bottom 'lines' of the rectangle refined by
	// the parameters. Add +1 to change this.
	GIF_DATA_AD(gs_dma_buf, xyz2, GS_XYZ2((x1+1)<<4, (y1+1)<<4, 0));

	SEND_GS_PACKET(gs_dma_buf);
}

