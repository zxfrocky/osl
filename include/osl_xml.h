#ifndef __OSL_XML_H__
#define __OSL_XML_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* XML�������� */
typedef struct tagXmlAttr
{
	char *name;
	char *value;
	struct tagXmlAttr *next;
}SXmlAttr;

/* XML�ڵ� */
typedef struct tagXmlNode
{
	/* TAG���� */
	char *name;
	/* ֵ, <>...</> */
	char *value;
	/* ���� */
	SXmlAttr *attrs;
	/* ���ڵ� */
	struct tagXmlNode *parent;
	/* �ӽڵ� */
	struct tagXmlNode *children;
	/* �ܽڵ� */
	struct tagXmlNode *siblings;
}SXmlNode;


/* ����XML������ */
void* osl_xml_create( char_t *buf, int32_t size );

/* ����XML������ */
void osl_xml_destroy( void* handle );

/* ����XML�ı�������ŵ����� */
SXmlNode* osl_xml_parse( void* handle, char *txtbuf, int32_t size );

/* ����XML�ı�������ŵ����� */
SXmlNode* osl_xml_parse_file( void* handle, char *file );

/* ���������� */
void osl_xml_clear( void* handle );

/* ����ָ�����ƵĽڵ� */
SXmlNode* osl_xml_search( void* handle, SXmlNode *root, char *name );

/* ��������ֵ */
char* osl_xml_get_attr( SXmlNode *ele, char *name );

/* ���XMLͷ */
int32_t osl_xml_output_header( void* handle, char *txtbuf, int32_t size, int32_t tab );

/* ���XML�ڵ������ڵ� */
int32_t osl_xml_output_content( void* handle, SXmlNode* root, char *txtbuf, int32_t size, int32_t tab );


#ifdef __cplusplus
}
#endif


#endif /* __OSL_XML_H__ */
