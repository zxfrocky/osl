#ifndef __OSL_FIFO_H__
#define __OSL_FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* 初始化系统消息队列 */
void* osl_fifo_create( int32_t msg_size, int32_t msg_num );

/* 释放队列资源 */
void osl_fifo_destroy( void* fifo );

/* 向系统消息队列添加消息,0=sucess,-1=failed */
int32_t osl_fifo_post( void* fifo, uint32_t type, void *buf, int32_t size, uint32_t param );

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_read( void* fifo, uint32_t *type, void *buf, int32_t size, uint32_t *param );

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_read_timeout( void* fifo, uint32_t *type, void *buf, int32_t size, uint32_t *param, int32_t timeout );

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_get_all( void* fifo, uint32_t *type, void **buf, int32_t size, int32_t num, uint32_t *param);

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_get_all_timeout( void* fifo, uint32_t *type, void **buf, int32_t size, int32_t num, uint32_t *param, int32_t timeout );

/* 清除消息队列中所有的消息 */
void osl_fifo_clear( void* fifo );

/* 取得系统消息队列消息数目 */
int32_t osl_fifo_get_num( void* fifo );

/* 取得系统消息队列大小 */
int32_t osl_fifo_get_size( void* fifo );

#ifdef __cplusplus
}
#endif

#endif /* __OSL_FIFO_H__ */
