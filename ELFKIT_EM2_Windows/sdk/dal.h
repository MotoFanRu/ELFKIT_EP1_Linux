#ifndef DAL_H
#define DAL_H

#include <typedefs.h>

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
