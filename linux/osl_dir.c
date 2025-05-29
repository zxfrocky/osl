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

/* 创建目录: 返回0表示成功,-1表示失败 */
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

	/* 合成完整的路径名并裁减为合法写法 */
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


/* 删除目录及旗下所有子目录和文件: 返回0表示成功,-1表示失败 */
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

	/* 合成完整的路径名并裁减为合法写法 */
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
	/* 遍历并删除目录所属的内容 */
	while( 1 )
	{
		/* 读取当前内容 */
		readdir64_r( dirobj, &entry, &direntp);
		if( direntp == NULL )
			break;
		/* .表示当前目录 */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == 0)
			continue;
		/* .表示父目录 */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == '.' && direntp->d_name[2] == 0)
			continue;
		/* 文件或目录名太长，无法处理 */
		if( sizeof(path) < len + sizeof(direntp->d_name) + 3 )
			continue;
		/* 合成完整的路径名 */
		strcpy( path+len, direntp->d_name );

		/* 取得类型 */
		if( stat64( path, &satabuf ) != 0 )
		   continue;
		/* 目录 */
		if(S_ISDIR(satabuf.st_mode))
		{
			/* 递归删除目录里面的内容 */
			osl_dir_delete( path, "" );
		}
		/* 文件 */
		else if(S_ISREG(satabuf.st_mode))
		{
			remove( path );
		}
		/* 连接 */
		else if(S_ISLNK(satabuf.st_mode))
		{
			unlink( path );
		}
	}

	/* 删除目录自身 */
	path[len] = 0;
	rmdir( path );
	
	/* 关闭目录 */
	closedir(dirobj);
	return 0;
}

/* 目录是否存在: 返回0表示成功,-1表示失败 */
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

	/* 合成完整的路径名并裁减为合法写法 */
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


/* 打开一个路径 */
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

	/* 合成完整的路径名并裁减为合法写法 */
	if( root && *root && dir && *dir )
		sprintf( tmp, "%s/%s", root, dir );
	else if( root && *root )
		sprintf( tmp, "%s", root );
	else if( dir && *dir )
		sprintf( tmp, "%s", dir );
	else
		return NULL;
	osl_str_trim_path( tmp );

	/* 打开目录 */
	dirobj = opendir( tmp );
	if( dirobj == NULL )
		return NULL;

	/* 申请内存保存过滤信息 */
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

/* 关闭一个路径 */
int32_t osl_dir_close( void* handle )
{
	SFindInfo *info = (SFindInfo *)handle;

	/* 关闭目录 */
	if( info->dirobj )
		closedir( info->dirobj );
	free( info );
	return 0;
}


/* 取得目录下第一个文件或子目录: 返回file含dir并以'/'结束, ... */
int32_t osl_dir_get_first( void* handle, SDirNodeInfo *node, char *file, int32_t size )
{
	rewinddir( ((SFindInfo *)handle)->dirobj );
	return osl_dir_get_next( handle, node, file, size );
}


/* 取得目录下下一个文件或子目录: 返回file含dir并以'/'结束, ... */
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
		/* 读当前目录内容 */
		readdir64_r( info->dirobj, &entry, &direntp );
		if( direntp == NULL )
			break;

		/* .表示当前目录 */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == 0)
			continue;
		/* .表示父目录 */
		if(direntp->d_name[0] == '.' && direntp->d_name[1] == '.' && direntp->d_name[2] == 0)
			continue;
		/* 如果目录或文件名过长，不能返回 */
		if( size < strlen(info->dir) + strlen(direntp->d_name) + 3 )
			continue;
		
		/* 如果目录或文件名过长，无法处理 */
		len = strlen( info->root ) + strlen( info->dir ) + strlen( direntp->d_name );
		if( sizeof(tmp) < len + 3 )
			continue;
		/* 合成完整的路径名并裁减为合法写法 */
		if( info->root[0] && info->dir[0] )
			sprintf( tmp, "%s/%s/%s", info->root, info->dir, direntp->d_name  );
		else if( info->root[0] )
			sprintf( tmp, "%s/%s", info->root, direntp->d_name );
		else if( info->dir[0] )
			sprintf( tmp, "%s/%s", info->dir, direntp->d_name );
		else
			sprintf( tmp, "%s", direntp->d_name );
		osl_str_trim_path( tmp );

		/* 检查类型 */
		if( stat64( tmp, &satabuf ) != 0 )
			continue;

		/*如果不是目录则启用过滤条件*/
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
				/* 是否匹配过滤条件 */
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
		/* 目录 */
		if(S_ISDIR(satabuf.st_mode))
		{
			/* 目录以'/'结束 */
			if( info->dir[0])
				sprintf( file, "%s/%s/", info->dir, direntp->d_name );
			else
				sprintf( file, "%s/", direntp->d_name );
			if( node )
				node->type = DFTYPE_DIR;
		}
		/* 文件 */
		else if(S_ISREG(satabuf.st_mode))
		{
			if( info->dir[0])
				sprintf( file, "%s/%s", info->dir, direntp->d_name );
			else
				sprintf( file, "%s", direntp->d_name );
			if( node )
				node->type = DFTYPE_FILE;
		}
		/* 连接 */
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

/* 取得磁盘空间信息(MB) */
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

	/* 合成完整的路径名并裁减为合法写法 */
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
