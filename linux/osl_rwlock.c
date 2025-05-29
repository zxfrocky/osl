#include "stdafx.h"
#include "osl_rwlock.h"


/* ´´½¨¶ÁÐ´Ëø */
void* osl_rwlock_create()
{
	pthread_rwlock_t *rwlock;
	rwlock = (pthread_rwlock_t *)malloc( sizeof(pthread_rwlock_t) );
	memset( rwlock, 0, sizeof(pthread_rwlock_t) );
	pthread_rwlock_init(rwlock,0);
	return rwlock;
}

/* Ïú»Ù¶ÁÐ´Ëø */
void osl_rwlock_destroy( void* rwlock )
{
	pthread_rwlock_destroy( (pthread_rwlock_t *)rwlock );
	free( rwlock );
}

/* ¶ÁËø¶¨ */
void osl_rwlock_read_lock( void* rwlock )
{
	pthread_rwlock_rdlock( (pthread_rwlock_t *)rwlock );
}

/* ¶Á½âËø */
void osl_rwlock_read_unlock( void* rwlock )
{
	pthread_rwlock_unlock( (pthread_rwlock_t *)rwlock );
}

/* Ð´Ëø¶¨ */
void osl_rwlock_write_lock( void* rwlock )
{
	pthread_rwlock_wrlock( (pthread_rwlock_t *)rwlock );
}

/* Ð´½âËø */
void osl_rwlock_write_unlock( void* rwlock )
{
	pthread_rwlock_unlock( (pthread_rwlock_t *)rwlock );
}