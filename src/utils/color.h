/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

#include "vecMath.h"

namespace Color
{
  inline color_t rainbow(float s)
  {
    float r = Math::sinf(s + 0.0f) * 75.0f + 128.0f;
    float g = Math::sinf(s + 2.0f) * 75.0f + 128.0f;
    float b = Math::sinf(s + 4.0f) * 75.0f + 128.0f;
    return {(uint8_t)r, (uint8_t)g, (uint8_t)b, 0xFF};
  }
}
