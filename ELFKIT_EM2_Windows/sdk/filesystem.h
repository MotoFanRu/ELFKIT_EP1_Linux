#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <typedefs.h>

// ����������� ��������� ������� �������� ����!

#define FILE_HANDLE_INVALID 	0xFFFF

#define FS_MAX_URI_NAME_LENGTH		264		// file://b/a.txt // please, use FS_MAX_URI_NAME_LENGTH+1
#define FS_MAX_PATH_NAME_LENGTH		258		// /b/a.txt //����������
#define FS_MAX_FILE_NAME_LENGTH		255		// a.txt // ����������

// ��� mode � DL_FsOpenFile. �� ������������� - ��. �������� ����������� ������� C stdio.h
enum
{
    FILE_READ_MODE = 0,         // ������� �� ������
    FILE_WRITE_MODE,            // ��� ������
    FILE_WRITE_EXIST_MODE,      // ������, ���� ���� �� ����������
    FILE_APPEND_MODE,           // ������ � ����� �����
    FILE_READ_PLUS_MODE,        // ������� ��� ������ � ������
    FILE_WRITE_PLUS_MODE,       // ������� ��� ������ � ������
    FILE_WRITE_EXIST_PLUS_MODE, // ������� ��� ������ � ������, ���� �� ����������
    FILE_APPEND_PLUS_MODE       // ������� ��� ������� ���� ��� ������/������ � �����
};

// ��� whence � DL_FsSeekFile
typedef enum
{
	SEEK_WHENCE_SET = 0,
	SEEK_WHENCE_CUR,
	SEEK_WHENCE_END
} SEEK_WHENCE_T;

//��� ��������
enum FS_REMOVE_T_ENUM_
{
	FS_REMOVE_DEFAULT = 0,
	FS_REMOVE_FORCE   = 1
};
typedef UINT8 FS_REMOVE_T;

//�������� �������
typedef struct 
{
	WCHAR		volume[3];
	WCHAR		descr[13];	// flash (for phone)
	UINT32		free_size;	// in bytes
	UINT32		vol_size;	// in bytes
	UINT16		attr;		// ??. For vol /b attr==type
	UINT8		unk1;		// 0x01
	UINT8		unk2;		// 0x00
	UINT16		type;		// ?? 0x40 - TRANS / 0x01 - flash. FileSystem???
	UINT16		unk3;
	UINT32		unk4;		// pointer to DSP??? only for TRANS
} VOLUME_DESCR_T;

// ��������� ������
typedef struct
{
	UINT32		flags; 
	UINT16		attrib; // ��������, � ��������� �������� �� �����, � ������� ���������&mask==attrib
	UINT16		mask;
} FS_SEARCH_PARAMS_T;

// ���������� ������
typedef struct
{
	WCHAR		name[259]; // ������ ��� ����� (� ����). ���� ���� � ����� �� ������� "file://a/" �����������
	UINT16		attrib;
	UINT16		owner;
} FS_SEARCH_RESULT_T;

typedef UINT8 FS_SEARCH_HANDLE_T;

// ��������
#define FS_ATTR_DEFAULT    	0x0000
#define FS_ATTR_READONLY   	0x0001
#define FS_ATTR_HIDDEN     	0x0002
#define FS_ATTR_SYSTEM     	0x0004
#define FS_ATTR_VOLUME     	0x0008
#define FS_ATTR_DIRECTORY  	0x0010
#define FS_ATTR_ARCHIVE    	0x0020

// ����� ������
#define FS_SEARCH_RECURSIVE			0x01
#define FS_SEARCH_SORT_BY_NAME			0x04
#define FS_SEARCH_START_PATH			0x08
#define FS_SEARCH_RECURSIVE_AND_SORT_BY_NAME	0x0D
#define FS_SEARCH_FOLDERS			0x10
#define FS_SEARCH_SORT_BY_DATE			0x20
#define FS_SEARCH_RECURSIVE_AND_SORT_BY_TIME	0x29
#define FS_SEARCH_COMBINED			0x40	// (?) �������� ����� � ���������� ������ �����
#define FS_SEARCH_EXCLUDE			0x80	// (?) 

FILE_HANDLE_T DL_FsOpenFile (WCHAR* uri, UINT8 mode, UINT16 owner);

UINT8 DL_FsCloseFile (FILE_HANDLE_T handle);

UINT8 DL_FsReadFile (void *buffer, UINT32 size, UINT32 count, FILE_HANDLE_T handle, UINT32* read);

UINT8 DL_FsWriteFile (void *buffer, UINT32 size, UINT32 count, FILE_HANDLE_T handle, UINT32 *written);
					  
