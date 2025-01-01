#ifndef DAL_H
#define DAL_H

#include <typedefs.h>

enum DISPLAY_TYPE_ENUM {
	DISPLAY_MAIN = 0,
	DISPLAY_CLI,
	DISPLAY_EXTERNAL,
	DISPLAY_MAIN_AND_CLI,
	DISPLAY_NONE
};
typedef UINT8 DISPLAY_TYPE_T;

//0x801AD34 - Адрес состояния DAL. 0 - не инициализирован, 1 - инициализирован (для v3x)

//Выключить дисплей
void DAL_DisableDisplay (UINT32 unk); // unk = 0

//Включить дисплей
void DAL_EnableDisplay (UINT32 unk); // unk = 0

//Состояние дисплея (1 - вкл., 0 - выкл.)
UINT8 DAL_GetDisplayState (void);

//Вылкючить подсветку
UINT32 DAL_BackLightOff (void);

//Включить подсветку
UINT32 DAL_BackLightOn (void);

//Состояние подсветки
UINT32 DAL_GetBackLightStatus (void);

//Изменить контрастность дисплея
UINT32 DAL_AdjustContrast(UINT8 ContrastValue);
#endif
