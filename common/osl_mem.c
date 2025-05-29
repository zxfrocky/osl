#include "stdafx.h"
#include "osl_mem.h"


/* �������� */
#define ALIGN_SIZE(size)	((size+align) & ~align)
/* ָ��((char*)p+size)���� */ 
#define ALIGN_PTR(p,size)	((size+align+(int64_t)p) & ~align)


/* �ڴ�ڵ� */
typedef struct _MemNode  
{
#ifdef _MEM_DEBUG
	char file[16];
	int32_t line;
#endif
	int64_t size;
	struct _MemNode *next;
}SMemNode;

typedef struct
{
	/* ���������� */
	char *buf;
	/* ������������С */
	int64_t size; 
	/* �����ֽ��� */
	int32_t align;

	/* �������� */
	SMemNode *list;
	/* ��ǰ�Ŀ��п��� */
	SMemNode *first;
	/* tail�����Ŀ��п��� */
	SMemNode *tail;

	/* ��ʷ�����ռ���ڴ��� */
	int64_t history_max_size;
}SMemHeader;

#ifdef _MEM_DEBUG
static const char *osl_mem_get_file_name( const char *path )
{
	int32_t n = 0;

	while( path[n] != 0 )
		n++;
	n--;
	while( 0 < n )
	{
		if( path[n] == '/' || path[n] == '\\' )
			return path+n+1;
		n--;
	}
	return path;
}
#endif


/* �����ڴ����ģ�� */
void* osl_mem_create( char *buf, int64_t size, int32_t align )
{
#if 1 /* ͳһʹ��16�ֽڶ��� */
	SMemHeader *xm = (SMemHeader *)(((uint64_t)buf+15)&(~15)); 

	memset( xm, 0, sizeof(SMemHeader) );
	xm->align = align;
	xm->buf = (char *)ALIGN_PTR( xm, sizeof(SMemHeader) );
	xm->size = buf + size - xm->buf;
#else
	SMemHeader *xm = (SMemHeader *)(((align-1+(int32_t)buf)/align)*align);
	
	memset( xm, 0, sizeof(SMemHeader) );
	xm->align = align;
	xm->buf = (char *)ALIGN_PTR( xm, sizeof(SMemHeader) );
	xm->size = buf + size - xm->buf;
#endif
	return xm;
}

/* �����ڴ����ģ�� */
void osl_mem_destroy( void* h )
{
}

/* ��� */
void osl_mem_reset( void* h )
{
	SMemHeader *xm = (SMemHeader *)h;

	xm->list = NULL;
	xm->first = NULL;
	xm->tail = NULL;
	xm->history_max_size = 0;
}

/* ����ڴ�ڵ㣬������ʷ���������Ĺ������ߴ� */
int64_t osl_mem_check( void* h, PMemCheckCallback pProc, void* param )
{
	SMemHeader *xm = (SMemHeader *)h;
	SMemNode *node = xm->list;
	int64_t align = xm->align-1;
	char *p;
	while( node && pProc )
	{
		p = (char*)ALIGN_PTR(node,sizeof(SMemNode));

#ifdef _MEM_DEBUG
		pProc( p, node->size, node->file, node->line, param );
#else
		pProc( p, node->size, "", -1, param );
#endif

		node = node->next;
	}
	return xm->history_max_size;
}

/* �ҵ��ڴ�p�������е�λ�� */
static SMemNode* find(void* h, void *p)
{
	SMemHeader *xm = (SMemHeader *)h;
	SMemNode *node = xm->list;
	int64_t align = xm->align-1;
	
	while( node )
	{
		if( p == (char*)ALIGN_PTR(node,sizeof(SMemNode)) )
			break;
		node = node->next;
	}
	return node;
}

/* ɨ�������Ƿ��оɿ���ڴ���������Ҫ��; */
static void* malloc_old( void* h, int64_t size ,const char *file, int32_t line)
{
	SMemHeader *xm = (SMemHeader *)h;
	SMemNode *node, *new_node;
	int64_t align = xm->align-1;
	char *end;
	int64_t len; 
	int64_t firstlen;

	/* ���ͷ��buf֮��Ŀ�϶; */
	if( xm->first==NULL )
	{
		if( xm->list == NULL )
			return NULL;

		node = xm->list;
		len = ((int64_t)node) - ALIGN_PTR(xm->buf,sizeof(SMemNode));
		if( size <= len )
		{
			/* ���NODE */
			new_node = (SMemNode *)xm->buf;
			memset( new_node, 0, sizeof(SMemNode) );
#ifdef _MEM_DEBUG
			if( file )
			{
				osl_strncpy( new_node->file, osl_mem_get_file_name(file), sizeof(new_node->file)-1 );
				new_node->line = line;
			}
#endif
			new_node->size = ALIGN_SIZE( size );
			
			/* ���뵽����ͷ; */
			xm->list = new_node;
			new_node->next = node;
			xm->first = new_node;

			return (char*)ALIGN_PTR(new_node, sizeof(SMemNode)); 
		}
		firstlen = len;
	}
	else
	{
		node = xm->first;
		firstlen = 0;
	}
	
	/* �����������֮��ļ�϶�Ƿ������һ��ĳ���; */
	while( node->next )
	{
		end = (char*)ALIGN_PTR(node,sizeof(SMemNode)) + ALIGN_SIZE(node->size);
		len = ((char*)node->next) - (char*)ALIGN_PTR(end, sizeof(SMemNode));
		
		/* �����һ���п�̫С��������ͷ����ֽ��������޸����ָ�� */
		if( firstlen <= (int32_t)align )
		{
			xm->first = node;
			firstlen = len;
		}
		
		/* �ҵ�����Ҫ��Ŀ�; */
		if( size <= len )
		{
			/* ���NODE */
			new_node = (SMemNode *)end;
			memset( new_node, 0, sizeof(SMemNode) );
#ifdef _MEM_DEBUG
			if( file )
			{
				osl_strncpy( new_node->file, osl_mem_get_file_name(file), sizeof(new_node->file)-1 );
				new_node->line = line;
			}
#endif
			new_node->size = ALIGN_SIZE( size );
			
			/* ���뵽����; */
			new_node->next = node->next;
			node->next = new_node;
			
			return (char*)ALIGN_PTR(new_node, sizeof(SMemNode)); 
		}
		node=node->next;
	}
	return NULL;
}

