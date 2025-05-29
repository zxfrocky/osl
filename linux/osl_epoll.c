#include "stdafx.h"
#include "osl_epoll.h"
#include "sys/epoll.h"

/* 生成一个epoll专用的文件描述符，max_num是指定生成描述符的最大范围 */
void* osl_epoll_create(int32_t max_num)
{
	return (void *)epoll_create(max_num);
}

/* 销毁epoll */
void osl_epoll_destroy(void* handle)
{
	close((int)handle);
}

/* 功能：用于控制某个文件描述符上的事件，可以注册事件，修改事件，删除事件。
    @epfd：由 epoll_create 生成的epoll专用的文件描述符；
    @op：要进行的操作，EPOLL_CTL_ADD 注册、EPOLL_CTL_MOD 修改、EPOLL_CTL_DEL 删除；
    @fd：关联的文件描述符；
    @event：指向epoll_event的指针；
    成功：0；失败：-1
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

/* 功能：该函数用于轮询I/O事件的发生；
    @epfd：由epoll_create 生成的epoll专用的文件描述符；
    @epoll_event：用于回传代处理事件的数组；
    @maxevents：每次能处理的事件数；
    @timeout：等待I/O事件发生的超时值；
    成功：返回发生的事件数；失败：-1
*/
int32_t osl_epoll_wait(void* handle, SEpollEvent *events, int maxevents, int timeout)
{
	return epoll_wait((int)handle, events, maxevents, timeout);
}


