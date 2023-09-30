#ifndef UTIL_H
#define UTIL_H

#include <typedefs.h>
#include <stdarg.h>
#include <time_date.h>



/***************************************
   Unicode strings routines
****************************************/

// strcpy копирует строку, указанную src (включа€ завершающий знак NULL) в массив, указанный dst.
WCHAR* u_strcpy( WCHAR* dst,  WCHAR* src ); 
// копирует n символов из src в dst	
// добавл€ет строку src к строке dst
WCHAR* u_strcat( WCHAR* dst,  WCHAR* src );
// добавл€ет n символов из src к строке dst
// длина строки
UINT32 u_strlen( WCHAR* str );

// преобразует строку в нижний регистр
//WCHAR* u_strmakelower( WCHAR* str ); not yet
// в верхний регистр
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
}  //  опирует строку (n символов дл€ strncpy), на которую указывает <src> (включа€ завершающий символ окончани€), в массив, на который указывает <dest>.
char* strcat( char* dst,  char* src );  // ƒобавл€ет строку <src> (n символов дл€ strncat) к строке <dst>, 
INT32 strcmp( char* str1,  char* str2 );							// перезаписыва€ символ окончани€ в конце <dst> и добавл€€ к строке символ окончани€.
char* strcat( char* dst,  char* src );
INT32 strncmp( char* str1,  char* str2,  UINT32 n ); 

// длина строки
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
