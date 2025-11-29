/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Hash
{
  constexpr uint32_t crc32(const char* txt)
  {
    int i = 0;
    uint32_t crc = 0xFFFFFFFF;
    while(txt[i] != 0) {
      uint32_t byte = txt[i];
      crc = crc ^ byte;
      for(int j = 7; j >= 0; j--) {
        uint32_t mask = -(crc & 1);
        crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      ++i;
    }
    return ~crc;
  }
}
