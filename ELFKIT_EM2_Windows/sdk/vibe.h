#ifndef VIBE_H
#define VIBE_H

#include <typedefs.h>

//¬ключает вибро
UINT32 vibe_TurnOn (void);

//выключает вибро
UINT32 vibe_TurnOff (void);

//устанавливает длительность вибрировани€ в мс.
UINT32 vibe_SetTimer (UINT32 duration);

//останавливает таймер и выключает вибро
UINT32 vibe_StopTimer (void);

//стандартный обработчик истечени€ времени вибрировани€
UINT32 vibe_HandleTimeout (void);
#endif
