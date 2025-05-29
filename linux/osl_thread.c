#include "stdafx.h"
#include "osl.h"
#include "osl_thread.h"
#include <time.h>
#include <errno.h>

typedef struct
{
	/* �߳����� */
	char name[32];
	/* �߳�ID */
	pthread_t tid;
	/* �߳����Զ��� */
	pthread_attr_t attr;
	/* �̵߳����ȼ� */
	int32_t priority;
	/* �����źŵ� */
	sem_t *ctlsem;
	/* �˳��źŵ� */
	sem_t *endsem;

	/* �̻߳ص����� */
	PThreadProc proc;
	/* �ص��������� */
	//uint32_t param;
	//uint32_t expend;

    void *param;
    void *expend;
	/* �Ƿ���ͣ */
	int32_t pause;
	/* ��ʾ��ǰ�߳��Ƿ��˳� */
	int32_t quit;
	/* �Ƿ����û�ִ�к�����û�з��� */
	int32_t busy;

}thread_info_t;

static void* osl_thread_proc( void* lpParam );

/* �����̣߳��ɹ�������ͣ״̬��priority=0(���), priority=255(���) */
void* osl_thread_create( char *name, uchar_t priority, int32_t stack_size,
					   PThreadProc proc, void * param, void* expend )
{
	thread_info_t *info = NULL;
	int size;

	size = sizeof(thread_info_t) + sizeof(sem_t) * 2;
	info = (thread_info_t*)malloc( size );
	if( info == NULL )
		return NULL;

	memset( info, 0, size );
	info->proc = proc;
	info->param = param;
	info->expend = expend;
	info->priority = priority;
	info->pause = 1;
	strncpy( info->name, name, sizeof( info->name )-1 );
	info->ctlsem = (sem_t *)(info + 1);
	info->endsem = info->ctlsem + 1;

	/* �����̵߳����ȼ� */
	pthread_attr_init( &info->attr );
	/* Modify for work successful */
#if 0	
	if( priority > 0 )
	{
		pthread_attr_getschedpolicy( &info->attr, &policy );
		policy = SCHED_RR;
		if( pthread_attr_setschedpolicy( &info->attr, policy ) != 0 )
		{
			printf( "osl_thread_open: setschedpolicy %d.", errno );
			pthread_attr_destroy( &info->attr );
			goto THREAD_FAILED;
		}
		else
		{
			pthread_attr_getschedparam( &info->attr, &sparam );
			sparam.sched_priority = priority;
			if( pthread_attr_setschedparam( &info->attr, &sparam ) != 0 )
			{
				printf( "osl_thread_open: setschedparam %d.\n", errno );
				pthread_attr_destroy( &info->attr );
				goto THREAD_FAILED;
			}
		}	
	}
	else
	{
		pthread_attr_setscope(&info->attr, PTHREAD_SCOPE_SYSTEM);
	}
#endif


	/* �����źŵ� */
	if( sem_init( info->ctlsem, 0, 0 ) == -1 )
	{
		pthread_attr_destroy( &info->attr );
		goto THREAD_FAILED;
	}
	if( sem_init( info->endsem, 0, 0 ) == -1 )
	{
		sem_destroy( info->ctlsem );
		pthread_attr_destroy( &info->attr );
		goto THREAD_FAILED;
	}

	/* ��ʼ�����߳� */
	if( pthread_create( &info->tid, &info->attr, osl_thread_proc, (void*)info ) != 0 )
	{
		sem_destroy( info->endsem );
		sem_destroy( info->ctlsem );
		pthread_attr_destroy( &info->attr );
		goto THREAD_FAILED;
	}

	return info;

THREAD_FAILED:

	free( info );

	return NULL;
}

/* �����߳�(ֻ����ͼ������ǿ�������߳�)������0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_thread_try_destroy( void* thread, int32_t timeout )
{
	thread_info_t *info = (thread_info_t *)thread;
	struct timespec ts;
	int32_t ret;

	/* ֪ͨ�߳̽��� */
	info->quit = 1;
	if( info->ctlsem )
		sem_post( info->ctlsem );

	/* �ȴ��߳̽���*/
	if( info->tid != 0 && info->endsem )
	{
		if( timeout < 0 )
			sem_wait( info->endsem );
		else if( 0 < timeout )
		{
			ts.tv_sec = time(NULL) + timeout/1000;
			ts.tv_nsec = (timeout%1000)*1000;
			sem_timedwait( info->endsem, (const struct timespec *)&ts );
		}
	}

	ret = info->tid == 0 ? 0 : -1;

	if( info->endsem )
	{
		sem_destroy( info->endsem );
		info->endsem = NULL;
	}
	if( info->ctlsem )
	{
		sem_destroy( info->ctlsem );
		info->ctlsem = NULL;
	}

	free( info );

	return ret;
}

