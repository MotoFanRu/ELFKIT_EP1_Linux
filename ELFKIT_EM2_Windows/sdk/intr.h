#ifndef INTR_H
#define INTR_H

#include <typedefs.h>

void intr_irq_disable(UINT8 intr);	//отключает прерывание intr
void intr_irq_enable(UINT8 intr);	//включает прерывание intr

UINT32 suDisableAllInt(void); // отключает прерывания
void suSetInt(UINT32 mask);   // включает прерывания, в параметрах передаётся то, что вернула suDisableAllInt

#endif
