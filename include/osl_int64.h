#ifndef __OSL_INT64_H__
#define __OSL_INT64_H__

#ifdef __cplusplus
extern "C"
{
#endif


/* network -> host */
uint64_t osl_ntohl64(uint64_t arg64);

/* host -> network */
uint64_t osl_htonl64(uint64_t arg64);


#ifdef __cplusplus
}
#endif

#endif  /* __OSL_INT64_H__ */
