#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define TRUE			(BOOL)(1)
#define FALSE			(BOOL)(0)

#define true			(BOOL)(1)
#define false			(BOOL)(0)

#define RESULT_OK		(0)
#define RESULT_FAIL		(1)

#define NULL			(0)

typedef signed char		INT8;
typedef unsigned char		UINT8;
typedef signed short		INT16;
typedef unsigned short		UINT16;
typedef signed long		INT32;
typedef unsigned long		UINT32;
typedef signed long long	INT64;
typedef unsigned long long	UINT64;

typedef void			VOID;

typedef UINT8			BOOL;
typedef UINT8			BYTE;
typedef UINT16			WCHAR;

typedef UINT16			WORD;
typedef UINT16 *		PWORD;

typedef UINT32			DWORD;
typedef UINT32 *		PDWORD;

//**********************************************
//Согласно процессорному модулю для mcore
//typedef UINT8			TYPE_B
//typedef UINT16		TYPE_H
//typedef UINT32		TYPE_W
//**********************************************

typedef UINT32			EVENT_CODE_T;
typedef UINT32			UIS_DIALOG_T;
typedef UINT32			SU_PORT_T;
typedef	UINT64			DL_FS_MID_T;	// физический идентификатор файла
typedef UINT16			FILE_HANDLE_T;	// идентификатор файла

typedef UINT16			REG_ID_T;
typedef UINT16			APP_ID_T;
typedef UINT32			AFW_ID_T;
typedef UINT32			LOG_ID_T;


enum FREE_BUF_FLAG_ENUM
{
	FBF_FREE = 100,  // ОС должна освободить буффер
	FBF_LEAVE        // Приложение должно следить за освобождением буффера
} ;
typedef UINT8 FREE_BUF_FLAG_T;

typedef struct
{
	SU_PORT_T port;
	UINT32    handle;
} IFACE_DATA_T;

typedef struct
{
	UINT32 R0;
	UINT32 R1;
} _u64;

typedef struct APPLICATION_T APPLICATION_T;
typedef struct EVENT_STACK_T EVENT_STACK_T;
typedef struct EVENT_T EVENT_T;

//EXTERN_LIB - используюется для того чтобы хапать значения из либы O_o
typedef UINT32 EXTERN_LIB(void);

#endif
