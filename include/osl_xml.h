#ifndef __OSL_XML_H__
#define __OSL_XML_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* XML属性链表 */
typedef struct tagXmlAttr
{
	char *name;
	char *value;
	struct tagXmlAttr *next;
}SXmlAttr;

/* XML节点 */
typedef struct tagXmlNode
{
	/* TAG名称 */
	char *name;
	/* 值, <>...</> */
	char *value;
	/* 属性 */
	SXmlAttr *attrs;
	/* 父节点 */
	struct tagXmlNode *parent;
	/* 子节点 */
	struct tagXmlNode *children;
	/* 弟节点 */
	struct tagXmlNode *siblings;
}SXmlNode;


/* 创建XML分析器 */
void* osl_xml_create( char_t *buf, int32_t size );

/* 销毁XML分析器 */
void osl_xml_destroy( void* handle );

/* 分析XML文本，结果放到树中 */
SXmlNode* osl_xml_parse( void* handle, char *txtbuf, int32_t size );

/* 分析XML文本，结果放到树中 */
SXmlNode* osl_xml_parse_file( void* handle, char *file );

/* 清除分析结果 */
void osl_xml_clear( void* handle );

/* 查找指定名称的节点 */
SXmlNode* osl_xml_search( void* handle, SXmlNode *root, char *name );

/* 查找属性值 */
char* osl_xml_get_attr( SXmlNode *ele, char *name );

/* 输出XML头 */
int32_t osl_xml_output_header( void* handle, char *txtbuf, int32_t size, int32_t tab );

/* 输出XML节点的子孙节点 */
int32_t osl_xml_output_content( void* handle, SXmlNode* root, char *txtbuf, int32_t size, int32_t tab );


#ifdef __cplusplus
}
#endif


#endif /* __OSL_XML_H__ */
