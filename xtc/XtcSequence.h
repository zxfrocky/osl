#ifndef __XTCSEQUENCE_H__
#define __XTCSEQUENCE_H__

#include "Xtc.h"

/* �������У���ƽ�������ʵ�� */
template<class VALUE_TYPE>
class CXtcSequence
{
/* �Ƚϻص����� */
typedef int (*PSequenceCompareCallback)( bool item1_is_key, void* item1, void* item2, void *param );
public:
	/* �������ڵ� */
	typedef struct tagQueueNode
	{
		/* ֵ�����������λ���Ա�֤positionָ�����ֱ��ת��ΪVALUEָ�� */
		VALUE_TYPE value;
		/* ���߶� */
		int height;
		/* �ӽڵ������*/
		int son_num;
		/* ���ڵ� */
		struct tagQueueNode* parent;
		/* ���ӽڵ� */
		struct tagQueueNode* left;
		/* ���ӽڵ� */
		struct tagQueueNode* right;
	}SQueueNode;

	/* ���졢�������� */
	CXtcSequence();
	virtual ~CXtcSequence();

	/* �����ڴ�����ͷŻص����� */
	void SetMemoryCallback( PMallocCallback pMallocProc, PFreeCallback pFreeProc, void* param );
	/* ���ñȽϻص����� */
	void SetCompareCallback( PSequenceCompareCallback pCompareProc, void* param );
	/* �����ڴ������ٶ� */
	void SetGrowNum( int num = 1024 );

	/* ���룬���ظ�Ԫ�����ڵ�ָ��λ�� */
	void* Insert( VALUE_TYPE& value );
	/* ɾ��ĳ���ڵ�(�����ӽڵ� */
	bool Remove( void *key );
	/* ɾ��ָ���ڵ� */
	void RemoveByPosition( void* position );
	/* ɾ�����нڵ� */
	int32_t RemoveAll();
	/* �滻Ԫ�أ�KEY��Ӧ��VALUE�����ظ�Ԫ�����ڵ�ָ��λ�� */
	void* Replace( void* key, VALUE_TYPE& value );
	/* ���ң����ظ�Ԫ�����ڵ�ָ��λ�� */
	void* Search( void* key, VALUE_TYPE* value );

	/* ��λ��һ��Ԫ�أ����ظ�Ԫ�ص�ָ��λ��(NULL��ʾʧ��) */
	void* GetFirst( VALUE_TYPE* value );
	/* ��λ��һ��Ԫ�أ����ظ�Ԫ�ص�ָ��λ��(NULL��ʾʧ��) */
	void* GetNext( VALUE_TYPE* value, void* position );
	/* ��λ��һ��Ԫ�أ����ظ�Ԫ�ص�ָ��λ��(NULL��ʾʧ��) */
	void* GetPrev( VALUE_TYPE* value, void* position );
	/* ��λ���һ��Ԫ�أ����ظ�Ԫ�ص�ָ��λ��(NULL��ʾʧ��) */
	void* GetLast( VALUE_TYPE* value );
	/* ��λָ����ŵ�Ԫ�� */
	void* SeekTo( int idx, VALUE_TYPE* value );
	/* position -> index */
	int GetIndex( void *position );

	/* ��ָ��λ�û��ֵ */
	VALUE_TYPE* GetValue( void* position );
	/* ��ȡԪ�ظ��� */
	int GetSize();
	/* ȡ������ */
	int GetHeight();

	/* ���������Ƿ��д����ش��������root=NULL��ʾ�Ӹ���ʼ���� */
	int Check( void* root = NULL );

protected:

	/* ����ת */
	int RotateLeft( SQueueNode** ppstRoot );
	/* �ҵ���ת */
	int RotateRight( SQueueNode** ppstRoot );
	/* ���ҵ��� */
	int LeftBalance( SQueueNode** ppstRoot );
	/* ������ */
	int RightBalance( SQueueNode** ppstRoot );

