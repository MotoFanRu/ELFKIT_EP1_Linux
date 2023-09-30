#ifndef SKINS_H
#define SKINS_H

#include <typedefs.h>

EXTERN_LIB MMA_UCP_URI;
#define mma_memory_str (WCHAR *)MMA_UCP_URI

//������ �������� �����
UINT16 UIS_GetCurrentSkinIndex(void);

//������ ����� �����������
UINT16 UIS_GetDefaultSkinIndex(void);

//��� �����
WCHAR* UIS_GetSkinName(UINT16 SkinIndex);

//���-�� ������
UINT16 UIS_GetNumSkins(void);

//���������� ����
void UIS_SetSkin(UINT16 SkinIndex);

//��������� ���� � �����, ��� �������� ����� (0 - /e/mobile/skins, 1 - /a/mobile/skins/)
WCHAR* UIS_GetSkinStorageFolder (signed char);

#endif
