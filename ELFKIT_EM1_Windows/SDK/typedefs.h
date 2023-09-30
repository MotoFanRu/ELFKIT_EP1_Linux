
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef signed char				INT8;
typedef unsigned char			UINT8;
typedef signed short			INT16;
typedef unsigned short			UINT16;
typedef signed long				INT32;
typedef unsigned long			UINT32; 
typedef signed long long		INT64;
typedef unsigned long long		UINT64;

typedef UINT8					BOOL;
typedef UINT16					WCHAR;

#define TRUE					(BOOL)(1)
#define FALSE					(BOOL)(0)

#define true					(BOOL)(1)
#define false					(BOOL)(0)

#define RESULT_OK				(0)
#define RESULT_FAIL				(1)

#define NULL					(0)

typedef UINT32					EVENT_CODE_T;
typedef UINT32					UIS_DIALOG_T;
typedef UINT32					SU_PORT_T;


typedef struct
{
    SU_PORT_T 	port; 
    UINT32 		unk;
} IFACE_DATA_T;

#endif
