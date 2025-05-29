#include "stdafx.h"
#include "osl_rwlock.h"


/* ������д�� */
void* osl_rwlock_create()
{
	pthread_rwlock_t *rwlock;
	rwlock = (pthread_rwlock_t *)malloc( sizeof(pthread_rwlock_t) );
	memset( rwlock, 0, sizeof(pthread_rwlock_t) );
	pthread_rwlock_init(rwlock,0);
	return rwlock;
}

/* ���ٶ�д�� */
void osl_rwlock_destroy( void* rwlock )
{
	pthread_rwlock_destroy( (pthread_rwlock_t *)rwlock );
	free( rwlock );
}

/* ������ */
void osl_rwlock_read_lock( void* rwlock )
{
	pthread_rwlock_rdlock( (pthread_rwlock_t *)rwlock );
}

/* ������ */
void osl_rwlock_read_unlock( void* rwlock )
{
	pthread_rwlock_unlock( (pthread_rwlock_t *)rwlock );
}

/* д���� */
void osl_rwlock_write_lock( void* rwlock )
{
	pthread_rwlock_wrlock( (pthread_rwlock_t *)rwlock );
}

/* д���� */
void osl_rwlock_write_unlock( void* rwlock )
{
	pthread_rwlock_unlock( (pthread_rwlock_t *)rwlock );
}