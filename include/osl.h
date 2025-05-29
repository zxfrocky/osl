#ifndef __OSL_H__
#define __OSL_H__

#ifdef __cplusplus
extern "C"
{
#endif



/* 初始化, 返回0表示成功，-1表示失败 */
int32_t osl_init( const char *name );

/* 关闭模块 */
void osl_exit();

/* 延时(us) */
void osl_usleep( int32_t us );

/* 取得开机至当前时间(ms) */
uint32_t osl_get_ms();

/* 取得开机至当前时间(us) */
uint64_t osl_get_us();

/* 取得UTC时间(s) */
uint32_t osl_get_utc();

/*取得当前系统时间(包括时区)*/
uint32_t osl_get_ltc();

/* 取得时区(北京=8) */
int32_t osl_get_time_zone();

/* 分析时间 */
struct tm *osl_localtime( uint32_t *utc );

/* 时间->字符串 */
char *osl_asctime( struct tm *st );

/* 取得现在的时间 */
char *osl_get_time_string();

/* 执行外部命令: 返回0表示成功，-1表示失败 */
int32_t osl_excute(char *cmd, char *buf, int32_t size);

/**获取cpu的个数 失败返回-1,成功返回cpu的个数*/
int32_t osl_get_cpu_count();

/* 读取控制台输入 */
void osl_gets( char_t *str );

/* 断言 */
void osl_assert( int32_t flag, const char *file, int32_t line );

/* 产生UUID */
int32_t osl_generate_uuid( char_t *buf, int32_t size );

#ifdef __cplusplus
}
#endif

#endif  /* __OSL_H__ */
