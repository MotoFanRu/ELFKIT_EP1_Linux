#ifndef MEMORY_H
#define MEMORY_H

#include <typedefs.h>


// выделение и освобождение пам€ти
void *suAllocMem( UINT32 size, INT32 *result ); //result can be NULL
void suFreeMem( void *ptr );
#define malloc(sz)		suAllocMem(sz, NULL)

//#define free(mb)		suFreeMem(mb);

#define free(p) { \
if(p) { \
suFreeMem(p); \
p=NULL; \
} \
}

void __rt_memset(char *dest, char val, UINT32 len)
{
    char *temp = (char *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
 // забивает блок зазмером n байтами c
void __rt_memclr( void *, UINT32 size );						// «абивает нул€ми блок пам€ти
#define memclr(m,s)		__rt_memclr((void *)m,(UINT32)s)
void * __rt_memcpy(void *dest, const void *src, int count)
{
    char *sp = (char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return ( void * )dest;
}		//  опирует блоки пам€ти
void * memcpy(void *dst, const void *src, UINT32 sz)
{
return __rt_memcpy((void *)dst,(const void *)src,(UINT32)sz);
}
//void * __rt_memset( void *, int, UINT32 size );				// «абивает указанным байтом блок пам€ти
#define memset(m,v,sz)		__rt_memset((void *)m,(int)v,(UINT32)sz)






#endif 
