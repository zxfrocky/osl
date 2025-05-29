#ifndef __XTCARRAY_H__
#define __XTCARRAY_H__

#include <new>
#include "Xtc.h"


/////////////////////////////////////////////////////////////////////////////
// CXtcArray<TYPE>

template<class TYPE>
class CXtcArray
{
public:
	CXtcArray();
	~CXtcArray();

	//设置内存分配释放回调函数
	void SetMemoryCallback( PMallocCallback pMallocProc, PFreeCallback pFreeProc, void *param );

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex, TYPE& newElement);
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, TYPE& newElement);
	int Add(TYPE& newElement);
	int Append(const CXtcArray& src);
	void Copy(const CXtcArray& src);

	// overloaded operator helpers
	TYPE operator[](int nIndex) const;
	TYPE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, TYPE& newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CXtcArray* pNewArray);

	/* 排序 */
	void Sort( PCompareCallback proc, void *param );

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

	//内存分配/释放函数
	PMallocCallback m_pMallocProc;
	PFreeCallback m_pFreeProc;
	void *m_param;

private:
	/* 快速排序算法 */
	void QuickSort( TYPE* pData, int low, int high, PCompareCallback proc, void *param );
};


/////////////////////////////////////////////////////////////////////////////
// CXtcArray<TYPE> inline functions

template<class TYPE>
inline int CXtcArray<TYPE>::GetSize() const
{
	return m_nSize;
}

template<class TYPE>
inline int CXtcArray<TYPE>::GetUpperBound() const
{
	return m_nSize-1;
}

template<class TYPE>
inline void CXtcArray<TYPE>::RemoveAll()
{
	SetSize(0, -1);
}

