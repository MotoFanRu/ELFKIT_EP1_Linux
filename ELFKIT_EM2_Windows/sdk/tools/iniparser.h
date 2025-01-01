#ifndef INIPARSER_H
#define INIPARSER_H

#include <mem.h>
#include <filesystem.h>
#include <utilities.h>

//стуктура описывающая ini файл
typedef struct
{
    char separator;        //разделитель для параметров
    char *buffer;          //буффер для файла
    char **strings;        //указатели на строки
    UINT32 strings_count;  //количество строк
} INI_DESCR_T;

//коды результатов выполнения
enum
{
    INI_RESULT_OK = 0,                  //выполнилось успешно

    //выделение памяти
    INI_RESULT_MEMORY_ALLOC_FAIL,       //ошибка выделения памяти

    //INI_Open
    INI_RESULT_OPEN_FILE_FAIL,          //ошибка открытия ini файла
    INI_RESULT_READ_FILE_FAIL,          //ошибка чтения ini файла

    //INI_ReadParameter
    INI_RESULT_VALUE_BUFFER_NOT_FOUND,  //не найден буффер для сохранения значения параметра
    INI_RESULT_FILE_BUFFER_NOT_FOUND,   //не найден буффер ini файла
    INI_RESULT_SECTION_NOT_FOUND,       //не найдена секция в ini файле
    INI_RESULT_PARAMETER_NOT_FOUND      //не найден параметр в ini файле
};
typedef UINT8 INI_RESULT_T;

INI_RESULT_T INI_Open (WCHAR *uri);
INI_RESULT_T INI_Close (VOID);
INI_RESULT_T INI_SetSeparator (char separator);
INI_RESULT_T INI_ReadParameter (const char *section_name, const char *param_name, const char *default_value, char *value_buffer, UINT32 buffer_size);

#endif
