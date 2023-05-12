#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "bcd.h"

struct _bcd_t {
	uint8_t l :4;
	uint8_t h :4;
}
#ifdef __linux
__attribute__((packed))
#endif
;

struct _sig_bcd_t {
	uint8_t l :4;
	uint8_t h :3;
	_Bool sig :1;
}
#ifdef __linux__
__attribute__((packed))
#endif
;

unsigned int get_bcd(byte b) {
	return (b >> 4) * 10 + (b & 0x0f);
}

void set_bcd(byte* b, unsigned int value) {
	struct _bcd_t bcd;

	bcd.l = value % 10;
	bcd.h = (value / 10) % 10;
	memcpy(b, &bcd, sizeof(byte));
}

const char* sprintf_bcd(char* buf, const byte* b, unsigned int digits) {
	unsigned int i, j;

	for (i = (digits + 1) / 2 - 1, j = 0; i >= 0 && j < digits; --i, j += 2) {
		sprintf(buf + j, "%02x", b[i]);
	}

	return buf;
}

void sscanf_bcd(const char* buf, byte* b, unsigned int digits) {
	unsigned int i, j;
	int value = 0;

	for (i = (digits + 1) / 2 - 1, j = 0; i >= 0 && j < digits; --i, j += 2) {
		if (sscanf(buf + j, "%02x", &value) >= 1) {
			b[i] = value;
		} else {
			b[i] = 0;
		}
	}
}

static unsigned long _pow10_tbl_8[8] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7 };

static unsigned long _pow10(unsigned int e) {
	unsigned int tbl_size = 8;

	if (e < tbl_size) {
		return _pow10_tbl_8[e];
	} else {
		unsigned int i;
		unsigned long long result = 1e7;

		for (i = tbl_size; i < e; ++i) {
			result *= 10;
		}

		return result;
	}
}

unsigned long read_unsignal_int(const byte* b, unsigned int digits) {
	unsigned int i;
	unsigned long result = 0;

	for (i = 0; i < digits / 2; ++i) {
		result += get_bcd(b[i]) * _pow10_tbl_8[i * 2];
	}

	return result;
}

void write_unsignal_int(byte* b, unsigned int digits, unsigned long value) {
	unsigned int i;

	for (i = 0; i < digits / 2; ++i) {
		set_bcd(&b[i], value);
		value /= 100;
	}
}

long read_signal_int(const byte* b, unsigned int digits) {
	long result;
	bool negative;
	struct _sig_bcd_t* sig;

	sig = (struct _sig_bcd_t*) &b[(digits - 1) / 2];
	negative = sig->sig;
	sig->sig = 0;
	result = read_unsignal_int(b, digits);

	return negative ? -result : result;
}

void write_signal_int(byte* b, unsigned int digits, long value) {
	bool negative;
	struct _sig_bcd_t* sig;

	negative = value < 0;
	value = value < 0 ? -value : value;

	write_unsignal_int(b, digits, value);

	sig = (struct _sig_bcd_t*) &b[(digits - 1) / 2];
	sig->sig = negative;
}

double read_signal_decimal(const byte* b, unsigned int digits, unsigned int decimals) {
	double result = read_signal_int(b, digits);

	return result / _pow10(decimals);
}

double read_unsignal_decimal(const byte* b, unsigned int digits, unsigned int decimals) {
	double result = read_unsignal_int(b, digits);

	return result / _pow10(decimals);
}

static unsigned long long _pow10_tbl_12[12] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11 };

unsigned long long read_unsignal_int64(const byte* b, unsigned int digits) {
	unsigned int i;
	unsigned long long result = 0;

	for (i = 0; i < digits / 2; ++i) {
		result += get_bcd(b[i]) * _pow10_tbl_12[i * 2];
	}

	return result;
}

void write_unsignal_int64(byte* b, unsigned int digits, unsigned long long value) {
	unsigned int i;

	for (i = 0; i < digits / 2; ++i) {
		set_bcd(&b[i], value);
		value /= 100;
	}
}

time_t read_datetime(const byte* b, unsigned int digits, date_offset_e offset) {
	unsigned int i, j;
	time_t sys_time;
	struct tm* tm;

	time(&sys_time);
	tm = localtime(&sys_time);

	switch (offset) {
	case DT_OFFSET_YEAR:
		tm->tm_year = 2000;
		/* no break */
	case DT_OFFSET_MONTH:
		tm->tm_mon = 1;
		/* no break */
	case DT_OFFSET_DAY:
		tm->tm_mday = 1;
		/* no break */
	case DT_OFFSET_HOUR:
		tm->tm_hour = 0;
		/* no break */
	case DT_OFFSET_MINUTE:
		tm->tm_min = 0;
		/* no break */
	case DT_OFFSET_SECOND:
		tm->tm_sec = 0;
		break;
	}

	for (i = 0, j = offset + 2 - digits; i < digits / 2; ++i, j += 2) {
		int value = get_bcd(b[i]);
		switch (j) {
		case DT_OFFSET_SECOND:
			tm->tm_sec = value;
			break;
		case DT_OFFSET_MINUTE:
			tm->tm_min = value;
			break;
		case DT_OFFSET_HOUR:
			tm->tm_hour = value;
			break;
		case DT_OFFSET_DAY:
			tm->tm_mday = value;
			break;
		case DT_OFFSET_MONTH:
			tm->tm_mon = value - 1;
			break;
		case DT_OFFSET_YEAR:
			tm->tm_year = (value > 70) ? 0 : 100 + value;
			break;
		}
	}

	return mktime(tm);
}

char* bytes2bits(char *buf, uint8_t *val, int n)
{
    int i = 0, j = n / 8;
    char *ps = buf;

    do {
        i = (j == n / 8) ? (n % 8) : 8;

        for (; i > 0; --i) {
            ps += sprintf(ps, (val[j] >> (i-1)) & 0x01 ? "1" : "0");
        }
    } while(j--);

    return buf;
}

_Bool bits2bytes(char *buf, uint8_t *val, int n)
{
    int i = 0, j = n / 8, k = 0;
    char ch = 0;

    do {
        val[j] = 0;
        k = (j == n / 8) ? (n % 8) : 8;
        for (i = 0; i < k; ++i) {
            ch = *buf++;
            if (ch == '1')
                val[j] |= 0x01 << (k-i-1);
            else if (ch == '0')
                val[j] |= 0x00 << (k-i-1);
            else
                return false;
        }
    } while(j--);

    return true;
}

void ascii_to_binary(const char* buf, uint8_t* bin, unsigned int digits) {
    unsigned int i, j;
    int value = 0;

    for (i = 0, j = 0; i < (digits + 1) / 2 && j < digits; ++i, j += 2) {
        if (sscanf(buf + j, "%02X", &value) >= 1) {
            bin[i] = value;
        } else {
            bin[i] = 0;
        }
    }
}

char* binary_to_ascii(char* buf, uint8_t* bin, unsigned int len) {
    unsigned int i = 0;
    char *ps = buf;

    for (i = 0; i < len; ++i) {
        ps += sprintf(ps, "%02X", bin[i]);
    }

    return buf;
}

bool IsBigEndian()
{
    union temp
    {
        short int a;
        char b;
    } temp;

    temp.a = 0x1234;
    if (temp.b == 0x12) // 低字节存的是数据的高字节数据
    {
        return true;
    }
    else
    {
        return false;
    }
}

