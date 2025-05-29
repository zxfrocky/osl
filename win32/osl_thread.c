#include "stdafx.h"
#include "osl.h"
#include "osl_thread.h"
#include "osl_string.h"


typedef struct
{
	//��������
	char name[32];

	//�̵߳�ID��
	uint32_t tid;
	//�߳̾��
	void* thrd;
	//�����¼�
	void* event;

	//�Ƿ���ͣ
	int32_t pause;
	//�Ƿ��˳�
	int32_t quit;
	/* �Ƿ����û�ִ�к�����û�з��� */
	int32_t busy;

	//�̻߳ص�����
	PThreadProc proc;
	//�ص���������
	void* param;
	void* expend;

}thread_info_t;


static uint32_t osl_thread_proc( LPVOID lpParam );

/* �����̣߳��ɹ�������ͣ״̬��priority=0(���), priority=255(���) */
void* osl_thread_create( char *name, uchar_t priority, int32_t stack_size,
					   PThreadProc proc, void* param, void* expend )
{
	thread_info_t *info = (thread_info_t*)malloc( sizeof(thread_info_t) );
	if( info == NULL )
		return NULL;

	memset( info, 0, sizeof(thread_info_t) );
	info->quit = 0;
	info->pause = 1;
	info->param = param;
	info->expend = expend;
	info->proc = proc;
	info->thrd = INVALID_HANDLE_VALUE;
	osl_strncpy( info->name, name, sizeof( info->name )-1 );

	info->event = CreateSemaphore( NULL, 0, 1, NULL );
	if( info->event == NULL )
		goto THEAD_FAILED;

	//�����¼������߳�
	if( ( info->thrd = CreateThread( NULL, // default security attribute
									stack_size,    // default stack size
									(LPTHREAD_START_ROUTINE)osl_thread_proc,
									(LPVOID)info, // parameter is myself
									0,    // run immediately
									&(info->tid))) == INVALID_HANDLE_VALUE )
	{
		goto THEAD_FAILED;
	}

	SetThreadPriority( info->thrd, priority );

	return (void*)info;

THEAD_FAILED:
	osl_thread_destroy( info, 10 );
	return NULL;
}

/* �����߳�(ֻ����ͼ������ǿ�������߳�)������0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_thread_try_destroy( void* thread, int32_t timeout )
{
	thread_info_t *info = (thread_info_t*)thread;
	int32_t val;

	//�ر��߳�
	if( info->thrd != INVALID_HANDLE_VALUE )
	{
		//֪ͨ�߳̽���
		info->tid = 0;
		info->pause = 0;
		info->quit = 1;
		ReleaseSemaphore( info->event, 1, &val );

		//�ȴ��߳̽���
		if( WaitForSingleObject( info->thrd, timeout ) == WAIT_OBJECT_0 )
		{
			CloseHandle( info->thrd );
			info->thrd = INVALID_HANDLE_VALUE;

			CloseHandle( info->event );
			info->event = NULL;

			free( info );

			return 0;
		}
	}

	return -1;
}

/* �����߳�(���ܻ�ǿ������)������0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_thread_destroy( void* thread, int32_t timeout )
{
	thread_info_t *info = (thread_info_t*)thread;
	int32_t val;

	//�ر��߳�
	if( info->thrd != INVALID_HANDLE_VALUE )
	{
		//֪ͨ�߳̽���
		info->tid = 0;
		info->pause = 0;
		info->quit = 1;
		ReleaseSemaphore( info->event, 1, &val );

		//�ȴ��߳̽���, �����ʱ, ��ǿ�йر�
		if( WaitForSingleObject( info->thrd, timeout ) != WAIT_OBJECT_0 )
			TerminateThread( info->thrd, (uint32_t)-1 );

		//�ͷ��߳̾��
		CloseHandle( info->thrd );
		info->thrd = INVALID_HANDLE_VALUE;
	}

	if( info->event )
	{
		CloseHandle( info->event );
		info->event = NULL;
	}

	free( info );

	return 0;
}


/* �޸��߳����� */
void osl_thread_set_name( void* thread, char* name )
{
	thread_info_t *info = (thread_info_t*)thread;
	osl_strncpy( info->name, name, sizeof( info->name )-1 );
}

/* ��ȡ�߳����� */
void osl_thread_get_name( void* thread, char* buf, int32_t size )
{
	thread_info_t *info = (thread_info_t*)thread;

	osl_strncpy( buf, info->name, size-1 );
	buf[size-1] = 0;
}

/* �����߳����ȼ�  */
void osl_thread_set_priority( void* thread, uchar_t priority )
{
	thread_info_t *info = (thread_info_t*)thread;

	if( info->thrd != INVALID_HANDLE_VALUE )
		SetThreadPriority( info->thrd, priority );
}

/* ���ûص����� */
void osl_thread_set_proc( void* thread, PThreadProc proc, void* param, void* expend )
{
	thread_info_t *info = (thread_info_t*)thread;

	info->proc = proc;
	info->param = param;
	info->expend = expend;
}

/* ȡ���߳�ִ�к��� */
void osl_thread_get_proc( void* thread, PThreadProc *proc, void* *param, void* *expend )
{
	thread_info_t *info = (thread_info_t*)thread;

	if( proc )
		*proc = info->proc;
	if( param )
		*param = info->param;
	if ( expend )
		*expend = info->expend;
}

void osl_thread_start(void* thread)
{
	thread_info_t *info = (thread_info_t*)thread;
	info->pause = 0;
}
/* ��ͣ */
void osl_thread_pause( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;

	if( info->event )
	{
		WaitForSingleObject( info->event, 0 );
	}
	info->pause = 1;
}

/* ���� */
void osl_thread_resume( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;
	int32_t val;

	info->pause = 0;
	if( info->event )
		ReleaseSemaphore( info->event, 1, &val );
}

/* �Ƿ��������� */
int32_t osl_thread_is_running( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;

	return info->pause == 0 && info->quit == 0;
}

/* �Ƿ����û�ִ�к�����û�з��� */
int32_t osl_thread_is_busy( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;

	return info->busy;
}

/* �߳�ִ�к��� */
static uint32_t osl_thread_proc( LPVOID lpParam )
{
	thread_info_t *info = (thread_info_t *)lpParam;
	int32_t delay;

	while( info->tid != 0 && info->quit == 0 )
	{
		/* ���������ͣ״̬���������ڴ˴� */
		if( info->pause || info->proc == NULL )
		{
			WaitForSingleObject( info->event, 0 );
			WaitForSingleObject( info->event, INFINITE );
		}
		if( info->quit )
			break;

		/* ִ���û�����������ֵ<0��ʾ�˳���>0��ʾ��ʱ(ms) */
		if( info->proc )
		{
			info->busy = 1;
			delay = info->proc( info->param, info->expend );
			info->busy = 0;
		}
		else
			delay = 0;

		if( delay < 0 )
			break;
		else if( 0 < delay )
		{
			/* WIN32��select()ռ��CPU��Դ̫�� */
			WaitForSingleObject( info->event, delay );
		}
	}
	info->quit = 1;
	info->proc = NULL;
	info->tid = 0;

	return 1;
}

/**���̰߳󶨵�ĳ��cpu��,ʧ�ܷ���-1���ɹ�����0*/
int32_t osl_thread_bind_cpu( void* thread, int32_t cpu )
{
	thread_info_t *info = (thread_info_t*)thread;
	DWORD_PTR mask = cpu;

	SetThreadAffinityMask( info->thrd, mask );
	return 0;
}