	/* ���ң�����key���ڽڵ�λ�� */
	void* Search( SQueueNode* pstRoot, void* key, VALUE_TYPE* value );
	/* ��λָ����ŵ�Ԫ�� */
	void* SeekTo( SQueueNode* pstRoot, int idx, VALUE_TYPE* value );

	/* ���룬���ز���ڵ�λ�� */
	void* Insert( SQueueNode** ppstRoot, SQueueNode* itFather, VALUE_TYPE& value );
	/* ɾ�����е�ĳ���ڵ� */
	bool Remove( SQueueNode* pstRoot, void* key );
	/* ǿ��ɾ��ĳ���ڵ㼰���ӽڵ㣬���ܶ�ƽ���Ӱ�� */
	void ForceRemove( SQueueNode* node );
	/* ����ڵ�ĸ߶� */
	void RefreshNode( SQueueNode* node );
	/* ���Լ���ʼ����ڵ㣬�������ߺ������ڵ��� */
	int RefreshBackward( SQueueNode* node );
	/* �Ƚ� */
	int Compare( void* node1, void* node2, bool flag );

	
private:
	/* �ڴ����/�ͷź��� */
	PMallocCallback m_pMallocProc;
	PFreeCallback m_pFreeProc;
	void* m_pMemoryParam;
	int m_nGrowNum;

	/* �ȽϺ��� */
	PSequenceCompareCallback m_pCompareProc;
	void* m_pCompareParam;

	SQueueNode* m_pRoot;
	int m_iCount;

	SQueueNode* m_pIdle;/* ���нڵ� */
	SQueueNode* m_pList;/* �ڴ� */
	SQueueNode* m_pCurr;/* ��ǰ������ڴ�ָ�� */
	int m_iCurr;/* ��ǰ�ڴ��õ������� */
};

template<class VALUE_TYPE>
CXtcSequence<VALUE_TYPE>::CXtcSequence()
{
	m_pMallocProc = NULL;
	m_pFreeProc = NULL;
	m_pMemoryParam = 0;
	m_nGrowNum = 1024;
	
	m_pCompareProc = NULL;
	m_pCompareParam = 0;

	m_pRoot = NULL;
	m_iCount = 0;

	m_pIdle = NULL;
	m_pList = NULL;
	m_pCurr = NULL;
	m_iCurr = 0;
}

template<class VALUE_TYPE>
CXtcSequence<VALUE_TYPE>::~CXtcSequence()
{
	SQueueNode *node, *next;

	RemoveAll();/* û��ֱ���ͷ��ڴ棬��Ϊ�˵���ÿ���ڵ���������� */
	if( m_pFreeProc == NULL )
	{
		node = m_pList;
		while( node )
		{
			next = node->right;
			XTC_FREE( node );
			node = next;
		}
		m_pList = NULL;
		m_pIdle = NULL;
		m_pCurr = NULL;
		m_iCurr = 0;
	}
}


/* �����ڴ�����ͷŻص����� */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::SetMemoryCallback( PMallocCallback pMallocProc, PFreeCallback pFreeProc, void* param )
{
	m_pMallocProc = pMallocProc;
	m_pFreeProc = pFreeProc;
	m_pMemoryParam = param;
}

/* ���ñȽϻص����� */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::SetCompareCallback( PSequenceCompareCallback pCompareProc, void* param )
{
	m_pCompareProc = pCompareProc;
	m_pCompareParam = param;
}

/* �����ڴ������ٶ� */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::SetGrowNum( int num )
{
	m_nGrowNum = num;
}

/* ���� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Insert( VALUE_TYPE& value )
{
	return Insert(&m_pRoot, NULL, value);
}

/* ɾ�����е�ĳ���ڵ� */
template<class VALUE_TYPE>
bool CXtcSequence<VALUE_TYPE>::Remove( void* key )
{
	return Remove(m_pRoot, key);
}

