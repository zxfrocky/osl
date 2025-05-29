#include "stdafx.h"
#include "osl_base64.h"

#define XYSSL_ERR_BASE64_BUFFER_TOO_SMALL -0x0010  
#define XYSSL_ERR_BASE64_INVALID_CHARACTER -0x0012 

static const uchar_t base64_enc_map[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

static const uchar_t base64_dec_map[128] =
{
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 62, 127, 127, 127, 63, 52, 53,
	54, 55, 56, 57, 58, 59, 60, 61, 127, 127,
	127, 64, 127, 127, 127, 0, 1, 2, 3, 4,
	5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 127, 127, 127, 127, 127, 127, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
	39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 127, 127, 127, 127, 127
};

/*
* Encode a buffer into base64 format
*/
int32_t osl_base64_encode(const uchar_t *input, int32_t input_length,uchar_t *output, int32_t output_length)
{
	int32_t i, n;
	int32_t C1, C2, C3;
	uchar_t *p;

	if( input_length == 0 )
		return( 0 );

	n = (input_length << 3) / 6;

	switch( (input_length << 3) - (n * 6) )
	{
	case 2: n += 3; break;
	case 4: n += 2; break;
	default: break;
	}

	if( output_length < n + 1 )
	{
		return( XYSSL_ERR_BASE64_BUFFER_TOO_SMALL );
	}

	n = (input_length / 3) * 3;

	for( i = 0, p = output; i < n; i += 3 )
	{
		C1 = *input++;
		C2 = *input++;
		C3 = *input++;

		*p++ = base64_enc_map[(C1 >> 2) & 0x3F];
		*p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];
		*p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
		*p++ = base64_enc_map[C3 & 0x3F];
	}

	if( i < input_length )
	{
		C1 = *input++;
		C2 = ((i + 1) < input_length) ? *input++ : 0;

		*p++ = base64_enc_map[(C1 >> 2) & 0x3F];
		*p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

		if( (i + 1) < input_length )
			*p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
		else *p++ = '=';

		*p++ = '=';
	}
	output_length = p - output;
	*p = 0;
	return output_length;
	//return( 0 );
}

/*
* Decode a base64-formatted buffer
*/
int32_t osl_base64_decode(const uchar_t *input, int32_t input_length, uchar_t *output, int32_t output_length)
{
	int32_t i, j, n;
	unsigned long x;
	uchar_t *p;

	for( i = j = n = 0; i < input_length; i++ )
	{
		if( ( input_length - i ) >= 2 &&
			*(input+i) == '\r' && *(input+i+1) == '\n' )
			continue;

		if(*(input+i) == '\n' )
			continue;

		if(*(input+i) == '=' && ++j > 2 )
			return( XYSSL_ERR_BASE64_INVALID_CHARACTER );

		if(*(input+i) > 127 || base64_dec_map[*(input+i)] == 127 )
			return( XYSSL_ERR_BASE64_INVALID_CHARACTER );

		if( base64_dec_map[*(input+i)] < 64 && j != 0 )
			return( XYSSL_ERR_BASE64_INVALID_CHARACTER );

		n++;
	}

	if( n == 0 )
		return( 0 );

	n = ((n * 6) + 7) >> 3;

	if( output_length < n )
	{
		return( XYSSL_ERR_BASE64_BUFFER_TOO_SMALL );
	}

	for( j = 3, n = x = 0, p = output; i > 0; i--, input++ )
	{
		if( *input == '\r' || *input == '\n' )
			continue;

		j -= ( base64_dec_map[*input] == 64 );
		x = (x << 6) | ( base64_dec_map[*input] & 0x3F );

		if( ++n == 4 )
		{
			n = 0;
			if( j > 0 ) *p++ = (uchar_t)( x >> 16 );
			if( j > 1 ) *p++ = (uchar_t)( x >> 8 );
			if( j > 2 ) *p++ = (uchar_t)( x );
		}
	}

	output_length = (p - output);

	//return( 0 );
	return output_length;
}

/* ÉËº¦ÐÔ±àÂë£¨²»¿ÉÄæ£©*/
int32_t osl_base64_encode_hurt(const uchar_t *input, int32_t input_length, uchar_t *output, int32_t output_length)
{
	/* base64 ±à½âÂë */
	static uchar_t basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789XX";

	int32_t	i, j;
	int32_t	pad;

	if( output_length < (input_length * 4 / 3) )
		return 0;

	i = j = 0;
	while (i < input_length)
	{
		pad = 3 - (input_length - i);
		if (pad == 2)
		{
			output[j]   = basis_64[input[i]>>2];
			output[j+1] = basis_64[(input[i] & 0x03) << 4];
			output[j+2] = 'X';
			output[j+3] = 'X';
		}
		else if (pad == 1)
		{
			output[j]   = basis_64[input[i]>>2];
			output[j+1] = basis_64[((input[i] & 0x03) << 4) | ((input[i+1] & 0xf0) >> 4)];
			output[j+2] = basis_64[(input[i+1] & 0x0f) << 2];
			output[j+3] = 'X';
		}
		else
		{
			output[j]   = basis_64[input[i]>>2];
			output[j+1] = basis_64[((input[i] & 0x03) << 4) | ((input[i+1] & 0xf0) >> 4)];
			output[j+2] = basis_64[((input[i+1] & 0x0f) << 2) | ((input[i+2] & 0xc0) >> 6)];
			output[j+3] = basis_64[input[i+2] & 0x3f];
		}
		i += 3;
		j += 4;
	}
	return j;
}
