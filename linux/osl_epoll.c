#include "stdafx.h"
#include "osl_epoll.h"
#include "sys/epoll.h"

/* ����һ��epollר�õ��ļ���������max_num��ָ�����������������Χ */
void* osl_epoll_create(int32_t max_num)
{
	return (void *)epoll_create(max_num);
}

/* ����epoll */
void osl_epoll_destroy(void* handle)
{
	close((int)handle);
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
	if (op<1 || op>3)
	{
		// unknow epoll ctl option
		return -1;
	}
	return epoll_ctl((int)handle, op, fd, event);
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
	return epoll_wait((int)handle, events, maxevents, timeout);
}


