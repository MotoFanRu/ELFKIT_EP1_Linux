#ifndef WRAP_H
#define WRAP_H

#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>

typedef unsigned int		UINT32; 
typedef signed   int		INT32;
typedef unsigned short		UINT16;
typedef unsigned char		UINT8;
typedef char				WCHAR;
typedef UINT8				BOOL;


typedef FILE*				FILE_HANDLE_T;

#define FILE_HANDLE_INVALID	NULL

#define FILE_READ_MODE		"rb"
#define FILE_WRITE_MODE		"wb"

#define DL_FsOpenFile(a, b, c)		fopen(a, b); c
#define DL_FsReadFile(a,b,c,d,e)	(*e = fread(a, b, c, d)), 0
#define DL_FsCloseFile(handle)		fclose(handle)
#define DL_FsFSeekFile(handle, off, whence ) fseek(handle, off, whence)

__inline UINT32	DL_FsGetFileSize(FILE_HANDLE_T f)
{
	UINT32 sz;
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	return sz;
}


#define suFreeMem(a)				free(a)
#define suAllocMem(a,b)				malloc(a); b


/* Äëÿ whence â DL_FsSeekFile */
enum
{
    SEEK_WHENCE_SET = SEEK_SET,
    SEEK_WHENCE_CUR = SEEK_CUR,
    SEEK_WHENCE_END = SEEK_END
};

//#define conprintf(...) printf(__VA_ARGS__)
//#define dbg(...) printf(__VA_ARGS__)

__inline void cprintf(const char *format, ...)
{
    va_list   vars;

    va_start(vars, format);
    vprintf(format, vars);
    va_end(vars);
}

#define dbg cprintf

#define u_strcpy strcpy
#define u_strcat strcat
#define u_strlen strlen
#define u_atou(src, dst)			strcpy(dst, src)

__inline BOOL DL_FsFFileExist( WCHAR* uri )
{
	FILE	*f;
	f = fopen( uri, "r" );
	if( f == NULL ) return 0;
	fclose( f );
	return 1;
}


#endif