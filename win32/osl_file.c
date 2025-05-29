#include "stdafx.h"
#include "osl.h"
#include "osl_file.h"
#include "osl_string.h"

/* 重命名文件: 返回0表示成功,-1表示失败 */
int32_t osl_file_rename( const char *srcfile, const char *dstfile )
{
	return rename( srcfile, dstfile );
}

/* 复制文件: 返回0表示成功,-1表示失败 */
int32_t osl_file_copy( const char *srcfile, const char *dstfile )
{
	if( CopyFile( (PSTRING)srcfile, (PSTRING)dstfile, true ) == TRUE )
		return 0;
	else
		return -1;
}

/* 删除文件: 返回0表示成功,-1表示失败 */
int32_t osl_file_delete( const char *file )
{
	if( DeleteFile( (PSTRING)file ) == TRUE )
		return 0;
	else
		return -1;
}

/* 文件是否存在: 返回0表示成功,-1表示失败 */
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

/* 取得文件尺寸和最后更新时间: size=bytes, time=utc-ms, 返回0表示成功,-1表示失败*/
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

/* 重新设置文件最后一次修改时间, 返回0表示成功,-1表示失败 */
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

/* 打开文件, mode=OSL_FILE_READ_ONLY */
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

/* 关闭文件 */
void osl_file_close( void *fp )
{
	if( NULL != fp )
		CloseHandle( (HANDLE) fp );

	return;
}

/* 修改文件权限 */
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

/* 读文件: 返回0表示文件结束,-1表示失败,>0表示读取的尺寸 */
int32_t osl_file_read( void *fp, void *buf, int32_t size )
{
	/*实际读取的字节长度*/
	int32_t iLen;
	/*读取操作返回结果变量*/
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

/* 写文件 */
int32_t osl_file_write( void *fp, void *buf, int32_t size )
{
	/*实际写入的字节长度*/
	int32_t iLen;
	/*写入操作返回值*/
	BOOL  bFlag;
	
	if ( ( NULL != fp ) && ( NULL != buf ) && ( 0 <= size ) ) {
		bFlag = WriteFile( (HANDLE)fp, buf, size, &iLen, NULL);
		if( bFlag )
	       return iLen;			
	}
	
	return -1;
}

/* 定位文件位置: base=SEEK_SET, ... */
void osl_file_seek( void *fp, int64_t pos, int32_t base )
{
	BOOL bflag = FALSE;
	LARGE_INTEGER  li;
    li.QuadPart = pos;	

	if( NULL != fp  )
		bflag = SetFilePointerEx( (HANDLE)fp, li, NULL, base);
}


/* 取得文件位置 */
int64_t osl_file_tell( void *fp )
{
	LARGE_INTEGER pos;

	pos.QuadPart = 0;
	SetFilePointerEx( (HANDLE)fp, pos, &pos, FILE_CURRENT );

	return pos.QuadPart;
}

/* 文件是否结束: 返回1表示结束,0表示未结束 */
int32_t osl_file_eof( void *fp )
{
	LARGE_INTEGER pos, len;

	pos.QuadPart = 0;
	SetFilePointerEx( (HANDLE)fp, pos, &pos, FILE_CURRENT );
	GetFileSizeEx( (HANDLE)fp, &len );

	return pos.QuadPart == len.QuadPart;
}