/* ɾ��ָ���ڵ� */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::RemoveByPosition( void* position )
{
	int lh, rh;
	SQueueNode* pstRoot = (SQueueNode*)position;
	SQueueNode* replace = NULL;
	SQueueNode* parent = NULL;


	lh = pstRoot->left ? pstRoot->left->height : 0;
	rh = pstRoot->right ? pstRoot->right->height : 0;

	/* �������������������, ��ôѡ�������������KEY�滻���ñ�ɾ�ڵ� */
	if( rh <= lh )
	{
		replace = pstRoot->left;
		while( replace && replace->right )
			replace = replace->right;

		/* ��ԭ����λ�ò��replace */
		if( replace )
		{
			XTC_ASSERT( replace->right == NULL );

			/* �����KEY�ڵ�������Ů (��������Ů) */
			if( replace->left )
			{
				if( replace->parent->left == replace )
				{
					replace->parent->left = replace->left;
					replace->left->parent = replace->parent;
				}
				else
				{
					replace->parent->right = replace->left;
					replace->left->parent = replace->parent;
				}
			}
			/* �����KEY�ڵ�������Ů */
			else
			{
				if( replace->parent->left == replace )
					replace->parent->left = NULL;
				else
					replace->parent->right = NULL;
			}
		}
	}
	/* �������������������, ��ôѡ������������СKEY�滻���ñ�ɾ�ڵ� */
	else if ( lh < rh )
	{
		replace = pstRoot->right;
		while( replace && replace->left )
			replace = replace->left;

		/* ��ԭ����λ�ò��replace */
		if( replace )
		{
			XTC_ASSERT( replace->left == NULL );

			/* ����СKEY�ڵ�������Ů (��������Ů) */
			if( replace->right )
			{
				if( replace->parent->left == replace )
				{
					replace->parent->left = replace->right;
					replace->right->parent = replace->parent;
				}
				else
				{
					replace->parent->right = replace->right;
					replace->right->parent = replace->parent;
				}
			}
			/* ����СKEY�ڵ�������Ů */
			else
			{
				if( replace->parent->left == replace )
					replace->parent->left = NULL;
				else
					replace->parent->right = NULL;
			}
		}
	}

	/* ���𴦸��ڵ��п���ʧ�⣬����� */
	if( replace )
	{
		parent = replace->parent;
		lh = parent->left ? parent->left->height : 0;
		rh = parent->right ? parent->right->height : 0;
		if( lh + 1 < rh )
			RightBalance( &parent );
		else if( rh + 1 < lh )
			LeftBalance( &parent );
		RefreshBackward( parent );
	}

	/* ���������Ľڵ�ӵ����ڵ��� */
	parent = pstRoot->parent;
	if( parent )
	{
		if( parent->left == pstRoot )
			parent->left = replace;
		else
			parent->right = replace;
	}

	/* �ò������Ľڵ��滻��ɾ���ڵ� */
	if( replace )
	{
		replace->parent = pstRoot->parent;

		replace->left = pstRoot->left;
		if( replace->left )
			replace->left->parent = replace;

		replace->right = pstRoot->right;
		if (replace->right)
			replace->right->parent = replace;

		RefreshBackward( replace );
	}
	else
		RefreshBackward( parent );

	m_iCount--;
	if( pstRoot == m_pRoot )
		m_pRoot = replace;

	/* �����ڵ��а������� */
	pstRoot->value.~VALUE_TYPE();
	/* �ͷ��ڴ� */
	if( m_pFreeProc )
		m_pFreeProc( pstRoot, m_pMemoryParam );
	else
	{
		pstRoot->right = m_pIdle;
		m_pIdle = pstRoot;
	}
}


/* ɾ���������� */
template<class VALUE_TYPE>
int32_t CXtcSequence<VALUE_TYPE>::RemoveAll()
{
	int32_t cnt = 0;

	if( m_pRoot )
	{
		cnt = m_iCount;
		ForceRemove( m_pRoot );
		m_pRoot = NULL;
		m_iCount = 0;
	}

	return cnt;
}

