//те ф-ции которые закомменчены необходимо найти в прошивке

#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <typedefs.h>
#include <time_date.h>

/*****************************************************
 * Seconds in one year macors
 *****************************************************/

#define LEAP_YEAR_SECONDS (366*24*60*60)
#define NO_LEAP_YEAR_SECONDS (365*24*60*60)
#define SECONDS_IN_YEAR(year) ((year % 4) ? NO_LEAP_YEAR_SECONDS : LEAP_YEAR_SECONDS)


/*****************************************************
 * Bit clear, bit get and bit set macros
 *****************************************************/

#define BIT_CLR(var, bit) var = var & (~(1 << bit));
#define BIT_GET(var, bit) ((var & (1 << bit)) >> bit)
#define BIT_SET(var, bit) var = var | (1 << bit);

/*****************************************************
 * Minimum and maximum macros
 *****************************************************/

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define abs(i)	  ((i) < 0 ? -(i) : (i))

/*****************************************************
 * Математические ф-ции
 ****************************************************/

// Возвращает максимальное целое число, не большее аргумента value
// int floor (double value);

/*****************************************************
 * ANSI strings routines
 * 
 * ВНИМАНИЕ!
 * по стандарту лимитированные ф-ции с записью в dst является небезопасными,
 * ибо если длина строки src больше чем n, то в dst не будет записан символ окончания строки '\0'
 *****************************************************/

// Длина строки
INT32  strlen (const char *str);

// Конкатенация строк.
char * strcat (char *dst, const char *src);

// Лимитированая конкатенация
char * strncat (char *dst, const char *src, INT32 n);

// Сравнение строк с учётом регистра
int    strcmp (const char *s1, const char *s2);

// Сравнение строк без учёта регистра
int    stricmp (const char *s1, const char * s2);

// Лимитированное сравнение с учётом регистра
int    strncmp (const char *s1, const char *s2, INT32 n);

// Лимитированное сравнение без учёта регистра
int    strnicmp (const char *s1, const char *s2, INT32 n);

// Копирование строк
char * strcpy (char *dst, const char *src);

// Лимитированное копирование
char * strncpy (char *dst, const char *src, INT32 n);

// ищет в str первое вхождение символа chr
char * strchr (const char *str, int chr);

// ищет в str последнее вхождение символа chr
char * strrchr (const char *str, int chr);

// Поиск подстроки с учётом регистра
char * strstr (const char *str, const char *srch);

// Поиск подстроки без учёта регистра
char * stristr (const char *str, const char *srch);

// возвращает индекс символа в str1 который совпадает с одним из символов str2
INT32  strcspn (const char *str1, const char *str2);

// аналогично, только `НЕ`
INT32  strspn (const char *str1, const char *str2);

char * strtok (char *, const char *);

// Символ к верхнему регистру
int toupper (int c);

// Символ к нижнему регистру
int tolower (int c);

//Убирает с начала и с конца строки символы \n \t \r и ' '
//str - указатель на строку. last_symbol_of_str - указатель на последний символ строки
char * trimSpaces (char *str, char *last_symbol_of_str);


/*****************************************************
 * Unicode strings routines
 *****************************************************/

// Длина строки
INT32	u_strlen (const WCHAR *str);

// Размер строки ( (u_strlen(str) + 1) * sizeof(WCHAR) )
INT32   u_strsize (const WCHAR *str);

// Конкатенация строк. Добавляет строку str к строке dst
WCHAR *	u_strcat (WCHAR *dst, const WCHAR *str);

// Конкатенация строк. добавляет n символов из строки src к строке dst
WCHAR * u_strncat (WCHAR *dst, const WCHAR *src, UINT32 n);

// Сравнение строк
INT32   u_strcmp (const WCHAR *s1, const WCHAR * s2);

// Сравнение n символов из двух строк
INT32   u_strncmp (const WCHAR *s1, const WCHAR *s2, INT32 count);

// Копирование строк. Копирует строку src в строку dst
WCHAR *	u_strcpy (WCHAR *dst, const WCHAR *src);

// Копирование строк. Копирует n символов строки src в строку dst
WCHAR *	u_strncpy (WCHAR *dst, const WCHAR *src, INT32 count);

// ищет в str первое вхождение символа chr
WCHAR * u_strchr (const WCHAR *str, WCHAR chr);

// ищет в str последнее вхождение символа chr
WCHAR * u_strrchr (const WCHAR *str, WCHAR chr);

