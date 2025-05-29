#ifndef __OSL_QUEUE_H__
#define __OSL_QUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* 初始化系统消息队列 */
void* osl_queue_create( int32_t msg_size, int32_t msg_num );

/* 释放队列资源 */
void osl_queue_destroy( void* queue );

/* 向系统消息队列添加消息,0=sucess,-1=failed */
int32_t osl_queue_post( void* queue, uint32_t type, void *buf, int32_t size, uint32_t param );

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_queue_read( void* queue, uint32_t *type, void *buf, int32_t size, uint32_t *param );

/* 清除消息队列中所有的消息 */
void osl_queue_clear( void* queue );

/* 取得系统消息队列消息数目 */
int32_t osl_queue_get_num( void* queue );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_QUEUE_H__ */
