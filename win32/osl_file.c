#include "stdafx.h"
#include "osl.h"
#include "osl_file.h"
#include "osl_string.h"

/* �������ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_rename( const char *srcfile, const char *dstfile )
{
	return rename( srcfile, dstfile );
}

/* �����ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_copy( const char *srcfile, const char *dstfile )
{
	if( CopyFile( (PSTRING)srcfile, (PSTRING)dstfile, true ) == TRUE )
		return 0;
	else
		return -1;
}

/* ɾ���ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_delete( const char *file )
{
	if( DeleteFile( (PSTRING)file ) == TRUE )
		return 0;
	else
		return -1;
}

/* �ļ��Ƿ����: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_exist( const char *file )
{
	WIN32_FIND_DATA fileInfo; 
	HANDLE hFind; 
	
	hFind = FindFirstFile((PSTRING)file, &fileInfo); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind); 
		return 0;
	}
	return -1;
}

/* ȡ���ļ��ߴ��������ʱ��: size=bytes, time=utc-ms, ����0��ʾ�ɹ�,-1��ʾʧ��*/
int32_t osl_file_get_info( const char *file, SFileNodeInfo *node )
{
	WIN32_FIND_DATA fileInfo; 
	HANDLE hFind; 
	SYSTEMTIME st;
	struct tm tt;

	hFind = FindFirstFile((PSTRING)file ,&fileInfo); 
	if( hFind != INVALID_HANDLE_VALUE &&
	   (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
	{
	   if( node )
		{
			FileTimeToSystemTime( &fileInfo.ftLastWriteTime, &st );
			memset( &tt, 0, sizeof(tt) );
			tt.tm_year = st.wYear - 1900;
			tt.tm_mon = st.wMonth - 1;
			tt.tm_mday = st.wDay;
			tt.tm_hour = st.wHour;
			tt.tm_min = st.wMinute;
			tt.tm_sec = st.wSecond;
			node->modify_utc = (uint32_t)mktime( &tt );

			node->size = fileInfo.nFileSizeHigh;
			node->size = (node->size<<32) + fileInfo.nFileSizeLow;
		}

		FindClose(hFind); 
		
		return 0;
	}
	
	return -1;
}

/* ���������ļ����һ���޸�ʱ��, ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_set_modify_utc( const char* file, uint32_t modify_utc )
{	
	HANDLE hFile;
	SYSTEMTIME systime;
	FILETIME ft;
	struct tm* tt;
	time_t tmp = (time_t)modify_utc;

	tt = gmtime( &tmp );
	if( tt )
	{
		hFile = CreateFile((PSTRING)file,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if( hFile != INVALID_HANDLE_VALUE )
		{
			memset(&systime,0,sizeof(SYSTEMTIME));   
			systime.wYear = tt->tm_year+1900;
			systime.wMonth = tt->tm_mon+1;
			systime.wDay = tt->tm_mday;
			systime.wHour = tt->tm_hour;
			systime.wMinute = tt->tm_min;
			systime.wSecond = tt->tm_sec;
			systime.wDayOfWeek = tt->tm_wday;

			SystemTimeToFileTime( &systime, &ft );
			SetFileTime( hFile, NULL, NULL, &ft );
			CloseHandle( hFile );
			return 0;
		}
	}
	return -1;
}

/* ���ļ�, mode=OSL_FILE_READ_ONLY */
void* osl_file_open( const char *file, int32_t mode )
{
	HANDLE hFile = INVALID_HANDLE_VALUE;

	switch( mode )
	{
	case OSL_FILE_READ_ONLY:
		hFile = CreateFile((PSTRING)file,
			GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		break;
	case OSL_FILE_WRITE_ONLY:
		hFile = CreateFile((PSTRING)file,
			GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		break;
	case OSL_FILE_READ_WRITE:
		hFile = CreateFile((PSTRING)file,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		break;
	}
	return hFile == INVALID_HANDLE_VALUE ? NULL : hFile;
}

/* �ر��ļ� */
void osl_file_close( void *fp )
{
	if( NULL != fp )
		CloseHandle( (HANDLE) fp );

	return;
}

/* �޸��ļ�Ȩ�� */
void osl_file_chmod(const char *file, int32_t mode)
{
	switch( mode )
	{
	case OSL_FILE_READ_ONLY:
		SetFileAttributesA(file, FILE_ATTRIBUTE_READONLY);
		break;
	case OSL_FILE_WRITE_ONLY:
		SetFileAttributesA(file, FILE_ATTRIBUTE_NORMAL);
		break;
	case OSL_FILE_READ_WRITE:
		SetFileAttributesA(file, FILE_ATTRIBUTE_NORMAL);
		break;
	}
}

/* ���ļ�: ����0��ʾ�ļ�����,-1��ʾʧ��,>0��ʾ��ȡ�ĳߴ� */
int32_t osl_file_read( void *fp, void *buf, int32_t size )
{
	/*ʵ�ʶ�ȡ���ֽڳ���*/
	int32_t iLen;
	/*��ȡ�������ؽ������*/
	BOOL bFlag;
	
	if( (NULL != fp) && ( NULL != buf ) && ( 0 < size) ){
		bFlag = ReadFile( (HANDLE)fp, buf, size, &iLen, NULL);
		
		if( bFlag )
		{
			if( iLen == 0 )
				return 0;	
			else if( iLen > 0 )
				return iLen;
		}
	}
	
	return -1;
}

/* д�ļ� */
int32_t osl_file_write( void *fp, void *buf, int32_t size )
{
	/*ʵ��д����ֽڳ���*/
	int32_t iLen;
	/*д���������ֵ*/
	BOOL  bFlag;
	
	if ( ( NULL != fp ) && ( NULL != buf ) && ( 0 <= size ) ) {
		bFlag = WriteFile( (HANDLE)fp, buf, size, &iLen, NULL);
		if( bFlag )
	       return iLen;			
	}
	
	return -1;
}

/* ��λ�ļ�λ��: base=SEEK_SET, ... */
void osl_file_seek( void *fp, int64_t pos, int32_t base )
{
	BOOL bflag = FALSE;
	LARGE_INTEGER  li;
    li.QuadPart = pos;	

	if( NULL != fp  )
		bflag = SetFilePointerEx( (HANDLE)fp, li, NULL, base);
}


/* ȡ���ļ�λ�� */
int64_t osl_file_tell( void *fp )
{
	LARGE_INTEGER pos;

	pos.QuadPart = 0;
	SetFilePointerEx( (HANDLE)fp, pos, &pos, FILE_CURRENT );

	return pos.QuadPart;
}

/* �ļ��Ƿ����: ����1��ʾ����,0��ʾδ���� */
int32_t osl_file_eof( void *fp )
{
	LARGE_INTEGER pos, len;

	pos.QuadPart = 0;
	SetFilePointerEx( (HANDLE)fp, pos, &pos, FILE_CURRENT );
	GetFileSizeEx( (HANDLE)fp, &len );

	return pos.QuadPart == len.QuadPart;
}
