/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace RNG
{
  void setSeed(uint32_t newSeed);
  uint32_t randU32();
  float randF32Signed();

  inline fm_vec2_t randVec2Signed() {
    return { randF32Signed(), randF32Signed() };
  }
}