#include "stdafx.h"
#include "osl.h"
#include "osl_log.h"
#include "osl_base64.h"
#include <objbase.h>


static void *m_mutex = NULL;
static uchar_t m_exit = 0;



/* 初始化：0=success,-1=failed */
extern int32_t osl_log_init( int32_t level );

/* 关闭日志 */
extern void osl_log_exit();


/* 初始化, 返回0表示成功，-1表示失败 */
int32_t osl_init( const char *name )
{
	WSADATA wsaData;
	//int32_t i, seed_int[100];
	OSVERSIONINFO ver;
	BOOL bResult;

	//确保运行在正确的版本下 Windows NT (3.51, 4.0, or later)
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    bResult = GetVersionEx((LPOSVERSIONINFO) &ver);
    if ( (!bResult) ||(ver.dwPlatformId != VER_PLATFORM_WIN32_NT) )
    {
       osl_log_debug("ECHOSRV requires Windows NT 3.51 or later.");
	   return -1;
    }


	/* 初始化随机数生成器 */
	srand( (uint32_t)time(NULL) );  // 系统API都这样？应该不影响功能吧？
	//for( i=0; i<100; i++ )//WINDOWS+OPENSSL必须？
	//{
	//	seed_int[i] = rand();
	//	RAND_seed(seed_int, sizeof(seed_int));
	//}

	if( name )
	{
		m_mutex = CreateMutex( NULL, TRUE, (PSTRING)name );
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(m_mutex);
			m_mutex = NULL;
			return -1;
		}
	}
	WSAStartup(0x202,&wsaData);

	osl_log_init( OSL_LOG_LEVEL_ALL );

	m_exit = 0;
	return 0;
}

/* 释放模块 */
void osl_exit()
{
	osl_log_exit();
	WSACleanup();
	if( m_mutex )
	{
		CloseHandle( m_mutex );
		m_mutex = NULL;
	}
}

/* 延时 */
void osl_usleep( int32_t us )
{
	Sleep( us/1000 );
}


/* 取得自开机到现在的时间(ms) */
uint32_t osl_get_ms()
{
	return GetTickCount();
}


/* 取得开机至当前时间(us) */
uint64_t osl_get_us()
{
	LARGE_INTEGER cnt, freq;

	QueryPerformanceCounter( &cnt );
	QueryPerformanceFrequency( &freq );
	
	return cnt.QuadPart*1000000 / freq.QuadPart;
}

/* 取得UTC时间(s) */
uint32_t osl_get_utc()
{
	return (uint32_t)time(NULL);
}

/*取得当前系统时间(s,含时区)*/
uint32_t osl_get_ltc()
{
	time_t rtime;
	struct tm *nt;
	time_t newtime;

	newtime = time( NULL );
	nt = localtime( &newtime );
	
	rtime = mktime( nt );
	
	return (uint32_t)rtime;
}

/* 计算时区(北京=8) */
int32_t osl_get_time_zone()
{
	static time_t zone = -1;
	struct tm *pt;
	time_t now, tmp;

	if( zone == -1 )
	{
		/* 返回UTC时间 */
		now = time( NULL );
		/* 解析UTC时间 */
		pt = gmtime( &now );
		if( pt )
		{
			/* 构造本时区的UTC时间 */
			tmp = mktime( pt );
			zone = (now-tmp)/3600;
		}
		else
			zone = 8;
	}
	return (int32_t)zone;
}

/* 分析时间 */
struct tm *osl_localtime( uint32_t *utc )
{
	time_t tt = (time_t)*utc;
	return localtime( &tt );
}

/* 时间->字符串 */
char *osl_asctime( struct tm *st )
{
	return asctime( st );
}

/* 取得现在的时间 */
char *osl_get_time_string()
{
	struct tm *pt;
	time_t utc;
	pt = localtime( &utc );
	return asctime( pt );
}

/* 执行外部命令: 返回0表示成功，-1表示失败 */
int32_t osl_excute(char *cmd, char *buf, int32_t size)
{
	if ( cmd )
		system( cmd );
	return 0;
}

/**获取cpu的个数 失败返回-1,成功返回cpu的个数*/
int32_t osl_get_cpu_count()
{
	int32_t ret = -1;
	SYSTEM_INFO sysinfo[1] = {0};

	GetSystemInfo(sysinfo);

	if (sysinfo->dwNumberOfProcessors > 0)
		ret = sysinfo->dwNumberOfProcessors;
	return ret;
}

/* 读取控制台输入 */
void osl_gets( char_t *str )
{
	gets( str );
}

void os_sys_print_ip( uint32_t ip )
{
	unsigned char *p = (unsigned char *)&ip; /*注意p一定要定义为unsigned char*/

	int i;
	for(i = 4; i > 0; i--)            /*从高地址 还是低地址 开始输出*/
		printf("%d.", *(p + i - 1));    /*取决于你机器是大端还是小端（这里是小端）*/
}

/* 产生UUID,32个字符,不带字符'-' */
int32_t osl_generate_uuid( char_t *buf, int32_t size )
{
	GUID guid;
	int32_t ret;

	CoCreateGuid(&guid);
	ret = osl_base64_encode_hurt( (uchar_t*)&guid, sizeof(guid), (uchar_t*)buf, size-1 );
	buf[ret] = 0;
	return ret+1;
}

/* 断言 */
void osl_assert( int32_t flag, const char *file, int32_t line )
{
	char tmp[32];
	if( flag == 0 )
	{
		if(file)
		{
			OutputDebugString( (PSTRING)file );
			sprintf( tmp, "(%d):", line );
			OutputDebugString( (PSTRING)tmp );
		}
		OutputDebugString( (PSTRING)("warning: assert failed\n") );
	}
}

