/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "./miMemory.h"

constinit bool MiMem::isSupported = false;

#define ICACHE_LINESIZE 0x20

extern "C" {
  /**
  *  Copy `num_words` from hidden RDRAM bits associated with address `src` into `dst`.
  *  Original source from: Thar0
  */
  __attribute__((noinline))
  __attribute__((aligned(ICACHE_LINESIZE)))
  void ebusReadBytes(uint8_t* dst, uint32_t* src, size_t numBytes)
  {
    assert(numBytes % 4 == 0);

    // Pre-fill the cache as ebus test mode changes all RDRAM accesses, cache fetches
    // lock up the system while it is enabled.
    for (char* cur = (char*)(void*)ebusReadBytes; cur < &&__func_end; cur += ICACHE_LINESIZE)
    {
      __asm__ ("cache 0x14, (%0)" : : "r"(cur));
    }

    size_t  numWords = numBytes / 4;
    for (size_t i = 0; i < numWords; i++)
    {
      uint32_t ebus_read;

      // extract hidden bits for this word, appearing as the 4 low-order bits
      *MI_MODE = MI_WMODE_SET_EBUS;
      MEMORY_BARRIER();
      ebus_read = src[i];
      MEMORY_BARRIER();
      *MI_MODE = MI_WMODE_CLR_EBUS;
      MEMORY_BARRIER();

      // intentionally preserve the rest of the bits,
      // this should be zero on hardware
      dst[0] = (ebus_read >> 3) & 0b11111111'11110001;
      dst[1] = (ebus_read >> 2) & 0b11111111'11110001;
      dst[2] = (ebus_read >> 1) & 0b11111111'11110001;
      dst[3] = (ebus_read >> 0) & 0b11111111'11110001;
      dst += 4;
    }
    __func_end:;
  }
}

void MiMem::writeHiddenU16(volatile uint16_t *dst, uint16_t value, uint8_t hiddenBit)
{
  MEMORY_BARRIER();
  if(hiddenBit) {
    *dst = (value | 1); // LSB=1 forces both hidden-bits to be one
  } else {
    *dst = value & 0xFF00;  // LSB=0 forces both hidden-bits to be zero
  }
  MEMORY_BARRIER();
  // set correct low byte, destroys low hidden-bit, preserves upper
  ((volatile uint8_t*)dst)[1] = (uint8_t)value;
}


