#ifndef MAIN__H
#define MAIN__H

#include <loader.h>
#include <mem.h>
#include <tasks.h>
#include <ATI.h>

#include "common.h"
#include "resources.h"
#include "gfx.h"
#include "dbg.h"


#define TASK_STACK_SIZE		0x4000
#define	TASK_PRIORITY		0x18


extern UINT32 appTerminate();

void mainTask(void *arg);
void mainTaskInit();
void mainHandleInput();
u32 mainUpdateTick();



// PUBLIC

void mainTaskCreate();


#endif
