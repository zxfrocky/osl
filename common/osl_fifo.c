#include "stdafx.h"
#include "osl.h"
#include "osl_mem.h"
#include "osl_mutex.h"
#include "osl_fifo.h"

/* 消息结构 */
typedef struct
{
	uint32_t type;
	char *buf;
	int32_t bufsize;
	int32_t datsize;
	uint32_t param;
}osl_fifo_msg_t;

/* 消息队列 */
typedef struct
{
	/* 消息队列 */
	osl_fifo_msg_t *msgs;
	/* 消息队列长度 */
	int32_t num;
	/* 消息队列中的头位置 */
	int32_t head;
	/* 消息队列中的尾位置 */
	int32_t tail;

	/* 控制消息队列进和出的信号量 */
	void* mutex;
}osl_fifo_t;


/* 初始化系统消息队列 */
void* osl_fifo_create( int32_t msg_size, int32_t msg_num )
{
	int32_t size, i;
	osl_fifo_t *q = NULL;

	size = sizeof(osl_fifo_t) + ( sizeof(osl_fifo_msg_t) + msg_size ) * msg_num;
	q = (osl_fifo_t *)malloc( size );
	if( q )
	{
		memset( q, 0, sizeof(osl_fifo_t) );
		q->msgs = (osl_fifo_msg_t *)(((char*)q) + sizeof(osl_fifo_t));
		q->num = msg_num;
		for( i=0; i<msg_num; i++ )
		{
			q->msgs[i].buf = ((char*)(q->msgs + msg_num)) + i * msg_size;
			q->msgs[i].bufsize = msg_size;
			q->msgs[i].datsize = 0;
		}

		q->mutex = osl_mutex_create();
		if( q->mutex == NULL )
			goto osl_fifo_FAILED;
	}
	return q;

osl_fifo_FAILED:

	osl_fifo_destroy( q );

	return NULL;
}

/* 释放队列资源 */
void osl_fifo_destroy( void* fifo )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;

	if( q->mutex )
		osl_mutex_destroy( q->mutex );

	free( q );
}

/* 向系统消息队列添加消息,0=sucess,-1=failed */
int32_t osl_fifo_post( void* fifo, uint32_t type, void *buf, int32_t size, uint32_t param )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;
	osl_fifo_msg_t *msg;
	int32_t next, rtn = -1;

	if( osl_mutex_lock( q->mutex, -1 ) != 0 )
		return -1;
	next = q->tail + 1;
	if( q->num <= next )
		next = 0;
	msg = &q->msgs[q->tail];

	/* 挤掉最早的数据 */
	if ( next == q->head && size <= msg->bufsize )
	{
		q->head += 1;
		if( q->num <= q->head )
			q->head = 0;
	}	
	
	if( next != q->head && size <= msg->bufsize )
	{
		msg->type = type;
		msg->datsize = size;
		if( buf && 0 < size )
			memcpy( msg->buf, buf, size );
		msg->param = param;
		q->tail = next;

		rtn = 0;
	}
	osl_mutex_unlock( q->mutex );
	return rtn;
}

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_read( void* fifo, uint32_t *type, void *buf, int32_t size, uint32_t *param)
{
	return osl_fifo_read_timeout( fifo, type, buf, size, param, -1 );
}


/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_read_timeout( void* fifo, uint32_t *type, void *buf, int32_t size, uint32_t *param, int32_t timeout )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;
	osl_fifo_msg_t *msg;
	int32_t rtn;
	uint32_t start, now;

	/* 如果消息队列为空，等待消息 */
	now = start = osl_get_ms();
	while( q->head == q->tail )
	{
		if( 0 <= timeout && ( start + timeout <= now || now < start ) )
			return -1;
		osl_usleep( 10 );
		now = osl_get_ms();
	}

	if( osl_mutex_lock( q->mutex, -1 ) != 0 )
		return -1;

	if( q->head != q->tail )
	{
		msg = &q->msgs[q->head];

		*type = msg->type;
		if( buf && 0 < msg->datsize && 0 < size )
		{
			if( msg->datsize < size )
				size = msg->datsize;
			memcpy( buf, msg->buf, size<msg->datsize ? size:msg->datsize ); 
			if( param )
				*param = msg->param;
		}
		q->head++;
		if( q->num <= q->head )
			q->head = 0;
		rtn = 0;
	}
	else
		rtn = -1;

	osl_mutex_unlock( q->mutex);

	return rtn;
}

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_get_all( void* fifo, uint32_t *type, void **buf, int32_t size, int32_t num, uint32_t *param)
{
	return osl_fifo_get_all_timeout( fifo, type, buf, size, num, param, -1 );
}

/* 从系统消息队列提取消息,0=sucess,-1=failed */
int32_t osl_fifo_get_all_timeout( void* fifo, uint32_t *type, void **buf, int32_t size, int32_t num, uint32_t *param, int32_t timeout )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;
	osl_fifo_msg_t *msg;
	int32_t rtn, i, head;

	/* 如果消息队列为空，等待消息 */
	//now = start = osl_get_ms();
	//while( q->head == q->tail )
	//{
	//	if( 0 <= timeout && ( start + timeout <= now || now < start ) )
	//		return -1;
	//	osl_usleep( 10 );
	//	now = osl_get_ms();
	//}

	if(q->head == q->tail)
		return -1;

	if( osl_mutex_lock( q->mutex, -1 ) != 0 )
		return -1;

	rtn = 0;
	head = q->head;
	for (i=0; i<num; i++)
	{
		if( head != q->tail )
		{
			msg = &q->msgs[head];

			*type = msg->type;
			if( ((uchar_t *)buf+size*i) && 0 < msg->datsize && 0 < size )
			{
				if( msg->datsize < size )
					size = msg->datsize;
				memcpy( (uchar_t *)buf+size*i, msg->buf, size<msg->datsize ? size:msg->datsize ); 
				if( param )
					*param = msg->param;
			}
			else
				printf("get error!\n");
			head++;
			if( q->num <= head )
				head = 0;
			rtn++;
		}
		else
			break;
	}
	osl_mutex_unlock( q->mutex);

	return rtn;
}

/* 清除消息队列*/
void osl_fifo_clear( void* fifo )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;
	q->tail = q->head = 0;
}

/* 取得系统消息队列消息数目 */
int32_t osl_fifo_get_num( void* fifo )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;
	return ( q->tail - q->head + q->num ) % q->num;
}

/* 取得系统消息队列大小 */
int32_t osl_fifo_get_size( void* fifo )
{
	osl_fifo_t *q = (osl_fifo_t *)fifo;
	return q->num;
}

