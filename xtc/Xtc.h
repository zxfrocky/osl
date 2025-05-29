#ifndef __XTC_H__
#define __XTC_H__


#include "osl.h"
//#include <new.h>
#include <new>

#ifndef XTC_ASSERT
#define XTC_ASSERT(flag)	osl_assert( flag, __FILE__, __LINE__ )
#endif

#ifndef XTC_MALLOC
#define XTC_MALLOC(size)	malloc( size )
#endif

#ifndef XTC_FREE
#define XTC_FREE(p)			free( p )
#endif



//内存分配放函数
typedef void* (*PMallocCallback)( int32_t size, void *param );

//内存释放函数
typedef void  (*PFreeCallback)( void *buf, void *param );

//比较回调函数
typedef int32_t (*PCompareCallback)( void* item1, void* item2, void *param );


template<class TYPE>
inline void ConstructElements(TYPE* pElements, int nCount)
{
	memset((void*)pElements, 0, nCount * sizeof(TYPE));
	for (; nCount--; pElements++)
		::new( (void *)pElements ) TYPE;
}

template<class TYPE>
inline void DestructElements(TYPE* pElements, int nCount)
{
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE>
inline void CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	while (nCount--)
		*pDest++ = *pSrc++;
}


#endif
