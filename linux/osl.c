/* ע�⣺���ļ��еĺ�����ΪOSL�еĸ���ģ�����ģ���Ҫ��OSLģ���еĺ��������ں˶��� */

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
	
/* ��ʼ��������0��ʾ�ɹ���-1��ʾʧ�� */
int32_t osl_init( const char *name )
{
	int32_t semid;
	key_t key;
	struct sembuf sops;
	int32_t size = 32*1024;

	/* ��ʼ������������� */
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
	/*��ʼ��Kill�źŴ�����*/
	signal(SIGTERM, osl_kill);
	signal(SIGPIPE, osl_on_signal );
	return 0;
}


/* �ͷ�OSLģ�� */
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

/* ��ʱ(us) */
void osl_usleep( int32_t us )
{
	usleep( us );
}

/* ȡ���Կ��������ڵ�ʱ��(ms) */
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
		//now.tv_usec-start.tv_usec����Ϊһ������
}

/* ȡ�ÿ�������ǰʱ��(us) */
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

/**��ȡcpu�ĸ��� ʧ�ܷ���-1,�ɹ�����cpu�ĸ���*/
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
	unsigned char *p = (unsigned char *)&ip; /*ע��pһ��Ҫ����Ϊunsigned char*/

	int i;
	for(i = 4; i > 0; i--)            /*�Ӹߵ�ַ ���ǵ͵�ַ ��ʼ���*/
		printf("%d.", *(p + i - 1));    /*ȡ����������Ǵ�˻���С�ˣ�������С�ˣ�*/
}
#if 0
/* ����UUID,32���ַ�,�����ַ�'-' */
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
/* ���� */
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
