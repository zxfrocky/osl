#include "stdafx.h"
#include "osl_spin.h"

/* 创建自旋锁 */
void* osl_spin_create()
{
	CRITICAL_SECTION *spin;
	spin = (CRITICAL_SECTION *)malloc( sizeof(CRITICAL_SECTION) );
	memset( spin, 0, sizeof(CRITICAL_SECTION) );
	InitializeCriticalSection( spin );
	return spin;
}

/* 销毁自旋锁 */
void osl_spin_destroy( void* spin )
{
	DeleteCriticalSection( (CRITICAL_SECTION *)spin );
	free( spin );
}


/* 加锁 */
void osl_spin_lock( void* spin )
{
	EnterCriticalSection( (CRITICAL_SECTION *)spin );
}

/* 解锁 */
void osl_spin_unlock( void* spin )
{
	LeaveCriticalSection( (CRITICAL_SECTION *)spin );
}