/* �滻 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Replace( void* key, VALUE_TYPE& value )
{
	SQueueNode* node;

	node = (SQueueNode*)Search( m_pRoot, key, NULL );
	if( node )
		node->value = value;

	return node;
}


/* ���� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Search( void* key, VALUE_TYPE* value )
{ 
	return Search( m_pRoot, key, value );
}

/* ��λ��һ��Ԫ�أ����ظ�Ԫ�ص�λ�ã�����NULL��ʾ��Ԫ�� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::GetFirst( VALUE_TYPE* value )
{
	SQueueNode* node = m_pRoot;

	if( node == NULL )
		return NULL;

	while (node->left)
		node = node->left;
	
	if( value )
		*value = node->value;
	
	return node;
}

/* ��λ��һ��Ԫ�أ����ظ�Ԫ�ص�λ�ã�����NULL��ʾ���� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::GetNext( VALUE_TYPE* value, void* position )
{
	SQueueNode* node = (SQueueNode*)position;

	/* ���������������nextΪ������������ڵ� */
	if( node->right )
	{
		node = node->right;
		while (node->left)
			node = node->left;
	}
	/* ���������������next�����ڽڵ��ϣ����ڵ�Ϊ���ڽڵ����֧ */
	else
	{
		SQueueNode* parent;
		parent = node->parent;
		while( parent && parent->right == node )
		{
			node = parent;
			parent = parent->parent;
		}
		node = parent;
	}
	if( node )
	{
		if( value )
			*value = node->value;
	}
	return node;
}

/* ��λ��һ��Ԫ�أ����ظ�Ԫ�ص�λ�ã�����NULL��ʾ���� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::GetPrev( VALUE_TYPE* value, void* position )
{
	SQueueNode* node = (SQueueNode*)position;

	/* ���������������nextΪ�����������ҽڵ� */
	if( node->left )
	{
		node = node->left;
		while (node->right)
			node = node->right;
	}
	/* ���������������next�����ڽڵ��ϣ����ڵ�Ϊ���ڽڵ����֧ */
	else
	{
		SQueueNode* parent;
		parent = node->parent;
		while( parent && parent->left == node )
		{
			node = parent;
			parent = parent->parent;
		}
		node = parent;
	}
	if( node )
	{
		if( value )
			*value = node->value;
	}
	return node;
}


/* ��λ���һ��Ԫ�أ����ظ�Ԫ�ص�λ�ã�����NULL��ʾ��Ԫ�� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::GetLast( VALUE_TYPE* value )
{
	SQueueNode* node = m_pRoot;
	
	if( node == NULL )
		return NULL;

	while (node->right)
		node = node->right;
	
	if( value )
		*value = node->value;
	
	return node;
}

/* ��λָ����ŵ�Ԫ�� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::SeekTo( int idx, VALUE_TYPE* value )
{
	return SeekTo( m_pRoot, idx, value );
}

/* position -> index */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::GetIndex( void *position )
{
	SQueueNode *node = (SQueueNode *)position;
	int lson;

	lson = node->left ? node->left->son_num + 1 : 0;
	while( node->parent )
	{
		/* �������ڵ�����ӽ�㣬��ڵ㼰�����ӽڵ㶼���Լ�С */
		if( node == node->parent->right )
		{
			if( node->parent->left )
				lson += 2 + node->parent->left->son_num;
			else
				lson += 1;
		}
		node = node->parent;
	}

	return lson;
}

/* ��ָ��λ�û��ֵ */
template<class VALUE_TYPE>
VALUE_TYPE* CXtcSequence<VALUE_TYPE>::GetValue( void* position )
{
	return &((SQueueNode*)position)->value;
}

