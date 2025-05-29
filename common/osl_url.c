#include "stdafx.h"
#include "osl_string.h"
#include "osl_url.h"


/* ����url��destURL�ṹ��,0��ʾ�����ɹ�,-1����ʧ�� */
int32_t osl_url_parse( char *url, SUrlInfo *info )
{
	char* p = NULL;


	memset( info, 0, sizeof(SUrlInfo) );

	p = strstr( url, "://" );
	if( p )
	{
		/* ����Э������ */
		p += 3;

		/* �û���:���룬��������:��ַ */
		if( *p != '/' )
		{
			info->hostname_pos = (int32_t)(p - url);
			while( *p )
			{
				if( *p == ':' )
				{
					info->hostname_len = (int32_t)(p - url - info->hostname_pos);
					p++;
					info->hostport_pos = (int32_t)(p - url);
				}
				else if( *p=='/' )
				{
					if( 0 < info->hostname_len )
						info->hostport_len = (int32_t)(p - url - info->hostport_pos);
					else
						info->hostname_len = (int32_t)(p - url - info->hostname_pos);
					break;
				}
				else if( *p == '@' )
				{
					/* username:userpswd������hostname:hostport������ */
					if( 0 < info->hostname_len )
						info->hostport_len = (int32_t)(p - url - info->hostport_pos);
					else
						info->hostname_len = (int32_t)(p - url - info->hostname_pos);
					info->username_pos = info->hostname_pos;
					info->username_len = info->hostname_len;
					info->userpswd_pos = info->hostport_pos;	
					info->userpswd_len = info->hostport_len;	

					info->hostname_pos = (int32_t)(p + 1 - url);
					info->hostname_len = 0;
					info->hostport_pos = 0;	
					info->hostport_len = 0;
				}
				p++;
			}
			if( *p == 0 )
			{
				if( 0 < info->hostname_len )
					info->hostport_len = (int32_t)(p - url - info->hostport_pos);
				else
					info->hostname_len = (int32_t)(p - url - info->hostname_pos);
			}
		}
	}
	else
		p = url;

	/* ·�� */
	info->path_pos = (int32_t)(p - url);
	while( *p && *p != '?' )
		p++;
	info->path_len = (int32_t)(p - url - info->path_pos);

	/* ���� */
	if( *p == '?' )
	{
		p++;
		info->param_pos = (int32_t)(p - url);
		while( *p )
			p++;
		info->param_len = (int32_t)(p - url - info->param_pos);
	}

	return 0;
}

/* ȡ��URL�еĲ��� */
char* osl_url_get_param( char *url, char *name, char *value, int32_t valuesize )
{
	int32_t i=0;
	char *p;

	p = strstr( url, name );
	/* ȷ���ҵ�name���� */
	while( p != NULL )
	{
		/* param name ǰ������Ƿֽ��,��������ǵȺ� */
		if( ( p == url  || ( p > url && 
			( *(p-1) == '?' || *(p-1) == ';' || *(p-1) == '&' || *(p-1) == ',') ) )
			&&  *(p+strlen(name)) == '=' )	 
		{
			/* ��λ��'=' */
			p = p + strlen(name);
			/* skip '=' */
			p++;
			
			/* ����һ���ֽ��,��Ϊ�ǽ��� */
			i=0;
			for(;;p++)
			{
				if( *p==';' || *p=='&' || *p==',' || *p=='\0' )
				{
					break;
				}
				if( i < valuesize-1 )
				{
					value[i] = *p;
					i++;
				}
				else
					break;
			} 
			value[i] = '\0';
			return value;
		}
   		else
		    p = p + strlen(name);
		
		/* û���ҵ�param name,���������param name */	
		p = strstr(p,name);
	}

	return NULL;
}

/* ��URL����ȡ���� */
int32_t osl_url_get_param_int( char *url, char *name )
{
	char value[64];
	if( osl_url_get_param( url, name, value, sizeof(value) ) )
		return atoi( value );
	else
		return -1;
}


/* ��URL���б��� */
int32_t osl_url_encode( char* url, int32_t urllen, char *buf, int32_t size )
{
	static char table[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	int32_t i = 0, n = 0;

	while( url[i] && n<size )
	{
	 	if( ' ' <= url[i] && url[i] <= '~' )
		{
			buf[n++] = url[i];
		}
		else
		{
	    	buf[n++] = '%';
	    	buf[n++] = table[((uchar_t)url[i])>>4];
	    	buf[n++] = table[((uchar_t)url[i])&0xf];
		}
		i++;
	}
	buf[n++]='\0';
	return n - 1;
}

/* ��URL���н��� */
int32_t osl_url_decode( char* url, int32_t urllen, char *buf, int32_t size )
{
	int32_t i = 0;
	char *p = url;

	while( /* *p != ' ' &&  *p != '?' && */*p != 0 && p-url < urllen && i < size-1 )
	{
		if( *p == '%' && *(p+1) != 0 && *(p+2) != 0 )
		{
			p++;
			if( '0' <= *p && *p <= '9' )
				buf[i] = *p - '0';
			else if( 'A' <= *p && *p <= 'Z' )
				buf[i] = *p - 'A' + 10;
			else if( 'a' <= *p && *p <= 'z' )
				buf[i] = *p - 'a' + 10;
			p++;
			if( '0' <= *p && *p <= '9' )
				buf[i] = (buf[i]<<4) + *p - '0';
			else if( 'A' <= *p && *p <= 'Z' )
				buf[i] = (buf[i]<<4) + *p - 'A' + 10;
			else if( 'a' <= *p && *p <= 'z' )
				buf[i] = (buf[i]<<4) + *p - 'a' + 10;
			i++;
			p++;
		}
		else
		{
			buf[i] = *p;
			i++;
			p++;
		}
	}
	buf[i] = 0;
	return i;
}

/* ��ȡ�ֶ�ֵ */
char *osl_url_getheadval( char *buf, int32_t buflen, char *name, char *value, int32_t valuesize )
{
	return osl_str_getval( buf, buflen, name, value, valuesize );
}

