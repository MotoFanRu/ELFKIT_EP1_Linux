#ifndef MEMORY_H
#define MEMORY_H

#include <typedefs.h>

// ��������� ������
// ���� �������� ������ �� �������, �� � err ����� ��� ������
// ���� ������ ���������� ���� NULL �� ��� ������� ������ �����
void * suAllocMem (UINT32 size, INT32 *err);
void * uisAllocateMemory (UINT32 size, INT32 *err);

// ������� ������
void suFreeMem (void *addr);
void uisFreeMemory (void *addr);

// ������� ��� su*Mem (�� ���������)
// ��� ������� ������ ���������� su*Mem, ��� ������� - uis*Mem
// ����� ��������� ��� � ��� ������ ������, ������� ���������
// ��� ������� ��� ������!
// ���� ������������� device_Alloc_mem ��� ��������� ������, �� ����������� ������������ device_Free_mem_fn ��� �������!!!!!!
void * device_Alloc_mem (UINT32 count, UINT32 size);
void   device_Free_mem_fn (void *addr);

// ��� �������� ������
void * AmMemAlloc (UINT32 size);
void   AmMemFree (void *addr);

//��������� ����� �-���� �������� ������. 0 - uisAllocateMemory, ����� - suFreeMem 
UINT32 validateAddress (void *addr);

//12:50 18.01.2011 - ��� ��������� ������ ������������ ��� �-���. �������� ������� ����� ������!!!!
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

//1030FF0C sub_1030FF0C - �������� calloc! void * calloc (UINT32 size, UINT32 count); ������������ uisAllocateMemory
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


// �������� ����� ������
void* __rt_memcpy (void *dest, const void *src, UINT32 count);
void* memcpy (void *dest, const void *src, UINT32 count);

//�������� ���� ������ �������� len ������� val
void* __rt_memset (void *dest, char val, UINT32 len);
#define memset(dest, val, len) __rt_memset((void *)dest, (int)val, (UINT32)len)

//�������� ������ ���� ������
#define memclr(dest, len) __rt_memset((void *)dest, (int)0, (UINT32)len)

//���� ������ c � ������ n ������ ��� ������� ������, �� ������� ��������� src. ���������� ��������� �� ����������� ���� ��� NULL, ���� ������ �� ������ � ������ ������� ������.  
void * memchr (const void *src, int c, UINT32 n);

//�������� ���� �� n ���� �� src � dest. �����������  ����� ����������� ���������, ����,���� ����� �������������.
void * memmove (void *dest, const void *src, UINT32 n);

//���� � ������ �� ������ src ���� ������ search �������� n
void * mem_strstr (void *src, void *search, UINT32 n);

//���������� ������ n �������� ��������, ���������� ����������� buf1 � buf2.
//��������		��������� ���������
//������ ����		buf1 ������ buf2
//����			buf1 ����� buf2
//������ ����		buf1 ������ buf2
int memcmp (const void *buf1, const void *buf2, UINT32 n);

/*
 * EXL, 24-Apr-2023:
 * ������� ������� ���������� JVM ��� ��������� ������ � Java Heap (800 �� � �������).
 * ���� ������� ����� ��������� CORElet'��, ������� �������� �����.
 * ����� ����� �������������� ����� ��������� Java-���������� � ������������� ���.
 */

void* AmMemAllocPointer(int size);
void AmMemFreePointer(void *ptr);

#endif
