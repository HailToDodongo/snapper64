/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testSurface.h"

#include "../main.h"
#include "../renderer/text.h"
#include "../utils/rsp.h"
#include "../renderer/draw.h"
#include "../utils/miMemory.h"

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

  MiMem::setU64(surface.buffer, clearValue, surface.stride * surface.height);

  dpl.await();
  //a = get_ticks() - a; debugf("t: %lu us\n", TICKS_TO_US(a));
}

void TestSurface::draw(int x, int y)
{
  assert(x % 2 == 0);

  auto dst = (char*)ctx.fb->buffer;
  dst += (ctx.fb->stride * y) + (x * 4);

  auto src = (char*)get().buffer;

  switch(ctx.diffMode)
  {
    default:
    case DIFF_MODE::ACTUAL_COLOR:
    case DIFF_MODE::DIFF_COLOR:
    {
      for(int row=0; row < surface.height; ++row)
      {
        RSP::dmaMemcpy(dst, src, surface.stride);
        src += surface.stride;
        dst += ctx.fb->stride;
      }
    } break;

    case DIFF_MODE::ACTUAL_CVG:
    case DIFF_MODE::DIFF_CVG:
    {
      for(int row=0; row < surface.height; ++row)
      {
        for(int col=0; col < surface.width; ++col) {
          uint8_t cvg = *((uint32_t*)src + col) & 0xFF;
          *((uint32_t*)dst + col) = color_to_packed32({cvg, cvg, cvg, 0xFF});
        }
        src += surface.stride;
        dst += ctx.fb->stride;
      }
    } break;
  }

  color_t drawCol = lastTestSuccess ? color_t{0x55, 0xEE, 0x55} : color_t{0xEE, 0x55, 0x55};
  if(!ctx.saveData.autoAdvanceTest
    && ctx.diffMode != DIFF_MODE::ACTUAL_COLOR
    && ctx.diffMode != DIFF_MODE::ACTUAL_CVG
  ) {
    drawCol = color_t{0x66, 0x66, 0xFF};
  }

  Draw::quad(
    {x-1, y-1},
    {x + surface.width, y-1},
    {x + surface.width, y + surface.height},
    {x-1, y + surface.height},
    drawCol
  );

  Text::setColor(drawCol);
  Text::printSmall(x & ~0b11, y-7, name.c_str());
  //Text::printSmall(128, 128, "-.0123456789_:");
  Text::setColor();

  if(errorCount)
  {
    Text::printf(x, y + surface.height+1, "Errors: %d", errorCount);
  }
}
