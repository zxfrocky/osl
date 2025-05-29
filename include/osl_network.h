#ifndef __OSL_NETWORK_H__
#define __OSL_NETWORK_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* DNS����������IP��ַ�������ֽ��� */
uint32_t osl_network_dns( const char *hostname );

/* ȡ�ñ�������: ����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_network_get_local_name( char *name, int32_t size );

/* ȡ�ñ���IP��ַ */
uint32_t osl_network_get_local_ip();

/* ��ȡϵͳMAC��ַ,mac����6���ֽ�,����0��ʾ�ɹ�,-1��ʾʧ�� */
int32_t osl_network_get_mac( uchar_t *mac );

/* ��ȡϵͳMAC��ַ,str:"00:01:26:12:34:56",mac����6���ֽ�,����0��ʾ�ɹ�,-1��ʾʧ��  */
int32_t osl_network_convert_mac( char_t *str, uchar_t *mac );



#ifdef __cplusplus
}
#endif

#endif  /* __OS__OSL_NETWORK_H__L_H__ */
