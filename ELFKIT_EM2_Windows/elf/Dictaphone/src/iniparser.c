#include "iniparser.h"
//��������� ����������� ;

char *ini_buffer = NULL;
UINT32 ini_buffer_size = 0;
char ini_separator = '=';

BOOL INI_Open (WCHAR *uri)
{
    FILE_HANDLE_T hFile = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if(hFile == FILE_HANDLE_INVALID) return FALSE;

    ini_buffer_size = DL_FsGetFileSize(hFile);
    ini_buffer = (char*)malloc(ini_buffer_size + 2);
    if(ini_buffer == NULL) return FALSE;

    ini_buffer[0] = ' ';
    DL_FsReadFile(ini_buffer + 1, 1, ini_buffer_size, hFile, &ini_buffer_size);

    ini_buffer[ini_buffer_size + 1] = 0;
    ini_buffer_size += 2;

    DL_FsCloseFile(hFile);
    return TRUE;
}

VOID INI_Close (VOID)
{
    ini_buffer_size = 0;
    free(ini_buffer);
}

VOID INI_SetSeparator (char separator)
{
    ini_separator = separator;
}

BOOL INI_ReadParameter (const char *section_name, const char *param_name, const char *default_value, char *value_buffer, UINT32 buffer_size)
{
    UINT32 i = 0;
    char *ptr = NULL;           //��� �������� ������ ����������
    char *offset = ini_buffer;  //������ ini �����
    char *section_begin = NULL; //������ ������
    char *section_end = NULL;   //����� ������

    if(value_buffer == NULL || buffer_size == 0)
    {
        //�� ������ ������ ��� ���������� ��������� �/��� ��� ������ (+)
        return FALSE;
    }

    if(ini_buffer == NULL)
    {
        //ini �� ������ (+)
        strncpy(value_buffer, default_value, buffer_size);
        return FALSE;
    }

    //����� ������ (+)
    if(section_name == NULL)
    {
        //������ �� �������. ����� ����� ������������ �� ����� ini ����� (+)
        section_begin = ini_buffer;
        section_end = ini_buffer + ini_buffer_size;
    }
    else
    {
        //���� ������ �������, �� ���� ������ ���� ������ (+)
        while(*offset != 0)
        {
            if(*offset == '[' && !strncmp(offset + 1, section_name, strlen(section_name))) // ��� �� ����� ������
            {
                offset += strlen(section_name);
                while(*offset != 0xA && *offset != 0) offset++;

                section_begin = offset;
                break;
            }
            offset += 1;
        }

        if(section_begin == NULL)
        {
            //������ �� �������. ����� �� ������������ (+)
            strncpy(value_buffer, default_value, buffer_size);
            return FALSE;
        }
        else
        {
            //������ �������. ���� ������ ��������� ������ (-)
            //��� ���� ��� ��������. section_end ��������� �� ����� ������� (+)
            section_end = ini_buffer + ini_buffer_size;
        }
    }

    //���������. ����� ��������� (+)
    ptr = section_begin;
    while(1)
    {
        offset = strstr(ptr, param_name);
        if(offset == NULL || offset > section_end)
        {
            //���� �������� ����� ����, �� ������ ��������� ��� (+)
            //���� �������� ������ ��� �������� ����������� �� ����� ������, �� ������ ��������� � ������ ������ ��� (+)
            break;
        }

        offset += strlen(param_name) - 1;
        ptr = offset;

        //���� ����������� (+)
        while (*offset != 0x0A && *offset != 0 && *offset != ';' && offset < section_end)
        {
            if(*offset == ini_separator)
            {
                //����� �����������. ���������� ������ (+)
                offset += 1;
                value_buffer[0] = 0;
                while (*offset != 0x0A && *offset != 0 && *offset != ';' && offset < section_end)
                {
                    //�� ��� ������ ' ' ������������ (+)
                    if (*offset > ' ')
                    {
                        value_buffer[i] = *offset;
                        value_buffer[i+1] = 0;

                        i += 1;
                        if(i >= buffer_size) break;
                    }
                    offset += 1;
                }
                return TRUE;
            }
            offset += 1;
        }
    }

    //�� ����� �������� (+)
    strncpy(value_buffer, default_value, buffer_size);
    return FALSE;
}