/* ��ȡԪ�ظ��� */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::GetSize(void)
{
#if 0
	if( m_pRoot )
	{
		XTC_ASSERT( m_pRoot->son_num + 1 == m_iCount );
	}
	else
	{
		XTC_ASSERT( m_iCount == 0 );
	}
#endif
	return m_iCount;
}

/* ȡ������ */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::GetHeight()
{
	if( m_pRoot )
		return m_pRoot->height;
	else
		return 0;
}

/* ���������Ƿ��д����ش��������root=NULL��ʾ�Ӹ���ʼ���� */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::Check( void* root )
{
	int n = 0, lh, rh, lson, rson;
	SQueueNode* node;

	node = (SQueueNode *)( root ? root : m_pRoot);
	if( node == NULL )
		return 0;

	if( node->left )
		n += Check( node->left );
	if( node->right )
		n += Check( node->right );

	lh = node->left ? node->left->height : 0;
	rh = node->right ? node->right->height : 0;

	lson = node->left ? node->left->son_num + 1 : 0;
	rson = node->right ? node->right->son_num + 1 : 0;

	/* ���������ĸ߶Ȳ�ܳ���1 */
	if( lh+1 < rh || rh+1 < lh )
		n++;

	/* �߶�==�Ӹ߶�+1 */
	if( node->height != (lh>rh?lh:rh)+1 )
		n++;

	if( node->son_num != lson + rson )
		n++;

	/* �Ϸ�˳��: left <= father <= right */
	if( ( node->left && Compare( node, node->left, 0 ) < 0 ) ||
		( node->right &&  Compare( node->right, node, 0 ) < 0  ) )
	{
		n++;
	}
	return n;
}


/* ����ת */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::RotateLeft(SQueueNode** ppstRoot)
{
/*          A                C 
           / \              / \ 
          B   C     =>     A   E 
          |  / \          / \  | 
            D   E        B   D + 
            |   |        |   | 
                + 
*/
	SQueueNode* A = *ppstRoot;
	SQueueNode* C = A->right;
	SQueueNode* D = C->left;

	if (A->parent)
	{
		if (A->parent->left == A)
			A->parent->left = C;
		else
			A->parent->right = C;
	}

	C->parent = A->parent;
	A->right = D;
	if (D)
		D->parent = A;

	C->left = A;
	if (A)
		A->parent = C;

	/* ����A�ĸ߶� */
	RefreshNode( A );
	/* ����C�ĸ߶� */
	RefreshNode( C );

	*ppstRoot = C;

	return 0;
}

/* �ҵ���ת */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::RotateRight(SQueueNode** ppstRoot)
{ 
/*          A                B 
           / \              / \ 
          B   C     =>     D   A 
         / \  |            |  / \ 
        D   E              + E   C 
        |   |                |   | 
        + 
*/
	SQueueNode* A = *ppstRoot;
	SQueueNode* B = A->left;
	SQueueNode* E = B->right;

	if (A->parent)
	{
		if (A->parent->left == A)
			A->parent->left = B;
		else
			A->parent->right = B;
	}

	B->parent = A->parent;
	A->left = E;
	if (E)
		E->parent = A;

	B->right = A;
	if (A)
		A->parent = B;

	/* ����A�ĸ߶� */
	RefreshNode( A );
	/* ����B�ĸ߶� */
	RefreshNode( B );

	*ppstRoot = B;
	return 0;
}

