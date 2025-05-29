#include "stdafx.h"
#include "osl.h"
#include "osl_thread.h"
#include "osl_string.h"


typedef struct
{
	//任务名称
	char name[32];

	//线程的ID号
	uint32_t tid;
	//线程句柄
	void* thrd;
	//驱动事件
	void* event;

	//是否暂停
	int32_t pause;
	//是否退出
	int32_t quit;
	/* 是否在用户执行函数中没有返回 */
	int32_t busy;

	//线程回调函数
	PThreadProc proc;
	//回调函数参数
	void* param;
	void* expend;

}thread_info_t;


static uint32_t osl_thread_proc( LPVOID lpParam );

/* 创建线程，成功后处于暂停状态，priority=0(最高), priority=255(最低) */
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

	//创建事件处理线程
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

/* 销毁线程(只是试图而不是强制销毁线程)，返回0表示成功,-1表示失败 */
int32_t osl_thread_try_destroy( void* thread, int32_t timeout )
{
	thread_info_t *info = (thread_info_t*)thread;
	int32_t val;

	//关闭线程
	if( info->thrd != INVALID_HANDLE_VALUE )
	{
		//通知线程结束
		info->tid = 0;
		info->pause = 0;
		info->quit = 1;
		ReleaseSemaphore( info->event, 1, &val );

		//等待线程结束
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

/* 销毁线程(可能会强制销毁)，返回0表示成功,-1表示失败 */
int32_t osl_thread_destroy( void* thread, int32_t timeout )
{
	thread_info_t *info = (thread_info_t*)thread;
	int32_t val;

	//关闭线程
	if( info->thrd != INVALID_HANDLE_VALUE )
	{
		//通知线程结束
		info->tid = 0;
		info->pause = 0;
		info->quit = 1;
		ReleaseSemaphore( info->event, 1, &val );

		//等待线程结束, 如果超时, 则强行关闭
		if( WaitForSingleObject( info->thrd, timeout ) != WAIT_OBJECT_0 )
			TerminateThread( info->thrd, (uint32_t)-1 );

		//释放线程句柄
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


/* 修改线程名称 */
void osl_thread_set_name( void* thread, char* name )
{
	thread_info_t *info = (thread_info_t*)thread;
	osl_strncpy( info->name, name, sizeof( info->name )-1 );
}

/* 获取线程名称 */
void osl_thread_get_name( void* thread, char* buf, int32_t size )
{
	thread_info_t *info = (thread_info_t*)thread;

	osl_strncpy( buf, info->name, size-1 );
	buf[size-1] = 0;
}

/* 设置线程优先级  */
void osl_thread_set_priority( void* thread, uchar_t priority )
{
	thread_info_t *info = (thread_info_t*)thread;

	if( info->thrd != INVALID_HANDLE_VALUE )
		SetThreadPriority( info->thrd, priority );
}

/* 设置回调函数 */
void osl_thread_set_proc( void* thread, PThreadProc proc, void* param, void* expend )
{
	thread_info_t *info = (thread_info_t*)thread;

	info->proc = proc;
	info->param = param;
	info->expend = expend;
}

/* 取得线程执行函数 */
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
/* 暂停 */
void osl_thread_pause( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;

	if( info->event )
	{
		WaitForSingleObject( info->event, 0 );
	}
	info->pause = 1;
}

/* 继续 */
void osl_thread_resume( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;
	int32_t val;

	info->pause = 0;
	if( info->event )
		ReleaseSemaphore( info->event, 1, &val );
}

/* 是否正在运行 */
int32_t osl_thread_is_running( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;

	return info->pause == 0 && info->quit == 0;
}

/* 是否在用户执行函数中没有返回 */
int32_t osl_thread_is_busy( void* thread )
{
	thread_info_t *info = (thread_info_t*)thread;

	return info->busy;
}

/* 线程执行函数 */
static uint32_t osl_thread_proc( LPVOID lpParam )
{
	thread_info_t *info = (thread_info_t *)lpParam;
	int32_t delay;

	while( info->tid != 0 && info->quit == 0 )
	{
		/* 如果处于暂停状态，则阻塞在此处 */
		if( info->pause || info->proc == NULL )
		{
			WaitForSingleObject( info->event, 0 );
			WaitForSingleObject( info->event, INFINITE );
		}
		if( info->quit )
			break;

		/* 执行用户函数，返回值<0表示退出，>0表示延时(ms) */
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
			/* WIN32的select()占用CPU资源太多 */
			WaitForSingleObject( info->event, delay );
		}
	}
	info->quit = 1;
	info->proc = NULL;
	info->tid = 0;

	return 1;
}

/**将线程绑定到某个cpu上,失败返回-1，成功返回0*/
int32_t osl_thread_bind_cpu( void* thread, int32_t cpu )
{
	thread_info_t *info = (thread_info_t*)thread;
	DWORD_PTR mask = cpu;

	SetThreadAffinityMask( info->thrd, mask );
	return 0;
}

