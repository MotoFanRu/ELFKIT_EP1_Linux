//��������� ����������� ;
#include "iniparser.h"

INI_DESCR_T Ini = {'=', NULL, NULL, 0};

INI_RESULT_T INI_Open (WCHAR *uri)
{
    UINT32 i, size;
    FILE_HANDLE_T hFile;

    //��������� ini ���� (+)
    hFile = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if(hFile == FILE_HANDLE_INVALID) return INI_RESULT_OPEN_FILE_FAIL;

    //�������� ������ ini ����� (+)
    size = DL_FsGetFileSize(hFile);

    //�������� ������ ��� ini ����� (+)
    Ini.buffer = (char *)malloc(size + 1);
    if(Ini.buffer == NULL) return INI_RESULT_MEMORY_ALLOC_FAIL;

    //���������� ini ���� � ������ (+)
    DL_FsReadFile(Ini.buffer, 1, size, hFile, &size);
    if(size == 0)
    {
        free(Ini.buffer);
        return INI_RESULT_READ_FILE_FAIL;
    }

    //����� ���� ������ � ����� ������� (+)
    Ini.buffer[size + 1] = 0;
    DL_FsCloseFile(hFile);


    //��������� ������ ���������� �� ������
    char *string_end, *string_begin, *string_comment = NULL;
    char *string_separator = Ini.buffer;

    //��������� ���������� ����� (+)
    Ini.strings_count = 1;
    while(string_separator)
    {
        string_separator = strstr(string_separator, "\n");
        if(string_separator != NULL)
        {
            Ini.strings_count += 1;
            string_separator += 1;
        }
    }

    //�������� ������ ��� ���������� �� ������ ����� (+)
    Ini.strings = (char **)malloc(Ini.strings_count * sizeof(char *));
    if(Ini.strings == NULL)
    {
        Ini.strings_count = 0;
        free(Ini.buffer);
        return INI_RESULT_MEMORY_ALLOC_FAIL;
    }

    //��������� ��������� �� ������ ������ (+)
    Ini.strings[0] = Ini.buffer;
    string_separator = Ini.buffer;

    //��������� ��������� �� ��������� ������ (+)
    for(i = 1; i < Ini.strings_count; i += 1)
    {
        //���� ����������� (������ �������� �� ����� ������) (+)
        string_separator = strstr(string_separator, "\n");
        if(string_separator != NULL)
        {
            //������ �������� ������. ���������� ������ ���� ���� ������ (��������� ���������� ������)(+)
            *string_separator = 0;

            //��������� �� ��������� ������. ��� ����� ������� ������� i-�� ������
            string_separator += 1;
            Ini.strings[i] = string_separator;
        }

        //������� ���������� �� ���������� ������
        string_comment = strstr(Ini.strings[i - 1], ";");
        if(string_comment != NULL) *string_comment = 0;

        //������� ���������� ������� � ������ � ����� ���������� ������
        string_begin = Ini.strings[i - 1];
        string_end = Ini.strings[i - 1] + strlen(Ini.strings[i - 1]) - 1;
        trimSpaces(string_begin, string_end);
    }

    //������� ���������� �� ��������� ������
    string_comment = strstr(Ini.strings[Ini.strings_count - 1], ";");
    if(string_comment != NULL) *string_comment = 0;

    //������� ���������� ������� � ������ � ����� ��������� ������
    string_begin = Ini.strings[Ini.strings_count - 1];
    string_end = Ini.strings[Ini.strings_count - 1] + strlen(Ini.strings[Ini.strings_count - 1]) - 1;
    trimSpaces(string_begin, string_end);

    return INI_RESULT_OK;
}

INI_RESULT_T INI_Close (VOID)
{
    Ini.strings_count = 0;
    free(Ini.strings);
    free(Ini.buffer);
    return INI_RESULT_OK;
}

INI_RESULT_T INI_SetSeparator (char separator)
{
    Ini.separator = separator;
    return INI_RESULT_OK;
}

