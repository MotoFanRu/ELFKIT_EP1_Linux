#ifndef INIPARSER_H
#define INIPARSER_H

#include <mem.h>
#include <filesystem.h>
#include <utilities.h>

//�������� ����������� ini ����
typedef struct
{
    char separator;        //����������� ��� ����������
    char *buffer;          //������ ��� �����
    char **strings;        //��������� �� ������
    UINT32 strings_count;  //���������� �����
} INI_DESCR_T;

//���� ����������� ����������
enum
{
    INI_RESULT_OK = 0,                  //����������� �������

    //��������� ������
    INI_RESULT_MEMORY_ALLOC_FAIL,       //������ ��������� ������

    //INI_Open
    INI_RESULT_OPEN_FILE_FAIL,          //������ �������� ini �����
    INI_RESULT_READ_FILE_FAIL,          //������ ������ ini �����

    //INI_ReadParameter
    INI_RESULT_VALUE_BUFFER_NOT_FOUND,  //�� ������ ������ ��� ���������� �������� ���������
    INI_RESULT_FILE_BUFFER_NOT_FOUND,   //�� ������ ������ ini �����
    INI_RESULT_SECTION_NOT_FOUND,       //�� ������� ������ � ini �����
    INI_RESULT_PARAMETER_NOT_FOUND      //�� ������ �������� � ini �����
};
typedef UINT8 INI_RESULT_T;

INI_RESULT_T INI_Open (WCHAR *uri);
INI_RESULT_T INI_Close (VOID);
INI_RESULT_T INI_SetSeparator (char separator);
INI_RESULT_T INI_ReadParameter (const char *section_name, const char *param_name, const char *default_value, char *value_buffer, UINT32 buffer_size);

#endif
