#include "stdafx.h"
#include "osl.h"
#include "osl_log.h"
#include "osl_base64.h"
#include <objbase.h>


static void *m_mutex = NULL;
static uchar_t m_exit = 0;



/* ��ʼ����0=success,-1=failed */
extern int32_t osl_log_init( int32_t level );

/* �ر���־ */
extern void osl_log_exit();


/* ��ʼ��, ����0��ʾ�ɹ���-1��ʾʧ�� */
int32_t osl_init( const char *name )
{
	WSADATA wsaData;
	//int32_t i, seed_int[100];
	OSVERSIONINFO ver;
	BOOL bResult;

	//ȷ����������ȷ�İ汾�� Windows NT (3.51, 4.0, or later)
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    bResult = GetVersionEx((LPOSVERSIONINFO) &ver);
    if ( (!bResult) ||(ver.dwPlatformId != VER_PLATFORM_WIN32_NT) )
    {
       osl_log_debug("ECHOSRV requires Windows NT 3.51 or later.");
	   return -1;
    }


	/* ��ʼ������������� */
	srand( (uint32_t)time(NULL) );  // ϵͳAPI��������Ӧ�ò�Ӱ�칦�ܰɣ�
	//for( i=0; i<100; i++ )//WINDOWS+OPENSSL���룿
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

/* �ͷ�ģ�� */
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

/* ��ʱ */
void osl_usleep( int32_t us )
{
	Sleep( us/1000 );
}


/* ȡ���Կ��������ڵ�ʱ��(ms) */
uint32_t osl_get_ms()
{
	return GetTickCount();
}


/* ȡ�ÿ�������ǰʱ��(us) */
uint64_t osl_get_us()
{
	LARGE_INTEGER cnt, freq;

	QueryPerformanceCounter( &cnt );
	QueryPerformanceFrequency( &freq );
	
	return cnt.QuadPart*1000000 / freq.QuadPart;
}

/* ȡ��UTCʱ��(s) */
uint32_t osl_get_utc()
{
	return (uint32_t)time(NULL);
}

/*ȡ�õ�ǰϵͳʱ��(s,��ʱ��)*/
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

/* ����ʱ��(����=8) */
int32_t osl_get_time_zone()
{
	static time_t zone = -1;
	struct tm *pt;
	time_t now, tmp;

	if( zone == -1 )
	{
		/* ����UTCʱ�� */
		now = time( NULL );
		/* ����UTCʱ�� */
		pt = gmtime( &now );
		if( pt )
		{
			/* ���챾ʱ����UTCʱ�� */
			tmp = mktime( pt );
			zone = (now-tmp)/3600;
		}
		else
			zone = 8;
	}
	return (int32_t)zone;
}

/* ����ʱ�� */
struct tm *osl_localtime( uint32_t *utc )
{
	time_t tt = (time_t)*utc;
	return localtime( &tt );
}

/* ʱ��->�ַ��� */
char *osl_asctime( struct tm *st )
{
	return asctime( st );
}

/* ȡ�����ڵ�ʱ�� */
char *osl_get_time_string()
{
	struct tm *pt;
	time_t utc;
	pt = localtime( &utc );
	return asctime( pt );
}

/* ִ���ⲿ����: ����0��ʾ�ɹ���-1��ʾʧ�� */
int32_t osl_excute(char *cmd, char *buf, int32_t size)
{
	if ( cmd )
		system( cmd );
	return 0;
}

/**��ȡcpu�ĸ��� ʧ�ܷ���-1,�ɹ�����cpu�ĸ���*/
int32_t osl_get_cpu_count()
{
	int32_t ret = -1;
	SYSTEM_INFO sysinfo[1] = {0};

	GetSystemInfo(sysinfo);

	if (sysinfo->dwNumberOfProcessors > 0)
		ret = sysinfo->dwNumberOfProcessors;
	return ret;
}

/* ��ȡ����̨���� */
void osl_gets( char_t *str )
{
	gets( str );
}

void os_sys_print_ip( uint32_t ip )
{
	unsigned char *p = (unsigned char *)&ip; /*ע��pһ��Ҫ����Ϊunsigned char*/

	int i;
	for(i = 4; i > 0; i--)            /*�Ӹߵ�ַ ���ǵ͵�ַ ��ʼ���*/
		printf("%d.", *(p + i - 1));    /*ȡ����������Ǵ�˻���С�ˣ�������С�ˣ�*/
}

/* ����UUID,32���ַ�,�����ַ�'-' */
int32_t osl_generate_uuid( char_t *buf, int32_t size )
{
	GUID guid;
	int32_t ret;

	CoCreateGuid(&guid);
	ret = osl_base64_encode_hurt( (uchar_t*)&guid, sizeof(guid), (uchar_t*)buf, size-1 );
	buf[ret] = 0;
	return ret+1;
}

/* ���� */
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

