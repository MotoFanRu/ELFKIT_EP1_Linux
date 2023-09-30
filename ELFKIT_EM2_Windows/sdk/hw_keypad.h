#ifndef HW_KEYBOARD_H
#define HW_KEYBOARD_H

#include <typedefs.h>

#define ___KEY_0       0xFFFBFFFF
#define ___KEY_1       0xFDFFFFFF
#define ___KEY_2       0xFFFFFFDF
#define ___KEY_3       0xFBFFFFFF
#define ___KEY_4       0xFFFFFFBF
#define ___KEY_5       0xFFFEFFFF
#define ___KEY_6       0xFFFFFF7F
#define ___KEY_7       0xFFFFFEFF
#define ___KEY_8       0xFFFFFFEF
#define ___KEY_9       0xFFFFFFFE
#define ___KEY_STAR    0xFFFFFFF7
#define ___KEY_POUND   0xFFFFFFFB
#define ___KEY_GREEN   0xFFFFFBFF
#define ___KEY_RED     0xFFFFFFFF // -1 ???
#define ___KEY_C       0xFFFFBFFF
#define ___KEY_SKLEFT  0xFFFF7FFF
#define ___KEY_SKRIGHT 0xFFFFDFFF
#define ___KEY_OK      0x7FFFFFFF
#define ___KEY_LEFT    0xEFFFFFFF
#define ___KEY_RIGHT   0xF7FFFFFF
#define ___KEY_UP      0xBFFFFFFF
#define ___KEY_DOWN    0xDFFFFFFF
#define ___KEY_VIDEOC  0xFFFFF7FF
#define ___KEY_WEB     0xFFFFEFFF
#define ___KEY_SOFT    0xFF7FFFFF
#define ___KEY_SOFTUP  0xFFDFFFFF
#define ___KEY_SOFTDW  0xFFBFFFFF
#define ___KEY_SOFTCAM 0xFFEFFFFF
#define ___KEY_SOFTVOI 0xFFF7FFFF

/* Keypad Port module register struct. */
typedef struct
{
	UINT16 kpcr;      /* Keypad Control Register */
	UINT16 kpsr;      /* Keypad Status Register */
	UINT16 kddr;      /* Keypad Data Direction Register */
	UINT16 kpdr;      /* Keypad Data Register */
} KPP_T;		  /* 0x219000 - address for RAZR V3x R252211LD_U_85.9B.E6P */

_u64 HAPI_KEYPAD_read_2_pole_keycode(void* ptr);
void HAPI_WASTE_TIME_usec_coarse (void);

//Тестировалось только для v3x r252211ld_u_85.9b.e6p. Ф-ция возвращает состояние (отжата/нажата) кнопок (максимум двух)
//Адрес KPP_T для RAZR V3x - 0x219000
static UINT32 get_key_code(KPP_T* kpp)
{
	//dbg("keypad init...");
	kpp->kpcr = 0x00FF;
	kpp->kpsr = 0x0105;
	kpp->kddr = 0x0F00;
	kpp->kpdr = 0x0000;

	//dbg("Getting keypad state...");
	_u64 result = HAPI_KEYPAD_read_2_pole_keycode((void*)HAPI_WASTE_TIME_usec_coarse);

	//Такая проверка была в прошиве при входе в бут...
	if(result.R0 == 0xFFFFFFFF) return result.R1;
	else return 0;
}

#endif
