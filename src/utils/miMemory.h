/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include <libdragon.h>
#include <vector>

extern "C" {
  extern void* __mi_memset32(void *ptr, uint32_t value, size_t len);
}

namespace MiMem
{
  extern bool isSupported;

  /**
   * Writes a 16bit value to RDRAM, while setting the upper hidden bit to the given state.
   * The lower hidden bit is destroyed.
   * @param dst
   * @param value visible RDRAM value
   * @param hiddenBit desired state (0 or 1)
   */
  void writeHiddenU16(volatile uint16_t *dst, uint16_t value, uint8_t hiddenBit);

  std::vector<uint32_t> ebusReadBytes(uint32_t* src, size_t numBytes);
  std::vector<uint8_t> ebusReadBits(uint32_t* src, size_t numBytes);

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