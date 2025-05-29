#ifndef __XTCSEQUENCE_H__
#define __XTCSEQUENCE_H__

#include "Xtc.h"

/* 有序序列，用平衡二叉树实现 */
template<class VALUE_TYPE>
class CXtcSequence
{
/* 比较回调函数 */
typedef int (*PSequenceCompareCallback)( bool item1_is_key, void* item1, void* item2, void *param );
public:
	/* 二叉树节点 */
	typedef struct tagQueueNode
	{
		/* 值，必须放在首位，以保证position指针可以直接转化为VALUE指针 */
		VALUE_TYPE value;
		/* 树高度 */
		int height;
		/* 子节点个数（*/
		int son_num;
		/* 父节点 */
		struct tagQueueNode* parent;
		/* 左子节点 */
		struct tagQueueNode* left;
		/* 右子节点 */
		struct tagQueueNode* right;
	}SQueueNode;

	/* 构造、析构函数 */
	CXtcSequence();
	virtual ~CXtcSequence();

	/* 设置内存分配释放回调函数 */
	void SetMemoryCallback( PMallocCallback pMallocProc, PFreeCallback pFreeProc, void* param );
	/* 设置比较回调函数 */
	void SetCompareCallback( PSequenceCompareCallback pCompareProc, void* param );
	/* 设置内存增长速度 */
	void SetGrowNum( int num = 1024 );

	/* 插入，返回该元素所在的指针位置 */
	void* Insert( VALUE_TYPE& value );
	/* 删除某个节点(不含子节点 */
	bool Remove( void *key );
	/* 删除指定节点 */
	void RemoveByPosition( void* position );
	/* 删除所有节点 */
	int32_t RemoveAll();
	/* 替换元素，KEY对应的VALUE，返回该元素所在的指针位置 */
	void* Replace( void* key, VALUE_TYPE& value );
	/* 查找，返回该元素所在的指针位置 */
	void* Search( void* key, VALUE_TYPE* value );

	/* 定位第一个元素，返回该元素的指针位置(NULL表示失败) */
	void* GetFirst( VALUE_TYPE* value );
	/* 定位下一个元素，返回该元素的指针位置(NULL表示失败) */
	void* GetNext( VALUE_TYPE* value, void* position );
	/* 定位上一个元素，返回该元素的指针位置(NULL表示失败) */
	void* GetPrev( VALUE_TYPE* value, void* position );
	/* 定位最后一个元素，返回该元素的指针位置(NULL表示失败) */
	void* GetLast( VALUE_TYPE* value );
	/* 定位指定序号的元素 */
	void* SeekTo( int idx, VALUE_TYPE* value );
	/* position -> index */
	int GetIndex( void *position );

	/* 从指针位置获得值 */
	VALUE_TYPE* GetValue( void* position );
	/* 获取元素个数 */
	int GetSize();
	/* 取得树高 */
	int GetHeight();

	/* 检查二叉树是否有错，返回错误个数，root=NULL表示从根开始查起 */
	int Check( void* root = NULL );

protected:

	/* 左单旋转 */
	int RotateLeft( SQueueNode** ppstRoot );
	/* 右单旋转 */
	int RotateRight( SQueueNode** ppstRoot );
	/* 左右单旋 */
	int LeftBalance( SQueueNode** ppstRoot );
	/* 右左单旋 */
	int RightBalance( SQueueNode** ppstRoot );

	/* 查找，返回key所在节点位置 */
	void* Search( SQueueNode* pstRoot, void* key, VALUE_TYPE* value );
	/* 定位指定序号的元素 */
	void* SeekTo( SQueueNode* pstRoot, int idx, VALUE_TYPE* value );

	/* 插入，返回插入节点位置 */
	void* Insert( SQueueNode** ppstRoot, SQueueNode* itFather, VALUE_TYPE& value );
	/* 删除树中的某个节点 */
	bool Remove( SQueueNode* pstRoot, void* key );
	/* 强制删除某个节点及其子节点，不管对平衡的影响 */
	void ForceRemove( SQueueNode* node );
	/* 计算节点的高度 */
	void RefreshNode( SQueueNode* node );
	/* 从自己开始向祖节点，调整树高和所含节点数 */
	int RefreshBackward( SQueueNode* node );
	/* 比较 */
	int Compare( void* node1, void* node2, bool flag );

	
private:
	/* 内存分配/释放函数 */
	PMallocCallback m_pMallocProc;
	PFreeCallback m_pFreeProc;
	void* m_pMemoryParam;
	int m_nGrowNum;

