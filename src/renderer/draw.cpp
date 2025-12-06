/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "draw.h"
#include "../main.h"

namespace
{
}

void Draw::line(IVec2 start, IVec2 end, color_t color)
{
  auto buff = (uint32_t*)ctx.fb->buffer;
  uint32_t colorU32 = color_to_packed32(color);

  // horizontal
  if(start.y == end.y)
  {
    if(start.x > end.x) {
      std::swap(start, end);
    }

    buff += start.y * (ctx.fb->stride / 4) + start.x;
    for(int x=start.x; x < end.x; ++x) {
      *buff = colorU32; ++buff;
    }
    return;
  }

  // arbitrary
  float dx = end.x - start.x;
  float dy = end.y - start.y;
  float steps = fabsf(dx) > fabsf(dy) ? fabsf(dx) : fabsf(dy);
  float xInc = dx / steps;
  float yInc = dy / steps;
  float x = start.x;
  float y = start.y;
  for(int i=0; i <= (int)steps; ++i)
  {
    int xi = (int)(x + 0.5f);
    int yi = (int)(y + 0.5f);
    buff[yi * (ctx.fb->stride / 4) + xi] = colorU32;
    x += xInc;
    y += yInc;
  }
}