/* ���ҵ��� */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::LeftBalance( SQueueNode** ppstRoot )
{
	int lh, rh;
	SQueueNode* A = *ppstRoot;
	SQueueNode* B = A->left;
	SQueueNode* E = B->right;

	lh = B->left ? B->left->height : 0;
	rh = B->right ? B->right->height : 0;

	/* ���, LL �� */
	if( rh <= lh )
	{
		/*          A                B
		           / \              / \
		          B   C     =>     D   A
		         / \               |  / \
		        D   E              + E   C
		        |
		        +
		*/
		RotateRight(ppstRoot);
		if (m_pRoot == A)
			m_pRoot = B;
	}
	/* �Ҹ�, LR ��, ��ʱ��ı��Ŀ¼ֵ */
	else
	{
		/*          A                A                 E 
		           / \              / \              /   \ 
		          B   C    ==>     E   C    ==>     B     A 
		         / \ h|     L     / \        R     / \   / \ 
		        D   E            B   G            D   F G   C 
		       h|  / \          / \  | h-1       h|h-1| |h-1| h
		          F   G        D   F                  +
		      h-1 | h-1|      h| h-1 |
		          +                  +
		*/
		RotateLeft(&(*ppstRoot)->left);
		RotateRight(ppstRoot);
		if (m_pRoot == A)
			m_pRoot = E;
	}
	return 0;
}

/* ������ */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::RightBalance( SQueueNode** ppstRoot )
{
	int lh, rh;	
	SQueueNode* A = *ppstRoot;
	SQueueNode* C = A->right;
	SQueueNode* D = C->left;

	lh = C->left ? C->left->height : 0;
	rh = C->right ? C->right->height : 0;

	/* �Ҹ�, RR ��, ��ʱ����ı��Ŀ¼ֵ */
	if( lh <= rh )
	{
		/*          A                C 
		           / \              / \ 
		          B   C     =>     A   E 
		             / \          / \  | 
		            D   E        B   D + 
		                |              
		                + 
		*/
		RotateLeft(ppstRoot);
		if( m_pRoot == A )
			m_pRoot = C;
	}
	/* ���, RL ��, ��ʱ��ı��Ŀ¼ֵ */
	else
	{		
		/*          A                A                 D 
		           / \              / \              /   \ 
		          B   C    ==>     B   D    ==>     A     C 
		             / \    R         / \    L     / \   / \ 
		            D   E            F   C        B   F G   E 
		           / \                  / \             |     
		          F   G                G   E            + 
		              |                |     
		              +                +
		*/
		RotateRight(&(*ppstRoot)->right);
		RotateLeft(ppstRoot);
		if( m_pRoot == A )
			m_pRoot = D;
	}
	return 0;
}

/* ���ң�����key���ڽڵ�λ�� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Search(SQueueNode* pstRoot, void* key, VALUE_TYPE* value)
{
	int cmp;
	
	if( pstRoot == NULL )
		return NULL;

	cmp = Compare( key, &pstRoot->value, 1 );

	/* С�ڱ��ڵ㣬������������ */
	if( cmp < 0 )
		return Search( pstRoot->left, key, value );
	/* ���ڱ��ڵ㣬������������ */
	else if( 0 < cmp )
		return Search(pstRoot->right, key, value);
	/* ���Ǳ��ڵ� */
	else
	{
		if( value )
			*value = pstRoot->value;
		return pstRoot;
	}
	return NULL;
}

