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

//0x801AD34 - ����� ��������� DAL. 0 - �� ���������������, 1 - ��������������� (��� v3x)

//��������� �������
void DAL_DisableDisplay (UINT32 unk); // unk = 0

//�������� �������
void DAL_EnableDisplay (UINT32 unk); // unk = 0

//��������� ������� (1 - ���., 0 - ����.)
UINT8 DAL_GetDisplayState (void);

//��������� ���������
UINT32 DAL_BackLightOff (void);

//�������� ���������
UINT32 DAL_BackLightOn (void);

//��������� ���������
UINT32 DAL_GetBackLightStatus (void);

//�������� ������������� �������
UINT32 DAL_AdjustContrast(UINT8 ContrastValue);
#endif
