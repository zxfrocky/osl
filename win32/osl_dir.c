/***********************************************************************
    Copyright (C), 2008, Embedded Internet Solutions, Inc.
    FileName          : ReceiverFtp.h
    Description       : ftp�����ļ���ʵ��
    Version           : 3.0.0			
    Function List     : 
    1. -------
        
		  History: // ��ʷ�޸ļ�¼
         
    chenq 2008/08/08  3.0  build this moudle
************************************************************************/

#include "stdafx.h"
#include "osl.h"
#include "osl_dir.h"
#include "osl_string.h"

typedef struct
{
	char root[256];
	char dir[256];
	char filter[256];
	HANDLE hFind; 
	WIN32_FIND_DATA data;
}SFindInfo;

#define OCG_MAX_PATH_SIZE 512

/* ����Ŀ¼ */
int32_t osl_dir_create( const char *root, const char *dir )
{
	char *start, *p, tmp[256] = {0};
	//int32_t len = 0;
	size_t len = 0; 

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(tmp) < len + 3 )
		return -1;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s/", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s/", root );
	else if( dir && *dir )
		sprintf( tmp, "%s/", dir );
	else
		return -1;
	osl_str_trim_path( tmp );

	/* �𼶴���Ŀ¼ */
	p = start = tmp;
	while( *p )
	{
		if( *p == '/' )
			*p = '\\';
		if( start < p && *p == '\\' )
		{
			*p = 0;
			if( osl_dir_exist( "", tmp ) != 0 )
			{
				if (0 == CreateDirectory( (PSTRING)tmp, NULL ))
				{
					//printf("failededededededed.....\n");
					//ERROR_ALREADY_EXISTS
					len = GetLastError();
				}
			}
			*p = '\\';

			p++;
			start = p;
		}
		else
			p++;
	}
	return 0;
}


/* ɾ��Ŀ¼������������Ŀ¼���ļ� */
int32_t osl_dir_delete( const char *root, const char *dir )
{
	HANDLE hFind; 
	WIN32_FIND_DATA data; 
	char *p, tmp[256];
	//int32_t len = 0;
	size_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(tmp) < len + 3 )
		return -1;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s/*", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s/*", root );
	else if( dir && *dir )
		sprintf( tmp, "%s/*", dir );
	else
		return -1;
	osl_str_trim_path( tmp );

	hFind = FindFirstFile( (PSTRING)tmp, &data );

	/* ��ȡĿ¼ */
	p = tmp + strlen( tmp );
	while( *p != '/' )
		p--;
	*p = 0;

	if( hFind != INVALID_HANDLE_VALUE ) 
	{
		do
		{
			if( data.cFileName[0] == '.' )
			{
			}
			else if( ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
			{
				osl_dir_delete( (const char*)tmp, (const char*)data.cFileName );
			}
			else if( (p-tmp+2) + strlen((const char*)data.cFileName) < sizeof(tmp) )
			{
				sprintf( p, "/%s", data.cFileName );
				DeleteFile( (PSTRING)tmp );
				*p = 0;
			}
		}while( FindNextFile( hFind, &data ) );

		FindClose(hFind);
	}
	
	RemoveDirectory( (PSTRING)tmp );

	return 0;
}


/* Ŀ¼�Ƿ����: ����-1=�����ڣ�0=���� */
int32_t osl_dir_exist( const char *root, const char *dir )
{
	HANDLE hFind; 
	WIN32_FIND_DATA data; 
	char tmp[256];
	//int32_t len = 0;
	size_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(tmp) < len + 3 )
		return -1;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s/*", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s/*", root );
	else if( dir && *dir )
		sprintf( tmp, "%s/*", dir );
	else
		return -1;
	osl_str_trim_path( tmp );

	hFind = FindFirstFile( (PSTRING)tmp, &data );
	if( hFind != INVALID_HANDLE_VALUE ) 
	{
		FindClose(hFind); 
		return 0;
	}
	return -1;
}


/* ��һ��·�� */
void *osl_dir_open( const char *root, const char *dir, const char *filter )
{
	HANDLE hFind; 
	WIN32_FIND_DATA data;
	SFindInfo *info;
	char tmp[256], *f;
	//int32_t len = 0;
	size_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( filter )
		len += strlen( filter );
	if( sizeof(tmp) < len + 3 )
		return NULL;
	f = filter == NULL ? "*" : filter;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s/%s", root, dir, f );
	else if( root && *root )
		sprintf( tmp, "%s/%s", root, f );
	else if( dir && *dir )
		sprintf( tmp, "%s/%s", dir, f );
	else
		return NULL;
	osl_str_trim_path( tmp );

	hFind = FindFirstFile( (PSTRING)tmp, &data );
	if( hFind == INVALID_HANDLE_VALUE )
		return NULL;

	info = (SFindInfo*)malloc( sizeof(SFindInfo) );
	if( info == NULL )
		return NULL;

	memset( info, 0, sizeof(SFindInfo) );
	if( root )
		osl_strncpy( info->root, root, sizeof(info->root)-1 );
	if( dir )
		osl_strncpy( info->dir, dir, sizeof(info->dir)-1 );
	if( filter )
		osl_strncpy( info->filter, filter, sizeof(info->filter)-1 );
	info->hFind = hFind;
	info->data = data;
	return info;
}

