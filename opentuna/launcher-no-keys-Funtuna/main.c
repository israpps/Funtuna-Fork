
#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <libcdvd.h>

#include <fcntl.h>
#include <sbv_patches.h>
#include <libpad.h>
#include <stdio.h>
#include <debug.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>




#define NTSC 2
#define PAL 3

#define DELAY 2000

#define MAX_PATH 1025

// ELF-loading stuff
#define ELF_MAGIC 0x464c457f
#define ELF_PT_LOAD 1

extern u8 loader_elf[];
extern int size_loader_elf;




//------------------------------
typedef struct
{
	u8 ident[16]; // struct definition for ELF object header
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
//------------------------------
typedef struct
{
	u32 type; // struct definition for ELF program section header
	u32 offset;
	void *vaddr;
	u32 paddr;
	u32 filesz;
	u32 memsz;
	u32 flags;
	u32 align;
} elf_pheader_t;






void ResetIOP()
{
	SifInitRpc(0);
	while (!SifIopReset("", 0))
	{
	};
	while (!SifIopSync())
	{
	};
	SifInitRpc(0);
}

void InitPS2()
{
	//init_scr();
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	fioInit();
	sbv_patch_disable_prefix_check();
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	
}

void LoadElf(char *filename, char *party)
{
	u8 *boot_elf;
	elf_header_t *eh;
	elf_pheader_t *eph;
	void *pdata;
	int i;
	char *argv[2], bootpath[256];

	argv[0] = filename;
	argv[1] = filename;

	/* NB: LOADER.ELF is embedded  */
	boot_elf = (u8 *)loader_elf;
	eh = (elf_header_t *)boot_elf;
	if (_lw((u32)&eh->ident) != ELF_MAGIC)
		asm volatile("break\n");

	eph = (elf_pheader_t *)(boot_elf + eh->phoff);

	/* Scan through the ELF's program headers and copy them into RAM, then
									zero out any non-loaded regions.  */
	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != ELF_PT_LOAD)
			continue;

		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy(eph[i].vaddr, pdata, eph[i].filesz);

		if (eph[i].memsz > eph[i].filesz)
			memset(eph[i].vaddr + eph[i].filesz, 0,
				   eph[i].memsz - eph[i].filesz);
	}

	/* Let's go.  */
	SifExitRpc();
	FlushCache(0);
	FlushCache(2);

	ExecPS2((void *)eh->entry, NULL, 2, argv);
}



int file_exists(char filepath[])
{
	int fdn;

	fdn = open(filepath, O_RDONLY);
	if (fdn < 0)
		return 0;
	close(fdn);

	return 1;
}

int main(int argc, char *argv[])
{

	
	
	
	
	InitPS2();


	


		if (file_exists("mc0:/BOOT/BOOT.ELF"))
			LoadElf("mc0:/BOOT/BOOT.ELF", "mc0:/BOOT/");

		if (file_exists("mc1:/BOOT/BOOT.ELF"))
			LoadElf("mc1:/BOOT/BOOT.ELF", "mc1:/BOOT/");

		
	
		__asm__ __volatile__(
					    "	li $3, 0x04;"
					    "	syscall;"
					    "	nop;");
	return 0;
}
