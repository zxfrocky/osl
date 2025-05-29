#ifndef __OSL_SOCKET_H__
#define __OSL_SOCKET_H__

/************************* 提醒  ********************************
	recv(),send(),recvfrom(),sendto()是可重入函数，可重入函数
	一定是线程安全的(反之不一定),所以可以在多线程中同时调用上述函数
 ***************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* 创建socket, 返回-1表示失败, type=SOCK_STREAM, ... , protocol=IPPROTO_TCP,... */
SOCKET osl_socket_create( int32_t af, int32_t type, int32_t protocol ); 

/* 销毁socket */
void osl_socket_destroy( SOCKET skt );

/* 连接远端 */
int32_t osl_socket_connect( SOCKET skt, uint32_t ip, uint16_t port );

/* 绑定接收地址和端口 */
int32_t osl_socket_bind( SOCKET skt, uint32_t ip, uint16_t port );

/* 开始监听, max为悬挂队列长度，建议设为5 */
int32_t osl_socket_listen( SOCKET skt, int32_t max );

/* 等待远程连接 */
SOCKET osl_socket_accept( SOCKET skt, uint32_t *ip, uint16_t *port ); 

/* 输入输出控制 */
int32_t osl_socket_ioctl( SOCKET skt, int32_t type, uint32_t *val );

/* 设置参数 */
int32_t osl_socket_set_opt( SOCKET skt, int32_t level, int32_t optname, const char* optval, int32_t optlen );

/* 取得参数*/
int32_t osl_socket_get_opt( SOCKET skt, int32_t level, int32_t optname, int32_t* optval, int32_t* optlen );

/* 返回socket所绑定的本地地址端口 */
int32_t osl_socket_get_sockname( SOCKET skt, uint32_t *ip, uint16_t *port );

/* 检测状态,timeout单位ms, timeout=-1表示无限等待 */
int32_t osl_socket_select( SOCKET maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, int32_t timeout );

/* 发送数据 */
int32_t osl_socket_send( SOCKET skt, void *buf, int32_t size );

/* 接收数据 */
int32_t osl_socket_recv( SOCKET skt, void *buf, int32_t size );

/* 加入多播组 */
int32_t osl_socket_join( SOCKET skt, uint32_t ip );

/* 退出组播组 */
int32_t osl_socket_drop( SOCKET skt, uint32_t ip );

/* 发送数据 */
int32_t osl_socket_sendto( SOCKET skt, uint32_t ip, uint16_t port, void *buf, int32_t size ); 

/* 接收数据 */
int32_t osl_socket_recvfrom( SOCKET skt, uint32_t *ip, uint16_t *port, void *buf, int32_t size );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_SOCKET_H__ */
