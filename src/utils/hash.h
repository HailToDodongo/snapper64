/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Hash
{
  #pragma GCC push_options
  #pragma GCC optimize ("-O3")

  constexpr uint32_t crc32(const char* txt)
  {
    uint32_t crc = 0xFFFFFFFF;
    while (*txt) {
      crc ^= static_cast<uint8_t>(*txt++);
      for (int k = 0; k < 8; k++) {
        crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
      }
    }
    return ~crc;
  }

  #pragma GCC pop_options
}
