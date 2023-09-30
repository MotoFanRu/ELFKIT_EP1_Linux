#ifndef VIBE_H
#define VIBE_H

#include <typedefs.h>

//�������� �����
UINT32 vibe_TurnOn (void);

//��������� �����
UINT32 vibe_TurnOff (void);

//������������� ������������ ������������ � ��.
UINT32 vibe_SetTimer (UINT32 duration);

//������������� ������ � ��������� �����
UINT32 vibe_StopTimer (void);

//����������� ���������� ��������� ������� ������������
UINT32 vibe_HandleTimeout (void);
#endif
