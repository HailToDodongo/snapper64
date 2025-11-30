/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace RSP
{
  inline void dmaToRDRAM(uint32_t dmem, void *rdram, int size)
  {
    while (*SP_DMA_FULL) {}
    *SP_DMA_SPADDR = dmem;
    *SP_DMA_RAMADDR = (uint32_t)rdram;
    *SP_DMA_WRLEN = size-1;
    while (*SP_DMA_BUSY) {}
  }

  inline void dmaToDMEM(uint32_t dmem, void *rdram, int size)
  {
    while (*SP_DMA_FULL) {}
    *SP_DMA_SPADDR = dmem;
    *SP_DMA_RAMADDR = (uint32_t)rdram;
    *SP_DMA_RDLEN = size-1;
    while (*SP_DMA_BUSY) {}
  }

  inline void dmaMemcpy(void *rdramDst, void *rdramSrc, int size)
  {
    dmaToDMEM(0, rdramSrc, size);
    dmaToRDRAM(0, rdramDst, size);
  }
}