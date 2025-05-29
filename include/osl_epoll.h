#ifndef __OSL_EPOLL_H__
#define __OSL_EPOLL_H__

#include "stdafx.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef WIN32
#define OSL_EPOLL_CTL_ADD	0x01 /* 注册 */
#define OSL_EPOLL_CTL_MOD	0x02 /* 修改 */
#define OSL_EPOLL_CTL_DEL	0X03 /* 注销 */


#define OSL_EPOLL_IN		0x01 /* 表示对应的文件描述符可以读 */
#define OSL_EPOLL_OUT		0x02 /* 表示对应的文件描述符可以写 */
#define OSL_EPOLL_PRI		0x04 /* 表示对应的文件描述符有紧急的数可读 */
#define OSL_EPOLL_ERR		0x08 /* 表示对应的文件描述符发生错误 */
#define OSL_EPOLL_HUP		0x10 /* 表示对应的文件描述符被挂断 */
#define OSL_EPOLL_ET		0x20 /* ET的epoll工作模式 */

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

/* 生成一个epoll专用的文件描述符，num是指定生成描述符的最大范围，线程安全 */
void* osl_epoll_create(int32_t num);


/* 销毁epoll，线程安全 */
void osl_epoll_destroy(void* handle);


/* 功能：用于控制某个文件描述符上的事件，可以注册事件，修改事件，删除事件，线程安全。
    @epfd：由 epoll_create 生成的epoll专用的文件描述符；
    @op：要进行的操作，EPOLL_CTL_ADD 注册、EPOLL_CTL_MOD 修改、EPOLL_CTL_DEL 删除；
    @fd：关联的文件描述符；
    @event：指向epoll_event的指针；
    成功：0；失败：-1
*/
int32_t osl_epoll_ctl(void* handle, uint32_t op, SOCKET fd, SEpollEvent *event);


/* 功能：该函数用于轮询I/O事件的发生，线程安全；
    @epfd：由epoll_create 生成的epoll专用的文件描述符；
    @epoll_event：用于回传代处理事件的数组；
    @maxevents：每次能处理的事件数；
    @timeout：等待I/O事件发生的超时值；
    成功：返回发生的事件数；失败：-1

	注：事件发生后，注册在epfd上的socket fd的事件类型会被清空，所以如果下一个循环你
    还要关注这个socket fd的话，则需要用epoll_ctl(epfd,EPOLL_CTL_MOD,listenfd,&ev)来
    重新设置socket fd的事件类型
*/
int32_t osl_epoll_wait(void* handle, SEpollEvent *events, int32_t maxevents, int32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* __OSL_EPOLL_H__ */
