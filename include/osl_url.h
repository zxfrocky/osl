#ifndef __OSL_URL_H__
#define __OSL_URL_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	/*�����û�����ʼλ��*/
	int32_t username_pos;
	/*�����û�������*/
	int32_t username_len;
	/*�����û����뿪ʼλ��*/
	int32_t userpswd_pos;
	/*�����û����볤��*/
	int32_t userpswd_len;

	/* ��������(������IP��ַ)��ʼλ�� */
	int32_t hostname_pos;
	/* ��������(������IP��ַ)���� */
	int32_t hostname_len;
	/* �����˿�(����δָ��)��ʼλ�� */
	int32_t hostport_pos;
	/* �����˿�(����δָ��)���� */
	int32_t hostport_len;
	/* ·����ʼλ�� */
	int32_t path_pos;
	/* ·������ */
	int32_t path_len;
	/* ������ʼλ�� */
	int32_t param_pos;
	/* �������� */
	int32_t param_len;
}SUrlInfo;

/* ����URL,0��ʾ�����ɹ�,-1����ʧ�� */
int32_t osl_url_parse( char *url, SUrlInfo *info );
/* ȡ��URL�еĲ��� */
char* osl_url_get_param( char *url, char *name, char *value, int32_t valuesize );
/* ��URL����ȡ���� */
int32_t osl_url_get_param_int( char *url, char *name );
/* ��URL���б��� */
int32_t osl_url_encode( char* url, int32_t urllen, char *buf, int32_t size );
/* ��URL���н��� */
int32_t osl_url_decode( char* url, int32_t urllen, char *buf, int32_t size );

/* ��ȡ�ֶ�ֵ(����HTTP,RTSPЭ��ͷ���ֶ�ֵ) */
char *osl_url_getheadval( char *buf, int32_t buflen, char *name, char *value, int32_t valuesize );

/* ��ȡurl��ÿ��Ŀ¼��ֵ,����б�ָܷ������� */
int32_t osl_url_parse_items(char_t *puri, char_t (*p)[512], int32_t count);

#ifdef __cplusplus
}
#endif

#endif /* __OSL_URL_H__ */
