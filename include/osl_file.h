#ifndef __OSL_FILE_H__
#define __OSL_FILE_H__

#define OSL_FILE_READ_ONLY		1
#define OSL_FILE_WRITE_ONLY		2
#define OSL_FILE_READ_WRITE		4

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	uint32_t modify_utc;
	int64_t size;
}SFileNodeInfo;

/* �������ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_rename( const char *srcfile, const char *dstfile );

/* �����ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_copy( const char *srcfile, const char *dstfile );

/* ɾ���ļ�: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_delete( const char *file );

/* �ļ��Ƿ����: ����-1=�����ڣ�0=���� */
int32_t osl_file_exist( const char *file );

/* ȡ���ļ��ߴ��������ʱ��: size=bytes, time=utc-ms, ����0��ʾ�ɹ�,-1��ʾʧ��*/
int32_t osl_file_get_info( const char *file, SFileNodeInfo *node );

/* ���������ļ����һ���޸�ʱ��, ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_file_set_modify_utc( const char* file, uint32_t modify_utc );


/* ���ļ�, mode=OSL_FILE_READ_ONLY */
void* osl_file_open( const char *file, int32_t mode );

/* �ر��ļ� */
void osl_file_close( void *fp );

/* �޸��ļ�Ȩ�� */
void osl_file_chmod(const char *file, int32_t mode);

/* ��λ�ļ�λ��: base=SEEK_SET, ... */
void osl_file_seek( void *fp, int64_t pos, int32_t base );

/* ȡ���ļ�λ�� */
int64_t osl_file_tell( void *fp );

/* ���ļ�: ����0��ʾ�ļ�����,-1��ʾʧ��,>0��ʾ��ȡ�ĳߴ� */
int32_t osl_file_read( void *fp, void *buf, int32_t size );

/* д�ļ� */
int32_t osl_file_write( void *fp, void *buf, int32_t size );

/* �ļ��Ƿ����: ����1��ʾ����,0��ʾδ���� */
int32_t osl_file_eof( void *fp );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_FILE_H__ */
