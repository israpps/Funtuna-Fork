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
	
	FMCB embedded Mini Elf-Loader v1.0

---------------------------------------------------------------------------		
*/

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <string.h>
#include <stdio.h>
#include <debug.h>

void _ps2sdk_libc_init() {}
void _ps2sdk_libc_deinit() {}

#define DEBUG

#define RGBA(r, g, b, a)    \
        ((u64)(r)|((u64)(g) << 8)|((u64)(b) << 16)|((u64)(a) << 24))
        
#ifdef DEBUG
#define SETBG(r, g, b) *(u64*)0x120000e0 = RGBA(r, g, b, 0)
#else
#define SETBG(...)
#endif



//--------------------------------------------------------------

void launch_OSDSYS(void)
{
	__asm__ __volatile__(
		"	li $3, 0x04;"
		"	syscall;"
		"	nop;"
	);
}

//--------------------------------------------------------------

void execute_elf(u8 *pexecPath) 
{   // Execute an elf with SifLoadElf + ExecPS2
	t_ExecData exd;	
	int r;
	
	exd.epc = 0;
	exd.gp = 0;
	exd.sp = 0;

    SifLoadFileInit();
	r = SifLoadElf(pexecPath, &exd);
	if ((!r) && (exd.epc))
  	{
		char *args[1];
  	    SifLoadFileExit();
  	  	fioExit();
  	  	SifExitRpc(); //some programs need it to be here

  	  	FlushCache(0);
  	  	FlushCache(2);
  	  	
    	args[0] = pexecPath;
    	ExecPS2((void*)exd.epc, (void*)exd.gp, 1, args);

    	SETBG(0x00, 0x00, 0xff); // Error, Blue screen
    	while (1){;}
  	}
  	else {
  	    SifLoadFileExit();
  	  	fioExit();
  	  	SifExitRpc();
	  	
  		launch_OSDSYS();
	}
  	
 	SETBG(0xff, 0xff, 0xff);  	 // Error2, White screen
  	while (1){;}		
}

//--------------------------------------------------------------
// MAIN FUNC
//--------------------------------------------------------------

int main (int argc, char *argv[1])
{
  int i;	

  //clearing mem, so better not to have anything valuable on stack
  for (i = 0x100000; i < 0x2000000 ; i += 64) {
  asm (
    "\tsq $0, 0(%0) \n"
    "\tsq $0, 16(%0) \n"
    "\tsq $0, 32(%0) \n"
    "\tsq $0, 48(%0) \n"
    :: "r" (i) );
  }
     
  SifInitRpc(0);
  
  execute_elf(argv[0]);

  return 0;
}
