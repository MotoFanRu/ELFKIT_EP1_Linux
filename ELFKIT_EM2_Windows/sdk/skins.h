#ifndef SKINS_H
#define SKINS_H

#include <typedefs.h>

EXTERN_LIB MMA_UCP_URI;
#define mma_memory_str (WCHAR *)MMA_UCP_URI

//индекс текущего скина
UINT16 UIS_GetCurrentSkinIndex(void);

//индекс скина поумолчанию
UINT16 UIS_GetDefaultSkinIndex(void);

//им€ скина
WCHAR* UIS_GetSkinName(UINT16 SkinIndex);

//кол-во скинов
UINT16 UIS_GetNumSkins(void);

//установить скин
void UIS_SetSkin(UINT16 SkinIndex);

//¬озращает путь к папке, где хран€тс€ скины (0 - /e/mobile/skins, 1 - /a/mobile/skins/)
WCHAR* UIS_GetSkinStorageFolder (signed char);

#endif
