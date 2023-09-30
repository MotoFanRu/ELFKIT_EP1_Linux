#ifndef DNP_H
#define DNP_H

#include <typedefs.h>

//��������� ����� dnp.prf
//1. DNP_HEADER_T
//2. DNP_ELEMENT_T * DNP_HEADER_T::records_num
//3. Unicode ������

typedef struct
{
    UINT16 records_num;
} DNP_HEADER_T;

typedef struct
{
    UINT32 string_id;       //id ������ �� �����
    UINT8  language_id;     //id �����
    UINT32 string_size;     //������ ������ � ������. ����� ������ = string_size/2 - 1
    WCHAR* string_data;     //��������� �� ������. ��� dnp.prf ��� �������� � ����� ������������ ������
} DNP_RECORD_T;

#endif
