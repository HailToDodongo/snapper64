/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace SRAM
{
  constexpr uint32_t ADDRESS = 0x0800'0000;

  inline void load(void* rdram, size_t size)
  {
    data_cache_hit_writeback_invalidate(rdram, size);
    MEMORY_BARRIER();
    dma_read_raw_async(rdram, ADDRESS, size);
    dma_wait();
  }

  inline void save(void* rdram, size_t size)
  {
    data_cache_hit_writeback_invalidate(rdram, size);
    MEMORY_BARRIER();
    dma_write_raw_async(rdram, ADDRESS, size);
    dma_wait();
  }

  inline void clear()
  {
    constexpr size_t CLEAR_SIZE = 1024;
    void* dummy = malloc_uncached(CLEAR_SIZE);
    memset(dummy, 0, CLEAR_SIZE);
    MEMORY_BARRIER();
    dma_write_raw_async(dummy, ADDRESS, CLEAR_SIZE);
    dma_wait();
  }
}