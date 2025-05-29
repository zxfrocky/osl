#ifndef __OSL_LOG_H__
#define __OSL_LOG_H__


#ifdef __cplusplus
extern "C"
{
#endif


/* 日志级别 */
#define	OSL_LOG_LEVEL_ALL		0x0
#define	OSL_LOG_LEVEL_DEBUG		0x1
#define	OSL_LOG_LEVEL_WARN		0x2
#define	OSL_LOG_LEVEL_ERROR		0x3
#define	OSL_LOG_LEVEL_NONE		0x8


/* 设置日志级别 */
void osl_log_set_level( int32_t level );

/* 获取日志级别, OSL_LOG_LEVEL_DEBUG, ... */
int32_t osl_log_get_level();

/* 设置日志输出方式 : url如下例子
	"file:///txt/log.txt"
	"udp://224.1.1.1:1024"
	"udp://192.168.27.10:5000"
	"udp://hostname:6000"
	"console://"
*/
int32_t osl_log_set_target( char *url );

/* 取得日志输出方式 */
void osl_log_get_target( char *buf, int32_t size );

/* 输出调试信息 */
int32_t osl_log_debug( char *format, ... );

/* 输出警告信息 */
int32_t osl_log_warn( char *format, ... );

/* 输出错误信息 */
int32_t osl_log_error( char *format, ... );

/* 输出二进制信息 */
int32_t osl_log_hex(const char *msg, const char *buf, uint32_t len, int32_t wide);


#ifdef __cplusplus
}
#endif

#endif /* __OSL_LOG_H__ */
