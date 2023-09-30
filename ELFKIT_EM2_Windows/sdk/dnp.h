#ifndef DNP_H
#define DNP_H

#include <typedefs.h>

//—труктура файла dnp.prf
//1. DNP_HEADER_T
//2. DNP_ELEMENT_T * DNP_HEADER_T::records_num
//3. Unicode строки

typedef struct
{
    UINT16 records_num;
} DNP_HEADER_T;

typedef struct
{
    UINT32 string_id;       //id строки из ленга
    UINT8  language_id;     //id €зыка
    UINT32 string_size;     //размер строки в байтах. длина строки = string_size/2 - 1
    WCHAR* string_data;     //указатель на строку. дл€ dnp.prf это смещение в файле относительно начала
} DNP_RECORD_T;

#endif
