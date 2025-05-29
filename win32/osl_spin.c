#include "stdafx.h"
#include "osl_spin.h"

/* ���������� */
void* osl_spin_create()
{
	CRITICAL_SECTION *spin;
	spin = (CRITICAL_SECTION *)malloc( sizeof(CRITICAL_SECTION) );
	memset( spin, 0, sizeof(CRITICAL_SECTION) );
	InitializeCriticalSection( spin );
	return spin;
}

/* ���������� */
void osl_spin_destroy( void* spin )
{
	DeleteCriticalSection( (CRITICAL_SECTION *)spin );
	free( spin );
}


/* ���� */
void osl_spin_lock( void* spin )
{
	EnterCriticalSection( (CRITICAL_SECTION *)spin );
}

/* ���� */
void osl_spin_unlock( void* spin )
{
	LeaveCriticalSection( (CRITICAL_SECTION *)spin );
}
