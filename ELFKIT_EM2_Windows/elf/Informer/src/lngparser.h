#define DEBUG

#ifndef LNGPARSER_H
#define LNGPARSER_H

#include <apps.h>
#include <filesystem.h>
#include <utilities.h>

typedef enum
{
    // дни недели
    LNG_MONDAY,
    LNG_TUESDAY,
    LNG_WEDNESDAY,
    LNG_THURSDAY,
    LNG_FRIDAY,
    LNG_SATURDAY,
    LNG_SUNDAY,


    // погода
    LNG_CLEAR, // ясно
    LNG_LITTLECLOUDY, // малооблачно
    LNG_CLOUDY, // облачно
    LNG_DULL, // пасмурно
    LNG_RAIN, // дождь
    LNG_DOWNPOUR, // ливень
    LNG_SNOW, // снег
    LNG_THUNDERSTORM, // гроза
    LNG_WITHOUTPRECIPITATION, // без осадков
    LNG_PERHAPS, // возможно
    LNG_NIGHT, // ночь
    LNG_MORNING, // утро
    LNG_DAY, // день
    LNG_EVENING, // вечер
    LNG_N, // север
    LNG_S, // юг
    LNG_O, // восток
    LNG_W, // запад
    LNG_MS, // м/с
    LNG_OLD, // остарел

    LNG_MAX

} LNG_RES_ID_T;


typedef struct
{
    WCHAR   *res;
    WCHAR   *str[LNG_MAX];
} LNG_RES_T;
extern LNG_RES_T Lang;


UINT32 ReadLang(WCHAR *uri);
void CloseLang(void);

#endif

