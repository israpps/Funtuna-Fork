#------------------------------------------------------------------------
# File:		ps2_asm.s
# Author:	Tony Saveski, t_saveski@yahoo.com
# Notes:	PS2 System routines implemented in assembler
#------------------------------------------------------------------------

#include "regnames.h"

#------------------------------------------------------------------------
.set noreorder

.text
.globl ps2_flush_cache

#------------------------------------------------------------------------
# void ps2_flush_cache(int); 			Flushes the PS2 Data Cache.
#------------------------------------------------------------------------
.align 7
.ent ps2_flush_cache
ps2_flush_cache:
	li	$3,100
	syscall
	jr	$31
	nop
.end ps2_flush_cache

