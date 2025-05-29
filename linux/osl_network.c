#include "stdafx.h"
#include <setjmp.h>
#include <signal.h>
#include <sys/sem.h>
#include "osl.h"


/*���ڱ��溯����תʱ��ջ��������Ϣ�ı���*/
static sigjmp_buf m_jmpbuf;
static sigset_t m_mask;
static void osl_host_handler( int32_t signo );


/* DNS����������IP��ַ�������ֽ��� */
uint32_t osl_network_dns( const char *hostname )
{
	struct hostent *he;
	uint32_t ip = 0;

	he = gethostbyname( hostname );
	if( he )
		memcpy( &ip, he->h_addr_list[0], 4 );

	return ip;
}

/* ȡ�ñ������� */
int32_t osl_network_get_local_name( char *name, int32_t size )
{
	memset( name, 0, size );
	return gethostname( name, size-1 );
}

/* ȡ�ñ���IP��ַ */
uint32_t osl_network_get_local_ip()
{
	int s;
	struct ifconf conf;
	struct ifreq *ifr;
	struct sockaddr_in *sin;
	char buff[256];
	int num;
	int i;
	uint32_t ip = 0;

	s = socket(PF_INET, SOCK_DGRAM, 0);
	conf.ifc_len = 256;
	conf.ifc_buf = buff;

	ioctl(s, SIOCGIFCONF, &conf);
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;

	for(i=0;i < num;i++)
	{
		sin = (struct sockaddr_in *)(&ifr->ifr_addr);

		ioctl(s, SIOCGIFFLAGS, ifr);
		if(((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
		{
			ip = sin->sin_addr.s_addr;
			break;
		}
		ifr++;
	}
	close(s);
	return ip;
}


/* ���mac ��ַ*/
int64_t osl_network_get_mac()
{
	char mac[32];
	int32_t size = 0;
	int64_t value = 0;
	char byte;
	char *p = NULL;
	char *s = NULL, *e = NULL;
	char bit[2];
	int32_t i;

	memset( mac, 0, sizeof(mac) );
	if( osl_excute( "./config/getnetworkpara.sh eth0 mac", mac, sizeof(mac) ) == 0 )
	{
		p = (char *)&value;
		p += 5;
		size = strlen(mac);

		s = e = mac;
		while( s <= (mac + size) )
		{
			e = strstr( s, ":" );
			if ( NULL == e )
				e = s + 2;
			*e = 0;

			sprintf( bit, "%s", s );
			for ( i = 0; i <2; i++ )
			{
				if ( 'a' <= bit[i] && bit[i] <= 'f' )
					bit[i] = 10 + (bit[i] - 'a');
				else if ( 'A' <= bit[i] && bit[i] <= 'F')
					bit[i] = 10 + (bit[i] - 'A');
				else
					bit[i] = bit[i] - '0';
			}
			byte = bit[0]*16 + bit[1];
			memcpy( p, &byte, 1 );
			p--;

			s = e + 1;
		}
	}
	
	return value;
}


/* ��ȡϵͳMAC��ַ,str:"00:01:26:12:34:56",mac����6���ֽ�,����0��ʾ�ɹ�,-1��ʾʧ��  */
int32_t osl_network_convert_mac( char_t *str, uchar_t *mac )
{
	int32_t i;
	char_t *p, c;

	c = 0;
	i = 0;
	p = str;
	while( *p && p < str+17 && i < 6 )
	{
		if( '0' <= *p && *p <= '9' )
			c = (c << 4) + (*p - '0');
		else if( 'a' <= *p && *p <= 'f' )
			c = (c << 4) + (*p - 'a') + 10;
		else if( 'A' <= *p && *p <= 'F' )
			c = (c << 4) + (*p - 'A') + 10;
		else if( *p == ':' || *p == '-' )
			mac[i++] = c;
		else
			return -1;
		p++;
	}
	mac[5] = c;
	return 0;
}

static void osl_host_handler( int32_t signo )
{
	siglongjmp( m_jmpbuf, 1 );
}

/*������dns����������ip��ַ(0��ʾʧ��)�������ֽ���s_timeout ��ʾ��ʱʱ��, ��λ����(s) */
uint32_t osl_network_gethostbyname( const char *hostname, int32_t s_timeout )
{
	struct hostent hostinfo, *result;
	char buf[1024];
	int rc;
	uint32_t ip = 0;
	struct sigaction act;
	
	/*�źż����*/
	sigemptyset( &m_mask );
	/*����ʱ�ź���ӵ��źż���*/
	sigaddset( &m_mask, SIGALRM );

	/*���ò�׽�źŵĺ�����ַ*/
	act.sa_handler = osl_host_handler;
	act.sa_flags = 0;
	/*act�źż����*/
	sigemptyset(&act.sa_mask);

	/*�޸ĳ�ʱ�źŶ���*/
	if (sigaction(SIGALRM, &act, NULL) < 0 )
	{
		printf("sigaction error!\n");
		return 0;
	}
	
	/*��¼��ʱ�̶�ջ��������Ϣ�����ڳ�ʱ��ת*/
	if ( sigsetjmp ( m_jmpbuf, 1 ) == 1 )
	{
		/*ȡ����ʱ�źŵ�����*/
		alarm (0);
		/*ִ�е��˱�ʾ��ȡdnsʧ��(��ʱ)��ֱ�ӷ���*/
		printf( "gethostbyname timeout!!!\n" );
		return 0;
	}
	
	/*���ó�ʱ�ź�,��λ:��(s)*/
	alarm (s_timeout);

	/*���ÿ������gethostbyname*/
	if ( gethostbyname_r (hostname, &hostinfo, buf, 1024, &result, &rc) != 0 )
	{
		/*��ȡʧ�ܣ�ȡ����ʱ�źŲ�����*/
		printf( "gethostname failed!\n" );
		alarm(0);	
		return 0;
	} 
	else
	{
		/*�ɹ�*/
		alarm(0);
		memcpy( &ip, hostinfo.h_addr_list[0], 4 );
	}
	return ip;
}

