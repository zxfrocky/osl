#include "stdafx.h"
#include "osl_socket.h"
#include <sys/socket.h>

/* ����socket, af=AF_INET,..., type=SOCK_STREAM, ... , protocol=IPPROTO_TCP,... */
SOCKET osl_socket_create( int32_t af, int32_t type, int32_t protocol )
{
	SOCKET skt = -1;
	//int32_t bufsize = 1024*1024*5;
	struct linger lgr;

	skt = socket( af, type, protocol );
	if( skt != -1 )
	{
		lgr.l_onoff=1;
		lgr.l_linger=0;
		setsockopt( skt, SOL_SOCKET , SO_LINGER , &lgr , sizeof(lgr) );
	//	setsockopt( skt, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize, sizeof(bufsize) );
	//	setsockopt( skt, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof(bufsize) );
	}
	return skt;
}

/* ����socket */
void osl_socket_destroy( SOCKET skt )
{
	if( skt != -1 )
		close( skt );
}

/* ����Զ�� */
int32_t osl_socket_connect( SOCKET skt, uint32_t ip, uint16_t port )
{
	struct sockaddr_in addr;
	
	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_addr = *((struct in_addr *)&ip);
	addr.sin_port = port;

	/* ��ʼ���� */
	return connect( skt, (struct sockaddr *)&addr, sizeof(addr) );
}

/* �󶨽��յ�ַ�Ͷ˿�, severʹ��bind()�뱾���󶨣�clientʹ��connect()��Զ���������� */
int32_t osl_socket_bind( SOCKET skt, uint32_t ip, uint16_t port )
{
	struct sockaddr_in local;

	memset( &local, 0, sizeof(local) );
	local.sin_family = AF_INET;
	if( ip == inet_addr("127.0.0.1") )
		local.sin_addr.s_addr = INADDR_ANY;
	else
		local.sin_addr.s_addr = ip;
	local.sin_port = port;
	return bind ( skt, (struct sockaddr *)&local, sizeof(local) );
}


/* ��ʼ����, maxΪ���Ҷ��г��ȣ�������Ϊ5 */
int32_t osl_socket_listen( SOCKET skt, int32_t max )
{
	return listen( skt, max );
}

/* �ȴ�Զ������ */
SOCKET osl_socket_accept( SOCKET skt, uint32_t *ip, uint16_t *port )
{
	struct sockaddr_in from;
	socklen_t slen = sizeof(from);

	/*�ȴ�CLIENT������*/
	skt = accept( skt, (struct sockaddr *)&from, &slen );
	if( 0 <= skt )
	{
		if( ip )
			*ip = from.sin_addr.s_addr;
		if( port )
			*port = from.sin_port;
	}

	return skt;
}

/* ����������� */
int32_t osl_socket_ioctl( SOCKET skt, int32_t type, uint32_t *val )
{
	return ioctl( skt, (long)type, val );
}

/* ���ò��� */
int32_t osl_socket_set_opt( SOCKET skt, int32_t level, int32_t optname, const char* optval, int32_t optlen )
{
	return setsockopt( skt, level, optname, optval, optlen );
}

/* ȡ�ò���*/
int32_t osl_socket_get_opt( SOCKET skt, int32_t level, int32_t optname, int32_t* optval, int32_t* optlen )
{
	return getsockopt( skt, level, optname, (void*)optval, (socklen_t*)optlen );
}

/* ����socket���󶨵ı��ص�ַ�˿� */
int32_t osl_socket_get_sockname( SOCKET skt, uint32_t *ip, uint16_t *port )
{
	struct sockaddr_in addr;
	int32_t ret, len = sizeof(addr);
	ret = getsockname( skt, (struct sockaddr*)&addr, &len );
	if( ip )
		*ip = addr.sin_addr.s_addr;
	if( port )
		*port = addr.sin_port;
	return ret;
}

/* ���״̬,timeout��λms, timeout=-1��ʾ���޵ȴ� */
int32_t osl_socket_select( SOCKET maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, int32_t timeout )
{
	struct timeval tv;

	if( 0 <= timeout )
	{
		tv.tv_sec = timeout/1000;
		tv.tv_usec = (timeout%1000)*1000;
		return select( maxfd, readfds, writefds, exceptfds, &tv );
	}
	else
		return select( maxfd, readfds, writefds, exceptfds, NULL );
}


/* �������� */
int32_t osl_socket_send( SOCKET skt, void *buf, int32_t size )
{
	return send( skt, buf, size, 0 );
}

/* �������� */
int32_t osl_socket_recv( SOCKET skt, void *buf, int32_t size )
{
	return recv( skt, buf, size, 0 );
}


/* ����ಥ��, ip�������鲥��ַ */
int32_t osl_socket_join( SOCKET skt, uint32_t ip )
{
	//int32_t ttl = 127;
	//int32_t fFlag = 1;
	struct ip_mreq mreq;

	memset( &mreq, 0, sizeof(mreq) );
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = INADDR_ANY;

	/* ������ȱʡ�Ż����� */
	//setsockopt( skt, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl) );
	//setsockopt( skt, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&fFlag, sizeof(fFlag) );

	/* ����ಥ�� */
	return setsockopt( skt, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq) );
}


/* �˳��鲥��, ip�������鲥��ַ  */
int32_t osl_socket_drop( SOCKET skt, uint32_t ip )
{
	struct ip_mreq mreq;
	memset( &mreq, 0, sizeof(mreq) );
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = INADDR_ANY;
	return setsockopt( skt, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq) );
}

/* �������� */
int32_t osl_socket_sendto( SOCKET skt, uint32_t ip, uint16_t port, void *buf, int32_t size )
{
	struct sockaddr_in sn;
	int32_t slen = sizeof(sn);

	memset( &sn, 0, sizeof(sn) );
	slen = sizeof(sn);
	sn.sin_family = AF_INET;
	sn.sin_addr.s_addr = ip;
	sn.sin_port=  port;

	return sendto( skt, buf, size, 0, (struct sockaddr *)&sn, slen );
}

/* �������� */
int32_t osl_socket_recvfrom( SOCKET skt, uint32_t *ip, uint16_t *port, void *buf, int32_t size )
{
	struct sockaddr_in from;
	socklen_t slen = sizeof( from );

	memset( &from, 0, sizeof(from) );
	slen = recvfrom( skt, buf, size, 0, (struct sockaddr *)&from, &slen );

	if( ip )
		*ip = from.sin_addr.s_addr;
	if( port )
		*port = from.sin_port;

	return slen;
}
