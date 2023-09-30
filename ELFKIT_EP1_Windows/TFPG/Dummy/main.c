
#include "main.h"


u32					gTick, gFPS;

static u32			sPrevKeyboard;
static UINT64		sLastTime;


void * __wrap_memcpy(void *dst, const void *src, size_t sz )
{
    return __rt_memcpy( dst, src, sz );
}

// Точка входа в таск
void mainTask(void *arg)
{
	s32				error = 0;

	mainTaskInit();

	gfxInit();
	resInit();
	
	while( gGlobalState != GLOBAL_STATE_EXIT )
    {

		mainHandleInput();

		mainUpdateTick();

		gfxFlushDoublebuffer();

		#ifdef WIN32
			suSleep(20, &error);
		#else
			suSleep(1, &error);
		#endif
    }


	resTerminate();
	gfxTerminate();

	// Сигнализируем приложению, что мы завершаем работу
    appTerminate();

	// Таск самоуничтожится после выхода из этой функции
}


void mainTaskInit()
{
	sPrevKeyboard = DL_KeyKjavaGetKeyState();
	gTick = 0;
}


void mainTaskCreate()
{
	// Задаём точку входа для таска, размер стека и приоритет
	suCreateTask( mainTask, TASK_STACK_SIZE, TASK_PRIORITY );
}


u32 mainUpdateTick()
{
	UINT64				currentTime;
	u32					dt;
	
	currentTime = suPalTicksToMsec( suPalReadTime() );

	dt = (u32)(currentTime - sLastTime);
	
	sLastTime = currentTime;

	//gTick = dt;
	gTick = (gTick + dt) / 2;

	if(gTick != 0)
		gFPS = 1000 * 10 / gTick;
		
	return gTick;
}


void mainHandleInput()
{
	u32					keys;
	u32 				key = 0x00080000;
	
	keys = DL_KeyKjavaGetKeyState();
	
	while (key)
	{
		// Current key's state changed
		if ((sPrevKeyboard & key) != (keys & key))
		{
			// On key press
			if( keys & key )
			{
				switch( key )
				{
					case MULTIKEY_STAR:
						gGlobalState = GLOBAL_STATE_EXIT;
					break;
					
					case MULTIKEY_POUND:
						dbgf("FPS x 10: %d", gFPS);
					break;
				}
			}
		}

		// On key hold
		if( keys & key )
		{
			switch( key )
			{
					
			}
		}

		key >>= 1;
	}
	
	sPrevKeyboard = keys;
}

