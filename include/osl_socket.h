#ifndef __OSL_SOCKET_H__
#define __OSL_SOCKET_H__

/************************* ����  ********************************
	recv(),send(),recvfrom(),sendto()�ǿ����뺯���������뺯��
	һ�����̰߳�ȫ��(��֮��һ��),���Կ����ڶ��߳���ͬʱ������������
 ***************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* ����socket, ����-1��ʾʧ��, type=SOCK_STREAM, ... , protocol=IPPROTO_TCP,... */
SOCKET osl_socket_create( int32_t af, int32_t type, int32_t protocol ); 

/* ����socket */
void osl_socket_destroy( SOCKET skt );

/* ����Զ�� */
int32_t osl_socket_connect( SOCKET skt, uint32_t ip, uint16_t port );

/* �󶨽��յ�ַ�Ͷ˿� */
int32_t osl_socket_bind( SOCKET skt, uint32_t ip, uint16_t port );

/* ��ʼ����, maxΪ���Ҷ��г��ȣ�������Ϊ5 */
int32_t osl_socket_listen( SOCKET skt, int32_t max );

/* �ȴ�Զ������ */
SOCKET osl_socket_accept( SOCKET skt, uint32_t *ip, uint16_t *port ); 

/* ����������� */
int32_t osl_socket_ioctl( SOCKET skt, int32_t type, uint32_t *val );

/* ���ò��� */
int32_t osl_socket_set_opt( SOCKET skt, int32_t level, int32_t optname, const char* optval, int32_t optlen );

/* ȡ�ò���*/
int32_t osl_socket_get_opt( SOCKET skt, int32_t level, int32_t optname, int32_t* optval, int32_t* optlen );

/* ����socket���󶨵ı��ص�ַ�˿� */
int32_t osl_socket_get_sockname( SOCKET skt, uint32_t *ip, uint16_t *port );

/* ���״̬,timeout��λms, timeout=-1��ʾ���޵ȴ� */
int32_t osl_socket_select( SOCKET maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, int32_t timeout );

/* �������� */
int32_t osl_socket_send( SOCKET skt, void *buf, int32_t size );

/* �������� */
int32_t osl_socket_recv( SOCKET skt, void *buf, int32_t size );

/* ����ಥ�� */
int32_t osl_socket_join( SOCKET skt, uint32_t ip );

/* �˳��鲥�� */
int32_t osl_socket_drop( SOCKET skt, uint32_t ip );

/* �������� */
int32_t osl_socket_sendto( SOCKET skt, uint32_t ip, uint16_t port, void *buf, int32_t size ); 

/* �������� */
int32_t osl_socket_recvfrom( SOCKET skt, uint32_t *ip, uint16_t *port, void *buf, int32_t size );


#ifdef __cplusplus
}
#endif

#endif /* __OSL_SOCKET_H__ */
