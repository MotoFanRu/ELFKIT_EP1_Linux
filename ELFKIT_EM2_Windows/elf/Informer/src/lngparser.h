#define DEBUG

#ifndef LNGPARSER_H
#define LNGPARSER_H

#include <apps.h>
#include <filesystem.h>
#include <utilities.h>

typedef enum
{
    // ��� ������
    LNG_MONDAY,
    LNG_TUESDAY,
    LNG_WEDNESDAY,
    LNG_THURSDAY,
    LNG_FRIDAY,
    LNG_SATURDAY,
    LNG_SUNDAY,


    // ������
    LNG_CLEAR, // ����
    LNG_LITTLECLOUDY, // �����������
    LNG_CLOUDY, // �������
    LNG_DULL, // ��������
    LNG_RAIN, // �����
    LNG_DOWNPOUR, // ������
    LNG_SNOW, // ����
    LNG_THUNDERSTORM, // �����
    LNG_WITHOUTPRECIPITATION, // ��� �������
    LNG_PERHAPS, // ��������
    LNG_NIGHT, // ����
    LNG_MORNING, // ����
    LNG_DAY, // ����
    LNG_EVENING, // �����
    LNG_N, // �����
    LNG_S, // ��
    LNG_O, // ������
    LNG_W, // �����
    LNG_MS, // �/�
    LNG_OLD, // �������

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

