/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Draw
{
  union IVec2
  {
    struct { int32_t x, y; };
    int32_t v[2];
  };

  void line(IVec2 start, IVec2 end, color_t color);

  inline void lineF(const fm_vec2_t &start, const fm_vec2_t &end, color_t color) {
    line(IVec2{ (int32_t)start.x, (int32_t)start.y }, IVec2{ (int32_t)end.x, (int32_t)end.y }, color);
  }

  inline void tri(const IVec2 &a, const IVec2 &b, const IVec2 &c, color_t color) {
    line(a, b, color);
    line(b, c, color);
    line(c, a, color);
  }

  inline void quad(const IVec2 &a, const IVec2 &b, const IVec2 &c, const IVec2 &d, color_t color) {
    line(a, b, color);
    line(b, c, color);
    line(c, d, color);
    line(d, a, color);
  }
}