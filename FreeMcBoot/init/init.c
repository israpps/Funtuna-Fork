//we don't need most of this... just standard headers
#include <types.h>
#include <stdio.h>
#include <sysclib.h>
#include <thbase.h>
#include <thsemap.h>
#include <intrman.h>
#include <sysmem.h>
#include <sifman.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <loadcore.h>
//#include <sys/stat.h>
#include <sys/fcntl.h>
#include <ioman.h>


#define MODNAME "custominitcd"
IRX_ID(MODNAME, 0x01, 0x01);

vu8 cdvdRead(u8 num)
{
  vu8 cdtest;
  cdtest = (*(vu8*)(0xbf402000 + num));
  //printf("cdvdRead%02x = 0x%02x\n", num, cdtest);
  DelayThread(1000); //just to make sure
  return cdtest;
}
void cdvdWrite(u8 num, u8 val)
{
  //printf("cdvdWrite%02x, val = 0x%02x\n", num, val);
  (*(vu8*)(0xbf402000 + num)) = val;
  DelayThread(1000); //just to make sure
}

void tm_readConfig(u8 arg1, u8 arg2, u8 arg3, u8* mem)
{
  int k, resGet;
  u8 result;

  cdvdRead(0x05);
  cdvdRead(0x05);
  while (cdvdRead(0x17) != 0x40) {;}
  
  cdvdWrite(0x17, arg1);
  cdvdWrite(0x17, arg2);
  cdvdWrite(0x17, arg3);

  cdvdWrite(0x16, 0x40);
  while (cdvdRead(0x16) != 0x40){;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  resGet = 0;
  for (k = 0; k < arg3; k++)
  {
    while (cdvdRead(0x17) != 0x40) {;}
    cdvdWrite(0x16, 0x41);
    while (cdvdRead(0x16) != 0x41){;}
    while (cdvdRead(0x17) != 0x40)
    {
      result = cdvdRead(0x18);
      if (mem != NULL) mem[resGet] = result;
      resGet++;
    }
  }
  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x43);
  while (cdvdRead(0x16) != 0x43){;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
}

void mechaconAuth()
{
  int k;
  
  while (cdvdRead(0x17) != 0x40) {;}
  

  cdvdWrite(0x17, 0);

  cdvdWrite(0x16, 0x80);
  while (cdvdRead(0x16) != 0x80) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x81);
  while (cdvdRead(0x16) != 0x81) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 16; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x82);
  while (cdvdRead(0x16) != 0x82) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 8; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x83);
  while (cdvdRead(0x16) != 0x83) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x8f);
  while (cdvdRead(0x16) != 0x8f) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x84);
  while (cdvdRead(0x16) != 0x84) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x85);
  while (cdvdRead(0x16) != 0x85) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 16; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x86);
  while (cdvdRead(0x16) != 0x86) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  
  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 8; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x87);
  while (cdvdRead(0x16) != 0x87) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x8f);
  while (cdvdRead(0x16) != 0x8f) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x88);
  while (cdvdRead(0x16) != 0x88) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x17, 0x08);

  cdvdWrite(0x16, 0x80);
  while (cdvdRead(0x16) != 0x80) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x17, 0x08);

  cdvdWrite(0x16, 0x81);
  while (cdvdRead(0x16) != 0x81) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }


  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 16; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x82);
  while (cdvdRead(0x16) != 0x82) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 8; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x83);
  while (cdvdRead(0x16) != 0x83) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x8f);
  while (cdvdRead(0x16) != 0x8f) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  
  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x84);
  while (cdvdRead(0x16) != 0x84) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x85);
  while (cdvdRead(0x16) != 0x85) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 16; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x86);
  while (cdvdRead(0x16) != 0x86) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  for (k = 0; k < 8; k++)
  {
    cdvdWrite(0x17, 0xff);
  }

  cdvdWrite(0x16, 0x87);
  while (cdvdRead(0x16) != 0x87) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x8f);
  while (cdvdRead(0x16) != 0x8f) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
  

  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x88);
  while (cdvdRead(0x16) != 0x88) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
}

void tm_subcommands()
{
  while (cdvdRead(0x17) != 0x40) {;}

  cdvdWrite(0x17, 0x00);

  cdvdWrite(0x16, 0x03);
  while (cdvdRead(0x16) != 0x03) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
}

