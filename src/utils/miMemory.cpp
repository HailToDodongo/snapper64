/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "./miMemory.h"

void MiMem::writeAligned(volatile uint64_t *addr, uint64_t value, int bytes)
{
  do {
    *MI_MODE = MI_WMODE_SET_REPEAT | (bytes > 128 ? 127 : (bytes-1));
    *addr = value;
    bytes -= 128; // we only care about the iteration count, the size is clamped above
    addr += 128 / 8;
  } while(bytes > 0);
}

void MiMem::zeroUnaligned(const volatile char *addr, int bytes) {
  uint32_t misalign = ((uint32_t)addr) & 0b111;
  bytes += misalign;
  uint64_t value = (misalign & 0b110) ? 0xAABBCCDD'FF000000 : 0;
  do {
    asm volatile (".balign 32");
    *MI_MODE = MI_WMODE_SET_REPEAT | (bytes > 128 ? 127 : (bytes-1));
    //asm volatile ("sb $zero, 0(%0)\n" :: "r"(addr) : "memory");
    asm volatile ("sb %0, 0(%1)\n" ::"r"(value), "r"(addr) : "memory");
    bytes -= 128; // we only care about the iteration count, the size is clamped above
    addr += 128;
  } while(bytes > 0);
}

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

namespace
{
  /**
   * Writes a 16bit value to RDRAM, while setting the upper hidden bit to the given state.
   * The lower hidden bit is destroyed.
   * @param dst
   * @param value visible RDRAM value
   * @param hiddenBit desired state (0 or 1)
   */
  void writeHiddenU16(volatile uint16_t *dst, uint16_t value, uint8_t hiddenBit)
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

  constinit sprite_t* bg{nullptr};

  constexpr uint32_t HIDDEN_MASK_COUNT = 110;
  constexpr uint32_t HIDDEN_MASK_COUNT_SAFE = 128;

  __attribute__((aligned(8)))
  constinit uint16_t *hiddenMask{nullptr};
}

