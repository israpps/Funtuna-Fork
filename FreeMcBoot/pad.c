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

 The following Code is from uLaunchELF
 
 ---------------------------------------------------------------------------
*/

#include <tamtypes.h>
#include <kernel.h>
#include <libpad.h>

// ntsc_pal
#define NTSC			2
#define PAL				3

// For video Mode
extern int VMode;

u32 new_pad;

int  readPad(void);
void waitAnyPadReady(void);
int  setupPad(void);

//________________ From uLaunchELF ______________________

static char padBuf_t[2][256] __attribute__((aligned(64)));
struct padButtonStatus buttons_t[2];
u32 padtype_t[2];
u32 paddata, paddata_t[2];
u32 old_pad = 0, old_pad_t[2] = {0, 0};
u32 new_pad_t[2];
u32 joy_value = 0;
static int test_joy = 0;

#define PAD_R3_V0 0x010000
#define PAD_R3_V1 0x020000
#define PAD_R3_H0 0x040000
#define PAD_R3_H1 0x080000
#define PAD_L3_V0 0x100000
#define PAD_L3_V1 0x200000
#define PAD_L3_H0 0x400000
#define PAD_L3_H1 0x800000

//----------------------------------------------------------------
int readPad(void)
{
	static int n[2]={0,0}, nn[2]={0,0};
	int port, state, ret[2];

	for(port=0; port<2; port++){
		if((state=padGetState(port, 0))==PAD_STATE_STABLE
			||(state == PAD_STATE_FINDCTP1)){
			//Deal with cases where pad state is valid for padRead
			ret[port] = padRead(port, 0, &buttons_t[port]);
			if (ret[port] != 0){
				paddata_t[port] = 0xffff ^ buttons_t[port].btns;
				if((padtype_t[port] == 2) && (1 & (test_joy++))){//DualShock && time for joy scan
					joy_value=0;
					if(buttons_t[port].rjoy_h >= 0xbf){
						paddata_t[port]=PAD_R3_H1;
						joy_value=buttons_t[port].rjoy_h-0xbf;
					}else if(buttons_t[port].rjoy_h <= 0x40){
						paddata_t[port]=PAD_R3_H0;
						joy_value=-(buttons_t[port].rjoy_h-0x40);
					}else if(buttons_t[port].rjoy_v <= 0x40){
						paddata_t[port]=PAD_R3_V0;
						joy_value=-(buttons_t[port].rjoy_v-0x40);
					}else if(buttons_t[port].rjoy_v >= 0xbf){
						paddata_t[port]=PAD_R3_V1;
						joy_value=buttons_t[port].rjoy_v-0xbf;
					}else if(buttons_t[port].ljoy_h >= 0xbf){
						paddata_t[port]=PAD_L3_H1;
						joy_value=buttons_t[port].ljoy_h-0xbf;
					}else if(buttons_t[port].ljoy_h <= 0x40){
						paddata_t[port]=PAD_L3_H0;
						joy_value=-(buttons_t[port].ljoy_h-0x40);
					}else if(buttons_t[port].ljoy_v <= 0x40){
						paddata_t[port]=PAD_L3_V0;
						joy_value=-(buttons_t[port].ljoy_v-0x40);
					}else if(buttons_t[port].ljoy_v >= 0xbf){
						paddata_t[port]=PAD_L3_V1;
						joy_value=buttons_t[port].ljoy_v-0xbf;
					}
				}
				new_pad_t[port] = paddata_t[port] & ~old_pad_t[port];
				if(old_pad_t[port]==paddata_t[port]){ //if no change of pad data
					n[port]++;
					if(VMode == NTSC){ //Fix repeats for NTSC
						if(n[port]>=25){ //NTSC initial repeat delay == 25 loops (0.416 sec)
							new_pad_t[port]=paddata_t[port];
							if(nn[port]++ < 20)	n[port]=20; //early repeats use 25-20 loops
							else			n[port]=23;           //later repeats use 25-23 loops
						}
					}else{ //Fix repeats for PAL
						if(n[port]>=21){ //PAL initial repeat delay == 21 loops (0.42 sec)
							new_pad_t[port]=paddata_t[port];
							if(nn[port]++ < 20)	n[port]=17; //early repeats use 21-17 loops
							else			n[port]=19;           //later repeats use 21-19 loops
						}
					}
				}else{ //pad data has changed !
					n[port]=0;
					nn[port]=0;
					old_pad_t[port] = paddata_t[port];
				}
			}
		}else{
			//Deal with cases where pad state is not valid for padRead
			new_pad_t[port]=0;
		}  //ends 'if' testing for state valid for padRead
	}  //ends for
	new_pad = new_pad_t[0]|new_pad_t[1];
	return (ret[0]|ret[1]);
}
//----------------------------------------------------------------
// Wait for specific PAD, but also accept disconnected state
void waitPadReady(int port, int slot)
{
	int state, lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;
	while((state != PAD_STATE_DISCONN)
		&& (state != PAD_STATE_STABLE)
		&& (state != PAD_STATE_FINDCTP1)){
		if (state != lastState)
			padStateInt2String(state, stateString);
		lastState = state;
		state=padGetState(port, slot);
	}
}
//---------------------------------------------------------------------------
// Wait for any PAD, but also accept disconnected states
void waitAnyPadReady(void)
{
	int state_1, state_2;

	state_1 = padGetState(0, 0);
	state_2 = padGetState(1, 0);
	while((state_1 != PAD_STATE_DISCONN) && (state_2 != PAD_STATE_DISCONN)
		&& (state_1 != PAD_STATE_STABLE) && (state_2 != PAD_STATE_STABLE)
		&& (state_1 != PAD_STATE_FINDCTP1) && (state_2 != PAD_STATE_FINDCTP1)){
		state_1 = padGetState(0, 0);
		state_2 = padGetState(1, 0);
	}
}
//----------------------------------------------------------------
// setup PAD
int setupPad(void)
{
	int ret, i, port, state, modes;

	padInit(0);

	for(port=0; port<2; port++){
		padtype_t[port] = 0;  //Assume that we don't have a proper PS2 controller
		if((ret = padPortOpen(port, 0, &padBuf_t[port][0])) == 0)
			return 0;
		waitPadReady(port, 0);
		state = padGetState(port, 0);
		if(state != PAD_STATE_DISCONN){ //if anything connected to this port
			modes = padInfoMode(port, 0, PAD_MODETABLE, -1);
			if (modes != 0){ //modes != 0, so it may be a dualshock type
				for(i=0; i<modes; i++){
					if (padInfoMode(port, 0, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK){
						padtype_t[port] = 2; //flag normal PS2 controller
						break;
					}
				} //ends for (modes)
			} else { //modes == 0, so this is a digital controller
				padtype_t[port] = 1; //flag digital controller
			}
			if(padtype_t[port] == 2)                                        //if DualShock
				padSetMainMode(port, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);   //Set DualShock
			else                                                            //else
				padSetMainMode(port, 0, PAD_MMODE_DIGITAL, PAD_MMODE_UNLOCK);   //Set Digital
			waitPadReady(port, 0);                                          //Await completion
		} else {                                          //Nothing is connected to this port
				padSetMainMode(port, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK); //Fake DualShock
				waitPadReady(port, 0);                                        //Await completion
		}
	} //ends for (port)
	return 1;
}
