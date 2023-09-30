/* ********************************************************************
 *  Debugger/��������
 * ********************************************************************
 *  (c) G-XaD 2008
 *  version: 1.01.00
 *
 *  �������������:
 * 1) ������������ 'dbg.o'
 * 2) ���������� ���� ���� (� ��������� 'dbg.h') ����� ������ #include
 * 3) ������������ � ���� ��������� �������
 * 4) ����� �������� ����� ������� ���������� ��������� ������� ������ "#define DEBUG" ����� ������ include
 * 5) ����� �������� ����� ���������� ��������� ��������� � ��������� ������ ������� ������ "#definde DEBUG_MEMORY"
 *      ����� ������ include
 *    ���� ��� ���� ���������� ����� ���������, �� �������� ���� 'dbg.o' ����� �� ����� ������ �� ������ ��������� ����������
 *
 *
 *  �������
 * 1) void warn( const char * format, ... );
 *      ��������������� ����� ��������� �� �������� 'warn'(��������!) � midway
 * 2) void dbg( const char * format, ... );
 *      ��������������� ����� ��������� �� �������� 'dbg'(�������) � midway
 * 3) void err( const char * format, ... );
 *      ��������������� ����� ��������� �� �������� 'err'(������) � midway. ����������� ��� ����������� ������� �� ���������
 * 4) void * malloc(size_t size);
 *      ������ � ��������� ������ �� ��������� size ���� ����������� ������
 * 5) void free( void * mem );
 *      ��������� ��������� ������ � ���, ��� ���������� ����� ������ ��� �� �����
 * 6) void ddbg( const void * data, size_t size );
 *      ����� � midway ����� ������ �������� size, ������� ���������� �� ������� data
 * 7) void udbg( const char * format, const WCHAR * ustr );
 *      ��������������� ����� ��������� � midway � ����� ������ �������
 * 8) DEBUG_TIME ...
 * 
 * ********************************************************************/


#ifndef APP_DEBUGGER
#define APP_DEBUGGER

#include <typedefs.h>
#include <mem.h>
#include <utilities.h>
#include <time_date.h>

// ��� ���������� ����� ������� �� ���������� ����������
extern const char		app_name[];

/* ********************************************************************
 *  ��������� ���������, ������� � ���� ���������
 */

#define TRACE_ERR_STR			"err: "
#define TRACE_WARN_STR			"warn: "
#define TRACE_DBG_STR			"dbg: "
#define TRACE_U_STR				"udbg: "

#define TRACE(format, ...)		PFprintf(format, ## __VA_ARGS__)
#define TRACE_STRING(str)	    PFprintf(str)


/* ********************************************************************
 *  ���������� ��������� ��������� � ��������� ������
 */

#ifdef DEBUG_MEMORY

#define malloc(sz) \
	dbg_malloc(app_name, sz,__FILE__,__LINE__)
#define free(mb) \
	dbg_free(app_name, mb,__FILE__,__LINE__)
/*
#else // DEBUG_MEMORY

#define malloc(sz) \
	suAllocMem(sz, NULL)
#define free(mb) \
	suFreeMem(mb)
*/
#endif

/* ********************************************************************
 *  ���������� �������� ���������
 */

#ifdef DEBUG

#define err(format, ...) \
	TRACE("%s: " TRACE_ERR_STR  "\"%s\"@%d: " format "\n", app_name, __FILE__, __LINE__ , ## __VA_ARGS__)
#define warn(format, ...) \
	TRACE("%s: " TRACE_WARN_STR "\"%s\"@%d: " format "\n", app_name, __FILE__, __LINE__ , ## __VA_ARGS__)
#define dbg(format, ...) \
	TRACE("%s: " TRACE_DBG_STR  "@%d: " format "\n", app_name,  __LINE__ , ## __VA_ARGS__)
#define ddbg(d, sz) \
	dbg_data(app_name, d, sz, __FILE__, __LINE__)
#define udbg(format, ustr) \
	dbg_unicode(app_name, "%s: " TRACE_U_STR "@%d: " format "\n", ustr, __FILE__, __LINE__)

#else // DEBUG

#define err(format, ...)
#define warn(format, ...)
#define dbg(format, ...)
#define ddbg(d, sz)
#define udbg(format, ustr)

#endif

/* ********************************************************************
 *  �������� ������� ����������
 */

#ifdef DEBUG_TIME

#define dtime_init() \
	unsigned int  dbg_timer_var;\
	dbg_timer_init(app_name, __FILE__, __LINE__, &dbg_timer_var)
#define dtime() \
	dbg_time(app_name, __FILE__, __LINE__, dbg_timer_var)

#else

#define dtime_init()
#define dtime()

#endif

/* ********************************************************************
 *  ������ �������������. �� ������ �� ������������!
 */



#ifdef __cplusplus
extern "C" {
#endif

void * dbg_malloc( const char *, size_t, const char *, int );
void dbg_free( const char *, void *, const char *, int );
void dbg_data( const char *, const void *, UINT16, const char *, int );
void dbg_unicode( const char *, const char *, const WCHAR *, const char *, int );
extern void dbg_timer_init( const char *, const char *, int, unsigned int * );
extern void dbg_time( const char *, const char *, int, unsigned int );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // APP_DEBUGGER
