#ifndef BYTEORDER_H_
#define BYTEORDER_H_

#include <stdint.h>
#include "byteswap.h"

#ifdef __USE_BSD

# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define uhtobe16(x) __bswap_16 (x)
#  define uhtole16(x) (x)
#  define ube16toh(x) __bswap_16 (x)
#  define ule16toh(x) (x)

#  define uhtobe32(x) __bswap_32 (x)
#  define uhtole32(x) (x)
#  define ube32toh(x) __bswap_32 (x)
#  define ule32toh(x) (x)

#  define uhtobe64(x) __bswap_64 (x)
#  define uhtole64(x) (x)
#  define ube64toh(x) __bswap_64 (x)
#  define ule64toh(x) (x)
# else
#  define uhtobe16(x) (x)
#  define uhtole16(x) __bswap_16 (x)
#  define ube16toh(x) (x)
#  define ule16toh(x) __bswap_16 (x)

#  define uhtobe32(x) (x)
#  define uhtole32(x) __bswap_32 (x)
#  define ube32toh(x) (x)
#  define ule32toh(x) __bswap_32 (x)

#  define uhtobe64(x) (x)
#  define uhtole64(x) __bswap_64 (x)
#  define ube64toh(x) (x)
#  define ule64toh(x) __bswap_64 (x)
# endif


uint16_t be16dec(const void *ptr);
uint32_t be32dec(const void *ptr);
uint64_t be64dec(const void *ptr);
uint16_t le16dec(const void *ptr);
uint32_t le32dec(const void *ptr);
uint64_t le64dec(const void *ptr);

void be16enc(void *ptr, uint16_t h16);
void be32enc(void *ptr, uint32_t h32);
void be64enc(void *ptr, uint64_t h64);
void le16enc(void *ptr, uint16_t h16);
void le32enc(void *ptr, uint32_t h32);
void le64enc(void *ptr, uint64_t h64);
#endif //__USE_BSD
double ledbltoh(double val);
double htoledbl(double val);

#endif /* BYTEORDER_H_ */
