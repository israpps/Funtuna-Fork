#------------------------------------------------------------------------
# File:		dma_asm.s
# Author:	Tony Saveski, t_saveski@yahoo.com
# Notes:	DMA routines implemented in assembler
#------------------------------------------------------------------------

#include "regnames.h"

#------------------------------------------------------------------------
.set noreorder

.text
.globl dma_reset

#------------------------------------------------------------------------
# void dma_reset();
#------------------------------------------------------------------------
# Ripped straight from 3stars or something. Will 'clean up later'.
#------------------------------------------------------------------------
.align 7
.ent dma_reset
dma_reset:
	li	$2,0x1000A000
	nop
	sw	$0,0x80($2)
	sw	$0,0($2)
	sw	$0,0x30($2)
	sw	$0,0x10($2)
	sw	$0,0x50($2)
	sw	$0,0x40($2)
	li	$2,0xFF1F
	sw	$2,0x1000E010
	lw	$2,0x1000E010
	li	$3,0xFF1F
	and	$2,$3
	sw	$2,0x1000E010
	sw	$0,0x1000E000
	sw	$0,0x1000E020
	sw	$0,0x1000E030
	sw	$0,0x1000E050
	sw	$0,0x1000E040
	li	$3,1
	lw	$2,0x1000E000
	ori	$3,$2,1
	nop
	sw	$3,0x1000E000
	nop

	daddu	v0,zero,zero	# return 0
	jr		ra
	nop
.end dma_reset