	/* 比较函数 */
	PSequenceCompareCallback m_pCompareProc;
	void* m_pCompareParam;

	SQueueNode* m_pRoot;
	int m_iCount;

	SQueueNode* m_pIdle;/* 空闲节点 */
	SQueueNode* m_pList;/* 内存 */
	SQueueNode* m_pCurr;/* 当前申请的内存指针 */
	int m_iCurr;/* 当前内存用到那里了 */
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

	RemoveAll();/* 没有直接释放内存，是为了调用每个节点的析构函数 */
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


/* 设置内存分配释放回调函数 */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::SetMemoryCallback( PMallocCallback pMallocProc, PFreeCallback pFreeProc, void* param )
{
	m_pMallocProc = pMallocProc;
	m_pFreeProc = pFreeProc;
	m_pMemoryParam = param;
}

/* 设置比较回调函数 */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::SetCompareCallback( PSequenceCompareCallback pCompareProc, void* param )
{
	m_pCompareProc = pCompareProc;
	m_pCompareParam = param;
}

/* 设置内存增长速度 */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::SetGrowNum( int num )
{
	m_nGrowNum = num;
}

/* 插入 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Insert( VALUE_TYPE& value )
{
	return Insert(&m_pRoot, NULL, value);
}

/* 删除树中的某个节点 */
template<class VALUE_TYPE>
bool CXtcSequence<VALUE_TYPE>::Remove( void* key )
{
	return Remove(m_pRoot, key);
}

/* 删除指定节点 */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::RemoveByPosition( void* position )
{
	int lh, rh;
	SQueueNode* pstRoot = (SQueueNode*)position;
	SQueueNode* replace = NULL;
	SQueueNode* parent = NULL;


	lh = pstRoot->left ? pstRoot->left->height : 0;
	rh = pstRoot->right ? pstRoot->right->height : 0;

	/* 如果左子树高于右子树, 那么选择左子树中最大KEY替换填充该被删节点 */
	if( rh <= lh )
	{
		replace = pstRoot->left;
		while( replace && replace->right )
			replace = replace->right;

		/* 从原来的位置拆除replace */
		if( replace )
		{
			XTC_ASSERT( replace->right == NULL );

			/* 该最大KEY节点有左子女 (必无右子女) */
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
			/* 该最大KEY节点无左子女 */
			else
			{
				if( replace->parent->left == replace )
					replace->parent->left = NULL;
				else
					replace->parent->right = NULL;
			}
		}
	}
	/* 如果左子树低于右子树, 那么选择右子树中最小KEY替换填充该被删节点 */
	else if ( lh < rh )
	{
		replace = pstRoot->right;
		while( replace && replace->left )
			replace = replace->left;

		/* 从原来的位置拆除replace */
		if( replace )
		{
			XTC_ASSERT( replace->left == NULL );

			/* 该最小KEY节点有右子女 (必无左子女) */
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
			/* 该最小KEY节点无右子女 */
			else
			{
				if( replace->parent->left == replace )
					replace->parent->left = NULL;
				else
					replace->parent->right = NULL;
			}
		}
	}

	/* 被拆处父节点有可能失衡，需调整 */
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

	/* 将拆下来的节点接到父节点上 */
	parent = pstRoot->parent;
	if( parent )
	{
		if( parent->left == pstRoot )
			parent->left = replace;
		else
			parent->right = replace;
	}

	/* 用拆下来的节点替换被删除节点 */
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

	/* 析构节点中包含的类 */
	pstRoot->value.~VALUE_TYPE();
	/* 释放内存 */
	if( m_pFreeProc )
		m_pFreeProc( pstRoot, m_pMemoryParam );
	else
	{
		pstRoot->right = m_pIdle;
		m_pIdle = pstRoot;
	}
}


/* 删除所有数据 */
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

/* 替换 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Replace( void* key, VALUE_TYPE& value )
{
	SQueueNode* node;

	node = (SQueueNode*)Search( m_pRoot, key, NULL );
	if( node )
		node->value = value;

	return node;
}


/* 查找 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Search( void* key, VALUE_TYPE* value )
{ 
	return Search( m_pRoot, key, value );
}

/* 定位第一个元素，返回该元素的位置，返回NULL表示无元素 */
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

/* 定位下一个元素，返回该元素的位置，返回NULL表示结束 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::GetNext( VALUE_TYPE* value, void* position )
{
	SQueueNode* node = (SQueueNode*)position;

	/* 如果有右子树，则next为右子树的最左节点 */
	if( node->right )
	{
		node = node->right;
		while (node->left)
			node = node->left;
	}
	/* 如果无右子树，则next在祖宗节点上，本节点为祖宗节点的左支 */
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

/* 定位上一个元素，返回该元素的位置，返回NULL表示结束 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::GetPrev( VALUE_TYPE* value, void* position )
{
	SQueueNode* node = (SQueueNode*)position;

	/* 如果有左子树，则next为左子树的最右节点 */
	if( node->left )
	{
		node = node->left;
		while (node->right)
			node = node->right;
	}
	/* 如果无左子树，则next在祖宗节点上，本节点为祖宗节点的右支 */
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


/* 定位最后一个元素，返回该元素的位置，返回NULL表示无元素 */
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

/* 定位指定序号的元素 */
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
		/* 如果是祖节点的右子结点，祖节点及其左子节点都比自己小 */
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

/* 从指针位置获得值 */
template<class VALUE_TYPE>
VALUE_TYPE* CXtcSequence<VALUE_TYPE>::GetValue( void* position )
{
	return &((SQueueNode*)position)->value;
}

/* 获取元素个数 */
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

/* 取得树高 */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::GetHeight()
{
	if( m_pRoot )
		return m_pRoot->height;
	else
		return 0;
}

/* 检查二叉树是否有错，返回错误个数，root=NULL表示从根开始查起 */
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

