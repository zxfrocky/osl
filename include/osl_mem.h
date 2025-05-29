#ifndef __OSL_MEM_H__
#define __OSL_MEM_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* �ڴ�й©���ص����� */
typedef void (*PMemCheckCallback)( char *buf, int64_t size, const char *file, int32_t line, void* param );


/* �����ڴ����ģ�� */
void* osl_mem_create( char *buf, int64_t size, int32_t align );
/* �����ڴ����ģ�� */
void osl_mem_destroy( void* h );
/* ��� */
void osl_mem_reset( void* h );
/* ����ڴ�ڵ㣬������ʷ���������Ĺ������ߴ� */
int64_t osl_mem_check( void* h, PMemCheckCallback pProc, void* param );

/* �����ڴ� */
void *osl_mem_alloc( void* h, int64_t size, const char *file, int32_t line, int64_t *memsize );
/* �ͷ��ڴ� */
int64_t osl_mem_free( void* h, void *p, const char *file, int32_t line );
/* �����ַ��� */
char *osl_mem_strdup( void* h, const char *s, const char *file, int32_t line );
/* ���������ڴ� */
void *osl_mem_realloc( void* h, void *p, int64_t size, const char *file, int32_t line );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_MEM_H__ */
