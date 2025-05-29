#ifndef __OSL_SPIN_H__
#define __OSL_SPIN_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* 创建自旋锁 */
void* osl_spin_create();

/* 销毁自旋锁 */
void osl_spin_destroy( void* spin );

/* 加锁 */
void osl_spin_lock( void* spin );

/* 解锁 */
void osl_spin_unlock( void* spin );


#ifdef __cplusplus
}
#endif

#endif /*__OSL_SPIN_H__*/
