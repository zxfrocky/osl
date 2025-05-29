#ifndef __OSL_NETWORK_H__
#define __OSL_NETWORK_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* DNS解析，返回IP地址，网络字节序 */
uint32_t osl_network_dns( const char *hostname );

/* 取得本机名称: 返回0表示成功,-1表示失败 */
int32_t osl_network_get_local_name( char *name, int32_t size );

/* 取得本机IP地址 */
uint32_t osl_network_get_local_ip();

/* 获取系统MAC地址,mac至少6个字节,返回0表示成功,-1表示失败 */
int32_t osl_network_get_mac( uchar_t *mac );

/* 获取系统MAC地址,str:"00:01:26:12:34:56",mac至少6个字节,返回0表示成功,-1表示失败  */
int32_t osl_network_convert_mac( char_t *str, uchar_t *mac );



#ifdef __cplusplus
}
#endif

#endif  /* __OS__OSL_NETWORK_H__L_H__ */
