#ifndef CRYPT_H
#define CRYPT_H

#include <typedefs.h>

#define HASH_MD5_DATA_LENGTH 16

typedef struct 
{
    UINT8   data[HASH_MD5_DATA_LENGTH];
} HASH_MD5_T;

void MD5 (HASH_MD5_T *dst, const void *src, UINT32 src_length);

#endif