	/* 左右子树的高度差不能超过1 */
	if( lh+1 < rh || rh+1 < lh )
		n++;

	/* 高度==子高度+1 */
	if( node->height != (lh>rh?lh:rh)+1 )
		n++;

	if( node->son_num != lson + rson )
		n++;

	/* 合法顺序: left <= father <= right */
	if( ( node->left && Compare( node, node->left, 0 ) < 0 ) ||
		( node->right &&  Compare( node->right, node, 0 ) < 0  ) )
	{
		n++;
	}
	return n;
}


/* 左单旋转 */
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

	/* 调整A的高度 */
	RefreshNode( A );
	/* 调整C的高度 */
	RefreshNode( C );

	*ppstRoot = C;

	return 0;
}

/* 右单旋转 */
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

	/* 调整A的高度 */
	RefreshNode( A );
	/* 调整B的高度 */
	RefreshNode( B );

	*ppstRoot = B;
	return 0;
}

/* 左右单旋 */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::LeftBalance( SQueueNode** ppstRoot )
{
	int lh, rh;
	SQueueNode* A = *ppstRoot;
	SQueueNode* B = A->left;
	SQueueNode* E = B->right;

	lh = B->left ? B->left->height : 0;
	rh = B->right ? B->right->height : 0;

	/* 左高, LL 型 */
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
	/* 右高, LR 型, 此时会改变跟目录值 */
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

/* 右左单旋 */
template<class VALUE_TYPE>
int CXtcSequence<VALUE_TYPE>::RightBalance( SQueueNode** ppstRoot )
{
	int lh, rh;	
	SQueueNode* A = *ppstRoot;
	SQueueNode* C = A->right;
	SQueueNode* D = C->left;

	lh = C->left ? C->left->height : 0;
	rh = C->right ? C->right->height : 0;

	/* 右高, RR 型, 此时不会改变跟目录值 */
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
	/* 左高, RL 型, 此时会改变跟目录值 */
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

/* 查找，返回key所在节点位置 */
template<class VALUE_TYPE>
void* CXtcSequence<VALUE_TYPE>::Search(SQueueNode* pstRoot, void* key, VALUE_TYPE* value)
{
	int cmp;
	
	if( pstRoot == NULL )
		return NULL;

	cmp = Compare( key, &pstRoot->value, 1 );

	/* 小于本节点，继续找左子树 */
	if( cmp < 0 )
		return Search( pstRoot->left, key, value );
	/* 大于本节点，继续找右子树 */
	else if( 0 < cmp )
		return Search(pstRoot->right, key, value);
	/* 就是本节点 */
	else
	{
		if( value )
			*value = pstRoot->value;
		return pstRoot;
	}
	return NULL;
}

/* 定位指定序号的元素 */
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


/* 插入，返回插入节点位置 */
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
			if( m_pIdle )/* 空闲链表有节点可回收 */
			{
				pInsert = m_pIdle;
				m_pIdle = m_pIdle->right;
			}
			else if( m_pCurr && m_iCurr < m_nGrowNum )/* 刚分配数组有节点可用 */
			{
				pInsert = m_pCurr + m_iCurr;
				m_iCurr++;
			}
			else
			{
				/* 一次申请一大块内存，在大数据量的时候能提高速度 */
				m_pCurr = (SQueueNode *)XTC_MALLOC( sizeof(SQueueNode)*m_nGrowNum );
				pInsert = m_pCurr + 1;//把第一个节点空出来做内存管理用
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

		/* 保证节点中的类都被构造过 */
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

	/* 小于本节点插入到左子树 */
	if( cmp < 0 )
		pInsert = (SQueueNode*)Insert( &pstRoot->left, pstRoot, value );
	/* 等于本节点挂在矮树上 */
	else if( cmp == 0 )
	{
		/* 如果左子节点为空，则挂在左边 */
		if( pstRoot->left == NULL )
			pInsert = (SQueueNode*)Insert( &pstRoot->left, pstRoot, value );
		/* 如果右子节点为空，则挂在右边 */
		else if( pstRoot->right == NULL )
			pInsert = (SQueueNode*)Insert( &pstRoot->right, pstRoot, value );
		/* 挂在矮树上 */
		else
		{
			if( pstRoot->left->height <= pstRoot->right->height )
				pInsert = (SQueueNode*)Insert( &pstRoot->left, pstRoot, value );
			else
				pInsert = (SQueueNode*)Insert( &pstRoot->right, pstRoot, value );
		}
	}
	/* 大于本节点插入到右子树， */
	else
		pInsert = (SQueueNode*)Insert( &pstRoot->right, pstRoot, value );

	/* 插入节点成功 */
	if( pInsert )
	{
		lh = pstRoot->left ? pstRoot->left->height : 0;
		rh = pstRoot->right ? pstRoot->right->height : 0;

		/* 如果左右子树高度相差超过1，调整平衡 */
		if( lh + 1 < rh )
			RightBalance( &pstRoot );
		else if( rh + 1 < lh )
			LeftBalance( &pstRoot );

		RefreshBackward( pstRoot );
	}

	return pInsert;
}

/* 删除树中的某个节点，返回1表示删除成功，0表示删除失败 */
template<class VALUE_TYPE>
bool CXtcSequence<VALUE_TYPE>::Remove(SQueueNode* pstRoot, void* key)
{
	int cmp, lh, rh;
	bool ret;

	if( pstRoot == NULL )
		return false;

	/* 与当前节点比较 */
	cmp = Compare( key, &pstRoot->value, 1 );

	/* 如果当前节点不是要删除的节点，则在左右子树中寻找 */
	if( cmp != 0 )
	{
		if( cmp < 0 )
			ret = Remove( pstRoot->left, key );
		else
			ret = Remove( pstRoot->right, key );

		/* 如果删除成功，要保证被删除点父节点的平衡 */
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


/* 强制删除某个节点及其子节点，不管对平衡的影响 */
template<class VALUE_TYPE>
void CXtcSequence<VALUE_TYPE>::ForceRemove( SQueueNode* node )
{
	if( node->left )
		ForceRemove( node->left );
	if( node->right )
		ForceRemove( node->right );

	/* 析构节点中包含的类 */
	node->value.~VALUE_TYPE();
	/* 释放内存 */
	if( m_pFreeProc )
		m_pFreeProc( node, m_pMemoryParam );
	else
	{
		node->right = m_pIdle;
		m_pIdle = node;
	}
}

/* 计算节点的高度 */
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

/* 从自己开始向祖节点，调整树高和所含节点数 */
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

		/* 如果节点信息没有发生变化，则终止更新，但要注意：
		   当前节点以及刚刚被旋转的节点都已经更新过了，不包含在此列 */
		if( tmp != skip && tmp->height == height && tmp->son_num == son_num )
			break;

		tmp = tmp->parent;
		ret++;
	}
	return ret;
}

/* 比较 */
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
