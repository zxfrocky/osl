#include "stdafx.h"
#include "osl.h"
#include "osl_file.h"
#include "osl_string.h"
#include "osl_xml.h"


typedef enum
{
	MARK_END = 0,
	MARK_TEXT,
	MARK_TAGBEGIN,      /* '<'  */
	MARK_CLOSETAGBEGIN, /* '</' */
	MARK_TAGEND,        /* '>'  */
	MARK_CLOSETAGEND,   /* '/>' */
	MARK_EQUAL,         /* '='  */
	MARK_NAME,
	MARK_VALUE
}EMarkType;


typedef struct
{
	/* 根节点 */
	SXmlNode *root;

	/* 工作缓冲区 */
	char_t *buf;
	/* 工作缓冲区大小 */
	int32_t bufsize;
	/* 可用区域位置 */
	int32_t tail;

	/* 是否已经进入'<' */
	int32_t bInTag;
	/* 当前处理XML内容位置 */
	char_t *ptr;
	/* 当前类型 */
	int32_t tagtype;
}SBclXml;


/* 分析下一个元素 */
static int32_t osl_xml_parse_element( void* handle, SXmlNode *parent, SXmlNode *lastnode, SXmlNode **ppNode );
/* 取得下一个TAG */
static EMarkType osl_xml_get_mark_tag( void* handle, char_t **pmark );
/* 取得下一个IN_TAG */
static EMarkType osl_xml_get_mark_in_tag( void* handle, char_t **pmark );
/* 取得下一个OUT_TAG */
EMarkType osl_xml_get_mark_out_tag( void* handle, char_t **pmark );
/* 分配内存 */
static void *osl_xml_malloc( void* handle, int32_t size );
/* 复制字符串 */
static char_t *osl_xml_strdup( void* handle, char_t *str, int32_t len );


/* 创建XML分析器 */
void* osl_xml_create( char_t* buf, int32_t size )
{
	SBclXml *xml;

	memset( buf, 0, size );

	xml = (SBclXml *)buf;
	xml->buf = ((char_t*)xml) + sizeof(SBclXml);
	xml->bufsize = size - sizeof(SBclXml);
	return xml;
}

/* 销毁XML分析器 */
void osl_xml_destroy( void* handle )
{

}

/* 分析XML文本，结果放到树中 */
SXmlNode* osl_xml_parse( void* handle, char_t *txtbuf, int32_t size )
{
	SBclXml *xml = (SBclXml*)handle;
	SXmlNode *node, *lastnode;
	char_t tmpch = '\0';
	char_t *p;

	if( txtbuf == NULL || txtbuf[0] == '\0' )
		goto ERROR_EXIT;

	if( size < 0 )
	{
		p = txtbuf;
		while( *p )
			p++;
		size = (int32_t)(p -txtbuf);
	}
	tmpch = txtbuf[size];
	txtbuf[size] = '\0';

	xml->bInTag = 0;
	xml->ptr = txtbuf;
	if( size <= 0 )
		size = (int32_t)strlen( txtbuf );

	/* 分析根节点和根节点的兄弟 */
	lastnode = NULL;
	while( *xml->ptr && xml->ptr < txtbuf + size )
	{
		xml->tagtype = MARK_END;
		if( osl_xml_parse_element( xml, NULL, lastnode, &node ) != 0 )
			goto ERROR_EXIT;
	}
	txtbuf[size] = tmpch;
	return xml->root;

ERROR_EXIT:
	if( tmpch )
		txtbuf[size] = tmpch;

	return NULL;
}

/* 分析XML文件 */
SXmlNode* osl_xml_parse_file( void* handle, char_t *file )
{
	int32_t size;
	char_t *buf = NULL;
	void *fp = NULL;
	SXmlNode* root = NULL;

	/* 打开文件 */
	fp = osl_file_open( file, OSL_FILE_READ_ONLY );
	if( fp == NULL )
	{
		printf( "can't open the xml file !\n" );
		goto ERROR_EXIT;
	}

	/* 读取信息 */
	osl_file_seek( fp, 0, SEEK_END );
	size = (int32_t)osl_file_tell( fp );
	buf = (char_t*)malloc( size + 1 );
	if( buf == NULL )
	{
		printf( "no enough memory for %s !\n", file );
		goto ERROR_EXIT;
	}
	osl_file_seek( fp, 0, SEEK_SET );
	size = osl_file_read( fp, buf, size );
	buf[size] = 0;

	if( fp )
		osl_file_close( fp );

	/* 分析并保存 */
	root = osl_xml_parse( handle, buf, size );
	free( buf );
	return root;

ERROR_EXIT:
	if( fp )
		osl_file_close( fp );
	if( buf )
		free( buf );
	return NULL;
}

