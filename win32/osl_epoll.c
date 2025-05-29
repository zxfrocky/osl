#include "stdafx.h"
#include "osl_epoll.h"
#include "osl_mutex.h"

typedef struct
{
	SOCKET fd;
	SEpollEvent ee;
}SEpollUnit;

typedef struct
{
	SEpollUnit *queue;
	int32_t max_num;
	int32_t num;
	void *mutex;
}SEpoll;


/* ����һ��epollר�õ��ļ���������max_num��ָ�����������������Χ */
void* osl_epoll_create(int32_t max_num)
{
	SEpoll *epoll;
	int32_t size;

	size = sizeof(SEpoll) + max_num * sizeof(SEpollUnit);
	epoll = (SEpoll *)malloc( size );
	if( epoll )
	{
		memset( epoll, 0, size );
		epoll->queue = (SEpollUnit *)(epoll + 1);
		epoll->max_num = max_num;
		epoll->mutex = osl_mutex_create();
	}
	return epoll;
}


/* ����epoll */
void osl_epoll_destroy(void* handle)
{
	SEpoll *epoll = (SEpoll *)handle;
	if( epoll->mutex )
		osl_mutex_destroy( epoll->mutex );
	free( handle );
}


/* ���ܣ����ڿ���ĳ���ļ��������ϵ��¼�������ע���¼����޸��¼���ɾ���¼���
    @epfd���� epoll_create ���ɵ�epollר�õ��ļ���������
    @op��Ҫ���еĲ�����EPOLL_CTL_ADD ע�ᡢEPOLL_CTL_MOD �޸ġ�EPOLL_CTL_DEL ɾ����
    @fd���������ļ���������
    @event��ָ��epoll_event��ָ�룻
    �ɹ���0��ʧ�ܣ�-1
*/
int32_t osl_epoll_ctl(void* handle, uint32_t op, SOCKET fd, SEpollEvent *event)
{
	SEpoll *epoll = (SEpoll *)handle;
	int32_t i;
	int32_t ret = -1;

	osl_mutex_lock( epoll->mutex, -1 );
	switch( op )
	{
	case OSL_EPOLL_CTL_ADD:
		for( i=0; i<epoll->num; i++ )
		{
			if( epoll->queue[i].fd == (SOCKET)fd )
				break;
		}
		if( epoll->num <= i && epoll->num < epoll->max_num)
		{
			epoll->queue[epoll->num].fd = fd;
			epoll->queue[epoll->num].ee = *event;
			epoll->num++;
			ret = 0;
		}
		break;

	case OSL_EPOLL_CTL_MOD:
		for( i=0; i<epoll->num; i++ )
		{
			if( epoll->queue[i].fd == (SOCKET)fd )
			{
				epoll->queue[i].ee = *event;
				ret = 0;
				break;
			}
		}
		break;

	case OSL_EPOLL_CTL_DEL:
		for( i=0; i<epoll->num; i++ )
		{
			if( epoll->queue[i].fd == (SOCKET)fd )
			{
				while( i < epoll->num-1 )
				{
					epoll->queue[i] = epoll->queue[i+1];
					i++;
				}
				epoll->num--;
				ret = 0;
				break;
			}
		}
		break;
	}
	osl_mutex_unlock( epoll->mutex );

	return ret;
}

/* ���ܣ��ú���������ѯI/O�¼��ķ�����
    @epfd����epoll_create ���ɵ�epollר�õ��ļ���������
    @epoll_event�����ڻش��������¼������飻
    @maxevents��ÿ���ܴ�����¼�����
    @timeout���ȴ�I/O�¼������ĳ�ʱֵ��
    �ɹ������ط������¼�����ʧ�ܣ�-1
*/
int32_t osl_epoll_wait(void* handle, SEpollEvent *events, int maxevents, int timeout)
{
	SEpoll *epoll = (SEpoll *)handle;
	SEpollUnit *p;
	fd_set rset, wset;
	int32_t i, n;
	int32_t ret;
	struct timeval tv;

	osl_mutex_lock( epoll->mutex, -1 );

	n = 0;
	if( 0 < epoll->num )
	{
		FD_ZERO( &rset );
		FD_ZERO( &wset );
		for( i=0; i<epoll->num; i++ )
		{
			p = epoll->queue + i;
			if( (p->ee.events & OSL_EPOLL_IN) == OSL_EPOLL_IN && rset.fd_count < FD_SETSIZE )
				rset.fd_array[rset.fd_count++] = p->fd;
			if( (p->ee.events & OSL_EPOLL_OUT) == OSL_EPOLL_OUT && wset.fd_count < FD_SETSIZE )
				wset.fd_array[wset.fd_count++] = p->fd;
		}

		osl_mutex_unlock( epoll->mutex );
		if( 0 <= timeout )
		{

			tv.tv_sec = timeout/1000;
			tv.tv_usec = (timeout%1000)*1000;
			select( 0, &rset, &wset, NULL, &tv );
		}
		else
			select( 0, &rset, &wset, NULL, NULL );
		osl_mutex_lock( epoll->mutex, -1 );

		memset( events, 0, sizeof(SEpollEvent)*maxevents );
		for( i=0; i<epoll->num && n<maxevents; i++ )
		{
			p = epoll->queue + i;
			ret = 0;
			events[n].data = p->ee.data;
			if( FD_ISSET(p->fd, &rset) )
			{
				events[n].events |= OSL_EPOLL_IN;
				ret = 1;
			}
			if( FD_ISSET(p->fd, &wset) )
			{
				events[n].events |= OSL_EPOLL_OUT;
				ret = 1;
			}
			n += ret;
		}
	}
	osl_mutex_unlock( epoll->mutex );
	return n;
}
