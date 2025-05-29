#include "stdafx.h"
#include "osl.h"
#include "osl_log.h"
#include "osl_mutex.h"

#ifndef _DEBUG

/* ���������� */
void* osl_mutex_create()
{
	return CreateMutex( NULL, 0, NULL );
}

/* ���ٻ����� */
void osl_mutex_destroy( void* mutex )
{
	CloseHandle( mutex );
}

/* ����, timeout(ms)-1��ʾ���޵ȴ�������0��ʾ�ɹ�������-1��ʾʧ�� */
int32_t osl_mutex_lock( void* mutex, int32_t timeout )
{
	if( WaitForSingleObject( mutex, timeout ) == WAIT_OBJECT_0 )
		return 0;
	else
		return -1;
}

/* ���� */
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

/* ���������� */
void* osl_mutex_create()
{
	SMutex *m = (SMutex*)malloc( sizeof(SMutex) );
	memset( m, 0, sizeof(SMutex) );
	m->mutex = CreateMutex( NULL, 0, NULL );
	return m;
}

/* ���ٻ����� */
void osl_mutex_destroy( void* mutex )
{
	SMutex *m = (SMutex*)mutex;
	CloseHandle( m->mutex );
	free( m );
}


/* ����, timeout(ms)=-1��ʾ���޵ȴ�������0��ʾ�ɹ�������-1��ʾʧ�� */
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


/* ���� */
void osl_mutex_unlock( void* mutex )
{
	SMutex *m = (SMutex*)mutex;	
	m->count--;
	if( m->count < 0 )
		osl_log_error( "osl_mutex_unlock() error: %d\r\n", m->count );
	ReleaseMutex( m->mutex );
}

#endif