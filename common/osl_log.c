#include "stdafx.h"
#include "osl_dir.h"
#include "osl_file.h"
#include "osl_socket.h"
#include "osl_string.h"
#include "osl_url.h"
#include "osl_log.h"

/*最大日志文件大小*/
#define MAX_LOGFILE_SIZE	64*1024*1024

/* 最低LOG级别 */
static int32_t m_level = OSL_LOG_LEVEL_WARN;
/* 输出方式 */
static char m_url[1024];
/* UDP-LOG工作套解字 */
static SOCKET m_socket = -1;
/* UDP目的地ip地址 */
static uint32_t m_ip = 0;
/* UDP目的地ip端口 */
static uint16_t m_port = 0;
/* 文件方式输出日志：文件指针 */
static void *m_fp = NULL;
/* 文件尺寸 */
static uint32_t m_filesize = 0;

/* 输出日志 */
static int32_t osl_log_output( char *buf, int32_t size );



/* 初始化LOG */
int32_t osl_log_init( int32_t level )
{
	m_level = level;
	memset( m_url, 0, sizeof(m_url) );
	osl_strncpy( m_url, "console://", sizeof(m_url)-1 );
	m_socket = -1;
	m_ip = 0;
	m_port = 0;
	m_fp = NULL;
	m_filesize = 0;
	
	return 0;
}

/* 关闭LOG */
void osl_log_exit()
{
	if( -1 != m_socket )
	{
		m_ip = ntohl(m_ip);
		if( 224 <= (m_ip>>24) && (m_ip>>24) < 240 )
			osl_socket_drop( m_socket, m_ip );
		osl_socket_destroy( m_socket );
		m_socket = -1;
	}
	if( m_fp )
	{
		osl_file_close(m_fp);
		m_fp = NULL;
	}
}

/* 设置LOG最小级别 */
void osl_log_set_level( int32_t level )
{
	m_level = level;
}

/* 取得LOG最小级别 */
int32_t osl_log_get_level()
{
	return m_level;
}


/* 设置日志输出方式 : url如下例子
	"file:///txt/log.txt"
	"udp://224.1.1.1:1024"
	"udp://192.168.27.10:5000"
	"udp://hostname:6000"
	"console://"
*/
int32_t osl_log_set_target( char *url )
{
	SUrlInfo info;
	char tmp[256];
	struct hostent *h;
	int32_t i;
	uchar_t *p;

	if( -1 != m_socket )
	{
		m_ip = ntohl(m_ip);
		if( 224 <= (m_ip>>24) && (m_ip>>24) < 240 )
			osl_socket_drop( m_socket, m_ip );
		osl_socket_destroy( m_socket );
		m_socket = -1;
	}
	if( m_fp )
	{
		osl_file_close(m_fp);
		m_fp = NULL;
	}

	if( NULL == url )
		goto ERROR_EXIT;

	osl_strncpy( m_url, url, sizeof(m_url)-1 );

	/*输出到文件*/
	if( 0 == osl_strncmp_nocase( m_url, "file://", 7) )
	{
		/* 删除备份文件 */
		osl_str_snprintf(tmp,sizeof(tmp),  "%s.bak", m_url+7 );
		osl_file_delete( tmp );

		/* 备份最新的LOG文件 */
		osl_file_rename( m_url+7, tmp );

		/* 打开新文件 */
		m_fp = osl_file_open( m_url+7, OSL_FILE_WRITE_ONLY );
	}
	/*UDP输出*/
	else if ( 0 == osl_strncmp_nocase( m_url, "udp://", 6 ) ) 
	{	
		if( 0 != osl_url_parse( m_url, &info ) )
			goto ERROR_EXIT;

		if( 0 < info.hostport_len )
		{
			memset( tmp, 0, sizeof(tmp) );
			osl_strncpy( tmp, m_url+info.hostport_pos, info.hostport_len );
			m_port = htons( (uint16_t)atoi(tmp) );
		}
		else
			m_port = htons( 8000 );
		if( 0 < info.hostname_len )
		{
			memset( tmp, 0, sizeof(tmp) );
			osl_strncpy( tmp, m_url+info.hostname_pos, info.hostname_len );
			/* 判断是否IP地址 */
			for( i=0; tmp[i]!=0; i++ )
			{
				if( ( tmp[i] < '0' || '9' < tmp[i] ) && ( tmp[i] != '.' ) )
					break;
			}
			if( tmp[i] == 0 )
			{
				m_ip = inet_addr( tmp );
			}
			/* 主机名称 */
			else
			{
				h = gethostbyname( tmp );
				if( h == NULL )
					goto ERROR_EXIT;
				memcpy( &m_ip, h->h_addr_list[0], sizeof(m_ip) );
			}

			/* 创建新的SOCKET */
			m_socket = osl_socket_create( AF_INET, SOCK_DGRAM, 0 );
			if( m_socket < 0 )
					goto ERROR_EXIT;

			/*如果是组播地址,加入组播*/
			p = (uchar_t*)&m_ip;
			if ( 224 <= *p && *p < 240  )
			{
				/* 绑定本地端口 */
				if( osl_socket_bind( m_socket, INADDR_ANY, m_port ) < 0 )
					goto ERROR_EXIT;

				osl_socket_join( m_socket, m_ip );
			}
			else
			{
				/* 绑定本地端口 */
				if( osl_socket_bind( m_socket, m_ip, m_port ) < 0 )
					goto ERROR_EXIT;
			}
		}
	}
	/*输出到控制台*/
 	else if ( 0 == osl_strncmp_nocase( m_url, "console://", 10 ) )
	{
	}
	return 0;
ERROR_EXIT:
	return -1;
}