//regional functionality
u8 my_2ctoi(u8 char_in1, u8 char_in2)
{
  if ((char_in1 > 0x39) || (char_in1 < 0x30))
  {
    char_in1 = 0x30;
  }
  if ((char_in2 > 0x39) || (char_in2 < 0x30))
  {
    char_in2 = 0x30;
  }
  return ((char_in1 - 0x30)*10) + (char_in2 - 0x30);
}
u8 romver[16];
void tm_standard()
{
  int fd;
  if((fd = open("rom0:ROMVER", O_RDONLY)))
  {
    read(fd, romver, sizeof romver);
    close(fd);
  } else
  {
  //PAL 1.90 - default
    romver[0] = 0x30; //0
    romver[1] = 0x31; //1
    romver[2] = 0x39; //9
    romver[3] = 0x30; //0
    romver[4] = 0x45;
    romver[5] = 0x43;
  }
  
  while (cdvdRead(0x17) != 0x40) {;}

//Universal
//do it the hard way... why use sysclib ;)
  cdvdWrite(0x17, my_2ctoi(romver[0], romver[1])); //1
  cdvdWrite(0x17, my_2ctoi(romver[2], romver[3])); //90
  cdvdWrite(0x17, romver[4]);
  cdvdWrite(0x17, romver[5]);
/*
//PAL 1.90
  cdvdWrite(0x17, 0x01);
  cdvdWrite(0x17, 0x5a);
  cdvdWrite(0x17, 0x45);
  cdvdWrite(0x17, 0x43);
*/
/*
//NTSC 1.70
  cdvdWrite(0x17, 0x01);
  cdvdWrite(0x17, 0x46);
  cdvdWrite(0x17, 0x41);
  cdvdWrite(0x17, 0x43);
*/
  cdvdWrite(0x16, 0x1a);
  while (cdvdRead(0x16) != 0x1a) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }

  
  while (cdvdRead(0x17) != 0x40) {;}
  cdvdWrite(0x16, 0x22);
  while (cdvdRead(0x16) != 0x22) {;}
  while (cdvdRead(0x17) != 0x40)
  {
    cdvdRead(0x18);
  }
}
u32 config[6] __attribute__((aligned (128))) = {0,1,0,0,0,0};
u8 configArray[32];
int _start( int argc, char **argv)
{
  int intStatus, status;
  struct t_SifDmaTransfer dmaStruct;
  
  SifInitRpc(0);
//We don't really need this part, since we don't eve use
//the configs.... but that's what kernel and original osd does.
//Anyway, everything up to mechaconAuth can be skipped.
//There will be no interrupts, so we don't really need interrupt
//cause values... but in kernel/osd they're also checked not
//from interrupt handlers... just for some odd reason
  tm_readConfig(0,0,4, NULL);
  tm_readConfig(0,1,2, NULL);
  cdvdRead(0x05); //not needed... just to be exact
  cdvdRead(0x05); //not needed... just to be exact
  cdvdRead(0x05); //not needed... just to be exact
  
  tm_readConfig(0,0,4, NULL);
  tm_readConfig(0,1,2, configArray); //get OSD config data from NVRAM
  cdvdRead(0x05); //not needed... just to be exact
  cdvdRead(0x05); //not needed... just to be exact
  cdvdRead(0x05); //not needed... just to be exact
  
  cdvdRead(0x08); //not needed... just to be exact
  cdvdRead(0x08); //not needed... just to be exact
  
//here goes the essential part
  mechaconAuth();
  cdvdRead(0x08); //not needed... just to be exact
  cdvdRead(0x08); //not needed... just to be exact
  
  tm_subcommands();
  tm_standard(); //PAL/NTSC
  
//this will disable dvdvideo reading (other files than dvdv struct), just like osdsys does.
//It can be restored by either loading rom1:EROMDRV (at least EEUG says so, didn't try it)
//or additional inits. Just don't call it... blocking yourself is a really dumb solution ;)
  //tm_forbid();
    
  config[0] = configArray[16];
  config[1] = configArray[17];
  config[2] = configArray[18];
  config[3] = configArray[19];      
 
  // Get bios version 
  config[4] = (my_2ctoi(romver[0], romver[1]) * 100) + my_2ctoi(romver[2], romver[3]);
  config[5] = romver[4];  

  dmaStruct.size=24; //6 u32
  dmaStruct.attr=0;
  dmaStruct.src =(void*)&config;
  dmaStruct.dest=(void*)(0x20100000); //just send it anywhere... there is ~31MB of free memory ;).

  CpuSuspendIntr(&intStatus);
  status = SifSetDma(&dmaStruct, 1);
  CpuResumeIntr(intStatus);
  while(SifDmaStat(status)>=0);

  return 1;
}
