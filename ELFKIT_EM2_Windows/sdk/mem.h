#ifndef MEMORY_H
#define MEMORY_H

#include <typedefs.h>

// Выделение памяти
// Если выделить память не удалось, то в err будет код ошибки
// Если вторым параметром было NULL то при неудаче выйдет ребут
void * suAllocMem (UINT32 size, INT32 *err);
void * uisAllocateMemory (UINT32 size, INT32 *err);

// Очистка памяти
void suFreeMem (void *addr);
void uisFreeMemory (void *addr);

// Получше чем su*Mem (но медленный)
// Для больших блоков использует su*Mem, для меньших - uis*Mem
// После выделения еще и сам чистит память, поэтому медленный
// При неудаче нет ребута!
// Если использовался device_Alloc_mem для выделения памяти, то ОБЯЗАТЕЛЬНО использовать device_Free_mem_fn для очистки!!!!!!
void * device_Alloc_mem (UINT32 count, UINT32 size);
void   device_Free_mem_fn (void *addr);

// Ява менеджер памяти
void * AmMemAlloc (UINT32 size);
void   AmMemFree (void *addr);

//Проверяет какой ф-цией выделена память. 0 - uisAllocateMemory, иначе - suFreeMem 
UINT32 validateAddress (void *addr);

//12:50 18.01.2011 - Для выделения памяти использовать эти ф-ции. Доступны большие блоки памяти!!!!
void * mem_alloc (UINT32 mem_size)
{
	INT32 mem_err;
	void *mem_addr;

	if(mem_size == 0) return NULL;

	mem_addr = suAllocMem(mem_size, &mem_err);
	if(mem_err == 0) return mem_addr;

	mem_addr = uisAllocateMemory(mem_size, &mem_err);
	if(mem_err == 0) return mem_addr;

	return NULL;
}

#define mem_free(p)                          \
{                                            \
    if(p)                                    \
    {                                        \
        if(validateAddress(p)) suFreeMem(p); \
        else uisFreeMemory(p);               \
        p=NULL;                              \
    }                                        \
}

#define malloc(size) mem_alloc(size)
#define alloc(size)  mem_alloc(size)
#define mfree(addr)  mem_free(addr)
#define free(addr)   mem_free(addr)

//1030FF0C sub_1030FF0C - реальный calloc! void * calloc (UINT32 size, UINT32 count); Используется uisAllocateMemory
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


// Копирует блоки памяти
void* __rt_memcpy (void *dest, const void *src, UINT32 count);
void* memcpy (void *dest, const void *src, UINT32 count);

//забивает блок памяти размером len байтами val
void* __rt_memset (void *dest, char val, UINT32 len);
#define memset(dest, val, len) __rt_memset((void *)dest, (int)val, (UINT32)len)

//Забивает нулями блок памяти
#define memclr(dest, len) __rt_memset((void *)dest, (int)0, (UINT32)len)

//Ищет символ c в первых n байтах той области памяти, на которую указывает src. Возвращает указатель на совпадающий байт или NULL, если символ не найден в данной области памяти.  
void * memchr (const void *src, int c, UINT32 n);

//копирует блок из n байт из src в dest. Копирование  будет произведено корректно, даже,если блоки перекрываются.
void * memmove (void *dest, const void *src, UINT32 n);

//Ищет в памяти по адресу src блок данных search размером n
void * mem_strstr (void *src, void *search, UINT32 n);

//Сравнивает первые n символов массивов, адресуемых параметрами buf1 и buf2.
//Значение		Результат сравнения
//Меньше нуля		buf1 меньше buf2
//Нуль			buf1 равен buf2
//Больше нуля		buf1 больше buf2
int memcmp (const void *buf1, const void *buf2, UINT32 n);

/*
 * EXL, 24-Apr-2023:
 * Функции которые использует JVM для выделения памяти в Java Heap (800 КБ в дефолте).
 * Если телефон имеет поддержку CORElet'ов, функции работают сразу.
 * Иначе перед использованием нужно запустить Java-приложение и приостановить его.
 */

void* AmMemAllocPointer(int size);
void AmMemFreePointer(void *ptr);

#endif
