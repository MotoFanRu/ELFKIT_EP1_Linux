/* ********************************************************************
 * ����� ���������� ����
 * ********************************************************************
 *  (c) G-XaD 2008
 *  version: 1.00.00.00
 * ********************************************************************
 */
#ifndef CLASS_CCFILE_H
#define CLASS_CCFILE_H

#include <typedefs.h>
#include <utilities.h>
#include <filesystem.h>
#include <mem.h>



#ifndef __cplusplus
#error "This is a C++ header file. Please use `tcpp` or `armcpp` compiler."
#endif // __cplusplus


#define FILE_CACHE_SIZE		8*1024






class CCFile {
public:
	CCFile();
	CCFile( BOOL );
	~CCFile();
	
	// base file functions
	UINT32 Open( const WCHAR * p_uri );
	UINT32 Close( void );
	UINT32 Read( void * buf, size_t count, size_t * read );
	size_t GetSize( void );
	UINT32 SetPos( UINT32 newpos );
	UINT32 GetPos( void );
	void EnableCache( BOOL );
	
	

	
private:
	void FreeCache( void );
	FILE		hfile;		// ����
	UINT32		pos;		// ������� ������ �����
	char *		fcache;		// ��������� �� ���
	size_t		fcache_sz;	// ������� ���� ��� ���� � ����
	BOOL		b_cache;	// ������� �� ���
	BOOL		need_cache;	// ����� ���������� ��� ��������� ������
};


#endif // CLASS_FILE_EXTINI_H
