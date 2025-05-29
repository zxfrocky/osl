#include "stdafx.h"
#include "osl.h"
#include "osl_log.h"
#include "osl_mutex.h"

#ifndef _DEBUG

/* 创建互斥量 */
void* osl_mutex_create()
{
	return CreateMutex( NULL, 0, NULL );
}

/* 销毁互斥量 */
void osl_mutex_destroy( void* mutex )
{
	CloseHandle( mutex );
}

/* 上锁, timeout(ms)-1表示无限等待，返回0表示成功，返回-1表示失败 */
int32_t osl_mutex_lock( void* mutex, int32_t timeout )
{
	if( WaitForSingleObject( mutex, timeout ) == WAIT_OBJECT_0 )
		return 0;
	else
		return -1;
}

/* 解锁 */
void osl_mutex_unlock( void* mutex )
{
	ReleaseMutex( mutex );
}

#else

typedef struct 
{
	void *mutex;
	int32_t count;
}SMutex;

/* 创建互斥量 */
void* osl_mutex_create()
{
	SMutex *m = (SMutex*)malloc( sizeof(SMutex) );
	memset( m, 0, sizeof(SMutex) );
	m->mutex = CreateMutex( NULL, 0, NULL );
	return m;
}

/* 销毁互斥量 */
void osl_mutex_destroy( void* mutex )
{
	SMutex *m = (SMutex*)mutex;
	CloseHandle( m->mutex );
	free( m );
}


/* 上锁, timeout(ms)=-1表示无限等待，返回0表示成功，返回-1表示失败 */
int32_t osl_mutex_lock( void* mutex, int32_t timeout )
{	
	SMutex *m = (SMutex*)mutex;	
	if( WaitForSingleObject( m->mutex, timeout ) == WAIT_OBJECT_0 )
	{
		m->count++;
		if( 1 < m->count )
			osl_log_error( "osl_mutex_lock() error: %d\r\n",m->count );
		return 0;
	}
	else
		return -1;
}


/* 解锁 */
void osl_mutex_unlock( void* mutex )
{
	SMutex *m = (SMutex*)mutex;	
	m->count--;
	if( m->count < 0 )
		osl_log_error( "osl_mutex_unlock() error: %d\r\n", m->count );
	ReleaseMutex( m->mutex );
}

#endif