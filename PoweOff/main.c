/*	Poweroff - shuts down the PlayStation 2 console.
	Copyright (C) Belek666	*/
#include <kernel.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <iopheap.h>
#include <libpwroff.h>
#include <sbv_patches.h>

extern unsigned char poweroff_irx[];
extern unsigned int size_poweroff_irx;

int main(int argc, char *argv[])
{  
	SifInitRpc(0);
	SifIopReset("", 0);
	while(!SifIopSync());

	SifInitRpc(0);
	SifLoadFileInit();
	SifInitIopHeap();
	sbv_patch_enable_lmb();

	SifExecModuleBuffer(poweroff_irx, size_poweroff_irx, 0, NULL, NULL);
	poweroffInit();
	poweroffShutdown();

	SifExitRpc();
	
	return 0;
}
