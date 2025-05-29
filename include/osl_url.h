#ifndef __OSL_URL_H__
#define __OSL_URL_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	/*主机用户名开始位置*/
	int32_t username_pos;
	/*主机用户名长度*/
	int32_t username_len;
	/*主机用户密码开始位置*/
	int32_t userpswd_pos;
	/*主机用户密码长度*/
	int32_t userpswd_len;

	/* 主机名称(可能是IP地址)开始位置 */
	int32_t hostname_pos;
	/* 主机名称(可能是IP地址)长度 */
	int32_t hostname_len;
	/* 主机端口(可能未指明)开始位置 */
	int32_t hostport_pos;
	/* 主机端口(可能未指明)长度 */
	int32_t hostport_len;
	/* 路径开始位置 */
	int32_t path_pos;
	/* 路径长度 */
	int32_t path_len;
	/* 参数开始位置 */
	int32_t param_pos;
	/* 参数长度 */
	int32_t param_len;
}SUrlInfo;

/* 分析URL,0表示分析成功,-1分析失败 */
int32_t osl_url_parse( char *url, SUrlInfo *info );
/* 取得URL中的参数 */
char* osl_url_get_param( char *url, char *name, char *value, int32_t valuesize );
/* 从URL中提取整数 */
int32_t osl_url_get_param_int( char *url, char *name );
/* 对URL进行编码 */
int32_t osl_url_encode( char* url, int32_t urllen, char *buf, int32_t size );
/* 对URL进行解码 */
int32_t osl_url_decode( char* url, int32_t urllen, char *buf, int32_t size );

/* 提取字段值(例如HTTP,RTSP协议头部字段值) */
char *osl_url_getheadval( char *buf, int32_t buflen, char *name, char *value, int32_t valuesize );

/* 获取url中每层目录的值,即以斜杠分隔的内容 */
int32_t osl_url_parse_items(char_t *puri, char_t (*p)[512], int32_t count);

#ifdef __cplusplus
}
#endif

#endif /* __OSL_URL_H__ */
