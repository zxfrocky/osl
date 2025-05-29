#ifndef __OSL_DIR_H__
#define __OSL_DIR_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	DFTYPE_NULL,	//��Ч
	DFTYPE_FILE,	//��ʾ�ļ�
	DFTYPE_DIR,		//��ʾĿ¼
	DFTYPE_LINK,	//����
}EDirNodeType;

typedef struct
{
	EDirNodeType type;
	uint32_t modify_utc;
	int64_t size;
}SDirNodeInfo;

typedef enum
{
	OSL_DIR_TOTAL_SIZE,///�ļ�ϵͳ�Ĵ�С
	OSL_DIR_FREE_SIZE, ///���ɿռ�
	OSL_DIR_USED_SIZE, ///���ÿռ�
	OSL_DIR_AVAIL_SIZE ///�û�ʵ�ʿ���ʹ�õĿռ�
}EOslDirSpace;

/* ����Ŀ¼: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_dir_create( const char *root, const char *dir );

/* ɾ��Ŀ¼������������Ŀ¼���ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_dir_delete( const char *root, const char *dir );

/* Ŀ¼�Ƿ����: ����-1=�����ڣ�0=���� */
int32_t osl_dir_exist( const char *root, const char *dir );

/* ��һ��·�� */
void *osl_dir_open( const char *root, const char *dir, const char *filter );

/* �ر�һ��·�� */
int32_t osl_dir_close( void* handle );

/* ȡ��Ŀ¼�µ�һ���ļ�����Ŀ¼: ����file��dir����'/'����, ... */
int32_t osl_dir_get_first( void* handle, SDirNodeInfo *node, char *file, int32_t size );

/* ȡ��Ŀ¼����һ���ļ�����Ŀ¼: ����file��dir����'/'����, ... */
int32_t osl_dir_get_next( void* handle, SDirNodeInfo *node, char *file, int32_t size );

/* ȡ�ô��̿ռ���Ϣ(MB) */
uint32_t osl_dir_get_space_info( const char *root, const char *dir, EOslDirSpace type);

/* ��ȡĿ¼��Ϣ ����-1=ʧ�ܣ�0=�ɹ�*/
int32_t osl_dir_get_info(const char *root, const char *dir,SDirNodeInfo *node);

#ifdef __cplusplus
}
#endif


#endif /* __OSL_DIR_H__ */
