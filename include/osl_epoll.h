#ifndef __OSL_EPOLL_H__
#define __OSL_EPOLL_H__

#include "stdafx.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef WIN32
#define OSL_EPOLL_CTL_ADD	0x01 /* ע�� */
#define OSL_EPOLL_CTL_MOD	0x02 /* �޸� */
#define OSL_EPOLL_CTL_DEL	0X03 /* ע�� */


#define OSL_EPOLL_IN		0x01 /* ��ʾ��Ӧ���ļ����������Զ� */
#define OSL_EPOLL_OUT		0x02 /* ��ʾ��Ӧ���ļ�����������д */
#define OSL_EPOLL_PRI		0x04 /* ��ʾ��Ӧ���ļ��������н��������ɶ� */
#define OSL_EPOLL_ERR		0x08 /* ��ʾ��Ӧ���ļ��������������� */
#define OSL_EPOLL_HUP		0x10 /* ��ʾ��Ӧ���ļ����������Ҷ� */
#define OSL_EPOLL_ET		0x20 /* ET��epoll����ģʽ */

typedef union
{
	void *ptr;
	SOCKET fd;
	uint32_t u32;
	uint64_t u64;
}SEpollData;
typedef struct
{
	uint32_t events;		/* epoll event */
	SEpollData data;		/* User data variable */
}SEpollEvent;
#else
#include <sys/epoll.h>
typedef epoll_data_t SEpollData;
typedef struct epoll_event SEpollEvent;

#define OSL_EPOLL_CTL_ADD      EPOLL_CTL_ADD
#define OSL_EPOLL_CTL_DEL      EPOLL_CTL_DEL
#define OSL_EPOLL_CTL_MOD      EPOLL_CTL_MOD

#define OSL_EPOLL_IN       EPOLLIN
#define OSL_EPOLL_OUT      EPOLLOUT
#define OSL_EPOLL_PRI      EPOLLPRI
#define OSL_EPOLL_ERR      EPOLLERR
#define OSL_EPOLL_HUP      EPOLLHUP
#define OSL_EPOLL_ET       EPOLLET
#define OSL_EPOLL_LT	   EPOLLLT
#endif

/* ����һ��epollר�õ��ļ���������num��ָ�����������������Χ���̰߳�ȫ */
void* osl_epoll_create(int32_t num);


/* ����epoll���̰߳�ȫ */
void osl_epoll_destroy(void* handle);


/* ���ܣ����ڿ���ĳ���ļ��������ϵ��¼�������ע���¼����޸��¼���ɾ���¼����̰߳�ȫ��
    @epfd���� epoll_create ���ɵ�epollר�õ��ļ���������
    @op��Ҫ���еĲ�����EPOLL_CTL_ADD ע�ᡢEPOLL_CTL_MOD �޸ġ�EPOLL_CTL_DEL ɾ����
    @fd���������ļ���������
    @event��ָ��epoll_event��ָ�룻
    �ɹ���0��ʧ�ܣ�-1
*/
int32_t osl_epoll_ctl(void* handle, uint32_t op, SOCKET fd, SEpollEvent *event);


/* ���ܣ��ú���������ѯI/O�¼��ķ������̰߳�ȫ��
    @epfd����epoll_create ���ɵ�epollר�õ��ļ���������
    @epoll_event�����ڻش��������¼������飻
    @maxevents��ÿ���ܴ�����¼�����
    @timeout���ȴ�I/O�¼������ĳ�ʱֵ��
    �ɹ������ط������¼�����ʧ�ܣ�-1

	ע���¼�������ע����epfd�ϵ�socket fd���¼����ͻᱻ��գ����������һ��ѭ����
    ��Ҫ��ע���socket fd�Ļ�������Ҫ��epoll_ctl(epfd,EPOLL_CTL_MOD,listenfd,&ev)��
    ��������socket fd���¼�����
*/
int32_t osl_epoll_wait(void* handle, SEpollEvent *events, int32_t maxevents, int32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __OSL_EPOLL_H__ */