// ищет search в src с учётом регистра
WCHAR * u_strstr (const WCHAR *src, const WCHAR *search);

// ищет search в src без учёта регистра
WCHAR * u_stristr (const WCHAR *src, const WCHAR *search);

// Unicode Строку к верхнему регистру
WCHAR *	u_strmakeupper (WCHAR *str);

// Unicode Строку к верхнему регистру
WCHAR *	u_strmakelower (WCHAR *str);




/*****************************************************
 * Преобразование строк
 *****************************************************/

// ANSI string to Unicode
WCHAR *	u_atou (const char *src, WCHAR *dst);

//Unicode string to ascii (valid only for ascii-range symbols)
WCHAR * u_utoa (const WCHAR *src, char *dst);

//Unicode string to ascii (valid only for ascii-range symbols)
WCHAR * u_str_to_a (WCHAR *src, char *dst, UINT16 lenght);
#define u_nutoa(s, d, n) u_str_to_a(s, d, n);

// Unicode to UTF-8
UINT16 DL_Char_convUCS2toUTF8String (const WCHAR *src_ptr, UINT16 src_len, UINT8 *trg_ptr, UINT16 trg_len);

// UTF-8 to Unicode
UINT16 DL_Char_convUTF8toUCS2String (const UINT8 *src_ptr, UINT16 src_len, WCHAR *trg_ptr, UINT16 trg_len);

// CP-1251 to Unicode
UINT16 DL_Char_convCP1251toUCS2String (const UINT8 *src_ptr, UINT16 src_len, WCHAR *trg_ptr, UINT16 trg_len);

// KOI8-R to Unicode
// UINT16 DL_Char_convKOI8RtoUCS2String (const UINT8 *src_ptr, UINT16 src_len, WCHAR *trg_ptr, UINT16 trg_len);

// Unicode to Cyrilic
UINT16 DL_Char_convUCS2toCyrillicString(const WCHAR *src_ptr, UINT16 src_len, UINT8 *trg_ptr, UINT16 trg_len);

// Unicode to ASCII
UINT16 DL_Char_convUCS2toASCIIString(const WCHAR *src_ptr, UINT16 src_len, UINT8 *trg_ptr, UINT16 trg_len);


/*******************************************************
 *  Преобразование СТРОКА <-> ЧИСЛО
 *  str - указатель на троку
 *  end - ввозвращается указатель на строку в месте, где закончилось преобразование
 *  base, radix - основание СС.
 *  Если 0, то будет использоваться "умное" определение - "0x..." = HEX / "0..." = OCT / остальное DEC
 *******************************************************/

// преобразует строку в число long (INT32)
long strtol (const char *str, char **end, int base);

// строка -> unsigned long (UINT32)
unsigned long strtoul (const char *str, char **end, int base);

// число -> строку
char * _itoa (int value, char *string, int radix);
#define itoa(v,s,r) _itoa(v,s,r)

// число -> строка Unicode
WCHAR * u_ltou (long number, WCHAR *str);

// число -> строка Unicode
WCHAR * u_ultou (unsigned long number, WCHAR *str);

// Unicode строка -> unsigned long (UINT32)
long u_atol (const WCHAR *str);

// преобразует из dec в HEX. Результат в unicode строке
WCHAR * u_itoh (int value, WCHAR *dst);


/*****************************************************
 * other functions
 *****************************************************/

//void * bsearch( const void * key, const void * buf, INT32 num, INT32 size, int (*compare)(const void *, const void *) );
//void qsort( void * buf, INT32 num, INT32 size, int (*compare)(const void *, const void *) );

int rand (void);
void srand (unsigned int seed);

#define randomize()		srand((UINT32)suPalReadTime())
#define rand_init()		srand((UINT32)suPalReadTime())
#define random(num)		(rand() % (num))

int sprintf( char *str, const char *format, ... );
//int snprintf( char * str, INT32 maxlen, const char * format, ... );
//int vsprintf( char * buffer, const char * format, va_list arglist );
//int vsnprintf( char * buffer, INT32 maxlen, const char * format, va_list arglist );


void PFprintf( const char *format, ... ); // %d %x %s, a-la printf

int Nsscanf( const char *buffer, const char *format, ... /*arguments*/ );
#define sscanf(buffer, format, ...) Nsscanf(buffer,format, ## __VA_ARGS__)

//void suLogData(UINT32 * phandle, UINT32 msgid, UINT32 num_pairs, ...);

#endif
