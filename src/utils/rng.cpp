/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../utils/rng.h"

namespace
{
  constinit uint32_t seed = 0x12345678;
}

void RNG::setSeed(uint32_t newSeed)
{
  seed = newSeed;
}

uint32_t RNG::randU32()
{
  seed ^= (seed << 13);
  seed ^= (seed >> 17);
  seed ^= (seed << 5);
  return seed;
}

float RNG::randF32Signed()
{
  return ((float)(randU32() & 0xFFFF) * (1.0f/32768.0f)) - 1.0f;
}
