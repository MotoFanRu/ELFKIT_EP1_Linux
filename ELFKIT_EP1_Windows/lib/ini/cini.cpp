//#define DEBUG

//#define OPTIMIZE_1

#include <typedefs.h>
#include <dl.h>
#include <mem.h>
#include <resources.h>
#include <utilities.h>
#include "cini.h"
#include "..\dbg\dbg.h"

#define READ_BUFFER_SIZE			1024	// 1Kb



// constructors/destructor

CINI::CINI ( void )
{
	this->file = NULL;
	this->f_pos = 0;
	this->cur_section = NULL;
	this->cache_size = 0;
}

CINI::~CINI ( void )
{
	this->Close();
}


// class methods

UINT32 CINI::Open( const WCHAR * p_uri )
{
	FILE	f;
	
	udbg("CINI::Open: Enter, uri = %s", p_uri);
	
	if ( p_uri == NULL )
		return RESULT_FAIL;
	if ( this->file != NULL )
		if ( this->Close() != RESULT_OK )
			return RESULT_FAIL;
	if ( DL_FsFFileExist(p_uri) == FALSE )
		return RESULT_FAIL;
	f = DL_FsOpenFile(p_uri, FILE_READ_MODE, 0);
	if ( f == FILE_INVALID )
		return RESULT_FAIL;
	this->file = f;
	this->ResetPos();
	
	this->cache_size = 0;
	
	dbg("CINI::Open: END", NULL);
	
	return RESULT_OK;
}
UINT32 CINI::Close( void )
{
	if ( this->file != FILE_INVALID ) {
		if ( DL_FsCloseFile(this->file) != RESULT_OK )
			return RESULT_FAIL;
		this->file = NULL;
	}
	if ( this->cur_section != NULL )
		delete [] this->cur_section;
	this->FreeCache();
	return RESULT_OK;
}
void CINI::FreeCache( void )
{
	UINT32	i;
	for ( i=0; i<this->cache_size && i<SECTIONS_MAX_CACHE_SIZE; i++ )
		delete [] cache[i].name;	// �-��� delete ���������, ���� �������� NULL, ������ �� ����������
	this->cache_size = 0;
}
UINT32 CINI::ReadSections( void )
{
	char *	rbuf;
	FILE	f;
	UINT32	tmp_pos, i, j, pos, sz;
	UINT32	num = 0;
	size_t	read;
	
	f = this->file;
	if ( f == 0 )
		return 0;
	sz = DL_FsGetFileSize( f );
	rbuf = new char[READ_BUFFER_SIZE];
	if ( rbuf == NULL )
		return 0;
	this->FreeCache();
	tmp_pos = this->GetPos();
	pos = 0;
	for ( i=0; pos < sz; pos += i+1 ) {
		DL_FsFSeekFile( f, pos, SEEK_WHENCE_SET );
		DL_FsReadFile( rbuf, sizeof(char), READ_BUFFER_SIZE, f, (UINT32 *) &read );
		i = 0;
		if ( rbuf[i] == '[' ) {	// ������ ������
			for ( i=1; i<read; i++ ) {
				if ( rbuf[i] == ']' || rbuf[i] == 0xA )
					break;		// ����� ����� ������ ��� ����������� ������
			}
			if ( i >= read )	// �� ������� ������� ������
				continue;
			if ( rbuf[i] == ']' ) {	// ��� ��, ��� ��� �����
				j = i+1;
				while ( rbuf[j] != 0xA && j < read )	// ���������� ��� �� ����� ������
					j++;
				rbuf[i] = 0;
				dbg("section_name = %s", rbuf+1);
				
				// �������� �������� ������
				if ( num < SECTIONS_MAX_CACHE_SIZE ) {		// ���� ��������� ����� � ����
					this->cache[num].name = new char[i];	// rbuf[0] = '[', rbuf[i] = ']'
					strcpy(this->cache[num].name, rbuf+1);
					this->cache[num].f_pos = pos+j+1;
					if ( ++num == SECTIONS_MAX_CACHE_SIZE )
						break;	// ��� ����������� ��������, �� �����
				} else
					break;
			}
		}
		while ( rbuf[i] != 0xA && i < read )
			i++;
	}
	
	delete rbuf;
	this->cache_size = num;
	this->SetPos(tmp_pos);
	return num;
}

