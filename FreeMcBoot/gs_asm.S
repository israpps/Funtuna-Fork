#------------------------------------------------------------------------
# File:		gs_asm.s
# Author:	Tony Saveski, t_saveski@yahoo.com
# Notes:	GS routines implemented in assembler
#------------------------------------------------------------------------

#include "regnames.h"

#------------------------------------------------------------------------
.set noreorder

.text
.globl gs_set_imr
.globl gs_set_crtc

#------------------------------------------------------------------------
# void gs_set_imr(void);
#------------------------------------------------------------------------
.align 7
.ent gs_set_imr
gs_set_imr:
	li		a0,0x0000FF00
	ld		v0,csr
	dsrl	v0,16
	andi	v0,0xFF
	li		v1,0x71
	nop
	syscall
	nop

	daddu	v0,zero,zero	# return 0
	jr		ra
	nop
.end gs_set_imr

#------------------------------------------------------------------------
# void gs_set_crtc(uint8 int_mode, uint8 ntsc_pal, uint8 field_mode);
#------------------------------------------------------------------------
.align 7
.ent gs_set_crtc
gs_set_crtc:
	li		v1,0x02			# call SetGsCrt
	syscall
	nop

	daddu	v0,zero,zero	# return 0
	jr		ra
	nop
.end gs_set_crtc