template<class TYPE>
inline TYPE CXtcArray<TYPE>::GetAt(int nIndex) const
{
	XTC_ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE>
inline void CXtcArray<TYPE>::SetAt(int nIndex, TYPE& newElement)
{
	XTC_ASSERT(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement;
}

template<class TYPE>
inline TYPE& CXtcArray<TYPE>::ElementAt(int nIndex)
{
	XTC_ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE>
inline const TYPE* CXtcArray<TYPE>::GetData() const
{
	return (const TYPE*)m_pData;
}

template<class TYPE>
inline TYPE* CXtcArray<TYPE>::GetData()
{
	return (TYPE*)m_pData;
}

template<class TYPE>
inline int CXtcArray<TYPE>::Add(TYPE& newElement)
{
	int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex;
}

template<class TYPE>
inline TYPE CXtcArray<TYPE>::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

template<class TYPE>
inline TYPE& CXtcArray<TYPE>::operator[](int nIndex)
{
	return ElementAt(nIndex);
}

/////////////////////////////////////////////////////////////////////////////
// CXtcArray<TYPE> out-of-line functions

template<class TYPE>
CXtcArray<TYPE>::CXtcArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
	m_pMallocProc = NULL;
	m_pFreeProc = NULL;
	m_param = 0;
}

template<class TYPE>
CXtcArray<TYPE>::~CXtcArray()
{
	if (m_pData != NULL)
	{
		DestructElements<TYPE>(m_pData, m_nSize);
		if( m_pFreeProc )
			m_pFreeProc( m_pData, m_param );
		else
			XTC_FREE( m_pData );
		m_pData = NULL;
	}
}

//设置内存分配释放回调函数
template<class TYPE>
void CXtcArray<TYPE>::SetMemoryCallback( PMallocCallback pMallocProc, PFreeCallback pFreeProc, void *param )
{
	m_pMallocProc = pMallocProc;
	m_pFreeProc = pFreeProc;
	m_param = param;
}


template<class TYPE>
void CXtcArray<TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	XTC_ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			DestructElements<TYPE>(m_pData, m_nSize);

			if( m_pFreeProc )
				m_pFreeProc( m_pData, m_param );
			else
				XTC_FREE( m_pData );

			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		m_nSize = nNewSize;
		if( nNewSize < m_nGrowBy )
			nNewSize = m_nGrowBy;
		if( m_pMallocProc )
			m_pData = (TYPE*)m_pMallocProc( nNewSize * sizeof(TYPE), m_param );
		else
			m_pData = (TYPE*)XTC_MALLOC( nNewSize * sizeof(TYPE) );

		ConstructElements<TYPE>(m_pData, m_nSize);
		m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			ConstructElements<TYPE>(&m_pData[m_nSize], nNewSize-m_nSize);
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			DestructElements<TYPE>(&m_pData[nNewSize], m_nSize-nNewSize);
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		XTC_ASSERT(nNewMax >= m_nMaxSize);  // no wrap around

		TYPE* pNewData;
		if( m_pMallocProc )
			pNewData = (TYPE*)m_pMallocProc( nNewMax * sizeof(TYPE), m_param );
		else
			pNewData = (TYPE*)XTC_MALLOC( nNewMax * sizeof(TYPE) );

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

		// construct remaining elements
		XTC_ASSERT(nNewSize > m_nSize);
		ConstructElements<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

		// get rid of old stuff (note: no destructors called)
		if( m_pFreeProc )
			m_pFreeProc( m_pData, m_param );
		else
			XTC_FREE( m_pData );

		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE>
int CXtcArray<TYPE>::Append(const CXtcArray& src)
{
	XTC_ASSERT(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	CopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

template<class TYPE>
void CXtcArray<TYPE>::Copy(const CXtcArray& src)
{
	XTC_ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	CopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

template<class TYPE>
void CXtcArray<TYPE>::FreeExtra()
{
	if (m_nSize != m_nMaxSize && m_pData)
	{
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			if( m_pMallocProc )
				pNewData = (TYPE*)m_pMallocProc( m_nSize * sizeof(TYPE), m_param );
			else
				pNewData = (TYPE*)XTC_MALLOC( m_nSize * sizeof(TYPE) );

			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
		if( m_pFreeProc )
			m_pFreeProc( m_pData, m_param );
		else
			XTC_FREE( m_pData );

		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE>
void CXtcArray<TYPE>::SetAtGrow(int nIndex, TYPE& newElement)
{
	XTC_ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

template<class TYPE>
void CXtcArray<TYPE>::InsertAt(int nIndex, TYPE& newElement, int nCount /*=1*/)
{
	XTC_ASSERT(nIndex >= 0);    // will expand to meet need
	XTC_ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		DestructElements<TYPE>(&m_pData[nOldSize], nCount);
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		ConstructElements<TYPE>(&m_pData[nIndex], nCount);
	}

	// insert new value in the gap
	XTC_ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE>
void CXtcArray<TYPE>::RemoveAt(int nIndex, int nCount)
{
	XTC_ASSERT(nIndex >= 0);
	XTC_ASSERT(nCount >= 0);
	XTC_ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	DestructElements<TYPE>(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(TYPE));
	m_nSize -= nCount;
}

template<class TYPE>
void CXtcArray<TYPE>::InsertAt(int nStartIndex, CXtcArray* pNewArray)
{
	XTC_ASSERT(pNewArray != NULL);
	XTC_ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}


/* 排序 */
template<class TYPE>
void CXtcArray<TYPE>::Sort( PCompareCallback proc, void *param )
{
	if( 1 < m_nSize )
	{
		//vdl_log_debug("size = %d\n",m_nSize);
		QuickSort( m_pData, 0, m_nSize-1, proc, param );
	}
}

#if 0

/* 传统的快速排序，这个算法对长有序数组（比如长度超过1000）排序会造成递归溢出 */
template<class TYPE>
void CXtcArray<TYPE>::QuickSort( TYPE* pData, int low, int high, PCompareCallback proc, void *param )
{
	int left = low, right = high;
	TYPE tmp = pData[left];

	while( left < right )
	{
		while( left < right && proc( &tmp, pData+right, param ) <= 0 )
			right--;
		if( left < right )
		{
			pData[left] = pData[right];
			left++;
		}
		while( left < right && proc( pData+left, &tmp, param ) <= 0 )
			left++;
		if( left < right )
		{
			pData[right] = pData[left];
			right--;
		}
	}

	pData[left] = tmp;

	if( low < left-1 )
		QuickSort( pData, low, left-1, proc, param );
	if( left+1 < high )
		QuickSort( pData, left+1, high, proc, param );
}

#endif

/* 快速排序，huanghuaming: 改进了传统快速排序算法，通过不断调整基准值来避免递归溢出 */
template<class TYPE>
void CXtcArray<TYPE>::QuickSort( TYPE* pData, int low, int high, PCompareCallback proc, void *param )
{
	int left = low, right = high, mid = (low+high)/2;
	TYPE base, tmp;

	/* 定义基准为中间值M */
	base = pData[mid];
	while( left < right )
	{
		/* 在左半部寻找大值A */
		while( left < mid && proc( pData+left, &base, param ) <= 0 )
			left++;

		/* 在右半部寻找小值A */
		while( mid < right && proc( &base, pData+right, param ) <= 0 )
			right--;

		/* 如果A和B都找到，则交换A和B */
		if( left < mid && mid < right )
		{
			/* 交换 */
			tmp = pData[left];
			pData[left] = pData[right];
			pData[right] = tmp;

			left++;
			right--;
		}
		/* 如果只找到A，没有B，则交换A和M，调整基准值，从头再来 */
		else if( left < mid )
		{
			/* 交换 */
			tmp = pData[left];
			pData[left] = pData[mid];
			pData[mid] = tmp;

			/* 调整基准值从头开始计算 */
			base = tmp;
			left = low;
			right = high;
		}
		/* 如果只找到B，没有A，则交换B和M，调整基准值，从头再来 */
		else if( mid < right )
		{
			/* 交换 */
			tmp = pData[right];
			pData[right] = pData[mid];
			pData[mid] = tmp;

			/* 调整基准值从头开始计算 */
			base = tmp;
			left = low;
			right = high;
		}
	}
	if( low < left-1 )
		QuickSort( pData, low, left-1, proc, param );
	if( right+1 < high )
		QuickSort( pData, right+1, high, proc, param );
}


#endif /*__XTCARRAY_H__*/
