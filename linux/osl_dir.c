#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "stdafx.h"
#include <dirent.h>
#include <string.h>
#include <sys/statvfs.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <unistd.h>
#include "osl.h"
#include "osl_log.h"
#include "osl_dir.h"
#include "osl_string.h"

//#define _XOPEN_SOURCE 500
//#define _LP64


#define OCG_MAX_PATH_SIZE	512

typedef struct
{
	char root[256];
	char dir[256];
	char filter[256];
	DIR* dirobj;
}SFindInfo;

/* ����Ŀ¼: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_dir_create( const char *root, const char *dir )
{
	char path[OCG_MAX_PATH_SIZE];
	char *p;
	uint32_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(path) < len + 3 )
		return -1;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( path, "%s/%s/", root, dir );
	else if( root && *root )
		sprintf( path, "%s/", root );
	else if( dir && *dir )
		sprintf( path, "%s/", dir );
	else
		return -1;
	osl_str_trim_path( path );

	p = path;
	while( p && *p )
	{
		if( *p == '/' && p > path )
		{
			*p = 0;
			if(access(path, F_OK) != 0)
			{//R_OK|W_OK
				if(mkdir(path, S_IRWXU|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH) != 0)
				{
					osl_log_error("osl_dir_create %s failed\r\n", path);
				}
			}
			*p = '/';
		}
		p++;
	}
	return access(path, F_OK);
}


/* ɾ��Ŀ¼������������Ŀ¼���ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_dir_delete( const char *root, const char *dir )
{
	int32_t len = 0;
	DIR *dirobj;
	struct dirent64 entry;
	struct dirent64 *direntp=NULL;
	struct stat64 satabuf;
	char path[OCG_MAX_PATH_SIZE];

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(path) < len + 3 )
		{
	return -1;
		}

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( path, "%s/%s/", root, dir );
	else if( root && *root )
		sprintf( path, "%s/", root );
	else if( dir && *dir )
		sprintf( path, "%s/", dir );
	else
		return -1;

	osl_str_trim_path( path );

	dirobj = opendir( path );
	if(dirobj == NULL)
		return 0;
	len = strlen( path );
	/* ������ɾ��Ŀ¼���������� */
	while( 1 )
	{
		/* ��ȡ��ǰ���� */
		readdir64_r( dirobj, &entry, &direntp);
		if( direntp == NULL )
			break;
		/* .��ʾ��ǰĿ¼ */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == 0)
			continue;
		/* .��ʾ��Ŀ¼ */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == '.' && direntp->d_name[2] == 0)
			continue;
		/* �ļ���Ŀ¼��̫�����޷����� */
		if( sizeof(path) < len + sizeof(direntp->d_name) + 3 )
			continue;
		/* �ϳ�������·���� */
		strcpy( path+len, direntp->d_name );

		/* ȡ������ */
		if( stat64( path, &satabuf ) != 0 )
		   continue;
		/* Ŀ¼ */
		if(S_ISDIR(satabuf.st_mode))
		{
			/* �ݹ�ɾ��Ŀ¼��������� */
			osl_dir_delete( path, "" );
		}
		/* �ļ� */
		else if(S_ISREG(satabuf.st_mode))
		{
			remove( path );
		}
		/* ���� */
		else if(S_ISLNK(satabuf.st_mode))
		{
			unlink( path );
		}
	}

	/* ɾ��Ŀ¼���� */
	path[len] = 0;
	rmdir( path );
	
	/* �ر�Ŀ¼ */
	closedir(dirobj);
	return 0;
}

/* Ŀ¼�Ƿ����: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_dir_exist( const char *root, const char *dir )
{
	char path[OCG_MAX_PATH_SIZE];
	struct stat64 satabuf;
	uint32_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(path) < len + 3 )
		return -1;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( path, "%s/%s/", root, dir );
	else if( root && *root )
		sprintf( path, "%s/", root );
	else if( dir && *dir )
		sprintf( path, "%s/", dir );
	else
		return -1;
	osl_str_trim_path( path );

	if(stat64(path, &satabuf) != 0 || !(S_ISDIR(satabuf.st_mode))){
		return -1;
	}
	return 0;
}


/* ��һ��·�� */
void *osl_dir_open( const char *root, const char *dir, const char *filter )
{
	SFindInfo *info;
	DIR *dirobj;
	char tmp[OCG_MAX_PATH_SIZE];
	int32_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( filter )
		len += strlen( filter );
	if( sizeof(tmp) < len + 3 )
		return NULL;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s", root );
	else if( dir && *dir )
		sprintf( tmp, "%s", dir );
	else
		return NULL;
	osl_str_trim_path( tmp );

	/* ��Ŀ¼ */
	dirobj = opendir( tmp );
	if( dirobj == NULL )
		return NULL;

	/* �����ڴ汣�������Ϣ */
	info = (SFindInfo*)malloc( sizeof(SFindInfo) );
	if( info == NULL )
		return NULL;

	memset( info, 0, sizeof(SFindInfo) );
	if( root )
		strncpy( info->root, root, sizeof(info->root)-1 );
	if( dir )
		strncpy( info->dir, dir, sizeof(info->dir)-1 );
	if( filter )
		strncpy( info->filter, filter, sizeof(info->filter)-1 );
	else
		strncpy( info->filter, "*", sizeof(info->filter)-1 );
	info->dirobj = dirobj;

	return info;
}

/* �ر�һ��·�� */
int32_t osl_dir_close( void* handle )
{
	SFindInfo *info = (SFindInfo *)handle;

	/* �ر�Ŀ¼ */
	if( info->dirobj )
		closedir( info->dirobj );
	free( info );
	return 0;
}