/* �����ڴ� */
void *osl_mem_alloc( void* h, int64_t size, const char *file, int32_t line, int64_t *memsize )
{
	SMemHeader *xm = (SMemHeader*)h;
	SMemNode *tail = NULL;
	SMemNode *node;
	int64_t align = xm->align-1;
	char *p;

	size += (ALIGN_SIZE(sizeof(SMemNode)) - sizeof(SMemNode));

	/* �Ӿɿ���� */
	p = (char *)malloc_old( h, size, file, line );
	if( p )
	{
		if( xm->history_max_size < (p-xm->buf) + size )  // ָ��Ӧ�����޷������ͣ�����ָ������������п��ܵø����ġ�
			xm->history_max_size = (p-xm->buf) + size;
		if( memsize )
			*memsize = sizeof(SMemNode) + ALIGN_SIZE( size );
		return p;
	}
	/* ��һ������ */
	if( xm->list == NULL )
		node = (SMemNode *)xm->buf;
	else
	{
		tail = xm->tail;
		
		p = (char*)ALIGN_PTR( tail, sizeof(SMemNode) );	/* ����Ǹ�����buf */
		node = (SMemNode *)ALIGN_PTR( p, tail->size );	/* ����Ǹ�����buf��β��������Ϊ��һ����ַ */
	}
	p = (char*)ALIGN_PTR( node, sizeof(SMemNode) );	/* ��ȡ����buf */

	/* ����Ƿ����㹻���ڴ� */
	/* if( xm->buf+xm->size < p+size ) */
	if((int32_t)(xm->buf+xm->size-p) <= size )/* size���ܽϴ� */
	{
		if( memsize )
			*memsize = 0;
		return NULL;
	}
	/* ���node */
	memset( node, 0, sizeof(SMemNode) );
	node->size = ALIGN_SIZE( size );
#ifdef _MEM_DEBUG
		if( file )
		{
			osl_strncpy( node->file, osl_mem_get_file_name(file), sizeof(node->file)-1 );
			node->line = line;
		}
#endif
	
	/* �ѵ�ǰ�ڵ�ҵ�����β�� */
	if( tail == NULL )
		xm->list = node;
	else
		tail->next = node;

	if( xm->history_max_size < (p-xm->buf) + size )
		xm->history_max_size = (p-xm->buf) + size;
	
	xm->tail = node;

	if( memsize )
		*memsize = sizeof(SMemNode) + ALIGN_SIZE( size );

	return p;
}

/* ���������ڴ� */
void *osl_mem_realloc( void* h, void *p, int64_t size, const char *file, int32_t line )
{
	void *buf = NULL;
	SMemHeader *xm = (SMemHeader*)h;
	SMemNode* node = NULL;
	int64_t align = xm->align-1;
	
	/* ��λ */
	if( p )
	{
		node = find( h, p );
		if( node &&
			(size<=node->size ||
			(int64_t)ALIGN_PTR(node,sizeof(SMemNode))+size <= 
			(int64_t)( node->next ? node->next : (SMemNode*)(xm->buf + xm->size)) ) )
		{
			node->size = ALIGN_SIZE( size );
			return p;
		}
	}
	
	/* �ڴ治��,���·���; */
	buf = osl_mem_alloc( h, size, file, line, NULL );
	if( p )
	{
		/* ��������; */
		memcpy( buf, p, (size_t)(node ? node->size:size) );
		/* �ͷ�ԭ�����ڴ� */
		osl_mem_free( h, p, file, line );
	}
	
	return buf;
}

/* �����ַ��� */
char *osl_mem_strdup( void* h, const char *s, const char *file, int32_t line )
{
	char *p = (char*)osl_mem_alloc( h, strlen(s)+1, file, line, NULL  );
	if( p )
		memcpy(p,s,strlen(s)+1);
	return p;
}



/* �ͷ��ڴ� */
int64_t osl_mem_free( void* h, void *p, const char *file, int32_t line )
{
	SMemHeader *xm = (SMemHeader *)h;
	SMemNode *node;
	SMemNode *last = NULL;
	int64_t align = xm->align-1;
	int64_t size = 0;

	/* Ѱ��node */
	for( node=xm->list; node; node=node->next )
	{
		if( p == (void*)ALIGN_PTR(node,sizeof(SMemNode)) )
			break;
		last = node;
	}

	if( node )
	{
		size = sizeof(SMemNode) + node->size;

		/* �ͷŵ�Ϊͷ�ڵ� */
		if( node == xm->list )
			xm->list = node->next;
		else
			last->next = node->next;

		if( xm->first >= node )
			xm->first = last;
		if( xm->tail == node )
			xm->tail = last;
	}

	return size;
}

