#ifndef __OSL_H__
#define __OSL_H__

#ifdef __cplusplus
extern "C"
{
#endif



/* ��ʼ��, ����0��ʾ�ɹ���-1��ʾʧ�� */
int32_t osl_init( const char *name );

/* �ر�ģ�� */
void osl_exit();

/* ��ʱ(us) */
void osl_usleep( int32_t us );

/* ȡ�ÿ�������ǰʱ��(ms) */
uint32_t osl_get_ms();

/* ȡ�ÿ�������ǰʱ��(us) */
uint64_t osl_get_us();

/* ȡ��UTCʱ��(s) */
uint32_t osl_get_utc();

/*ȡ�õ�ǰϵͳʱ��(����ʱ��)*/
uint32_t osl_get_ltc();

/* ȡ��ʱ��(����=8) */
int32_t osl_get_time_zone();

/* ����ʱ�� */
struct tm *osl_localtime( uint32_t *utc );

/* ʱ��->�ַ��� */
char *osl_asctime( struct tm *st );

/* ȡ�����ڵ�ʱ�� */
char *osl_get_time_string();

/* ִ���ⲿ����: ����0��ʾ�ɹ���-1��ʾʧ�� */
int32_t osl_excute(char *cmd, char *buf, int32_t size);

/**��ȡcpu�ĸ��� ʧ�ܷ���-1,�ɹ�����cpu�ĸ���*/
int32_t osl_get_cpu_count();

/* ��ȡ����̨���� */
void osl_gets( char_t *str );

/* ���� */
void osl_assert( int32_t flag, const char *file, int32_t line );

/* ����UUID */
int32_t osl_generate_uuid( char_t *buf, int32_t size );

#ifdef __cplusplus
}
#endif

#endif  /* __OSL_H__ */
