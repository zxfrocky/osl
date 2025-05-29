#ifndef __OSL_SPIN_H__
#define __OSL_SPIN_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* ���������� */
void* osl_spin_create();

/* ���������� */
void osl_spin_destroy( void* spin );

/* ���� */
void osl_spin_lock( void* spin );

/* ���� */
void osl_spin_unlock( void* spin );


#ifdef __cplusplus
}
#endif

#endif /*__OSL_SPIN_H__*/