/* 取得日志输出方式 */
void osl_log_get_target( char *buf, int32_t size )
{
	osl_strncpy( buf, m_url, size-1 );
	buf[size-1] = 0;
}

/* 输出调试日志 */
int32_t osl_log_debug( char *format, ... )
{
    int32_t size;
	va_list args;
	char buf[8192];

	if( OSL_LOG_LEVEL_DEBUG < m_level )
		return 0;

	va_start( args, format );
	size = osl_str_vsnprintf( buf, sizeof(buf)-1, format, args );
	va_end(args);
	buf[size] = 0;
	size++;
	
	osl_log_output( buf, size );	
	return size;
}

/* 输出警告日志 */
int32_t osl_log_warn( char *format, ... )
{	
    int32_t size;
	va_list args;
	char buf[8192];

	if( OSL_LOG_LEVEL_WARN < m_level )
		return 0;

	va_start( args, format );
	size = osl_str_vsnprintf( buf, sizeof(buf)-1, format, args );
	va_end(args);
	buf[size] = 0;
	size++;
	
	osl_log_output( buf, size );	
	return size;
}


/* 输出错误日志 */
int32_t osl_log_error( char *format, ... )
{
	int32_t size;
	va_list args;
	char buf[8192];

	if( OSL_LOG_LEVEL_ERROR < m_level )
		return 0;

	va_start( args, format );
	size = osl_str_vsnprintf( buf, sizeof(buf)-1, format, args );
	va_end(args);
	buf[size] = 0;
	size++;
	
	osl_log_output( buf, size );	
	return size;
}

int32_t osl_log_hex(const char *msg, const char *buf, uint32_t len, int32_t wide)
{
    int32_t i;
    int32_t ret     = 0;
    int32_t nTabs   = 0;
    int32_t nlines  = 0;
    char msgbuf[128];
    const char hex_char[] = "0123456789ABCDEF";
    const unsigned char *ptr = (const unsigned char*)buf;
	
    if ((NULL == msg) || (NULL == buf) || ((8 != wide) && (16 != wide)))
    {
        return 0;
    }
	
    osl_log_debug("%s--> addr=%08lx %ld bytes\n", msg, buf, len);
	
    /* calculate how many prefixing Tab */
    while ((*msg == '\r') || (*msg == '\n'))
    {
        msg++;              // skip prefixing '\r' or '\n'
    }
	
    while (*msg++ == '\t')
    {
        nTabs++;
        if (6 == nTabs)
        {
            break;
        }
    }
	
    if (8 == wide)
    {
        nlines = (len + 0x07) >> 3;
    }
    else if (16 == wide)
    {
        nlines = (len + 0x0f) >> 4;
    }
	
    for (i = 0; i < nlines; i++)
    {
        int32_t j;
        char *dst  = msgbuf;
        int32_t nbytes = ((int32_t)len < wide) ? len : wide;
		
        for (j = 0; j < nTabs; j++)
        {
            *dst++ = '\t';
        }
		
        *dst++ = ' ';
        *dst++ = ' ';
        *dst++ = ' ';
        *dst++ = ' ';
		
        for (j = 0; j < nbytes; j++)
        {
            unsigned char ival = *ptr++;
			
            //一个char型变量用16进制输出，然后再输出一个空格，共占用三个字符空间
            *dst++ = hex_char[(ival >> 4) & 0x0F];
            *dst++ = hex_char[ival & 0x0F];
            *dst++ = ' ';
        }
		
        //输出一行数值后，输出三个空格，再输出该行数值的ASCII码字符
        //若nbytes小于一行的长度，即小于8或16，则以三个空格代替一个数值输出
        for (j = 0; j < wide - nbytes + 1; j++)
        {
            *dst++ = ' ';
            *dst++ = ' ';
            *dst++ = ' ';
        }
		
        ptr -= nbytes;
        for (j = 0; j < nbytes; j++)
        {
            if ((*ptr >= 0x20) && (*ptr <= 0x7e))
            {
                *dst = *ptr;
            }
            else
            {
                *dst = '.';
            }
            ptr++;
            dst++;
        }
        *dst = 0;
        ret += osl_log_debug("%s\n", msgbuf);
        len -= nbytes;
    }
	
    return ret;
}

/* 输出日志 */
static int32_t osl_log_output( char *buf, int32_t size )
{
	char tmp[256];

	if(!buf || size<=0 )
		return -1;
	
	/*输出到文件*/
	if( m_fp )
	{
		osl_file_write( m_fp, buf, size );
		m_filesize += size;

		/* 日志文件尺寸超出限制 */
		if( MAX_LOGFILE_SIZE <= m_filesize )
		{
			osl_file_close( m_fp );

			/* 删除备份文件 */
			osl_str_snprintf(tmp,sizeof(tmp),  "%s.bak", m_url+7 );
			osl_file_delete( tmp );

			/* 备份最新的LOG文件 */
			osl_file_rename( m_url+7, tmp );

			/* 打开新文件 */
			m_fp = osl_file_open( m_url+7, OSL_FILE_WRITE_ONLY );
			m_filesize = 0;
		}
#ifdef WIN32
		OutputDebugString( (PSTRING)buf );
#endif
	}
	/*UDP输出*/
	else if( m_socket != -1 )
	{
		osl_socket_sendto( m_socket, m_ip, m_port, buf, size );
	}
	/*输出到控制台*/
 	else if ( 0 == osl_strncmp_nocase( m_url, "console://", 10 ) )
	{
		printf( "%s", buf );
	}

	return 0;
}