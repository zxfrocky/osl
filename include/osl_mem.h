#ifndef __OSL_MEM_H__
#define __OSL_MEM_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* 内存泄漏检查回调函数 */
typedef void (*PMemCheckCallback)( char *buf, int64_t size, const char *file, int32_t line, void* param );


/* 创建内存管理模块 */
void* osl_mem_create( char *buf, int64_t size, int32_t align );
/* 销毁内存管理模块 */
void osl_mem_destroy( void* h );
/* 清空 */
void osl_mem_reset( void* h );
/* 检查内存节点，返回历史上曾经消耗过的最大尺寸 */
int64_t osl_mem_check( void* h, PMemCheckCallback pProc, void* param );

/* 分配内存 */
void *osl_mem_alloc( void* h, int64_t size, const char *file, int32_t line, int64_t *memsize );
/* 释放内存 */
int64_t osl_mem_free( void* h, void *p, const char *file, int32_t line );
/* 复制字符串 */
char *osl_mem_strdup( void* h, const char *s, const char *file, int32_t line );
/* 重新申请内存 */
void *osl_mem_realloc( void* h, void *p, int64_t size, const char *file, int32_t line );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_MEM_H__ */