/* ��λָ����ŵ�Ԫ�� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::SeekTo( SQueueNode* pstRoot,int idx, VALUE_TYPE* value )
{
	if( pstRoot == NULL )
		return NULL;

	if( pstRoot->left )
	{
		if( idx <= pstRoot->left->son_num )
			return SeekTo( pstRoot->left, idx, value );
		else if( idx == pstRoot->left->son_num + 1 )
		{
			if( value )
				*value = pstRoot->value;
			return pstRoot;
		}
		else if( idx <= pstRoot->son_num )
			return SeekTo( pstRoot->right, idx-(pstRoot->left->son_num+2), value );
		else
			return NULL;
	}
	else if( idx == 0 )
	{
		if( value )
			*value = pstRoot->value;
		return pstRoot;
	}
	else
	{
		return SeekTo( pstRoot->right, idx-1, value );
	}
}


/* ���룬���ز���ڵ�λ�� */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Insert(SQueueNode** ppstRoot, SQueueNode* itFather, VALUE_TYPE& value )
{ 
	SQueueNode* pstRoot = *ppstRoot;
	SQueueNode* pInsert = NULL;
	int cmp, lh, rh;
	//extern int32_t g_mem_cnt;

	if( pstRoot == NULL )
	{
		if( m_pMallocProc )
			pInsert = (SQueueNode *)m_pMallocProc( sizeof(SQueueNode), m_pMemoryParam );
		else
		{
			if( m_pIdle )/* ���������нڵ�ɻ��� */
			{
				pInsert = m_pIdle;
				m_pIdle = m_pIdle->right;
			}
			else if( m_pCurr && m_iCurr < m_nGrowNum )/* �շ��������нڵ���� */
			{
				pInsert = m_pCurr + m_iCurr;
				m_iCurr++;
			}
			else
			{
				/* һ������һ����ڴ棬�ڴ���������ʱ��������ٶ� */
				m_pCurr = (SQueueNode *)XTC_MALLOC( sizeof(SQueueNode)*m_nGrowNum );
				pInsert = m_pCurr + 1;//�ѵ�һ���ڵ�ճ������ڴ������
				m_iCurr = 2;
				if ( m_pCurr == NULL )
				{
					printf(">>malloc error codec=%d",GetLastError());
				}
				//g_mem_cnt++;
				memset( m_pCurr, 0, sizeof(SQueueNode) );
				m_pCurr->right = m_pList;
				m_pList = m_pCurr;
			}
		}
		memset(pInsert, 0, sizeof(SQueueNode));

		/* ��֤�ڵ��е��඼������� */
		::new( (void *)&pInsert->value ) VALUE_TYPE;

		pInsert->value = value;
		pInsert->left = NULL;
		pInsert->right = NULL;
		pInsert->parent = itFather;
		pInsert->height = 1;

		m_iCount++;
		*ppstRoot = pInsert;
		return pInsert;
	}

	cmp = Compare( &value, &pstRoot->value, 0 );

	/* С�ڱ��ڵ���뵽������ */
	if( cmp < 0 )
		pInsert = (SQueueNode*)Insert( &pstRoot->left, pstRoot, value );
	/* ���ڱ��ڵ���ڰ����� */
	else if( cmp == 0 )
	{
		/* ������ӽڵ�Ϊ�գ��������� */
		if( pstRoot->left == NULL )
			pInsert = (SQueueNode*)Insert( &pstRoot->left, pstRoot, value );
		/* ������ӽڵ�Ϊ�գ�������ұ� */
		else if( pstRoot->right == NULL )
			pInsert = (SQueueNode*)Insert( &pstRoot->right, pstRoot, value );
		/* ���ڰ����� */
		else
		{
			if( pstRoot->left->height <= pstRoot->right->height )
				pInsert = (SQueueNode*)Insert( &pstRoot->left, pstRoot, value );
			else
				pInsert = (SQueueNode*)Insert( &pstRoot->right, pstRoot, value );
		}
	}
	/* ���ڱ��ڵ���뵽�������� */
	else
		pInsert = (SQueueNode*)Insert( &pstRoot->right, pstRoot, value );

	/* ����ڵ�ɹ� */
	if( pInsert )
	{
		lh = pstRoot->left ? pstRoot->left->height : 0;
		rh = pstRoot->right ? pstRoot->right->height : 0;

		/* ������������߶�����1������ƽ�� */
		if( lh + 1 < rh )
			RightBalance( &pstRoot );
		else if( rh + 1 < lh )
			LeftBalance( &pstRoot );

		RefreshBackward( pstRoot );
	}

	return pInsert;
}

