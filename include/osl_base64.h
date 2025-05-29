#ifndef __OSL_BASE64_H__
#define __OSL_BASE64_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* base64���� */
int32_t osl_base64_encode(const uchar_t *input, int32_t input_length, uchar_t *output, int32_t output_length);

/* base64���� */
int32_t osl_base64_decode(const uchar_t *input, int32_t input_length, uchar_t *output, int32_t output_length);

/* base64�˺��Ա��루�����棩*/
int32_t osl_base64_encode_hurt(const uchar_t *input, int32_t input_length, uchar_t *output, int32_t output_length);

#ifdef __cplusplus
}
#endif

#endif /* __OSL_BASE64_H__ */
