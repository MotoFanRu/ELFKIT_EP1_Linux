//учитывает комментарии ;
#include "iniparser.h"

INI_DESCR_T Ini = {'=', NULL, NULL, 0};

INI_RESULT_T INI_Open (WCHAR *uri)
{
    UINT32 i, size;
    FILE_HANDLE_T hFile;

    //открываем ini файл (+)
    hFile = DL_FsOpenFile(uri, FILE_READ_MODE, 0);
    if(hFile == FILE_HANDLE_INVALID) return INI_RESULT_OPEN_FILE_FAIL;

    //получаем размер ini файла (+)
    size = DL_FsGetFileSize(hFile);

    //выделяем память для ini файла (+)
    Ini.buffer = (char *)malloc(size + 1);
    if(Ini.buffer == NULL) return INI_RESULT_MEMORY_ALLOC_FAIL;

    //вычитываем ini файл в память (+)
    DL_FsReadFile(Ini.buffer, 1, size, hFile, &size);
    if(size == 0)
    {
        free(Ini.buffer);
        return INI_RESULT_READ_FILE_FAIL;
    }

    //пишем ноль символ в конец буффера (+)
    Ini.buffer[size + 1] = 0;
    DL_FsCloseFile(hFile);


    //формируем список указателей на строки
    char *string_end, *string_begin, *string_comment = NULL;
    char *string_separator = Ini.buffer;

    //вычисляем количество строк (+)
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

    //выделяем память для указателей на начала строк (+)
    Ini.strings = (char **)malloc(Ini.strings_count * sizeof(char *));
    if(Ini.strings == NULL)
    {
        Ini.strings_count = 0;
        free(Ini.buffer);
        return INI_RESULT_MEMORY_ALLOC_FAIL;
    }

    //формируем указатель на первую строку (+)
    Ini.strings[0] = Ini.buffer;
    string_separator = Ini.buffer;

    //формируем указатели на остальные строки (+)
    for(i = 1; i < Ini.strings_count; i += 1)
    {
        //ищем разделитель (символ перехода на новую строку) (+)
        string_separator = strstr(string_separator, "\n");
        if(string_separator != NULL)
        {
            //символ перехода найден. записываем вместо него ноль символ (окончание предыдущей строки)(+)
            *string_separator = 0;

            //смещаемся на следующий символ. это будет началом текущей i-ой строки
            string_separator += 1;
            Ini.strings[i] = string_separator;
        }

        //удаляем коментарии из предыдущей строки
        string_comment = strstr(Ini.strings[i - 1], ";");
        if(string_comment != NULL) *string_comment = 0;

        //удаляем незначащие пробелы в начале и конце предыдущей строки
        string_begin = Ini.strings[i - 1];
        string_end = Ini.strings[i - 1] + strlen(Ini.strings[i - 1]) - 1;
        trimSpaces(string_begin, string_end);
    }

    //удаляем коментарии из последней строки
    string_comment = strstr(Ini.strings[Ini.strings_count - 1], ";");
    if(string_comment != NULL) *string_comment = 0;

    //удаляем незначащие пробелы в начале и конце последней строки
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
        //не указан буффер для сохранения параметра и/или его размер (+)
        return INI_RESULT_VALUE_BUFFER_NOT_FOUND;
    }

    if(Ini.strings == NULL)
    {
        //ini не открыт (+)
        strncpy(value_buffer, default_value, buffer_size);
        return INI_RESULT_FILE_BUFFER_NOT_FOUND;
    }

    //поиск секции (+)
    if(section_name == NULL)
    {
        //секция не указана. поиск будет производится по всему ini файлу (+)
        params_begin = 0;
        params_end = Ini.strings_count;
    }
    else
    {
        //формируем строку, которую будем искать (+)
        size = strlen(section_name) + 3;
        char *sec = (char *)malloc(size);
        if(sec == NULL)
        {
            //если память не выделилась, то возвращаем значение по умолчанию (+)
            strncpy(value_buffer, default_value, buffer_size);
            return INI_RESULT_MEMORY_ALLOC_FAIL;
        }
        sprintf(sec, "[%s]\0", section_name);

        //ищем секцию. ищем в каждой строке строку sec (+)
        for(i = 0; i < Ini.strings_count; i += 1)
        {
            if(strstr(Ini.strings[i], sec) != NULL)
                break;
        }
        free(sec);

        //проверяем нашли ли секцию
        if(i >= Ini.strings_count)
        {
            //если i больше количества строк, то значит секцию не нашли. поиск не продолжается (+)
            strncpy(value_buffer, default_value, buffer_size);
            return INI_RESULT_SECTION_NOT_FOUND;
        }
        else
        {
            //секцию нашли. индекс строки с которой идут параметры i+1 (+)
            params_begin = i + 1;

            //индекс строки с которой начинается следующая секция (+)
            for(i = params_begin; i < Ini.strings_count; i += 1)
            {
                //ищем в строке открывающую и закрывающую скобки
                char *open_bracket = strchr(Ini.strings[i], '[');
                char *close_bracket = strchr(Ini.strings[i], ']');
                char *param_separator = strchr(Ini.strings[i], Ini.separator);

                //первое условие: найдены обе скобки и открывающая скобка расположена раньше закрывающей
                if(open_bracket != NULL && close_bracket != NULL && open_bracket < close_bracket)
                {
                    //второе условие: не найден разделитель или разделитель находится между скобками
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

    //Понеслась. Поиск параметра (+)
    for(params_begin; params_begin < params_end; params_begin += 1)
    {
        //ищем параметр (par) и разделитель (sep) в строке (+)
        char *par = strstr(Ini.strings[params_begin], param_name);
        char *sep = strchr(par, Ini.separator);

        if(par != NULL && sep != NULL)
        {
            //нашли параметр и разделитель
            //формируем указатель на данные (+)
            char *data = sep + 1;
            size = strlen(data);

            //копируем данные в выходной буффер и убираем незначащие пробелы в начале и в конце строки
            strncpy(value_buffer, data, size + 1);
            trimSpaces(value_buffer, (char *)(value_buffer + size - 1));

            return INI_RESULT_OK;
        }
    }

    //не нашли параметр (+)
    strncpy(value_buffer, default_value, buffer_size);
    return INI_RESULT_PARAMETER_NOT_FOUND;
}
