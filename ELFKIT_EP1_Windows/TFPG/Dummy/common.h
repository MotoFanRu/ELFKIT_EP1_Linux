#ifndef COMMON__H
#define COMMON__H

#include <typedefs.h>


typedef unsigned char		u8;
typedef signed char			s8;
typedef unsigned short		u16;
typedef signed short		s16;
typedef unsigned long		u32;
typedef signed long			s32;



#define	FP_BASE				10
#define	FP_ONE				(1<<FP_BASE)

typedef s32					FIXED;



#define	GLOBAL_STATE_EXIT	0
#define GLOBAL_STATE_INIT	1
#define GLOBAL_STATE_MAIN	2

extern u32					gGlobalState;



extern u32					gTick, gFPS;


typedef struct
{
	FIXED	x, y;

} Vector2D;



#endif