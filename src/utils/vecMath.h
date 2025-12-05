/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Math
{
  constexpr float PI = 3.14159265359f;

  float sinf(float angle);

  static float cosf(float angle) {
    return sinf(angle + (PI / 2.0f));
  }
}
