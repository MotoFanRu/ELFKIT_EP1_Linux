#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <typedefs.h>
#include <utilities.h>

#define CLIPBOARD_LEN 10244

EXTERN_LIB CLIPBOARD;
#define clipboard (WCHAR *)CLIPBOARD

#define ustr2cb(w) u_strcpy(clipboard, w)
#define cb2ustr(w) u_strcpy(w, clipboard)

#endif