/* �ر�һ��·�� */
int32_t osl_dir_close( void* handle )
{
	SFindInfo *info = (SFindInfo*)handle;
	if( info->hFind != INVALID_HANDLE_VALUE )
		FindClose( info->hFind );
	free( info );
	return 0;
}


/* ȡ��Ŀ¼�µ�һ���ļ�����Ŀ¼: ����file��dir����'/'����, ... */
int32_t osl_dir_get_first( void* handle, SDirNodeInfo *node, char *file, int32_t size )
{
	SFindInfo *info = (SFindInfo*)handle;
	SYSTEMTIME st;
	struct tm tt;

	if( info->hFind == INVALID_HANDLE_VALUE )
		return -1;

	file[0] = 0;
	do
	{
		//�ļ���Ϊ'.'����������ļ�ֻ�к�׺������
		if( info->data.cFileName[0] == '.' &&
			(info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
		{
		}
		else if( strlen((const char*)info->dir) + strlen((const char*)info->data.cFileName) + 2 < (uint32_t)size )
		{
			if( info->dir[0])
			{
				if( ( info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
					sprintf( file, "%s/%s/", info->dir, info->data.cFileName );
				else
					sprintf( file, "%s/%s", info->dir, info->data.cFileName );
			}
			else
			{
				if( ( info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
					sprintf( file, "%s/", info->data.cFileName );
				else
					osl_strncpy( file, (const char*)info->data.cFileName, size-1 );
			}
			osl_str_trim_path( file );

			if( node )
			{
				if( ( info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
					node->type = DFTYPE_DIR;
				else
					node->type = DFTYPE_FILE;
				FileTimeToSystemTime( &info->data.ftLastWriteTime, &st );
				memset( &tt, 0, sizeof(tt) );
				tt.tm_year = st.wYear - 1900;
				tt.tm_mon = st.wMonth - 1;
				tt.tm_mday = st.wDay;
				tt.tm_hour = st.wHour;
				tt.tm_min = st.wMinute;
				tt.tm_sec = st.wSecond;
				node->modify_utc = (uint32_t)mktime( &tt );
				node->size = info->data.nFileSizeHigh;
				node->size = (node->size<<32) + info->data.nFileSizeLow;
			}
			return 0;
		}
	}while( FindNextFile( info->hFind, &info->data ) );

	return -1;
}


/* ȡ��Ŀ¼����һ���ļ�����Ŀ¼: ����file��dir����'/'����, ... */
int32_t osl_dir_get_next( void* handle, SDirNodeInfo *node, char *file, int32_t size )
{
	SFindInfo *info = (SFindInfo*)handle;
	SYSTEMTIME st;
	struct tm tt;

	if( info->hFind == INVALID_HANDLE_VALUE )
		return -1;

	file[0] = 0;
	while( FindNextFile( info->hFind, &info->data ) )
	{
		//windows�¿��ܴ���ֻ�к�׺�����ļ�
		/*if( info->data.cFileName[0] == '.' )
		{
		}
		else if( strlen(info->dir) + strlen(info->data.cFileName) + 2 < (uint32_t)size )*/
		if( strlen((const char*)info->dir) + strlen((const char*)info->data.cFileName) + 2 < (uint32_t)size )
		{
			if( info->dir[0])
			{
				if( ( info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
					sprintf( file, "%s/%s/", info->dir, info->data.cFileName );
				else
					sprintf( file, "%s/%s", info->dir, info->data.cFileName );
			}
			else
			{
				if( ( info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
					sprintf( file, "%s/", info->data.cFileName );
				else
					osl_strncpy( file, (const char*)info->data.cFileName, size-1 );
			}
			osl_str_trim_path( file );

			if( node )
			{
				if( ( info->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
					node->type = DFTYPE_DIR;
				else
					node->type = DFTYPE_FILE;
				FileTimeToSystemTime( &info->data.ftLastWriteTime, &st );
				memset( &tt, 0, sizeof(tt) );
				tt.tm_year = st.wYear - 1900;
				tt.tm_mon = st.wMonth - 1;
				tt.tm_mday = st.wDay;
				tt.tm_hour = st.wHour;
				tt.tm_min = st.wMinute;
				tt.tm_sec = st.wSecond;
				node->modify_utc = (uint32_t)mktime( &tt );
				node->size = info->data.nFileSizeHigh;
				node->size = (node->size<<32) + info->data.nFileSizeLow;
			}
			return 0;
		}
	}

	return -1;
}

/* ȡ�ô��̿ռ���Ϣ(MB) */
uint32_t osl_dir_get_space_info( const char *root, const char *dir, EOslDirSpace type)
{
	/*·����������������*/
	char tmp[256];
	/*���û����õĴ��̿ռ�*/
	uint64_t nFreeBytesAvailable = 0;
	/*�Ǵ����ܹ��Ŀռ�*/
	uint64_t nTotalNumberOfBytes = 0;
	/*�Ǵ��̿��еĿռ�*/
	uint64_t nTotalNumberOfFreeBytes = 0;
	size_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(tmp) < len + 3 )
		return 0;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s", root );
	else if( dir && *dir )
		sprintf( tmp, "%s", dir );
	else
		return 0;
	osl_str_trim_path( tmp );

	if ( GetDiskFreeSpaceEx((PSTRING)tmp,
							(PULARGE_INTEGER)&nFreeBytesAvailable,
							(PULARGE_INTEGER)&nTotalNumberOfBytes,
							(PULARGE_INTEGER)&nTotalNumberOfFreeBytes) )
	{
		switch(type)
		{
		case OSL_DIR_TOTAL_SIZE:
			return (uint32_t)(nTotalNumberOfBytes/1024/1024);
			break;
		case OSL_DIR_FREE_SIZE:
			return (uint32_t)(nTotalNumberOfFreeBytes/1024/1024);
			break;
		case OSL_DIR_USED_SIZE:
			return (uint32_t)((nTotalNumberOfBytes - nFreeBytesAvailable)/1024/1024);
			break;
		case OSL_DIR_AVAIL_SIZE:
			return (uint32_t)(nFreeBytesAvailable/1024/1024);
			break;
		}
	}

	return 0;
}

/* ��ȡĿ¼��Ϣ ����-1=ʧ�ܣ�0=�ɹ�*/
int32_t osl_dir_get_info(const char *root, const char *dir,SDirNodeInfo *node)
{
	HANDLE hFind; 
	WIN32_FIND_DATA data;
	SYSTEMTIME st;
	struct tm tt;
	char tmp[256],*p;
	size_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(tmp) < len + 3 )
		return -1;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s/", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s", root  );
	else if( dir && *dir )
		sprintf( tmp, "%s/", dir );
	else
		return -1;

	p=tmp;//ȥ������� '/', 
	while( *p )
		p++;
	if( *(p-1) == '/' )
		*(p-1) = 0;

	hFind = FindFirstFile( (PSTRING)tmp, &data );
	if( hFind != INVALID_HANDLE_VALUE && 
		(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
	{
		node->type = DFTYPE_DIR;
	}
	else if( hFind != INVALID_HANDLE_VALUE )
	{
		node->type = DFTYPE_FILE;
	}
	else
	{
		return -1;
	}
	if( node )
	{
		FileTimeToSystemTime( &data.ftLastWriteTime, &st );
		memset( &tt, 0, sizeof(tt) );
		tt.tm_year = st.wYear - 1900;
		tt.tm_mon = st.wMonth - 1;
		tt.tm_mday = st.wDay;
		tt.tm_hour = st.wHour;
		tt.tm_min = st.wMinute;
		tt.tm_sec = st.wSecond;
		node->modify_utc = (uint32_t)mktime( &tt );
		node->size = osl_dir_get_space_info(root,dir,OSL_DIR_USED_SIZE)*1024*1024;//ת��Ϊbyte
	}

	FindClose(hFind); 
	return 0;
}
