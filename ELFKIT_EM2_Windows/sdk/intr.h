#ifndef INTR_H
#define INTR_H

#include <typedefs.h>

void intr_irq_disable(UINT8 intr);	//��������� ���������� intr
void intr_irq_enable(UINT8 intr);	//�������� ���������� intr

UINT32 suDisableAllInt(void); // ��������� ����������
void suSetInt(UINT32 mask);   // �������� ����������, � ���������� ��������� ��, ��� ������� suDisableAllInt

#endif
