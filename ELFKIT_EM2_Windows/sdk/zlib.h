#ifndef ZLIB_H
#define ZLIB_H

#include <typedefs.h>

#define MAX_WBITS	15	//Проверено

#define Z_SYNC_FLUSH	2	//Не проверял

#define Z_OK		0	//Не проверял
#define Z_STREAM_END	1	//Не проверял
#define Z_NEED_DICT	2	//Не проверял
#define Z_ERRNO		(-1)	//Не проверял
#define Z_STREAM_ERROR	(-2)	//Проверено
#define Z_DATA_ERROR	(-3)	//Не проверял
#define Z_MEM_ERROR	(-4)	//Не проверял
#define Z_BUF_ERROR	(-5)	//Не проверял
#define Z_VERSION_ERROR	(-6)	//Не проверял

//Проверено
struct internal_state;

//поля структуры не проверены, но размер правильный!!!
typedef struct /* z_stream_s */
{
	BYTE *	next_in;  /* next input byte */
	UINT32	avail_in;  /* number of bytes available at next_in */
	UINT32	total_in;  /* total nb of input bytes read so far */

	BYTE *	next_out; /* next output byte should be put there */
	UINT32	avail_out; /* remaining free space at next_out */
	UINT32	total_out; /* total nb of bytes output so far */

	char *	msg;	  /* last error message, NULL if no error */
	struct internal_state * state; /* not visible by applications */

	void *	zalloc_func;  /* used to allocate the internal state */
	void *	zfree_func;   /* used to free the internal state */
	void *	opaque_func;  /* private data object passed to zalloc and zfree */

	int		data_type;  /* best guess about the data type: binary or text */
	UINT32	adler;	  /* adler32 value of the uncompressed data */
	UINT32	reserved;   /* reserved for future use */
} z_stream;	//size = 56 0x38


int inflate (z_stream * strm, int flush);

int inflateEnd (z_stream * strm);

int inflateInit2_ (z_stream * strm, int windowBits, const char *version, int stream_size);
#define inflateInit2(strm,windowBits) inflateInit2_(strm,windowBits,"1.2.1",sizeof(z_stream))

#endif
