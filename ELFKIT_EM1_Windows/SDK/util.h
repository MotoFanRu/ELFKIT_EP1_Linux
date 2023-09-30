#ifndef UTIL_H
#define UTIL_H

#include <typedefs.h>
#include <stdarg.h>
#include <time_date.h>



/***************************************
   Unicode strings routines
****************************************/

// strcpy �������� ������, ��������� src (������� ����������� ���� NULL) � ������, ��������� dst.
WCHAR* u_strcpy( WCHAR* dst,  WCHAR* src ); 
// �������� n �������� �� src � dst	
// ��������� ������ src � ������ dst
WCHAR* u_strcat( WCHAR* dst,  WCHAR* src );
// ��������� n �������� �� src � ������ dst
// ����� ������
UINT32 u_strlen( WCHAR* str );

// ����������� ������ � ������ �������
//WCHAR* u_strmakelower( WCHAR* str ); not yet
// � ������� �������
WCHAR* u_strmakeupper( WCHAR* str );

INT32 u_strcmp( WCHAR* str1,  WCHAR* str2 );

/***************************************
   Convert
****************************************/

// unicode to ascii (valid only for ascii-range symbols) 
char* u_utoa( WCHAR* ustr,  char* astr )
{
    int i, h = 1;
    for( i = 0; i < u_strlen( ustr ); i++ )
    {
        astr[ i ] = ( ( char * )ustr )[ h ];
        h += 2;
    }
    astr[ i ] = 0;
}
// ascii  to unicode 
WCHAR* u_atou( char* astr,  WCHAR* ustr );

/***************************************
   ANSI strings routines
****************************************/

char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while (*s1++ = *s2++);
    return s1_p;
}  // �������� ������ (n �������� ��� strncpy), �� ������� ��������� <src> (������� ����������� ������ ���������), � ������, �� ������� ��������� <dest>.
char* strcat( char* dst,  char* src );  // ��������� ������ <src> (n �������� ��� strncat) � ������ <dst>, 
INT32 strcmp( char* str1,  char* str2 );							// ������������� ������ ��������� � ����� <dst> � �������� � ������ ������ ���������.
char* strcat( char* dst,  char* src );
INT32 strncmp( char* str1,  char* str2,  UINT32 n ); 

// ����� ������
UINT32 strlen( char *str)
{
    int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

#define randomize()     srand((UINT32)suPalReadTime())

/***************************************
  MATH
****************************************/

#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))

#define abs(x) 			((x) < 0 ? -(x) : (x))

int sprintf(char *str, const char *format, ...); 

#endif
