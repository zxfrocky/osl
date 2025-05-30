#ifndef __XTCQUEUE_H__
#define __XTCQUEUE_H__

#include <new>
#include "Xtc.h"


/////////////////////////////////////////////////////////////////////////////
// CXtcQueue<TYPE>

template<class TYPE>
class CXtcQueue
{
public:
	CXtcQueue();
	~CXtcQueue();

// Operations
	void Create(int32_t size);
	void Destroy();
	bool Post(TYPE& msg);
	bool Read(TYPE* msg);
	void Clear();
	int GetCount();
	int GetMaxCount();
	TYPE& operator[](int nIndex);


// Implementation
protected:
	TYPE* m_pData;
	int m_nSize;
	int m_nHead;
	int m_nTail;
};


/////////////////////////////////////////////////////////////////////////////
// CXtcQueue<TYPE> inline functions

template<class TYPE>
inline void CXtcQueue<TYPE>::Clear()
{
	m_nHead = m_nTail = 0;
}

template<class TYPE>
inline int CXtcQueue<TYPE>::GetCount()
{
	return (m_nTail-m_nHead+m_nSize)%m_nSize;
}

template<class TYPE>
inline int CXtcQueue<TYPE>::GetMaxCount()
{
	return m_nSize-1;
}

template<class TYPE>
inline TYPE& CXtcQueue<TYPE>::operator[](int nIndex)
{
	return m_pData[(nIndex+m_nHead)%m_nSize];
}

/////////////////////////////////////////////////////////////////////////////
// CXtcQueue<TYPE> out-of-line functions

template<class TYPE>
CXtcQueue<TYPE>::CXtcQueue()
{
	m_pData = NULL;
	m_nSize = m_nHead = m_nTail = 0;
}

template<class TYPE>
CXtcQueue<TYPE>::~CXtcQueue()
{
	if (m_pData != NULL)
	{
		DestructElements<TYPE>(m_pData, m_nSize);
		XTC_FREE( m_pData );
		m_pData = NULL;
	}
	m_nSize = m_nHead = m_nTail = 0;
}


template<class TYPE>
void CXtcQueue<TYPE>::Create(int32_t size)
{
	XTC_ASSERT(size >= 0);

	m_nSize = size;

	// shrink to nothing
	if (m_pData != NULL)
	{
		DestructElements<TYPE>(m_pData, m_nSize);

		XTC_FREE( m_pData );

		m_pData = NULL;
	}

	if (m_pData == NULL)
	{
		m_pData = (TYPE*)XTC_MALLOC( m_nSize * sizeof(TYPE) );

		ConstructElements<TYPE>(m_pData, m_nSize);
	}
	m_nHead = m_nTail = 0;
}

template<class TYPE>
void CXtcQueue<TYPE>::Destroy()
{
	if (m_pData != NULL)
	{
		DestructElements<TYPE>(m_pData, m_nSize);
		XTC_FREE( m_pData );
		m_pData = NULL;
	}
	m_nSize = m_nHead = m_nTail = 0;
}

template<class TYPE>
bool CXtcQueue<TYPE>::Post(TYPE& msg)
{
	int32_t h = (m_nTail + 1) % m_nSize;
	if (h != m_nHead)
	{
		m_pData[m_nTail] = msg;
		m_nTail = h;
		
		return true;
	}
	else
	{
		return false;
	}
}

template<class TYPE>
bool CXtcQueue<TYPE>::Read(TYPE* msg)
{
	if (m_nHead != m_nTail)
	{
		*msg = m_pData[m_nHead];
		m_nHead = (m_nHead + 1) % m_nSize;
		return true;
	}
	else
	{
		return false;
	}
}


#endif /*__XTCQUEUE_H__*/