/* 清除分析结果 */
void osl_xml_clear( void* handle )
{
	SBclXml *xml = (SBclXml*)handle;

	xml->root = NULL;
	xml->tail = 0;
	xml->bInTag = 0;
	xml->ptr = NULL;
	xml->tagtype = MARK_END;
}

/* 查找指定名称的节点 */
SXmlNode* osl_xml_search( void* handle, SXmlNode *root, char_t *name )
{
	SBclXml *xml = (SBclXml*)handle;
	SXmlNode *node, *child;

	if( root == NULL )
		root = xml->root;

	node = root->children;
	while( node )
	{
		if( osl_strcmp_nocase( name, node->name ) == 0 )
			return node;

		child = osl_xml_search( handle, node, name );
		if( child )
			return child;

		node = node->siblings;
	}

	return NULL;
}

/* 查找属性值 */
char_t* osl_xml_get_attr( SXmlNode *node, char_t *name )
{
	SXmlAttr *attr = node->attrs;

	while( attr )
	{
		if( osl_strcmp_nocase( attr->name, name ) == 0 )
			return attr->value;
		attr = attr->next;
	}
	return NULL;
}


/* 输出XML头 */
int32_t osl_xml_output_header( void* handle, char_t *txtbuf, int32_t size, int32_t tab )
{
	int32_t i, n=0;

	for( i=0; i<tab; i++ )
		txtbuf[i] = '\t';
	n = tab + osl_str_snprintf( txtbuf+tab, size-tab, "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n" );
	txtbuf[n] = 0;

	return n;
}


/* 输出XML节点的子孙节点 */
int32_t osl_xml_output_content( void* handle, SXmlNode* root, char_t *txtbuf, int32_t size, int32_t tab )
{
	SBclXml *xml = (SBclXml*)handle;
	SXmlNode *node, *child;
	SXmlAttr *attr;
	int32_t i, n=0;

	node = root ? root : xml->root;
	while( node && n+256 < size )
	{
		/* 输出<name */
		for( i=0; i<tab; i++ )
			n += osl_str_snprintf( txtbuf+n, size-n, "\t" );
		n += osl_str_snprintf( txtbuf+n, size-n, "<%s", node->name );

		/* 输出属性 */
		attr = node->attrs;
		while( attr )
		{
			n += osl_str_snprintf( txtbuf+n, size-n, " %s=\"%s\"", attr->name, attr->value );
			attr = attr->next;
		}

		/* 没有值和子节点时，直接以/>结束 */
		child = node->children;
		if( node->value == NULL && child == NULL )
		{
			n += osl_str_snprintf( txtbuf+n, size-n, " />\n" );
		}
		else
		{
			n += osl_str_snprintf( txtbuf+n, size-n, ">" );
			if( node->value )
				n += osl_str_snprintf( txtbuf+n, size-n, "%s", node->value );

			/* 输出子节点 */
			if( child )
			{
				n += osl_str_snprintf( txtbuf+n, size-n, "\n" );
				n += osl_xml_output_content( handle, child, txtbuf+n, size-n, tab+1 );
				for( i=0; i<tab; i++ )
					n += osl_str_snprintf( txtbuf+n, size-n, "\t" );
			}

			n += osl_str_snprintf( txtbuf+n, size-n, "</%s>\n", node->name );
		}
		node = node->siblings;;
	}
	txtbuf[n] = 0;
	return n;
}


