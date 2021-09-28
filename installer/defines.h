//---------------------------------------------------------------------------
// File:	defines.h
// Author:	Tony Saveski, t_saveski@yahoo.com
// Notes:	Commonly used data-type definitions.
//---------------------------------------------------------------------------
#ifndef DEFINES_H
#define DEFINES_H

#define SUCCESS		1
#define FAILURE		0

typedef char	int8;
typedef short	int16;
typedef int		int32;

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

//---------------------------------------------------------------------------
#ifdef PS2_EE

typedef unsigned long	uint64;
typedef long			int64;

typedef struct int128
{
	int64 lo, hi;
} int128 __attribute__((aligned(16)));

typedef struct uint128
{
	uint64 lo, hi;
} uint128 __attribute__((aligned(16)));

//typedef int 			int128 __attribute__(( mode(TI), aligned(16) ));
//typedef unsigned int 	uint128 __attribute__(( mode(TI), aligned(16) ));

#endif // PS2_EE

//---------------------------------------------------------------------------
#ifdef WIN32
typedef unsigned __int64	uint64;
typedef __int64				int64;
#define __attribute__(X)
#endif


#endif // DEFINES_H
