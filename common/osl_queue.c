#include "stdafx.h"
#include "osl.h"
#include "osl_mem.h"
#include "osl_mutex.h"
#include "osl_queue.h"

/* ��Ϣ�ṹ */
typedef struct
{
	uint32_t type;
	char *buf;
	int32_t bufsize;
	int32_t datsize;
	uint32_t param;
}osl_queue_msg_t;

/* ��Ϣ���� */
typedef struct
{
	/* ��Ϣ���� */
	osl_queue_msg_t *msgs;
	/* ��Ϣ���г��� */
	int32_t num;
	/* ��Ϣ�����е�ͷλ�� */
	int32_t head;
	/* ��Ϣ�����е�βλ�� */
	int32_t tail;

	/* ������Ϣ���н��ͳ����ź��� */
	void* mutex;
}osl_queue_t;


/* ��ʼ��ϵͳ��Ϣ���� */
void* osl_queue_create( int32_t msg_size, int32_t msg_num )
{
	int32_t size, i;
	osl_queue_t *q = NULL;

	size = sizeof(osl_queue_t) + ( sizeof(osl_queue_msg_t) + msg_size ) * msg_num;
	q = (osl_queue_t *)malloc( size );
	if( q )
	{
		memset( q, 0, sizeof(osl_queue_t) );
		q->msgs = (osl_queue_msg_t *)(((char*)q) + sizeof(osl_queue_t));
		q->num = msg_num;
		for( i=0; i<msg_num; i++ )
		{
			q->msgs[i].buf = ((char*)(q->msgs + msg_num)) + i * msg_size;
			q->msgs[i].bufsize = msg_size;
			q->msgs[i].datsize = 0;
		}

		q->mutex = osl_mutex_create();
		if( q->mutex == NULL )
			goto osl_queue_FAILED;
	}
	return q;

osl_queue_FAILED:

	osl_queue_destroy( q );

	return NULL;
}

/* �ͷŶ�����Դ */
void osl_queue_destroy( void* queue )
{
	osl_queue_t *q = (osl_queue_t *)queue;

	if( q->mutex )
		osl_mutex_destroy( q->mutex );

	free( q );
}

/* ��ϵͳ��Ϣ���������Ϣ,0=sucess,-1=failed */
int32_t osl_queue_post( void* queue, uint32_t type, void *buf, int32_t size, uint32_t param )
{
	osl_queue_t *q = (osl_queue_t *)queue;
	osl_queue_msg_t *msg;
	int32_t next, rtn = -1;

	if( osl_mutex_lock( q->mutex, -1 ) != 0 )
		return -1;
	next = q->tail + 1;
	if( q->num <= next )
		next = 0;
	msg = &q->msgs[q->tail];
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

/* ��ϵͳ��Ϣ������ȡ��Ϣ,0=sucess,-1=failed */
int32_t osl_queue_read( void* queue, uint32_t *type, void *buf, int32_t size, uint32_t *param )
{
	osl_queue_t *q = (osl_queue_t *)queue;
	osl_queue_msg_t *msg;
	int32_t rtn;

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


/* �����Ϣ����*/
void osl_queue_clear( void* queue )
{
	osl_queue_t *q = (osl_queue_t *)queue;
	q->tail = q->head = 0;
}

/* ȡ��ϵͳ��Ϣ������Ϣ��Ŀ */
int32_t osl_queue_get_num( void* queue )
{
	osl_queue_t *q = (osl_queue_t *)queue;
	return ( q->tail - q->head + q->num ) % q->num;
}

