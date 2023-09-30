#include "iniparser.h"
//учитывает комментарии ;

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
    char *ptr = NULL;           //для контроля поиска параметров
    char *offset = ini_buffer;  //начало ini файла
    char *section_begin = NULL; //начало секции
    char *section_end = NULL;   //конец секции

    if(value_buffer == NULL || buffer_size == 0)
    {
        //не указан буффер для сохранения параметра и/или его размер (+)
        return FALSE;
    }

    if(ini_buffer == NULL)
    {
        //ini не открыт (+)
        strncpy(value_buffer, default_value, buffer_size);
        return FALSE;
    }

    //поиск секции (+)
    if(section_name == NULL)
    {
        //секция не указана. поиск будет производится по всему ini файлу (+)
        section_begin = ini_buffer;
        section_end = ini_buffer + ini_buffer_size;
    }
    else
    {
        //если секция указана, то ищем начало этой секции (+)
        while(*offset != 0)
        {
            if(*offset == '[' && !strncmp(offset + 1, section_name, strlen(section_name))) // это та самая секция
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
            //секция не найдена. поиск не продолжается (+)
            strncpy(value_buffer, default_value, buffer_size);
            return FALSE;
        }
        else
        {
            //секция найдена. ищем начало следующей секции (-)
            //тут пока что заглушка. section_end указывает на конец буффера (+)
            section_end = ini_buffer + ini_buffer_size;
        }
    }

    //Понеслась. Поиск параметра (+)
    ptr = section_begin;
    while(1)
    {
        offset = strstr(ptr, param_name);
        if(offset == NULL || offset > section_end)
        {
            //если смещение равно нулю, то такого параметра нет (+)
            //если смещение больше чем смещение указывающее на конец секции, то такого параметра в данной секции нет (+)
            break;
        }

        offset += strlen(param_name) - 1;
        ptr = offset;

        //ищем разделитель (+)
        while (*offset != 0x0A && *offset != 0 && *offset != ';' && offset < section_end)
        {
            if(*offset == ini_separator)
            {
                //нашли разделитель. вычитываем данные (+)
                offset += 1;
                value_buffer[0] = 0;
                while (*offset != 0x0A && *offset != 0 && *offset != ';' && offset < section_end)
                {
                    //всё что меньше ' ' пропускается (+)
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

    //не нашли параметр (+)
    strncpy(value_buffer, default_value, buffer_size);
    return FALSE;
}
