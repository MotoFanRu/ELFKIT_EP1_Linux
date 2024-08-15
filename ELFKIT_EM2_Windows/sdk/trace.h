#ifndef TRACE_H
#define TRACE_H

#include <typedefs.h>
#include <mem.h>

#include <utilities.h>
#include <time_date.h>

#define TRACE_ERR_STR	"err: "
#define TRACE_WARN_STR	"warn: "
#define TRACE_DBG_STR	"dbg: "
#define TRACE_U_STR	"udbg: "

// Имя приложения будет браться из глобальной переменной
#if defined(EM1) || defined(EM2)
#define app_name g_app_name
#endif

extern const char app_name[];

//#define DEBUG в начале файла для использования отладочных сообщений
#ifdef DEBUG

#define TRACE(format, ...)	PFprintf(format, ## __VA_ARGS__)
#define TRACE_STRING(str)	PFprintf(str)

#define err(format, ...) \
	TRACE("%s: " TRACE_ERR_STR  "\"%s\"@%d: " format "\n", app_name, __FILE__, __LINE__ , ## __VA_ARGS__)
#define warn(format, ...) \
	TRACE("%s: " TRACE_WARN_STR "\"%s\"@%d: " format "\n", app_name, __FILE__, __LINE__ , ## __VA_ARGS__)
#define dbg(format) \
	TRACE("%s: " TRACE_DBG_STR  "@%d: " format "\n", app_name,  __LINE__)
#define dbgf(format, ...) \
	TRACE("%s: " TRACE_DBG_STR  "@%d: " format "\n", app_name,  __LINE__ , ## __VA_ARGS__)
#define print(format, ...) \
	TRACE("%s: " format "\n", app_name, ## __VA_ARGS__)

UINT32 udbg(char *format, WCHAR *ustr)
{
	char *astr = malloc(u_strlen(ustr) + 1);
	if(!astr) return RESULT_OK;

	u_utoa(ustr, astr);

	TRACE("%s: " TRACE_DBG_STR  "@%d: " "%s%s\n", app_name, __LINE__, format, astr);
	free(astr);

	return RESULT_OK;
}

#else // DEBUG

#define err(format, ...)
#define warn(format, ...)
#define dbg(format)
#define dbgf(format, ...)
#define print(format, ...)

#define TRACE(format, ...)
#define TRACE_STRING(str)

#endif // DEBUG

#endif