INI_RESULT_T INI_ReadParameter (const char *section_name, const char *param_name, const char *default_value, char *value_buffer, UINT32 buffer_size)
{
    UINT32 size, i = 0;
    UINT32 params_begin = 0;
    UINT32 params_end = 0;

    if(value_buffer == NULL || buffer_size == 0)
    {
        //�� ������ ������ ��� ���������� ��������� �/��� ��� ������ (+)
        return INI_RESULT_VALUE_BUFFER_NOT_FOUND;
    }

    if(Ini.strings == NULL)
    {
        //ini �� ������ (+)
        strncpy(value_buffer, default_value, buffer_size);
        return INI_RESULT_FILE_BUFFER_NOT_FOUND;
    }

    //����� ������ (+)
    if(section_name == NULL)
    {
        //������ �� �������. ����� ����� ������������ �� ����� ini ����� (+)
        params_begin = 0;
        params_end = Ini.strings_count;
    }
    else
    {
        //��������� ������, ������� ����� ������ (+)
        size = strlen(section_name) + 3;
        char *sec = (char *)malloc(size);
        if(sec == NULL)
        {
            //���� ������ �� ����������, �� ���������� �������� �� ��������� (+)
            strncpy(value_buffer, default_value, buffer_size);
            return INI_RESULT_MEMORY_ALLOC_FAIL;
        }
        sprintf(sec, "[%s]\0", section_name);

        //���� ������. ���� � ������ ������ ������ sec (+)
        for(i = 0; i < Ini.strings_count; i += 1)
        {
            if(strstr(Ini.strings[i], sec) != NULL)
                break;
        }
        free(sec);

        //��������� ����� �� ������
        if(i >= Ini.strings_count)
        {
            //���� i ������ ���������� �����, �� ������ ������ �� �����. ����� �� ������������ (+)
            strncpy(value_buffer, default_value, buffer_size);
            return INI_RESULT_SECTION_NOT_FOUND;
        }
        else
        {
            //������ �����. ������ ������ � ������� ���� ��������� i+1 (+)
            params_begin = i + 1;

            //������ ������ � ������� ���������� ��������� ������ (+)
            for(i = params_begin; i < Ini.strings_count; i += 1)
            {
                //���� � ������ ����������� � ����������� ������
                char *open_bracket = strchr(Ini.strings[i], '[');
                char *close_bracket = strchr(Ini.strings[i], ']');
                char *param_separator = strchr(Ini.strings[i], Ini.separator);

                //������ �������: ������� ��� ������ � ����������� ������ ����������� ������ �����������
                if(open_bracket != NULL && close_bracket != NULL && open_bracket < close_bracket)
                {
                    //������ �������: �� ������ ����������� ��� ����������� ��������� ����� ��������
                    if(param_separator == NULL || (param_separator > open_bracket && param_separator < close_bracket))
                    {
                        break;
                    }
                }
            }

            if(i >= Ini.strings_count) params_end = Ini.strings_count;
            else params_end = i;
        }
    }

    //���������. ����� ��������� (+)
    for(params_begin; params_begin < params_end; params_begin += 1)
    {
        //���� �������� (par) � ����������� (sep) � ������ (+)
        char *par = strstr(Ini.strings[params_begin], param_name);
        char *sep = strchr(par, Ini.separator);

        if(par != NULL && sep != NULL)
        {
            //����� �������� � �����������
            //��������� ��������� �� ������ (+)
            char *data = sep + 1;
            size = strlen(data);

            //�������� ������ � �������� ������ � ������� ���������� ������� � ������ � � ����� ������
            strncpy(value_buffer, data, size + 1);
            trimSpaces(value_buffer, (char *)(value_buffer + size - 1));

            return INI_RESULT_OK;
        }
    }

    //�� ����� �������� (+)
    strncpy(value_buffer, default_value, buffer_size);
    return INI_RESULT_PARAMETER_NOT_FOUND;
}