/* 分析下一个元素 */
static int32_t osl_xml_parse_element( void* handle, SXmlNode *parent, SXmlNode *lastnode, SXmlNode **ppNode )
{
	SBclXml *xml = (SBclXml*)handle;
	EMarkType type;
	SXmlNode *node, *child;
	SXmlAttr *lastattr, *attr;
	char_t *mark;

	*ppNode = NULL;

	/* 寻找'<' */
	type = (EMarkType)xml->tagtype;
	if( type != MARK_TAGBEGIN )
		type = osl_xml_get_mark_tag( handle, &mark );
	if( *xml->ptr == 0 )
		return 0;
	if( type != MARK_TAGBEGIN )
		return -1;

	/* 寻找元素名称 */
	type = osl_xml_get_mark_tag( handle, &mark );
	if( type != MARK_NAME || mark == NULL )
		return -1;

	/* '<!--'是注释，跳过，直到'>'为止 */
	if( strncmp( mark, "!--", 3 ) == 0 || osl_strcmp_nocase( mark, "!DOCTYPE" ) == 0 )
	{
		while( type != MARK_TAGEND || *xml->ptr == 0 )
			type = osl_xml_get_mark_tag( handle, &mark );
		return 0;
	}

	/* 保存元素名称 */
	node = (SXmlNode *)osl_xml_malloc( handle, sizeof(SXmlNode) );
	if( node == NULL )
		return -1;
	memset( node, 0, sizeof(SXmlNode) );
	node->name = mark;

	/* 分析元素属性<...> */
	lastattr = NULL;
	while( 1 )
	{
		type = osl_xml_get_mark_tag( handle, &mark );
		if( type != MARK_NAME || mark == NULL )
			break;

		attr = (SXmlAttr *)osl_xml_malloc( handle, sizeof(SXmlAttr) );
		memset( attr, 0, sizeof(*attr) );

		attr->name = mark;

		/* 寻找属性分隔符'=' */
		type = osl_xml_get_mark_tag( handle, &mark );
		if(type != MARK_EQUAL)
			return -1;

		/* 寻找属性值 */
		type = osl_xml_get_mark_tag( handle, &mark );
		if(type != MARK_VALUE)
			return -1;

		attr->value = mark;

		if(lastattr)
			lastattr->next = attr;
		else
			node->attrs = attr;
		lastattr = attr;
	}

	if( osl_strcmp_nocase(node->name,"?xml") == 0 )
		return 0;

	/* 将节点加入到树中 */
	node->parent = parent;
	if( lastnode )
		lastnode->siblings = node;
	else if( parent )
		parent->children = node;
	else
		xml->root = node;

	/* 找到'>'，分析: '>...</' */
	if( type == MARK_TAGEND )
	{
		type = osl_xml_get_mark_tag( handle, &mark );
		/* 分析所有直系子节点 */
		lastnode = NULL;
		while( type == MARK_TAGBEGIN )
		{
			xml->tagtype = MARK_TAGBEGIN;
			if( osl_xml_parse_element( handle, node, lastnode, &child ) != 0 )
				return -1;

			lastnode = child;
			type = osl_xml_get_mark_tag( handle, &mark );
		}

		/* 文本 */
		if( type == MARK_TEXT )
		{
			node->value = mark;
			/* 寻找'</' */
			type = osl_xml_get_mark_tag( handle, &mark );
		}
	}
	/* 找到'</' */
	if( type == MARK_CLOSETAGBEGIN )
	{
		type = osl_xml_get_mark_tag( handle, &mark );
		if( type != MARK_NAME || osl_strcmp_nocase(mark, node->name) != 0 )
		{
			return -1;
		}
		type = osl_xml_get_mark_tag( handle, &mark );
		if( type != MARK_TAGEND )
		{
			return -1;
		}
		*ppNode = node;
		return 0;
	}
	/* 找到'/>' */
	else if( type == MARK_CLOSETAGEND )
	{
		*ppNode = node;
		return 0;
	}

	return -1;
}

/* 取得下一个TAG */
static EMarkType osl_xml_get_mark_tag( void* handle, char_t **pmark )
{
	SBclXml *xml = (SBclXml*)handle;

	if( xml->bInTag )
		return osl_xml_get_mark_in_tag( handle, pmark );
	else
		return osl_xml_get_mark_out_tag( handle, pmark );
}

