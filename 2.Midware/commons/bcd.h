#ifndef BCD_H_
#define BCD_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef unsigned char byte;
typedef unsigned char bcd_t;

#ifdef  __cplusplus
extern "C" {
#endif

unsigned int get_bcd(byte b);
void set_bcd(byte* b, unsigned int value);

const char* sprintf_bcd(char* buf, const byte* b, unsigned int digits);
void sscanf_bcd(const char* buf, byte* b, unsigned int digits);

/** 读有符号整数 */
long read_signal_int(const byte* b, unsigned int digits);
/** 写有符号整数 */
void write_signal_int(byte* b, unsigned int digits, long value);

/** 读无符号整数 */
unsigned long read_unsignal_int(const byte* b, unsigned int digits);
/** 写无符号整数 */
void write_unsignal_int(byte* b, unsigned int digits, unsigned long value);

/** 读无符号64bit整数 */
unsigned long long read_unsignal_int64(const byte* b, unsigned int digits);
/** 写无符号64bit整数 */
void write_unsignal_int64(byte* b, unsigned int digits, unsigned long long value);

/** 读有符号浮点数 */
double read_signal_decimal(const byte* b, unsigned int digits, unsigned int decimals);
/** 读无符号浮点数 */
double read_unsignal_decimal(const byte* b, unsigned int digits, unsigned int decimals);

typedef enum _date_offset_e {
	DT_OFFSET_SECOND = 0,
	DT_OFFSET_MINUTE = DT_OFFSET_SECOND + 2,
	DT_OFFSET_HOUR = DT_OFFSET_MINUTE + 2,
	DT_OFFSET_DAY = DT_OFFSET_HOUR + 2,
	DT_OFFSET_MONTH = DT_OFFSET_DAY + 2,
	DT_OFFSET_YEAR = DT_OFFSET_MONTH + 2
} date_offset_e;

/** 读时间
 * digits：数字位数
 * offset：开始偏移 */
time_t read_datetime(const byte* b, unsigned int digits, date_offset_e offset);

char* bytes2bits(char *buf, uint8_t *val, int n);
_Bool bits2bytes(char *buf, uint8_t *val, int n);

void ascii_to_binary(const char* buf, uint8_t* bin, unsigned int digits);
char* binary_to_ascii(char* buf, uint8_t* bin, unsigned int len);
bool IsBigEndian();

#ifdef  __cplusplus
}
#endif

#endif /* BCD_H_ */
