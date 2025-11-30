/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testSurface.h"

#include "../main.h"
#include "../renderer/text.h"
#include "../utils/rsp.h"

void TestSurface::attachAndClear(color_t clearColor)
{
  //uint64_t a = get_ticks();
  name.clear();
  lastTestSuccess = false;

  uint64_t clearValue = color_to_packed32(clearColor);
  clearValue = (clearValue << 32) | clearValue;

  if((surface.flags & SURFACE_FLAGS_TEXFORMAT) == FMT_RGBA16)
  {
    assertf(false, "@TODO: handle RGBA16 surface");
  }

  RDP::DPL dpl{64};
  dpl.add(RDP::attach(surface))
    .add(RDP::syncFull())
    .runAsync();

  sys_hw_memset64(surface.buffer, clearValue, surface.stride * surface.height);

  dpl.await();
  //a = get_ticks() - a; debugf("t: %lu us\n", TICKS_TO_US(a));
}

void TestSurface::draw(int x, int y)
{
  assert(x % 2 == 0);

  auto dst = (char*)ctx.fb->buffer;
  dst += (ctx.fb->stride * y) + (x * 4);

  auto src = (char*)get().buffer;

  for(int row=0; row < surface.height; ++row) {
    RSP::dmaMemcpy(dst, src, surface.stride);
    src += surface.stride;
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
