/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "hash.h"

/*uint32_t Hash::crc32(const char* txt)
{
  uint32_t crc = 0xFFFFFFFF;
  while (*txt) {
    crc ^= static_cast<uint8_t>(*txt++);
    for (int k = 0; k < 8; k++) {
      crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
    }
  }
  return ~crc;
}*/
