#ifndef INIPARSER_H
#define INIPARSER_H

#include <mem.h>
#include <filesystem.h>
#include <utilities.h>

BOOL INI_Open (WCHAR *uri);
VOID INI_Close (VOID);
VOID INI_SetSeparator (char separator);
BOOL INI_ReadParameter (const char *section_name, const char *param_name, const char *default_value, char *value_buffer, UINT32 buffer_size);

#endif
