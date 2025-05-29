#ifndef __OSL_RWLOCK_H__
#define __OSL_RWLOCK_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* ´´½¨¶ÁÐ´Ëø */
void* osl_rwlock_create();

/* Ïú»Ù¶ÁÐ´Ëø */
void osl_rwlock_destroy( void* rwlock );

/* ¶ÁËø¶¨ */
void osl_rwlock_read_lock( void* rwlock );

/* ¶Á½âËø */
void osl_rwlock_read_unlock( void* rwlock );

/* Ð´Ëø¶¨ */
void osl_rwlock_write_lock( void* rwlock );

/* Ð´½âËø */
void osl_rwlock_write_unlock( void* rwlock );


#ifdef __cplusplus
}
#endif

#endif