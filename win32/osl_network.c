#include "stdafx.h"
#include "osl.h"


/* DNS解析，返回IP地址，网络字节序 */
uint32_t osl_network_dns( const char *hostname )
{
	struct hostent *he;
	uint32_t ip = 0;

	he = gethostbyname( hostname );
	if( he )
		memcpy( &ip, he->h_addr_list[0], 4 );

	return ip;
}

/* 取得本机名称 */
int32_t osl_network_get_local_name( char *name, int32_t size )
{
	memset( name, 0, size );
	return gethostname( name, size-1 );
}

/* 取得本机IP地址 */
uint32_t osl_network_get_local_ip()
{
	char name[256];
	struct hostent *he;
	uint32_t ip = 0;
	
	memset( name, 0, sizeof(name) );
	gethostname( name, sizeof(name)-1 );

	he = gethostbyname( name );
	if( he )
		memcpy( &ip, he->h_addr_list[0], 4 );
	return ip;
}

/* 获取系统MAC地址,mac至少6个字节，返回0表示成功，-1表示失败 */
int32_t osl_network_get_mac( uchar_t *mac )
{
	static uchar_t local_mac[6] = {0,0,0,0,0,0};
	uint32_t ulSize;
    PIP_ADAPTER_INFO pInfo;
	int32_t i, ret = -1;

	if( local_mac[0] == 0 && local_mac[1] == 0 && local_mac[2] == 0 && 
		local_mac[3] == 0 && local_mac[4] == 0 && local_mac[5] == 0 )
	{
		ulSize = 0;
		pInfo = NULL;
		if( GetAdaptersInfo( pInfo, &ulSize ) == ERROR_BUFFER_OVERFLOW )/* 获取缓冲区大小 */
		{
			pInfo = (PIP_ADAPTER_INFO)malloc( ulSize );
			if( GetAdaptersInfo( pInfo, &ulSize ) == NO_ERROR )
			{
				for( i=0; i<6 && i<(int32_t)pInfo->AddressLength; i++ )
					local_mac[i] = pInfo->Address[i];
				memcpy( mac, local_mac, 6 );
				ret = 0;
			}
			free( pInfo );
		}
	}
	else
	{
		memcpy( mac, local_mac, 6 );
		ret = 0;
	}
	return ret;
}

/* 获取系统MAC地址,str:"00:01:26:12:34:56",mac至少6个字节,返回0表示成功,-1表示失败  */
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
