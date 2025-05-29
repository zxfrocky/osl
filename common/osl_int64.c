#include "stdafx.h"

/* ²âÊÔ×Ö½ÚË³Ðò */
static int32_t osl_endian_test_order();

/* ×Ö½ÚË³Ðò£º1=big_endian,-1=little_enddian */
static int32_t m_order = 0;

/* network -> host */
uint64_t osl_ntohl64(uint64_t arg64)
{
	uint64_t res64;
	uint32_t low, high;

	if( m_order == 0 )
		m_order = osl_endian_test_order();

	if( m_order == -1 )
	{
		low = (uint32_t) (arg64 & (uint64_t)0x00000000FFFFFFFF);
		high = (uint32_t) ((arg64 & ((uint64_t)0xFFFFFFFF00000000)) >> 32);
		low = ntohl(low);
		high = ntohl(high);

		res64 = (uint64_t) high + (((uint64_t) low) << 32);
		return res64;
	}
	else
		return arg64;
}

/* host -> network */
uint64_t osl_htonl64(uint64_t arg64)
{
	uint64_t res64;
	uint32_t low, high;

	if( m_order == 0 )
		m_order = osl_endian_test_order();

	if( m_order == -1 )
	{
		low = (uint32_t) (arg64 & (uint64_t)0x00000000FFFFFFFF);
		high = (uint32_t) ((arg64 & ((uint64_t)0xFFFFFFFF00000000)) >> 32);
		low = htonl(low);
		high = htonl(high);

		res64 = (uint64_t) high + (((uint64_t) low) << 32);
		return res64;
	}
	else
		return arg64;
}

static int32_t osl_endian_test_order()
{
	uint64_t val = 0x0102030405060708;
	uchar_t *p = (uchar_t*)&val;

	return *p == 0x01 ? 1 : -1;
}
