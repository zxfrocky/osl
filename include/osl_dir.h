#ifndef __OSL_DIR_H__
#define __OSL_DIR_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	DFTYPE_NULL,	//无效
	DFTYPE_FILE,	//表示文件
	DFTYPE_DIR,		//表示目录
	DFTYPE_LINK,	//连接
}EDirNodeType;

typedef struct
{
	EDirNodeType type;
	uint32_t modify_utc;
	int64_t size;
}SDirNodeInfo;

typedef enum
{
	OSL_DIR_TOTAL_SIZE,///文件系统的大小
	OSL_DIR_FREE_SIZE, ///自由空间
	OSL_DIR_USED_SIZE, ///已用空间
	OSL_DIR_AVAIL_SIZE ///用户实际可以使用的空间
}EOslDirSpace;

/* 创建目录: 返回0表示成功,-1表示失败 */
int32_t osl_dir_create( const char *root, const char *dir );

/* 删除目录及旗下所有子目录和文件: 返回0表示成功,-1表示失败 */
int32_t osl_dir_delete( const char *root, const char *dir );

/* 目录是否存在: 返回-1=不存在，0=存在 */
int32_t osl_dir_exist( const char *root, const char *dir );

/* 打开一个路径 */
void *osl_dir_open( const char *root, const char *dir, const char *filter );

/* 关闭一个路径 */
int32_t osl_dir_close( void* handle );

/* 取得目录下第一个文件或子目录: 返回file含dir并以'/'结束, ... */
int32_t osl_dir_get_first( void* handle, SDirNodeInfo *node, char *file, int32_t size );

/* 取得目录下下一个文件或子目录: 返回file含dir并以'/'结束, ... */
int32_t osl_dir_get_next( void* handle, SDirNodeInfo *node, char *file, int32_t size );

/* 取得磁盘空间信息(MB) */
uint32_t osl_dir_get_space_info( const char *root, const char *dir, EOslDirSpace type);

/* 获取目录信息 返回-1=失败，0=成功*/
int32_t osl_dir_get_info(const char *root, const char *dir,SDirNodeInfo *node);

#ifdef __cplusplus
}
#endif


#endif /* __OSL_DIR_H__ */
