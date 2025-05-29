#ifndef __OSL_RWLOCK_H__
#define __OSL_RWLOCK_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* ������д�� */
void* osl_rwlock_create();

/* ���ٶ�д�� */
void osl_rwlock_destroy( void* rwlock );

/* ������ */
void osl_rwlock_read_lock( void* rwlock );

/* ������ */
void osl_rwlock_read_unlock( void* rwlock );

/* д���� */
void osl_rwlock_write_lock( void* rwlock );

/* д���� */
void osl_rwlock_write_unlock( void* rwlock );


#ifdef __cplusplus
}
#endif

#endif