BOOL CINI::FindSection( const char * sname )
{
	char *	rbuf;
	FILE	f;
	UINT32	sz, pos, i, j;
	BOOL	status = FALSE;
	UINT32	len;
	size_t	read;
	
	f = this->file;
	if ( f == 0 )
		return FALSE;
	// ���� � ����
	for ( i=0; i<this->cache_size && i<SECTIONS_MAX_CACHE_SIZE; i++ ) {
		if ( strcmp(sname, this->cache[i].name) == 0 ) {
			this->SetPos(this->cache[i].f_pos);
			return TRUE;
		}
	}
	
	len = strlen(sname);
	
	sz = DL_FsGetFileSize( f );
	DL_FsFSeekFile( f, 0, SEEK_WHENCE_SET );	// ������ �� ������, �� ����� ��������� �������� ������
	pos = 0;
	rbuf = new char[READ_BUFFER_SIZE];
	if ( rbuf == NULL )
		return FALSE;
	
	// ������ 1 ������ �� ��������, ������� ���� �������� � ��� ���� �� ������ ��� READ_BUFFER_SIZE
	for ( ; pos < sz; pos += i+1 ) {
		dbg("pos = %d", pos);
		DL_FsFSeekFile( f, pos, SEEK_WHENCE_SET );
		DL_FsReadFile( rbuf, sizeof(char), READ_BUFFER_SIZE, f, (UINT32 *) &read );
		
		i = 0;
		dbg("rbuf[%d] = %c, read = %d", i, rbuf[i], read);
		if ( rbuf[i] == '[' ) {	// ������ ������
#ifdef OPTIMIZE_1			
			if ( rbuf[i+len+1] == ']' ) {
				// �������� �����,��������� ���. ����� ���������� ������
				j = i+1;
				while ( rbuf[j] != 0xA && j < read )	// ���� ����� ������
					j++;
				if ( strncmp(rbuf+i+1, sname, len) == 0 ) {
					// ��� ���
					j++;
					this->SetPos(pos+j);
					
					if ( this->cur_section != NULL )
						delete [] this->cur_section;
					this->cur_section = new char[len+1];
					memcpy(this->cur_section, sname, len);
					this->cur_section[len] = 0;
					status = TRUE;
					break;			// ����� �� ����� ������ �����
				} else {
					i = j;
				}
			}
			
#else
			for ( i=1; i<read; i++ ) {
				if ( rbuf[i] == ']' || rbuf[i] == 0xA )
					break;		// ����� ����� ������ ��� ����������� ������
			}
			if ( i >= read )	// �� ������� ������� ������
				continue;
			if ( rbuf[i] == ']' ) {	// ��� ��, ��� ��� �����
				j = i+1;
				while ( rbuf[j] != 0xA && j < read )	// ���������� ��� �� ����� ������
					j++;
				rbuf[i] = 0;
				dbg("section_name = %s", rbuf+1);
				if ( strcmp(rbuf+1, sname) == 0 ) {	// ������� ������
					/* ��������� ��������� ������ �� ��������� ������ � ����� �� �-���,
						������ ���������� ��� ������ ������� */
					j++;	// ���������� ����� ������
					this->SetPos(pos+j);
					
					if ( this->cur_section != NULL )
						delete [] this->cur_section;
					this->cur_section = new char[len+1];
					strcpy(this->cur_section, sname);
					status = TRUE;
					break;			// ����� �� ����� ������ �����
				} else {	// ��� �� �� =\, ������� ��������� ������!
					i = j;
				}
			}
#endif
		}
		while ( rbuf[i] != 0xA && i < read )
			i++;
	}
	delete [] rbuf;
	return status;
}

