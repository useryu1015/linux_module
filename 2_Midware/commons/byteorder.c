#include <string.h>
#include <endian.h>
#include "byteorder.h"

#ifdef __USE_BSD

uint16_t be16dec(const void *ptr) {
	uint16_t be16 = 0;
	memmove(&be16, ptr, sizeof(uint16_t));
	return ube16toh(be16);
}

uint32_t be32dec(const void *ptr) {
	uint32_t be32 = 0;
	memmove(&be32, ptr, sizeof(uint32_t));
	return ube32toh(be32);
}

uint64_t be64dec(const void *ptr) {
	uint64_t be64 = 0;
	memmove(&be64, ptr, sizeof(uint64_t));
	return ube64toh(be64);
}

uint16_t le16dec(const void *ptr) {
	uint16_t le16 = 0;
	memmove(&le16, ptr, sizeof(uint16_t));
	return ule16toh(le16);
}

uint32_t le32dec(const void *ptr) {
	uint32_t le32 = 0;
	memmove(&le32, ptr, sizeof(uint32_t));
	return ule32toh(le32);
}
uint64_t le64dec(const void *ptr) {
	uint64_t le64 = 0;
	memmove(&le64, ptr, sizeof(uint64_t));
	return ule64toh(le64);
}

void be16enc(void *ptr, uint16_t h16) {
	uint16_t be16 = uhtobe16(h16);
	memmove(ptr, &be16, sizeof(uint16_t));
}

void be32enc(void *ptr, uint32_t h32) {
	uint32_t be32 = uhtobe32(h32);
	memmove(ptr, &be32, sizeof(uint32_t));
}

void be64enc(void *ptr, uint64_t h64) {
	uint64_t be64 = uhtobe64(h64);
	memmove(ptr, &be64, sizeof(uint64_t));
}

void le16enc(void *ptr, uint16_t h16) {
	uint16_t le16 = uhtole16(h16);
	memmove(ptr, &le16, sizeof(uint16_t));
}

void le32enc(void *ptr, uint32_t h32) {
	uint32_t le32 = uhtole32(h32);
	memmove(ptr, &le32, sizeof(uint32_t));
}

void le64enc(void *ptr, uint64_t h64) {
	uint64_t le64 = uhtole64(h64);
	memmove(ptr, &le64, sizeof(uint64_t));
}
#endif //__USE_BSD

static inline double __dbl_swap(double val) {
# if __FLOAT_WORD_ORDER == __BIG_ENDIAN
	int i;
	uint8_t b[8], *p = (uint8_t*) &val;

	for (i = 0; i < 4; ++i) {
		b[i] = p[i + 4];
		b[i + 4] = p[i];
	}

	return *(double*) (b);
#else
	return val;
#endif

}

double ledbltoh(double val) {
	return __dbl_swap(val);
}

double htoledbl(double val) {
	return __dbl_swap(val);
}
