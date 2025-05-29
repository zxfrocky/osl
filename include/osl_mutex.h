#ifndef __OSL_MUTEX_H__
#define __OSL_MUTEX_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* ���������� */
void* osl_mutex_create();

/* ���ٻ����� */
void osl_mutex_destroy( void* mutex );

/* ����, timeout(ms)=-1��ʾ���޵ȴ�������0��ʾ�ɹ�������-1��ʾʧ�� */
int32_t osl_mutex_lock( void* mutex, int32_t timeout );

/* ���� */
void osl_mutex_unlock( void* mutex );


#ifdef __cplusplus
}
#endif

#endif  /* __OSL_MUTEX_H__ */
