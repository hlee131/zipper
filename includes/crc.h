#ifndef CRC_H 
#define CRC_H
#include <stdint.h>

uint32_t crc32(uint8_t* data, size_t length); 
uint8_t crc8(uint8_t* data, size_t length);
#endif