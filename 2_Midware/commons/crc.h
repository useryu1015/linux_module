#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>
#include <stdbool.h>
#include "zlog.h"

#define SENSOR_CRC16_CALC(BUF, LEN) (crc16_big_endian(BUF, LEN))
#define MD66_CRC16_CALC(BUF, LEN)   (crc16_lit_endian(BUF, LEN))

#define SENSOR_CRC16_CHECK(PS, PC)  (crc16_check(PS, PC, true))
#define MD66_CRC16_CHECK(PS, PC)    (crc16_check(PS, PC, false))


uint16_t crc16_big_endian(uint8_t *buffer, uint16_t buffer_length);
uint16_t crc16_lit_endian(uint8_t *buffer, uint16_t buffer_length);
_Bool crc16_check(uint8_t *ps, uint8_t *pc, _Bool is_big_endian);


#endif /* _CRC_H_ */