BOOL CINI::ReadParam( const char * param_name, char * dst, size_t buf_size, const char * default_value )
{
	FILE	f;
	char *	rbuf;
	UINT32	start_pos;		// ������� ��������� ����� ������� ��������� � ������.
							//  ����� ������� � ������ ������ � ���� ������, ���� �� ����� ������� � ������
	BOOL	status = FALSE;
	UINT32	sz, pos, i, pname_len, j;
	size_t	read;
	
	dbg("ReadParam: Enter, f=0x%x", this->file);
	
	f = this->file;
	if ( f == NULL )
		return FALSE;
	pos = start_pos = this->f_pos;
	sz = DL_FsGetFileSize( f );
	dbg("ReadParam: DL_FsGetFileSize DONE, sz=%d", sz);
	rbuf = new char[READ_BUFFER_SIZE];
	dbg("ReadParam: new char[READ_BUFFER_SIZE] DONE, rbuf = 0x%p", rbuf);
	if ( rbuf == NULL )
		return FALSE;
	pname_len = strlen(param_name);
	dbg("ReadParam: strlen DONE, pname_len=%d", pname_len);
	
	for ( i=0; pos < sz; pos += i+1 ) {
		dbg("pos = %d", pos);
		DL_FsFSeekFile( f, pos, SEEK_WHENCE_SET );
		DL_FsReadFile( rbuf, sizeof(char), READ_BUFFER_SIZE, f, (UINT32 *) &read );
		
		i = 0;
		dbg("rbuf[%d] = %c, read = %d", i, rbuf[i], read);
		if ( rbuf[i] == '[' ) {	// ������ ������
			// ������ �� �������!
			status = FALSE;
			break;
		}
		dbg("rbuf[%d] = %c, read = %d", i, rbuf[i], read);
		if ( rbuf[pname_len] == '=' && strncmp(param_name, rbuf, pname_len) == 0 ) {
			// ������ ��������
			status = TRUE;
			break;
		}
		// ���������� ������
		dbg("skip simbols to end of string", NULL);
		while ( i < read && rbuf[i] != 0xA )
			i++;
	}
	dbg("ReadParam: Search param DONE, status=%d", status);
	
	if ( status ) {
		// ����������� � dst �������� ���������
		for ( i=pname_len+1, j=0; i<read && j<buf_size-1; i++, j++ ) {
			if ( rbuf[i] == 0xD || rbuf[i] == 0xA || rbuf[i] == 0 )		// WIN
				break;
			dst[j] = rbuf[i];
		}
		if ( j <= buf_size )
			dst[j] = 0;
	} else {
		for ( j=0; j<buf_size-1; j++ ) {
			if ( default_value[j] == 0 )		// WIN
				break;
			dst[j] = default_value[j];
		}
		if ( j <= buf_size )
			dst[j] = 0;
	}
	
	delete [] rbuf;
	dbg("ReadParam: delete [] rbuf DONE", NULL);
	DL_FsFSeekFile(f, start_pos, SEEK_WHENCE_SET);
	dbg("ReadParam: DL_FsFSeekFile DONE", NULL);
	
	dbg("ReadParam: END, status = %d", status);
	
	return status;
}

UINT32 CINI::ResetPos( void )
{
	FILE	f = this->file;
	
	if ( f != NULL )
		DL_FsFSeekFile( f, 0, SEEK_WHENCE_SET );
	else
		return RESULT_FAIL;
	this->f_pos = 0;
	if ( this->cur_section != NULL )
		delete [] this->cur_section;
	this->cur_section = NULL;
	return RESULT_OK;
}
UINT32 CINI::GetPos( void )
{
	return this->f_pos;
}
void CINI::SetPos( UINT32 new_pos )
{
	DL_FsFSeekFile( this->file, new_pos, SEEK_WHENCE_SET );
	this->f_pos = new_pos;
}
const char * CINI::GetCurSection( void )
{
	return this->cur_section;
}
