UINT8 DL_FsFSeekFile (FILE_HANDLE_T handle, INT32 off, SEEK_WHENCE_T whence);

// ������ �����
UINT32 DL_FsGetFileSize (FILE_HANDLE_T handle);
UINT32 DL_FsSGetFileSize (const WCHAR *uri, UINT16 owner);

// ��� ����� ��� ���������� 
WCHAR* DL_FsGetFileName (FILE_HANDLE_T handle, WCHAR *name);
WCHAR* DL_FsSGetFileName (const WCHAR *uri, WCHAR *name);

// ��� ����� � �����������
WCHAR* DL_FsGetFileNameWithExt (FILE_HANDLE_T handle, WCHAR *name);
WCHAR* DL_FsSGetFileNameWithExt (const WCHAR *uri, WCHAR *name);

// ���� � ����� (��� �����)
WCHAR * DL_FsGetPath (FILE_HANDLE_T handle, WCHAR *path);
WCHAR * DL_FsSGetPath (const WCHAR *uri, WCHAR *path);

// ��� �����
WCHAR* DL_FsGetFileType (FILE_HANDLE_T handle, WCHAR *type); //��� ������� �������� ��� ������ ��� ������, ������� ������ �� ����� �. �� ������ :( 
WCHAR* DL_FsSGetFileType (const WCHAR *uri, WCHAR *type); //� ��� �-��� �������� ��������� �� ����� �������!

// ���� �� ������� ���������� ���� /a /b /e
WCHAR* DL_FsGetVolume (FILE_HANDLE_T handle, WCHAR *vol);
WCHAR* DL_FsSGetVolume (const WCHAR *uri, WCHAR *vol);

BOOL DL_FsDirExist (const WCHAR *uri);
BOOL DL_FsFFileExist (const WCHAR *uri);

UINT32 DL_FsFGetPosition( FILE_HANDLE_T handle );

// ������������� ����
UINT8 DL_FsRenameFile (FILE_HANDLE_T handle,  WCHAR *new_name);
UINT8 DL_FsSRenameFile (const WCHAR *old_uri,  WCHAR* new_name,  UINT16 owner);

// ������� ����
UINT8 DL_FsDeleteFile (const WCHAR *uri, UINT16 param1);

// ����������� ����
UINT8 DL_FsFMoveFile (WCHAR *src_uri, WCHAR *dst_uri, UINT16 owner);

// ������� ����������
UINT8 DL_FsMkDir (WCHAR *uri, UINT16 owner);

// ������� ����������
UINT8 DL_FsRmDir (WCHAR *uri, UINT16 owner, FS_REMOVE_T rmtype);

UINT8 DL_FsFSetAttr (FILE_HANDLE_T handle,  UINT16 attrib);
UINT8 DL_FsSSetAttr (const WCHAR *uri, UINT16 owner, UINT16 attrib);

UINT16 DL_FsGetAttr (FILE_HANDLE_T handle);
UINT16 DL_FsSGetAttr (WCHAR *uri, UINT16 owner);

void DL_FsFlush (void);

// �������� ������ ������
WCHAR * DL_FsVolumeEnum (WCHAR *result); // result = L{0,'/',0,'a',0xff,0xfe,0,'/', ...}

// �������� ������� (�����)
VOLUME_DESCR_T * DL_FsGetVolumeDescr (WCHAR *volume, VOLUME_DESCR_T *vd);

// �������� ���������� ������������� �����
UINT32 DL_FsGetIDFromURI (const WCHAR *uri, DL_FS_MID_T *id);

// �������� ���� � ����� �� id
WCHAR * DL_FsGetURIFromID (const DL_FS_MID_T *id, WCHAR *uri);

//UINT32 DL_FsICopyFile( void * r0, WCHAR * uri1, WCHAR * uri2, UINT32 param3, UINT32 uarg_0 );	//r0=0, r3=0

// ������� ����������� ������ ������
// search_string ����������� �� uri �����, ��� ������ �����, ����� ����������� 0xFFFE, ����� ��������.
// ��������: "file://b/Elf/\xFFFE*.elf"
UINT16 DL_FsSSearch (FS_SEARCH_PARAMS_T params, const WCHAR *search_string, FS_SEARCH_HANDLE_T *handle /*out*/, UINT16 *res_count /*out*/, UINT16 owner);

// ������� ��� ��������� [�����] ������ ����������� ������
UINT16 DL_FsSearchResults (FS_SEARCH_HANDLE_T handle, UINT16 start_index /*0 based*/, UINT16 *count /* in and out*/, FS_SEARCH_RESULT_T *results);

UINT16 DL_FsSearchClose (FS_SEARCH_HANDLE_T handle);

#endif
