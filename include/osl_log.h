#ifndef __OSL_LOG_H__
#define __OSL_LOG_H__


#ifdef __cplusplus
extern "C"
{
#endif


/* ��־���� */
#define	OSL_LOG_LEVEL_ALL		0x0
#define	OSL_LOG_LEVEL_DEBUG		0x1
#define	OSL_LOG_LEVEL_WARN		0x2
#define	OSL_LOG_LEVEL_ERROR		0x3
#define	OSL_LOG_LEVEL_NONE		0x8


/* ������־���� */
void osl_log_set_level( int32_t level );

/* ��ȡ��־����, OSL_LOG_LEVEL_DEBUG, ... */
int32_t osl_log_get_level();

/* ������־�����ʽ : url��������
	"file:///txt/log.txt"
	"udp://224.1.1.1:1024"
	"udp://192.168.27.10:5000"
	"udp://hostname:6000"
	"console://"
*/
int32_t osl_log_set_target( char *url );

/* ȡ����־�����ʽ */
void osl_log_get_target( char *buf, int32_t size );

/* ���������Ϣ */
int32_t osl_log_debug( char *format, ... );

/* ���������Ϣ */
int32_t osl_log_warn( char *format, ... );

/* ���������Ϣ */
int32_t osl_log_error( char *format, ... );

/* �����������Ϣ */
int32_t osl_log_hex(const char *msg, const char *buf, uint32_t len, int32_t wide);


#ifdef __cplusplus
}
#endif

#endif /* __OSL_LOG_H__ */
