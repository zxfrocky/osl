#ifndef __OSL_STRING_H__
#define __OSL_STRING_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdafx.h"

#define osl_strcmp	strcmp

/* 不区分大小写比较字符串的前n个字符 */
int32_t osl_strncmp_nocase( const char *str1, const char *str2, int32_t size );

/* 不区分大小比较字符串 */
int32_t osl_strcmp_nocase( const char *str1, const char *str2 );

/* 取得字符串长度 */
int32_t osl_strlen( const char *str );

/* 字符串拷贝 */
int32_t osl_strncpy( const char *str1, const char *str2, int32_t maxlen );

/* 字符串查找 */
char* osl_strstr( const char *str, const char *substr );

/* 把字符串按delim定义的字符分割，并返回分割后的第一个串 */
char* osl_str_split( char** str, const char *delim );

/* 把最多size个字符输入到字符串 */
int32_t osl_str_snprintf( char *str, int32_t size, const char *format, ... );

/* 把最多size个字符输入到字符串 */
int32_t osl_str_vsnprintf( char *str, int32_t size, const char *format, va_list ap );

/* 清除字符串两边的' ','\t','\r','\n' */
char* osl_str_trim( char *str );

/* 裁减路径，去掉'.'和'..'，将'\'变为'/' */
char* osl_str_trim_path( char *path );

/* 计算尺寸：1m=1024*1024, 1k=1024, 1=1 */
uint32_t osl_str_calcsize( char *str );

/* 提取字段值: "command: PAUSE\r\n duration: 100000\r\n", 如name="duration"，则value="100000" */
char *osl_str_getval( char *buf, int32_t buflen, char *name, char *value, int32_t valuelen );

/* 分析是不是IP地址，返回0表示是 */
int32_t osl_str_isaddr( char* buf );

/* 字符串转为64位整数 */
int64_t osl_str_str2int64( char *str );

/* 比较路径: 根据操作系统决定是否需要区分大小写*/
int32_t osl_str_cmppath( char *path1, char *path2, int32_t len );

/* 把时间转化为字符串 */
int32_t osl_str_time2str( uint32_t utc, char *buf, int32_t size );

/* 把字符串转化为时间(utc) */
uint32_t osl_str_str2time( char *strtime, int32_t size );

/* UTC->DATE(HHHH/MM/DD) */
char_t *osl_str_utc2date( uint32_t utc, char_t *buf, int32_t size );

/* DATE(HHHH/MM/DD)->UTC */
uint32_t osl_str_date2utc( char_t *date );

#ifdef __cplusplus
}
#endif

#endif /* __OSL_STRING_H__ */
