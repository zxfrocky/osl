#include "stdafx.h"
#include "osl_string.h"
#include "osl_url.h"


/* 分析url到destURL结构中,0表示分析成功,-1分析失败 */
int32_t osl_url_parse( char *url, SUrlInfo *info )
{
	char* p = NULL;


	memset( info, 0, sizeof(SUrlInfo) );

	p = strstr( url, "://" );
	if( p )
	{
		/* 跳过协议名称 */
		p += 3;

		/* 用户名:密码，主机名称:地址 */
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
					/* username:userpswd保存在hostname:hostport里面了 */
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

	/* 路径 */
	info->path_pos = (int32_t)(p - url);
	while( *p && *p != '?' )
		p++;
	info->path_len = (int32_t)(p - url - info->path_pos);

	/* 参数 */
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

/* 取得URL中的参数 */
char* osl_url_get_param( char *url, char *name, char *value, int32_t valuesize )
{
	int32_t i=0;
	char *p;

	p = strstr( url, name );
	/* 确认找到name参数 */
	while( p != NULL )
	{
		/* param name 前面必须是分界符,后面必须是等号 */
		if( ( p == url  || ( p > url && 
			( *(p-1) == '?' || *(p-1) == ';' || *(p-1) == '&' || *(p-1) == ',') ) )
			&&  *(p+strlen(name)) == '=' )	 
		{
			/* 定位到'=' */
			p = p + strlen(name);
			/* skip '=' */
			p++;
			
			/* 到下一个分界符,认为是结束 */
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
		
		/* 没有找到param name,继续向后找param name */	
		p = strstr(p,name);
	}

	return NULL;
}

/* 从URL中提取整数 */
int32_t osl_url_get_param_int( char *url, char *name )
{
	char value[64];
	if( osl_url_get_param( url, name, value, sizeof(value) ) )
		return atoi( value );
	else
		return -1;
}


/* 对URL进行编码 */
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

/* 对URL进行解码 */
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

/* 提取字段值 */
char *osl_url_getheadval( char *buf, int32_t buflen, char *name, char *value, int32_t valuesize )
{
	return osl_str_getval( buf, buflen, name, value, valuesize );
}

