/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testSurface.h"

#include "../main.h"
#include "../renderer/text.h"

void TestSurface::draw(int x, int y)
{
  char* dst = (char*)ctx.fb->buffer;
  dst += ctx.fb->stride * y;
  dst += x * 4;
  for(int row=0; row < surface.height; ++row) {
    memcpy(dst, (char*)surface.buffer + row * surface.stride, surface.width * 4);
    dst += ctx.fb->stride;
  }

  uint32_t borderCol = lastTestSuccess ? 0x55EE55'00 : 0xEE5555'00;

  // draw white border around image
  for(int row=-1; row <= surface.height; ++row) {
    if(row == -1 || row == surface.height) {
      // top/bottom border
      uint32_t *borderPtr = (uint32_t*)ctx.fb->buffer;
      borderPtr += (y + row) * (ctx.fb->stride / 4);
      borderPtr += x - 1;
      for(int col=0; col < surface.width + 2; ++col) {
        *borderPtr++ = borderCol;
      }
    } else {
      // left/right border
      uint32_t *leftPtr = (uint32_t*)ctx.fb->buffer;
      leftPtr += (y + row) * (ctx.fb->stride / 4);
      leftPtr += x - 1;
      *leftPtr = borderCol;

      uint32_t *rightPtr = (uint32_t*)ctx.fb->buffer;
      rightPtr += (y + row) * (ctx.fb->stride / 4);
      rightPtr += x + surface.width;
      *rightPtr = borderCol;
    }
  }

  Text::setColor(color_from_packed32(borderCol));
  Text::printSmall(x & ~0b11, y-7, name.c_str());
  //Text::printSmall(128, 128, "-.0123456789_:");
  Text::setColor();
}
