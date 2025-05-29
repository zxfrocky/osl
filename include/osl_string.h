#ifndef __OSL_STRING_H__
#define __OSL_STRING_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdafx.h"

#define osl_strcmp	strcmp

/* �����ִ�Сд�Ƚ��ַ�����ǰn���ַ� */
int32_t osl_strncmp_nocase( const char *str1, const char *str2, int32_t size );

/* �����ִ�С�Ƚ��ַ��� */
int32_t osl_strcmp_nocase( const char *str1, const char *str2 );

/* ȡ���ַ������� */
int32_t osl_strlen( const char *str );

/* �ַ������� */
int32_t osl_strncpy( const char *str1, const char *str2, int32_t maxlen );

/* �ַ������� */
char* osl_strstr( const char *str, const char *substr );

/* ���ַ�����delim������ַ��ָ�����طָ��ĵ�һ���� */
char* osl_str_split( char** str, const char *delim );

/* �����size���ַ����뵽�ַ��� */
int32_t osl_str_snprintf( char *str, int32_t size, const char *format, ... );

/* �����size���ַ����뵽�ַ��� */
int32_t osl_str_vsnprintf( char *str, int32_t size, const char *format, va_list ap );

/* ����ַ������ߵ�' ','\t','\r','\n' */
char* osl_str_trim( char *str );

/* �ü�·����ȥ��'.'��'..'����'\'��Ϊ'/' */
char* osl_str_trim_path( char *path );

/* ����ߴ磺1m=1024*1024, 1k=1024, 1=1 */
uint32_t osl_str_calcsize( char *str );

/* ��ȡ�ֶ�ֵ: "command: PAUSE\r\n duration: 100000\r\n", ��name="duration"����value="100000" */
char *osl_str_getval( char *buf, int32_t buflen, char *name, char *value, int32_t valuelen );

/* �����ǲ���IP��ַ������0��ʾ�� */
int32_t osl_str_isaddr( char* buf );

/* �ַ���תΪ64λ���� */
int64_t osl_str_str2int64( char *str );

/* �Ƚ�·��: ���ݲ���ϵͳ�����Ƿ���Ҫ���ִ�Сд*/
int32_t osl_str_cmppath( char *path1, char *path2, int32_t len );

/* ��ʱ��ת��Ϊ�ַ��� */
int32_t osl_str_time2str( uint32_t utc, char *buf, int32_t size );

/* ���ַ���ת��Ϊʱ��(utc) */
uint32_t osl_str_str2time( char *strtime, int32_t size );

/* UTC->DATE(HHHH/MM/DD) */
char_t *osl_str_utc2date( uint32_t utc, char_t *buf, int32_t size );

/* DATE(HHHH/MM/DD)->UTC */
uint32_t osl_str_date2utc( char_t *date );

#ifdef __cplusplus
}
#endif

#endif /* __OSL_STRING_H__ */
