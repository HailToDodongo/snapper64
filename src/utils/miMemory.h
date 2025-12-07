/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include <libdragon.h>

extern "C" {
  extern void* __mi_memset32(void *ptr, uint32_t value, size_t len);
}

namespace MiMem
{
  extern bool isSupported;

  inline void setU64(void *addr, uint64_t value, size_t len)
  {
    if(isSupported) {
      sys_hw_memset64(addr, value, len);
    } else {
      uint64_t *ptr = static_cast<uint64_t*>(addr);
      uint64_t *ptrEnd = ptr + (len / 8);
      for(; ptr < ptrEnd; ++ptr)*ptr = value;
    }
  }

  inline bool checkSupport()
  {
    auto test = (uint32_t*)malloc_uncached(16);
    __mi_memset32(test, 0x12345678, 16);

    isSupported = test[0] == 0x12345678 && test[1] == 0x12345678
      && test[2] == 0x12345678 && test[3] == 0x12345678;

    free_uncached(test);

    return isSupported;
  }
}