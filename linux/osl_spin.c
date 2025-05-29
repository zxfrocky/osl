#include "stdafx.h"
#include "osl_spin.h"

/* 创建自旋锁 */
void* osl_spin_create()
{
	pthread_spinlock_t *spin;

	spin = (pthread_spinlock_t*)malloc( sizeof(pthread_spinlock_t) );
	memset( (void*)spin, 0, sizeof(pthread_spinlock_t) );
	pthread_spin_init(spin,0); 
	return (void*)spin;
}

/* 销毁自旋锁 */
void osl_spin_destroy( void* spin )
{
	pthread_spin_destroy( (pthread_spinlock_t*)spin );
	free( spin );
}

/* 加锁 */
void osl_spin_lock( void* spin )
{
	pthread_spin_lock( (pthread_spinlock_t*)spin ); 
}

/* 解锁 */
void osl_spin_unlock( void* spin )
{
	pthread_spin_unlock( (pthread_spinlock_t*)spin );  
}
