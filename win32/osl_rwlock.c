#include "stdafx.h"
#include "osl_rwlock.h"
#include "osl_atomic.h"
#include "windows.h"
#include "winuser.h"

//windows server 2008之后才支持读写锁
#if 0//(WINVER>=0x0600)
/* 创建读写锁 */
void* osl_rwlock_create()
{
	SRWLOCK *rwlock;

	rwlock = (SRWLOCK *)malloc( sizeof(SRWLOCK) );
	InitializeSRWLock( rwlock );

	return rwlock;
}

/* 销毁读写锁 */
void osl_rwlock_destroy( void* rwlock )
{
	free( rwlock );
}

/* 读锁定 */
void osl_rwlock_read_lock( void* rwlock )
{
	AcquireSRWLockShared( (SRWLOCK*)rwlock );
}

/* 读解锁 */
void osl_rwlock_read_unlock( void* rwlock )
{
	ReleaseSRWLockShared( (SRWLOCK*)rwlock );
}

/* 写锁定 */
void osl_rwlock_write_lock( void* rwlock )
{
	AcquireSRWLockExclusive( (SRWLOCK*)rwlock );
}

/* 写解锁 */
void osl_rwlock_write_unlock( void* rwlock )
{
	ReleaseSRWLockExclusive( (SRWLOCK*)rwlock );
}

#else

typedef struct
{
	int read_ref;
	int write_ref;
	CRITICAL_SECTION crisec;  
}SRwlock;

/* 创建读写锁 */
void* osl_rwlock_create()
{
	SRwlock *lock;

	lock = (SRwlock *)malloc( sizeof(SRwlock) );
	memset( lock, 0, sizeof(SRwlock) );
	InitializeCriticalSection( &lock->crisec );

	return lock;
}

/* 销毁读写锁 */
void osl_rwlock_destroy( void* rwlock )
{
	SRwlock *lock = (SRwlock *)rwlock;
	DeleteCriticalSection( &lock->crisec );  
	free( lock );
}

/* 读锁定 */
void osl_rwlock_read_lock( void* rwlock )
{
	SRwlock *lock = (SRwlock *)rwlock;
	char_t flag = 1;

	/* 读的时候不能有另外的线程在写，但是可以有另外的线程在读 */
	while( 0 < flag )
	{
		EnterCriticalSection( &lock->crisec );  
		if ( 0 < lock->write_ref )
		{
			flag = 2;
		}
		else
		{
			lock->read_ref++;
			flag = 0;
		}
		LeaveCriticalSection( &lock->crisec );  
		if( flag == 2 )
			Sleep(0);
	}
}

/* 读解锁 */
void osl_rwlock_read_unlock( void* rwlock )
{
	SRwlock *lock = (SRwlock *)rwlock;

	EnterCriticalSection( &lock->crisec );  
	if( 0 < lock->read_ref )
		lock->read_ref--;
	else
		printf( "rwlock read unlock error\r\n" );
	LeaveCriticalSection( &lock->crisec );  
}

/* 写锁定 */
void osl_rwlock_write_lock( void* rwlock )
{
	SRwlock *lock = (SRwlock *)rwlock;
	char_t flag = 1;

	/* 写的时候不能有另外的线程在写和读 */
	while( 0 < flag )
	{
		EnterCriticalSection( &lock->crisec );  
		if ( 0 < lock->read_ref || 0 < lock->write_ref )
		{
			flag = 2;
		}
		else
		{
			lock->write_ref++;
			flag = 0;
		}
		LeaveCriticalSection( &lock->crisec );  
		if( flag == 2 )
			Sleep(0);
	}
}

/* 写解锁 */
void osl_rwlock_write_unlock( void* rwlock )
{
	SRwlock *lock = (SRwlock *)rwlock;

	EnterCriticalSection( &lock->crisec );  
	if( 0 < lock->write_ref )
		lock->write_ref--;
	else
		printf( "rwlock write unlock error\r\n" );
	LeaveCriticalSection( &lock->crisec );  
}


#endif
