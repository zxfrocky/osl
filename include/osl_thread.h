#ifndef _OSL_THREAD_H__
#define _OSL_THREAD_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* �Ƽ��̶߳�ջ��С(bytes) */
#define THREAD_DEFAULT_STACK_SIZE	65536
/* �Ƽ��߳����ȼ� */
#define THREAD_DEFAULT_PRIORIOTY	100


/* �̻߳ص��������ᱻ��������,����<0��ʾ�˳��̣߳�>0��ʾ��ʱʱ��(ms) */
typedef int32_t (*PThreadProc)( void* param, void* expend );

/* �����̣߳��ɹ�������ͣ״̬��priority=0(���), priority=255(���) */
void* osl_thread_create( char *name, uchar_t priority, int32_t stack_size,
					   PThreadProc proc, void* param, void* expend );

/* �����߳�(ֻ����ͼ������ǿ�������߳�)������0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_thread_try_destroy( void* thread, int32_t timeout );

/* �����߳�(�����ͼʧ�����ǿ������)������0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_thread_destroy( void* thread, int32_t timeout );

/* �޸��߳����� */
void osl_thread_set_name( void* thread, char* name );

/* ��ȡ�߳����� */
void osl_thread_get_name( void* thread, char* buf, int32_t size );

/* �����߳����ȼ���priority�ķ�ΧΪ[0,255]��0��ʾ���ȼ���ߣ�255��� */
void osl_thread_set_priority( void* thread, uchar_t priority );

/* ȡ���߳����ȼ���priority�ķ�ΧΪ[0,255]��0��ʾ���ȼ���ߣ�255��� */
uchar_t osl_thread_get_priority( void* thread );

/* �޸��߳�ִ�к��� */
void osl_thread_set_proc( void* thread, PThreadProc proc, void* param, void* expend );

/* ȡ���߳�ִ�к��� */
void osl_thread_get_proc( void* thread, PThreadProc *proc, void* *param, void* *expend );

/* ��ͣ */
void osl_thread_pause( void* thread );

/* ���� */
void osl_thread_resume( void* thread );

/* �Ƿ���������:1=��,0=�� */
int32_t osl_thread_is_running( void* thread );

/* �Ƿ����û�ִ�к�����û�з��أ�1=yes,0=no */
int32_t osl_thread_is_busy( void* thread );

void osl_thread_start(void *thread);

/**���̰߳󶨵�ĳ��cpu��,ʧ�ܷ���-1���ɹ�����0*/
int32_t osl_thread_bind_cpu( void* thread, int32_t cpu );

#ifdef __cplusplus
}
#endif

#endif  /* _OSL_THREAD_H__ */
