/* 注意：本文件中的函数是为OSL中的各个模块服务的，不要用OSL模块中的函数创建内核对象 */

#include "stdafx.h"
#include <setjmp.h>
#include <signal.h>
#include <sys/sem.h>
#include "osl_mem.h"
#include "osl_log.h"
#include "osl_string.h"

//#include <uuid/uuid.h>

typedef struct _GUID
{
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
} GUID, UUID;

static pthread_mutex_t m_mutex;
static void *m_memory = NULL;
static void *m_buf = NULL;
static uchar_t m_exit = 0;

static void osl_on_signal( int32_t signal );
static void osl_kill(int32_t sig);
	
/* 初始化，返回0表示成功，-1表示失败 */
int32_t osl_init( const char *name )
{
	int32_t semid;
	key_t key;
	struct sembuf sops;
	int32_t size = 32*1024;

	/* 初始化随机数生成器 */
	srand( time(NULL) );

	if( name )
	{
		memcpy(&key, name, sizeof(key_t));
		semid = semget((key_t)key, 1, 0666|IPC_CREAT|IPC_EXCL);
		if(semid == -1)
		{
			printf("'%s': This program is running!\n", name);
			return -1;
		}
		sops.sem_num = 0;
		sops.sem_op =  1;
		sops.sem_flg = IPC_NOWAIT|SEM_UNDO;
		if (semop (semid, &sops, 1) == -1){
			printf("semop failed\n");
			return -1;
		}
		semctl(semid, 1, IPC_RMID);
	}

	pthread_mutex_init( &m_mutex, NULL );
	if(m_buf == NULL)
	{
		m_buf = malloc(size);
		m_memory = osl_mem_create( (char*)m_buf, size, 8 );
	}

	m_exit = 0;
	/*初始化Kill信号处理函数*/
	signal(SIGTERM, osl_kill);
	signal(SIGPIPE, osl_on_signal );
	return 0;
}


/* 释放OSL模块 */
void osl_exit()
{
	pthread_mutex_destroy( &m_mutex );
	if( m_memory )
	{
		osl_mem_destroy( m_memory );
		m_memory = NULL;
	}
	if(m_buf != NULL)
		free(m_buf);
}

/* 延时(us) */
void osl_usleep( int32_t us )
{
	usleep( us );
}

/* 取得自开机到现在的时间(ms) */
uint32_t osl_get_ms()
{
	static int32_t first = 1;
	static struct timeval start;
	struct timeval now;

	gettimeofday( &now, NULL );

	if( first )
	{
		start = now;
		first = 0;
		return 0;
	}
	else
	    return (now.tv_sec-start.tv_sec)*1000 + now.tv_usec/1000 - start.tv_usec/1000;
		//now.tv_usec-start.tv_usec可能为一个负数
}

/* 取得开机至当前时间(us) */
uint64_t osl_get_us()
{
	static int32_t first = 1;
	static struct timeval start;
	struct timeval now;
	uint64_t tt;

	gettimeofday( &now, NULL );
	
	if( first )
	{
		start = now;
		first = 0;
		return 0;
	}
	else
	{
		tt = now.tv_sec - start.tv_sec;
		tt *= 1000000;
		tt += now.tv_usec - start.tv_usec;
		return tt;
	}
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
	FILE *pp;

	if( cmd == NULL || cmd[0] == 0 )
		return 0;
	
	if( (pp = popen(cmd, "r") ) == NULL)	
	{
		osl_log_warn("popen() error!\r\n");
		return -1;	
	}
	
	if( buf == NULL )
	{
		pclose(pp);
		return 0;
	}

	memset( buf, 0, size );
	fread( buf, 1, size,  pp ); 
	pclose(pp);

	return -1;
}

/**获取cpu的个数 失败返回-1,成功返回cpu的个数*/
int32_t osl_get_cpu_count()
{
	int32_t cpu_count=0;
	char buf[1024]={0};
	FILE * pFile = fopen("/proc/cpuinfo", "rb");
	if (!pFile)
		return -1;
	while(!feof(pFile))
	{
		fgets(buf,sizeof(buf)-1,pFile);

		if (osl_strncmp_nocase("processor",buf,9)==0)
			cpu_count++;
	}

	fclose(pFile);
	return cpu_count;
}

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
#if 0
/* 产生UUID,32个字符,不带字符'-' */
int32_t osl_generate_uuid( char_t *buf, int32_t size )
{
	GUID guid;
	int32_t ret;

	uuid_generate((char *)&guid);
	//uuid_generate(reinterpret_cast<unsigned char *>(&guid));
	ret = osl_str_snprintf(
		buf, size,
		"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return ret+1;
}
#endif
/* 断言 */
void osl_assert( int32_t flag, const char *file, int32_t line )
{
	if( flag == 0 )
	{
		printf( "%s(%d): assert failed\n", file, line );
	}
}

static void osl_kill(int32_t sig)
{
	m_exit = 1;
}

uchar_t osl_is_exit()
{
	return m_exit;
}

static void osl_on_signal( int32_t signal )
{
	/* linux command: "man 7 siganl" */
	switch( signal )
	{
	case SIGHUP: /* 1 Term    Hangup detected on controlling terminal or death of controlling process */
		break;
	case SIGINT: /* 2 Term    Interrupt from keyboard */
		break;
	case SIGQUIT: /* 3 Core    Quit from keyboard */
		break;
	case SIGILL: /* 4 Core    Illegal Instruction */
		break;
	case SIGABRT: /* 6 Core    Abort signal from abort(3) */
		break;
	case SIGFPE: /* 8 Core    Floating point exception */
		break;
	case SIGKILL: /* 9 Term    Kill signal */
		break;
	case SIGSEGV: /* 11 Core    Invalid memory reference */
		break;
	case SIGPIPE: /* 13 Term    Broken pipe: write to pipe with no readers */
		break;
	case SIGALRM: /* 14 Term    Timer signal from alarm(2) */
		break;
	case SIGTERM: /* 15 Term    Termination signal */
		break;
	case SIGUSR1: /* Term    User-defined signal 1 */
		break;
	case SIGUSR2: /* Term    User-defined signal 2 */
		break;
	case SIGCHLD: /* Ign     Child stopped or terminated */
		break;
	case SIGCONT: /* Continue if stopped */
		break;
	case SIGSTOP: /* Stop    Stop process */
		break;
	case SIGTSTP: /*  Stop    Stop typed at tty */
		break;
	case SIGTTIN: /* Stop    tty input for background process */
		break;
	case SIGTTOU: /* Stop    tty output for background process */
		break;
	default:
		break;
	}
	printf( "sytem signal %d captured!\n", signal );
}
