#ifndef RESOURCES__H
#define RESOURCES__H

#include <ATI.h>
#include <filesystem.h>
#include <utilities.h>
#include <mem.h>
#include "common.h"



BOOL resInit();
BOOL resTerminate();

void resInitHomeDir(WCHAR *elfPath);
void* resLoadFile(WCHAR *path);

#endif

