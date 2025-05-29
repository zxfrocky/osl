#ifndef __OSL_FIFO_H__
#define __OSL_FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* ��ʼ��ϵͳ��Ϣ���� */
void* osl_fifo_create( int32_t msg_size, int32_t msg_num );

/* �ͷŶ�����Դ */
void osl_fifo_destroy( void* fifo );

/* ��ϵͳ��Ϣ���������Ϣ,0=sucess,-1=failed */
int32_t osl_fifo_post( void* fifo, uint32_t type, void *buf, int32_t size, uint32_t param );

/* ��ϵͳ��Ϣ������ȡ��Ϣ,0=sucess,-1=failed */
int32_t osl_fifo_read( void* fifo, uint32_t *type, void *buf, int32_t size, uint32_t *param );

/* ��ϵͳ��Ϣ������ȡ��Ϣ,0=sucess,-1=failed */
int32_t osl_fifo_read_timeout( void* fifo, uint32_t *type, void *buf, int32_t size, uint32_t *param, int32_t timeout );

/* ��ϵͳ��Ϣ������ȡ��Ϣ,0=sucess,-1=failed */
int32_t osl_fifo_get_all( void* fifo, uint32_t *type, void **buf, int32_t size, int32_t num, uint32_t *param);

/* ��ϵͳ��Ϣ������ȡ��Ϣ,0=sucess,-1=failed */
int32_t osl_fifo_get_all_timeout( void* fifo, uint32_t *type, void **buf, int32_t size, int32_t num, uint32_t *param, int32_t timeout );

/* �����Ϣ���������е���Ϣ */
void osl_fifo_clear( void* fifo );

/* ȡ��ϵͳ��Ϣ������Ϣ��Ŀ */
int32_t osl_fifo_get_num( void* fifo );

/* ȡ��ϵͳ��Ϣ���д�С */
int32_t osl_fifo_get_size( void* fifo );

#ifdef __cplusplus
}
#endif

#endif /* __OSL_FIFO_H__ */