/* ȡ��Ŀ¼�µ�һ���ļ�����Ŀ¼: ����file��dir����'/'����, ... */
int32_t osl_dir_get_first( void* handle, SDirNodeInfo *node, char *file, int32_t size )
{
	rewinddir( ((SFindInfo *)handle)->dirobj );
	return osl_dir_get_next( handle, node, file, size );
}


/* ȡ��Ŀ¼����һ���ļ�����Ŀ¼: ����file��dir����'/'����, ... */
int32_t osl_dir_get_next( void* handle, SDirNodeInfo *node, char *file, int32_t size )
{
	SFindInfo *info = (SFindInfo *)handle;
	char tmp[OCG_MAX_PATH_SIZE];
	struct dirent64 entry;
	struct dirent64 *direntp=NULL;
	struct stat64 satabuf;
	int32_t len;
	bool  bmatch = false;

	while( 1 )
	{
		/* ����ǰĿ¼���� */
		readdir64_r( info->dirobj, &entry, &direntp );
		if( direntp == NULL )
			break;

		/* .��ʾ��ǰĿ¼ */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == 0)
			continue;
		/* .��ʾ��Ŀ¼ */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == '.' && direntp->d_name[2] == 0)
			continue;
		/* ���Ŀ¼���ļ������������ܷ��� */
		if( size < strlen(info->dir) + strlen(direntp->d_name) + 3 )
			continue;
		
		/* ���Ŀ¼���ļ����������޷����� */
		len = strlen( info->root ) + strlen( info->dir ) + strlen( direntp->d_name );
		if( sizeof(tmp) < len + 3 )
			continue;
		/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
		if( info->root[0] && info->dir[0] )
			sprintf( tmp, "%s/%s/%s", info->root, info->dir, direntp->d_name  );
		else if( info->root[0] )
			sprintf( tmp, "%s/%s", info->root, direntp->d_name );
		else if( info->dir[0] )
			sprintf( tmp, "%s/%s", info->dir, direntp->d_name );
		else
			sprintf( tmp, "%s", direntp->d_name );
		osl_str_trim_path( tmp );

		/* ������� */
		if( stat64( tmp, &satabuf ) != 0 )
			continue;

		/*�������Ŀ¼�����ù�������*/
		if(!S_ISDIR(satabuf.st_mode))
		{
			char seps[] = "|";
			char *token = NULL;
			char filter[256];
			memset( filter, 0, sizeof(filter) );
			strncpy( filter, info->filter, sizeof(filter)-1 );
			
			token = strtok( filter, seps );
			while(token != NULL)
			{
				/* �Ƿ�ƥ��������� */
				if( fnmatch( token, direntp->d_name, 0 ) == 0 )
				{
					bmatch = true;
				}
				/* Get next token: */
     				token = strtok( NULL, seps );
			}
			if(!bmatch)
				continue;
		}
		/* Ŀ¼ */
		if(S_ISDIR(satabuf.st_mode))
		{
			/* Ŀ¼��'/'���� */
			if( info->dir[0])
				sprintf( file, "%s/%s/", info->dir, direntp->d_name );
			else
				sprintf( file, "%s/", direntp->d_name );
			if( node )
				node->type = DFTYPE_DIR;
		}
		/* �ļ� */
		else if(S_ISREG(satabuf.st_mode))
		{
			if( info->dir[0])
				sprintf( file, "%s/%s", info->dir, direntp->d_name );
			else
				sprintf( file, "%s", direntp->d_name );
			if( node )
				node->type = DFTYPE_FILE;
		}
		/* ���� */
		else if(S_ISLNK(satabuf.st_mode))
		{
			if( info->dir[0])
				sprintf( file, "%s/%s", info->dir, direntp->d_name );
			else
				sprintf( file, "%s", direntp->d_name );
			if( node )
				node->type = DFTYPE_LINK;
		}
		else
			continue;
		if( node )
		{
			node->size = satabuf.st_size;
			node->modify_utc = satabuf.st_ctime;
		}
		return 0;
	}

	return -1;
}

/* ȡ�ô��̿ռ���Ϣ(MB) */
uint32_t osl_dir_get_space_info( const char *root, const char *dir, EOslDirSpace type)
{
	char path[OCG_MAX_PATH_SIZE];
	struct statvfs sfs;
	fsblkcnt_t block;
	fsblkcnt_t bsize;
	uint32_t len = 0;

	if( root )
		len += strlen( root );
	if( dir )
		len += strlen( dir );
	if( sizeof(path) < len + 3 )
		return 0;

	/* �ϳ�������·�������ü�Ϊ�Ϸ�д�� */
	if( root && *root && dir && *dir )
		sprintf( path, "%s/%s", root, dir );
	else if( root && *root )
		sprintf( path, "%s", root );
	else if( dir && *dir )
		sprintf( path, "%s", dir );
	else
		return 0;
	osl_str_trim_path( path );

	if (statvfs(path, &sfs) != -1)
	{
		bsize = sfs.f_bsize;
		switch (type)
		{
		case OSL_DIR_TOTAL_SIZE:
			block = sfs.f_blocks;
			return (uint32_t)(bsize * block/(1024*1024));
			break;
		case OSL_DIR_FREE_SIZE:
			block = sfs.f_bfree;
			return (uint32_t)(bsize * block/(1024*1024));
			break;
		case OSL_DIR_USED_SIZE:
			block = sfs.f_blocks -sfs.f_bavail;
			return (uint32_t)(bsize * block/(1024*1024));
			break;
		case OSL_DIR_AVAIL_SIZE:
			block = sfs.f_bavail;
			return (uint32_t)(bsize * block/(1024*1024));
			break;
		}
	}
	return 0;
}