/* ɾ�����е�ĳ���ڵ㣬����1��ʾɾ���ɹ���0��ʾɾ��ʧ�� */
template<class VALUE_TYPE>
bool CXtcSequence<VALUE_TYPE>::Remove(SQueueNode* pstRoot, void* key)
{
	int cmp, lh, rh;
	bool ret;

	if( pstRoot == NULL )
		return false;

	/* �뵱ǰ�ڵ�Ƚ� */
	cmp = Compare( key, &pstRoot->value, 1 );

	/* �����ǰ�ڵ㲻��Ҫɾ���Ľڵ㣬��������������Ѱ�� */
	if( cmp != 0 )
	{
		if( cmp < 0 )
			ret = Remove( pstRoot->left, key );
		else
			ret = Remove( pstRoot->right, key );

		/* ���ɾ���ɹ���Ҫ��֤��ɾ���㸸�ڵ��ƽ�� */
		if( ret )
		{
			lh = pstRoot->left ? pstRoot->left->height : 0;
			rh = pstRoot->right ? pstRoot->right->height : 0;
			if( lh + 1 < rh )
				RightBalance( &pstRoot );
			else if( rh + 1 < lh )
				LeftBalance( &pstRoot );
			RefreshBackward( pstRoot );
		}
		return ret;
	}

	RemoveByPosition( pstRoot );

	return true;
}


/* ǿ��ɾ��ĳ���ڵ㼰���ӽڵ㣬���ܶ�ƽ���Ӱ�� */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::ForceRemove( SQueueNode* node )
{
	if( node->left )
		ForceRemove( node->left );
	if( node->right )
		ForceRemove( node->right );

	/* �����ڵ��а������� */
	node->value.~VALUE_TYPE();
	/* �ͷ��ڴ� */
	if( m_pFreeProc )
		m_pFreeProc( node, m_pMemoryParam );
	else
	{
		node->right = m_pIdle;
		m_pIdle = node;
	}
}

/* ����ڵ�ĸ߶� */
template<class VALUE_TYPE>
inline void CXtcSequence<VALUE_TYPE>::RefreshNode( SQueueNode* node )
{
	if( node->left && node->right )
	{
		node->height = node->left->height > node->right->height ? 
					   node->left->height + 1 : node->right->height + 1;
		node->son_num = node->left->son_num + node->right->son_num + 2;
	}
	else if( node->left )
	{
		node->height = node->left->height + 1;
		node->son_num = node->left->son_num + 1;
	}
	else if( node->right )
	{
		node->height = node->right->height + 1;
		node->son_num = node->right->son_num + 1;
	}
	else
	{
		node->height = 1;
		node->son_num = 0;
	}
}

/* ���Լ���ʼ����ڵ㣬�������ߺ������ڵ��� */
template<class VALUE_TYPE>
inline int CXtcSequence<VALUE_TYPE>::RefreshBackward(  SQueueNode* node )
{
	int son_num, height, lh, rh, ret = 0;
	SQueueNode *tmp, *skip;

	tmp = node;
	while( tmp )
	{
		lh = tmp->left ? tmp->left->height : 0;
		rh = tmp->right ? tmp->right->height : 0;
		if( lh + 1 < rh )
		{
			RightBalance( &tmp );
			skip = tmp;
		}
		else if( rh + 1 < lh )
		{
			LeftBalance( &tmp );
			skip = tmp;
		}
		else
			skip = node;

		height = tmp->height;
		son_num = tmp->son_num;
		RefreshNode( tmp );

		/* ����ڵ���Ϣû�з����仯������ֹ���£���Ҫע�⣺
		   ��ǰ�ڵ��Լ��ոձ���ת�Ľڵ㶼�Ѿ����¹��ˣ��������ڴ��� */
		if( tmp != skip && tmp->height == height && tmp->son_num == son_num )
			break;

		tmp = tmp->parent;
		ret++;
	}
	return ret;
}

/* �Ƚ� */
template<class VALUE_TYPE>
inline int CXtcSequence<VALUE_TYPE>::Compare( void* node1, void* node2, bool flag )
{
	if( m_pCompareProc )
	{
		return m_pCompareProc( flag, node1, node2, m_pCompareParam );
	}
	return 0;
}

#endif /*__XTCSEQUENCE_H__*/
