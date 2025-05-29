#include "stdafx.h"
#include "osl_string.h"


/* 不区分大小写比较字符串的前n个字符 */
int32_t osl_strncmp_nocase( const char *str1, const char *str2, int32_t size )
{
	uchar_t *p1 = (uchar_t *)str1;
	uchar_t *p2 = (uchar_t *)str2;
	uchar_t c1, c2;

	while( *p1 && *p2 && 
			p1 < ((uchar_t*)str1) + size )
	{
		c1 = toupper( *p1 );
		c2 = toupper( *p2 );
		if( c1 < c2 )
			return -1;
		else if( c2 < c1 )
			return 1;

		p1++;
		p2++;
	}
	if( p1 < ((uchar_t*)str1) + size )
	{
		c1 = toupper( *p1 );
		c2 = toupper( *p2 );
		if( c1 < c2 )
			return -1;
		else if( c2 < c1 )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}


/* 不区分大小比较字符串 */
int32_t osl_strcmp_nocase( const char *str1, const char *str2 )
{
	uchar_t *p1 = (uchar_t *)str1;
	uchar_t *p2 = (uchar_t *)str2;
	uchar_t c1, c2;

	while( *p1 && *p2 )
	{
		c1 = toupper( *p1 );
		c2 = toupper( *p2 );
		if( c1 < c2 )
			return -1;
		else if( c2 < c1 )
			return 1;

		p1++;
		p2++;
	}
	c1 = toupper( *p1 );
	c2 = toupper( *p2 );
	if( c1 < c2 )
		return -1;
	else if( c2 < c1 )
		return 1;
	else
		return 0;
}


/* 取得字符串长度 */
int32_t osl_strlen( const char *str )
{
	char *p = (char*)str;
	while( *p )
		p++;
	return (int32_t)(p-str);
}

/* 字符串拷贝 */
int32_t osl_strncpy( const char *str1, const char *str2, int32_t maxlen )
{
	char *p1 = (char*)str1;
	char *p2 = (char*)str2;
	while( p1<str1+maxlen )
	{
		*p1 = *p2;
		if( *p1 == 0 )
			break;
		p1++;
		p2++;
	}
	return (int32_t)(p1 - str1);
}

/* 字符串查找 */
char* osl_strstr( const char *str, const char *substr )
{
	return strstr( str, substr );
}

/* 把字符串按delim定义的字符分割，并返回分割后的第一个串 */
char* osl_str_split( char** str, const char *delim )
{
	char ch ='\0';
	char* pRet = NULL;
	if( *str == NULL )
		return NULL;
	
	ch = *(*str);
	pRet =*str;
	while(ch != '\0')
	{
		if( strchr(delim,ch) != NULL )
		{
			*(*str) = '\0';
			(*str)++;
			ch = *(*str);
			while( ch != '\0' && strchr(delim,ch) != NULL )
			{
				(*str)++;
				ch = *(*str);
			}
			break;
		}
		else
			(*str)++;
		ch = *(*str);
	}
	if( ch == '\0' )
		*str = NULL;
	
	return pRet;
}

/* 把最多size个字符输入到字符串 */
int32_t osl_str_snprintf( char *str, int32_t size, const char *format, ... )
{
	int32_t ret;
	va_list ap;

	va_start(ap, format);
#ifdef WIN32
	ret = _vsnprintf( str, size-1, format, ap );
#else
	ret = vsnprintf( str, size-1, format, ap );
#endif
	va_end(ap);
	if( ret < size )
		str[ret] = 0;
	return ret;
}

/* 把最多size个字符输入到字符串 */
int32_t osl_str_vsnprintf( char *str, int32_t size, const char *format, va_list ap )
{
	int32_t ret;
#ifdef WIN32
	ret = _vsnprintf( str, size-1, format, ap );
#else
	ret = vsnprintf( str, size-1, format, ap );
#endif
	if( ret < size )
		str[ret] = 0;
	return ret;
}

/* 清除字符串两边的' ','\t','\r','\n' */
char* osl_str_trim( char *str )
{
	int32_t i = 0, n = 0;
	while( str[n] == ' ' || str[n] == '\t' || str[n] == '\n' || str[n] == '\r' )
		n++;
	if( 0 < n )
	{
		while( str[n+i] != 0 )
		{
			str[i] = str[n+i];
			i++;
		}
	}
	else
	{
		while( str[i] != 0 )
			i++;
	}
	i--;
	while( 0 < i && ( str[i] == 0 || str[i] == ' ' || 
			str[i] == '\t' || str[i] == '\n' || str[i] == '\r' ) )
	{
		i--;
	}
	str[i+1] = 0;
	return str;
}


/* 裁减路径，去掉'.'和'..'，将'\'变为'/' */
char* osl_str_trim_path( char *path )
{
	char *p = path;
	char *p0;
	size_t n, len = strlen( path );

	/* 裁减路径，去掉'.'和'..'，将'\'变为'/' */
	while( *p )
	{
		if( *p == '\\' )
		{
			*p = '/';
		}
		else if( *p == '.' && ( *(p+1) == '/' || *(p+1) == '\\' ) )
		{
			memmove( p-1, p+1, path+len-p );
			p--;
			continue;
		}
		else if( *p == '.' && *(p+1) == '.' && ( *(p+2) == '/' || *(p+2) == '\\' ) )
		{
			n = 0;
			p0 = p;
			while( path < p0 && n < 2 )
			{
				p0--;
				if( *p0  == '/' )
					n++;
			}
			memmove( p0, p+2, path+len-p-1 );
			p = p0;
			continue;
		}

		if( *p == '/' && ( *(p+1) == '/' || *(p+1) == '\\' ) )
		{
			memmove( p, p+1, path+len-p );
			continue;
		}

		p++;
	}
	return path;
}

/* 计算尺寸：1m=1024*1024, 1k=1024, 1=1 */
uint32_t osl_str_calcsize( char *str )
{
	uint32_t n, size, co;
	char *p = str;

	co = 1;
	n = 0;
	size = 0;
	while( p[n] != 0 )
	{
		if( '0' <= p[n] && p[n] <= '9' )
			size = size * 10 + ( p[n] - '0' );
		else
		{
			if( p[n] == 'k' || p[n] == 'K' )
				co = 1024;
			else if( p[n] == 'm' || p[n] == 'M' )
				co = 1024*1024;
			break;
		}
		n++;
	}
	return size*co;
}

/* 提取字段值: "command: PAUSE\r\n duration: 100000\r\n", 如name="duration"，则value="100000" */
char *osl_str_getval( char *buf, int32_t buflen, char *name, char *value, int32_t valuelen )
{
	char *s, *e, *p;
	size_t len;
	size_t size;

	len = buflen >= 0 ? buflen : strlen(buf);

	memset( value, 0, valuelen );

	p = s = buf;
	while( p < buf+len )
	{
		/* 去掉开始的空格 */
		while( *p == ' ' ||  *p == ',' ||  *p == '\t' || *p == '\r' || *p == '\n' )
			p++;

		/* 记录行开始 */
		s = p;

		/* 找到行结束 */
		while( *p != '\r' && *p != '\n' && *p != '\0' )
			p++;
		e = p;

		/* 找到需要的字段 */
		if( osl_strncmp_nocase( s, name, (int32_t)strlen(name) ) == 0 )
		{
			/* 找到名称结束 */
			p = s;
			while( *p != ':' &&  *p != '=' &&  *p != '\t' && *p != '\r' && *p != '\n' && *p != '\0' )
				p++;
			if( *p == 0 || e <= p )
				goto NEXT_LINE;

			/* 找到值开始 */
			p++;
			while( *p == ' ' || *p == ':' ||  *p == '=' ||
				   *p == ',' || *p == '\t' || *p == '\r' || *p == '\n' )
			{
				p++;
			}
			while( s < e && *e == ' ' )
				e--;
			size = (valuelen-1)<(e-p) ? (valuelen-1):(e-p);
			if( value && 1 < valuelen )
				osl_strncpy( value, p, (int32_t)size );
			return value;
		}
NEXT_LINE:
		p = e + 1;
	}
	return NULL;
}


/* 分析是不是IP地址，返回0表示是 */
int32_t osl_str_isaddr( char* buf )
{
	int32_t i;
	int32_t len;
	len = (int32_t)strlen(buf);
	if( len == 0 )
		return -1;
	for( i=0; i<len; i++)
	{
		if(  ( buf[i] < '0' || buf[i] >'9' ) && ( buf[i] != '.' ) )
			return -1;
	}
	return 0;
}

/* 字符串转为64位整数 */
int64_t osl_str_str2int64( char *str )
{
#ifdef WIN32
	return _atoi64( str );
#else
	int64_t number = 0;
	char *s =NULL ;
	bool flag = true; 
	
	/*负数*/
	s = strstr( str, "-" );
	if( s )
	{
		str++;
		flag = false;
	}

	s = str;
	while( '0' <= *s && *s <='9' )
	{
		number = number * 10 + ((*s) - '0');
		s++;
	}

	if( false == flag )
		number = -number;
		
	return number;
#endif
}

/* 比较路径: 根据操作系统决定是否需要区分大小写*/
int32_t osl_str_cmppath( char *path1, char *path2, int32_t len )
{
#ifdef WIN32
	if( len < 0 )
		return osl_strcmp_nocase( path1, path2 );
	else
		return osl_strncmp_nocase( path1, path2, len );
#else
	if( len < 0 )
		return osl_strcmp( path1, path2 );
	else
		return strncmp( path1, path2, len );
#endif
}

/* 把时间转化为字符串 */
int32_t osl_str_time2str( uint32_t utc, char *buf, int32_t size )
{
	struct tm *nt;
	int32_t ret ;
	time_t tt = (time_t)utc;

	nt = localtime( &tt );
	if ( nt != NULL )
	{
		nt->tm_year += 1900;
		nt->tm_mon += 1;
#ifdef WIN32
		ret = _snprintf( buf, size, "%d-%d-%d %d:%d:%d", 
			nt->tm_year, nt->tm_mon, nt->tm_mday, nt->tm_hour, nt->tm_min, nt->tm_sec );
#else
		ret = snprintf( buf, size, "%d-%d-%d %d:%d:%d", 
			nt->tm_year, nt->tm_mon, nt->tm_mday, nt->tm_hour, nt->tm_min, nt->tm_sec );
#endif
	}
	else
		ret = 0;

	return ret;
}

/* 把字符串转化为时间(utc) */
uint32_t osl_str_str2time( char *strtime, int32_t size )
{
	time_t rtime;
	struct tm ntime;
	char *p = NULL, *s = NULL, *e = NULL;
	char val[10];
	int32_t i = 0;
	int32_t date[6];

	if ( NULL == strtime )
		return 0;

	/* 分别获取年月日小时分钟秒 */
	memset( date, 0, sizeof(date) );
	p = strtime;
	i = 0;
	while ( p < strtime + size && i < 6 )
	{
		e = strtime + size;
		while( *p == ' ' || *p == '-' || *p == ':' || *p == '.' || p == e || *p == '/' )
			p++;
		s = p;
		while( *p != '-' && *p != ':' && *p != '.' && *p != ' ' && p != e && *p != '/' )
			p++;
		e = p;
		memset( val, 0, sizeof(val) );
		memcpy( val, s, e - s);
		date[i] = atoi( val );
		i++;
		p = e;
	}

	memset( &ntime, 0, sizeof(ntime) );
	if ( date[0] > 1900 )
		ntime.tm_year = date[0] - 1900;
	if ( date[1] > 0 )
		ntime.tm_mon = date[1] - 1;
	ntime.tm_mday = date[2];
	ntime.tm_hour = date[3];
	ntime.tm_min = date[4];
	ntime.tm_sec = date[5];
	rtime = mktime( &ntime );

	return (uint32_t)rtime;
}

/* UTC->DATE(HHHH/MM/DD hh:mm:ss) */
char_t *osl_str_utc2date( uint32_t utc, char_t *buf, int32_t size )
{
	struct tm *tt;
	time_t time_utc = utc;

	tt = localtime( &time_utc );
	if( tt == NULL )
	{
		buf[0] = 0;
	}
	else
	{
#ifdef WIN32
	_snprintf( buf, size, "%04d/%02d/%02d %02d:%02d:%02d", tt->tm_year+1900, tt->tm_mon+1, tt->tm_mday,tt->tm_hour,tt->tm_min,tt->tm_sec );
#else
	snprintf( buf, size, "%04d/%02d/%02d %02d:%02d:%02d",  tt->tm_year+1900, tt->tm_mon+1, tt->tm_mday,tt->tm_hour,tt->tm_min,tt->tm_sec );
#endif
	}
	return buf;
}

/* DATE(HHHH/MM/DD)->UTC */
uint32_t osl_str_date2utc( char_t *date )
{
	return 0;
}
