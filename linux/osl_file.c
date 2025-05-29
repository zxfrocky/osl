#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <utime.h>
#include "stdafx.h"
#include "osl_file.h"

#define FILE_FD_OFFSET	1

/* 重命名文件: 返回0表示成功,-1表示失败 */
int32_t osl_file_rename( const char *srcfile, const char *dstfile )
{
	return rename( srcfile, dstfile ) == 0 ? 0 : -1;
}

/* 复制文件: 返回0表示成功,-1表示失败 */
int32_t osl_file_copy( const char *srcfile, const char *dstfile )
{
	int32_t len;
	int32_t fd1,fd2;
	char buffer[4096];
	fd1 = open(srcfile, O_RDONLY/*|O_LARGEFILE*/);
	fd2 = open(dstfile, O_CREAT/*|O_LARGEFILE*/|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if(fd1 == -1 || fd2 == -1){
		if(fd1 != -1)
			close(fd1);
		if(fd2 != -1)
			close(fd2);
		return -1;
	}

	for(;;){
		len = read(fd1 , buffer, sizeof(buffer));
		if(len > 0)
			write(fd2 , buffer, len);
		else
			break;

	}
	close(fd1);
	close(fd2);
	return 0;
}

/* 删除文件: 返回0表示成功,-1表示失败 */
int32_t osl_file_delete( const char *file )
{
	return (unlink( file ) == 0 ? 0 : -1);
}

/* 文件是否存在: 返回-1=不存在，0=存在 */
int32_t osl_file_exist( const char *file )
{
	struct stat64 satabuf;

	if(stat64(file,&satabuf) != 0 || !(S_ISREG(satabuf.st_mode))){
	   return -1;
	}
	return 0;
}

/* 取得文件尺寸和最后更新时间: size=bytes, time=utc-ms, 返回0表示成功,-1表示失败*/
int32_t osl_file_get_info( const char *file, SFileNodeInfo *node )
{
	struct stat64 satabuf;

	if(lstat64(file,&satabuf) != 0){
		return -1;
	}
	if( node )
	{
		node->size = satabuf.st_size;
		node->modify_utc = (uint32_t)satabuf.st_mtime;
	}
	return 0;
}
/* 重新设置文件最后一次修改时间, 返回0表示成功,-1表示失败 */
int32_t osl_file_set_modify_utc( const char* file, uint32_t modify_utc )
{
	struct utimbuf tbuf;
	
	tbuf.modtime = (time_t)modify_utc;
	return utime(file , &tbuf);
}

/* 打开文件, mode=OSL_FILE_READ_ONLY */
void* osl_file_open( const char *file, int32_t mode )
{
	int64_t fd = 0;

	switch( mode )
	{
	case OSL_FILE_READ_ONLY:
		fd = open(file, /*O_LARGEFILE|*/O_RDONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		break;
	case OSL_FILE_WRITE_ONLY:
		fd = open(file, O_CREAT|/*O_LARGEFILE|*/O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		break;
	case OSL_FILE_READ_WRITE:
		fd = open(file, /*O_LARGEFILE|*/O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		break;
	}
	fd += FILE_FD_OFFSET;
	return fd <= 0 ? NULL : (void*)fd;
}

/* 关闭文件 */
void osl_file_close( void *fp )
{
	close( ((int64_t)fp-FILE_FD_OFFSET));
}

/* 修改文件权限 */
void osl_file_chmod(const char *file, int32_t mode)
{
	switch( mode )
	{
	case OSL_FILE_READ_ONLY:
		chmod(file, S_IRUSR|S_IRGRP|S_IROTH);
		break;
	case OSL_FILE_WRITE_ONLY:
		chmod(file, S_IRWXU|S_IRWXG|S_IROTH);
		break;
	case OSL_FILE_READ_WRITE:
		chmod(file, S_IRWXU|S_IRWXG|S_IROTH);
		break;
	}
}

/* 定位文件位置: base=SEEK_SET, ... */
void osl_file_seek( void *fp, int64_t pos, int32_t base )
{
	lseek64(((int64_t)fp-FILE_FD_OFFSET), pos, (int32_t) base);
}

/* 取得文件位置 */
int64_t osl_file_tell( void *fp )
{
	return lseek64(((int64_t)fp-FILE_FD_OFFSET), 0, SEEK_CUR);
}

/* 读文件: 返回0表示文件结束,-1表示失败,>0表示读取的尺寸 */
int32_t osl_file_read( void *fp, void *buf, int32_t size )
{
	return read(((int64_t)fp-FILE_FD_OFFSET), buf, size);
}

/* 写文件 */
int32_t osl_file_write( void *fp, void *buf, int32_t size )
{
	return write(((int64_t)fp-FILE_FD_OFFSET), buf, size);
}

/* 文件是否结束: 返回1表示结束,0表示未结束 */
int32_t osl_file_eof( void *fp )
{
	struct stat64 satabuf;
	int64_t pos = lseek64(((int64_t)fp-FILE_FD_OFFSET), 0, SEEK_CUR);
	if(fstat64(((int64_t)fp-FILE_FD_OFFSET), &satabuf) != 0)
		return -1;
	return pos == satabuf.st_size ? 1 : 0;
}



