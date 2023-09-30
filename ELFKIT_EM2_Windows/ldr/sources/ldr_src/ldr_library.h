#ifndef LDR_LIBRARY_H
#define LDR_LIBRARY_H

#include <loader.h>
#include <filesystem.h>
#include <utilities.h>

//Ф-ция загрузки(перезагрузки) либы
UINT32 ldrLoadDefLibrary (void);

//Ф-ция поиска записи в либе по имени
LIBRARY_RECORD_T * ldrSearchInDefLibrary (char *record_name);

#endif

//формат либы:
//заголовок
//Запись 1
//Запись 2
//...
//Запись n
//Имя 1
//Имя 2
//...
//Имя n
//
//Проще формата и не придумаешь)
