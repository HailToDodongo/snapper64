/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../framework/testGroup.h"
#include "../utils/rng.h"
#include "../renderer/text.h"

namespace
{
  constexpr uint64_t OTHER_MODE = RDP::OtherMode()
    .cycleType(RDP::CYCLE::ONE)
    .ditherRGBA(RDP::DitherRGB::DISABLED)
    .ditherAlpha(RDP::DitherAlpha::DISABLED)
    .setAA(false)
    .forceBlend(false)
    .setImageRead(false)
    .setDepthWrite(false);
}

namespace Tests::RDPTestModeSpan
{
  TestGroup create()
  {
    TestGroup group{"RDP Test-Mode - Span Tri"};

    for(auto [type, i] : TestMatrix(4, 54))
    {
      std::string name = "Tri " + std::to_string(type) + " | " + std::to_string(i);
      group.test(name, [type, i](Assert& assert)
      {
        RDP::TestMode::enable();
        for(uint32_t s=0; s<RDP::TestMode::SPAN_WORDS; ++s) {
          // also indirectly checks if emulators actually overwrite the span buffer
          // when the RDP draws something
          RDP::TestMode::spanWrite(s, 0x55555555);
        }
        RDP::TestMode::disable();

        TestSurface spanSurf{FMT_RGBA32, 4, 128/4};
        TestSurface triSurf{FMT_RGBA32, 76, 64};

        spanSurf.clear();
        triSurf.attachAndClear();

        std::array<RDP::Vertex, 3> vtx{RDP::Vertex
          {.pos = {30,  0}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
          {.pos = {60, 33}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
          {.pos = {0,  50}, .color = {0.0f, 0.0f, 1.0f, 1.0f}},
        };

        // lower triangle into bottom scissor to see different span buffer sections
        float dir = type >= 2 ? -1.0f : 1.0f;
        if(type % 2 == 0) {
          for(auto &v : vtx)v.pos += {8, 4 + i * dir};
        } else {
          for(auto &v : vtx)v.pos += {4 + i * dir, 8};
        }

        // draw...
        RDP::DPL{16}.add(RDP::setOtherModes(OTHER_MODE))
         .add(RDP::setCC(RDPQ_COMBINER1((0,0,0,SHADE), (0,0,0,1))))
         .add(RDP::triangle(RDP::TriAttr::SHADE, vtx))
         .runSync();

        // ...and then extract all span data
        RDP::TestMode::enable();
        uint32_t w=0;
        for(uint32_t y=0; w<RDP::TestMode::SPAN_WORDS/4; ++y) {
          uint32_t valA = RDP::TestMode::spanRead(w+0);
          uint32_t valB = RDP::TestMode::spanRead(w+1);
          uint32_t valCvg = RDP::TestMode::spanRead(w+2);
          w += 4;

          valCvg |= valCvg << 8;
          valCvg |= valCvg << 16;

          spanSurf.setPixel(0, y, valA);
          spanSurf.setPixel(1, y, valB);
          spanSurf.setPixel(2, y, valCvg);
        }
        RDP::TestMode::disable();

        assert.surface(triSurf);
        assert.surface(spanSurf);

        triSurf.draw(32, 64);
        spanSurf.draw(148, 64);

        Text::setColor();
        Text::print(32, 44, "Surface");
        Text::print(148, 44, "Span Buffer");
      });
    }

    return group;
  }
}