/* �����߳�(�����ͼʧ�����ǿ������)������0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_thread_destroy( void* thread, int32_t timeout )
{
	thread_info_t *info = (thread_info_t *)thread;
	struct timespec ts;
	void *result;
	int32_t ret;

	/* ֪ͨ�߳̽��� */
	info->quit = 1;
	if( info->ctlsem )
		sem_post( info->ctlsem );

	/* �ȴ��߳̽���*/
	if( info->tid != 0 && info->endsem )
	{
		if( timeout < 0 )
			sem_wait( info->endsem );
		else if( 0 < timeout )
		{
			ts.tv_sec = time(NULL) + timeout/1000;
			ts.tv_nsec = (timeout%1000)*1000;
			sem_timedwait( info->endsem, (const struct timespec *)&ts );
		}
	}

	if( info->tid != 0 )
	{
		/* ǿ���˳� */
		pthread_kill( info->tid, -1 );
		/* �ȴ����� */
		pthread_join( info->tid, &result );
	}

	ret = info->tid == 0 ? 0 : -1;

	if( info->endsem )
	{
		sem_destroy( info->endsem );
		info->endsem = NULL;
	}
	if( info->ctlsem )
	{
		sem_destroy( info->ctlsem );
		info->ctlsem = NULL;
	}

	free( info );

	return ret;
}


/* �޸��߳����� */
void osl_thread_set_name( void* thread, char* name )
{
	thread_info_t *info = (thread_info_t*)thread;
	strncpy( info->name, name, sizeof( info->name )-1 );
}

/* ��ȡ�߳����� */
void osl_thread_get_name( void* thread, char* buf, int32_t size )
{
	thread_info_t *info = (thread_info_t*)thread;

	strncpy( buf, info->name, size-1 );
	buf[size-1] = 0;
}


/* �����߳����ȼ�  */
void osl_thread_set_priority( void* thread, uchar_t priority )
{
	thread_info_t *info = (thread_info_t*)thread;
	struct sched_param sparam;

	if( priority > 0 )
	{
		priority = sched_get_priority_max(SCHED_RR) - (priority * sched_get_priority_max(SCHED_RR)) / 255;
		sparam.__sched_priority = priority;
	}
	pthread_setschedparam(info->tid, SCHED_RR, &sparam );
}

/* ���ûص����� */
void osl_thread_set_proc( void* thread, PThreadProc proc, void*param, void* expend )
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
	if( expend )
		*expend = info->expend;
}

/* ��ͣ */
void osl_thread_pause( void* thread )
{
	((thread_info_t *)thread)->pause = 1;
}

void osl_thread_start(void *thread)
{
	((thread_info_t *)thread)->pause = 0;
}
/* ���� */
void osl_thread_resume( void* thread )
{
	thread_info_t *info = (thread_info_t *)thread;

	info->pause = 0;
	if( info->ctlsem )
		sem_post( info->ctlsem );
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
	return ((thread_info_t *)thread)->busy;
}

/* �߳�ִ�к��� */
static void* osl_thread_proc( void* lpParam )
{
	thread_info_t *info = (thread_info_t *)lpParam;
	int32_t delay;
	struct timespec interval, remainder;

	//prctl(PR_SET_NAME, (unsigned long)info->name);

	while( info->ctlsem )
	{
		//ȡ�ô����¼������
		if( info->pause || info->proc == NULL )
			sem_wait( info->ctlsem );

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
			//sched_yield();
			//usleep(delay*1000);
           interval.tv_sec = 0;
           interval.tv_nsec = delay*1000000;
           if( nanosleep(&interval, &remainder) == -1 )
		   {
			   if (errno == EINTR)//���߱��ж�
			   {
				   printf( "sleep EINTR\n" );
               }
           }
		}
	}
	info->quit = 1;
	info->proc = NULL;
	info->tid = 0;

	/* ֪ͨ�߳̽��� */
	if( info->endsem )
		sem_post( info->endsem );

	return 0;
}


/**���̰߳󶨵�ĳ��cpu��,ʧ�ܷ���-1���ɹ�����0*/
int32_t osl_thread_bind_cpu( void* thread, int32_t cpu )
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu,&mask);
	if ( pthread_setaffinity_np(((thread_info_t *)(thread))->tid, sizeof(mask),&mask) < 0)
	{
		return -1;
	}

	return 0;
}

