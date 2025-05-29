#ifndef __OSL_QUEUE_H__
#define __OSL_QUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* ��ʼ��ϵͳ��Ϣ���� */
void* osl_queue_create( int32_t msg_size, int32_t msg_num );

/* �ͷŶ�����Դ */
void osl_queue_destroy( void* queue );

/* ��ϵͳ��Ϣ���������Ϣ,0=sucess,-1=failed */
int32_t osl_queue_post( void* queue, uint32_t type, void *buf, int32_t size, uint32_t param );

/* ��ϵͳ��Ϣ������ȡ��Ϣ,0=sucess,-1=failed */
int32_t osl_queue_read( void* queue, uint32_t *type, void *buf, int32_t size, uint32_t *param );

/* �����Ϣ���������е���Ϣ */
void osl_queue_clear( void* queue );

/* ȡ��ϵͳ��Ϣ������Ϣ��Ŀ */
int32_t osl_queue_get_num( void* queue );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_QUEUE_H__ */
