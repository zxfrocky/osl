#include "stdafx.h"
#include "osl.h"
#include "osl_mutex.h"
#include <pthread.h> 

/* ���������� */
void* osl_mutex_create()
{
	int32_t ret = 0;
	pthread_mutex_t *mt;
    pthread_mutexattr_t pma;

	mt = (pthread_mutex_t*)malloc( sizeof(pthread_mutex_t) );
	if( mt )
	{
		memset( mt, 0, sizeof(pthread_mutex_t) );

		ret |= pthread_mutexattr_init( &pma );
		ret |= pthread_mutexattr_settype( &pma, PTHREAD_MUTEX_ERRORCHECK_NP );
		ret |= pthread_mutex_init( mt, &pma );
		ret |= pthread_mutexattr_destroy( &pma );

		//pthread_mutex_init( mt, NULL );
	}
	return mt;
}

/* ���ٻ����� */
void osl_mutex_destroy( void* mutex )
{
	pthread_mutex_t *mt = (pthread_mutex_t *)mutex;

	pthread_mutex_destroy( mt );

	free( mt );
}

/* ����, timeout(ms)=-1��ʾ���޵ȴ�������0��ʾ�ɹ�������-1��ʾʧ�� */
int32_t osl_mutex_lock( void* mutex, int32_t timeout )
{
	uint32_t start, now;
	
	if( timeout < 0 )
	{
		return pthread_mutex_lock( (pthread_mutex_t *)mutex );
	}
	else
	{
		now = start = osl_get_ms();
		while( now < start + timeout && start <= now )
		{
			if( pthread_mutex_trylock( (pthread_mutex_t *)mutex ) == 0 )
				return 0;
			osl_usleep( 1000 );
		}
	}
	return -1;
}

/* ���� */
void osl_mutex_unlock( void* mutex )
{
	pthread_mutex_unlock( (pthread_mutex_t *)mutex );
}