/* 取得下一个IN_TAG */
static EMarkType osl_xml_get_mark_in_tag( void* handle, char_t **pmark )
{
	SBclXml *xml = (SBclXml*)handle;
	char_t *p, c;

	/* 跳过空格 */
	while(*xml->ptr == ' ' || *xml->ptr == '\t' || *xml->ptr == '\r' || *xml->ptr == '\n' || *xml->ptr == ',' )
		++xml->ptr;

	c = *(xml->ptr++);

	if(c == '\0')
		return MARK_TAGEND;

	if(c == '>')
	{
		xml->bInTag = 0;
		return MARK_TAGEND;
	}

	if((c == '/' || c == '?') && *xml->ptr == '>')
	{
		xml->ptr++;
		xml->bInTag = 0;
		return MARK_CLOSETAGEND;
	}

	if(c == '=')
		return MARK_EQUAL;

	if(c == '\'' || c == '"')
	{
		p = xml->ptr;
		while(*xml->ptr && *xml->ptr != c)
			++xml->ptr;
		if(!(*xml->ptr))
			return MARK_TAGEND;
		if( pmark )
			*pmark = osl_xml_strdup( handle, p, (int32_t)(xml->ptr - p) );

		xml->ptr++;
		return MARK_VALUE;
	}

	/* 获取字符串的值 */
	p = xml->ptr - 1;
	while(*xml->ptr && *xml->ptr != ' ' && *xml->ptr != '\t' && *xml->ptr != '\r' && *xml->ptr != '\n'
			&& *xml->ptr != '>' && *xml->ptr != '/' && *xml->ptr != '?' && *xml->ptr != '=')
		++xml->ptr;
	if( 0 == *xml->ptr )
		return MARK_TAGEND;
	if( pmark )
		*pmark = osl_xml_strdup( handle, p, (int32_t)(xml->ptr - p) );
	return MARK_NAME;
}


/* 取得下一个OUT_TAG */
EMarkType osl_xml_get_mark_out_tag( void* handle, char_t **pmark )
{
	SBclXml *xml = (SBclXml*)handle;
	char_t c;
	int32_t inComment;
	char_t *start = NULL;
	char_t *end = NULL;

	inComment = 0;
	for(c=*(xml->ptr++); *xml->ptr != '\0' && c != '\0'; c=*(xml->ptr++))
	{
		/* <--   --> */
		if(inComment)
		{
			if(c == '-' && xml->ptr[0] == '-' && xml->ptr[1] == '>')
			{
				inComment = 0;
				xml->ptr += 2;
			}
			continue;
		}

		if(c == '\r' || c == '\n' || c == ' ' || c == '\t')
			continue;

		if(c == '<')
		{
			/* <!-- 注解 */
			if(*xml->ptr == '!' && xml->ptr[1]=='-' && xml->ptr[2]=='-')
			{
				inComment = 1;
				xml->ptr += 3;
				continue;
			}

			if( start == NULL || end == NULL )
			{
				xml->bInTag = 1;
				if(*xml->ptr == '/')
				{
					++xml->ptr;
					return MARK_CLOSETAGBEGIN;
				}
				return MARK_TAGBEGIN;
			}
			else
			{
				if( pmark )
					*pmark = osl_xml_strdup( handle, start, (int32_t)(end-start+1) );
				xml->ptr--;
				return MARK_TEXT;
			}
		}

		if( start == NULL )
			start = xml->ptr - 1;
		end = xml->ptr - 1;
	}

	if( start == NULL || end == NULL || end <= start )
		return MARK_END;

	if( pmark )
		*pmark = osl_xml_strdup( handle, start, (int32_t)(end-start+1) );
	return MARK_TEXT;
}

/* 分配内存 */
static void *osl_xml_malloc( void* handle, int32_t size )
{
	SBclXml *xml = (SBclXml*)handle;
	char_t *buf;

	if( xml->tail + size < xml->bufsize )
	{
		buf = xml->buf + xml->tail;
		xml->tail = xml->tail + ((size+3)&~3);
		return buf;
	}
	else
		return NULL;
}


/* 复制字符串 */
static char_t *osl_xml_strdup( void* handle, char_t *str, int32_t len )
{
	char_t *p;
	p = (char_t *)osl_xml_malloc( handle, len + 1);
	if( p )
	{
		osl_strncpy(p, str, len);
		p[len] = 0;
	}
	return p;
}
