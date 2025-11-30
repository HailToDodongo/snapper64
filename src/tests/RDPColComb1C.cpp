/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../renderer/dpl.h"
#include "../framework/testGroup.h"
#include "../framework/testSurface.h"
#include "../utils/vecMath.h"
#include "../renderer/vi.h"

namespace
{
  constexpr uint64_t OTHER_MODE = RDP::setOtherModes(RDP::OtherMode()
    .cycleType(RDP::CYCLE::ONE)
    .ditherRGBA(RDP::DitherRGB::DISABLED)
    .ditherAlpha(RDP::DitherAlpha::DISABLED)
    .setAA(false)
    .forceBlend(false)
    .setImageRead(false)
    .setDepthWrite(false)
  );

  constexpr auto TEST_CC = std::to_array<uint64_t>({
    // basic combinations for calculations
    RDPQ_COMBINER1((   0,    0,         0 , PRIM), (0,0,0,1)),
    RDPQ_COMBINER1((PRIM,    0, PRIM_ALPHA,    0), (0,0,0,1)),
    RDPQ_COMBINER1((PRIM,    0, PRIM_ALPHA, PRIM), (0,0,0,1)),
    RDPQ_COMBINER1((   1,    0,       PRIM, PRIM), (0,0,0,1)),
    RDPQ_COMBINER1((   1, PRIM, PRIM_ALPHA,    0), (0,0,0,1)),
    RDPQ_COMBINER1((   1, PRIM, PRIM_ALPHA, PRIM), (0,0,0,1)),
    RDPQ_COMBINER1((   0, PRIM, PRIM_ALPHA,    0), (0,0,0,1)),
    RDPQ_COMBINER1((   0, PRIM, PRIM_ALPHA, PRIM), (0,0,0,1)),

    // force overflow (ENV=0xFF)
    RDPQ_COMBINER1((PRIM,    0, PRIM_ALPHA,  ENV), (0,0,0,1)),
    RDPQ_COMBINER1((   1,    0,       PRIM,  ENV), (0,0,0,1)),
    RDPQ_COMBINER1((   1, PRIM, PRIM_ALPHA,  ENV), (0,0,0,1)),
  });
}

namespace Tests::RDPColComb1C
{
  TestGroup create()
  {
    TestGroup group{"RDP Color-Combiner 1C"};

    for(auto cc : TEST_CC)
    {
      group.test("CC: " + toHex(cc), [cc](Assert& assert)
      {
        TestSurface surf{FMT_RGBA32, 264, 264};
        surf.attachAndClear();

        RDP::DPL dpl{8'000};
        dpl.add(RDP::syncPipe())
          .add(RDP::setOtherModes(OTHER_MODE))
          .add(RDP::setCC(cc))
          .add(RDP::setEnvColor({0xFF, 0xFF, 0xFF, 0xFF}))
          .runSync();

        float rectSize = 1.0f;
        float pos[2]{0.0f, 2.0f};
        for(int y=0;  y<256; ++y)
        {
          dpl.reset();

          pos[0] = 2.0f;
          for(int x=0;  x<256; ++x)
          {
            color_t col{0,0,0,0};
            switch(y % 4) {
              case 0: col.r = (uint8_t)(x); break;
              case 1: col.g = (uint8_t)(x); break;
              case 2: col.b = (uint8_t)(x); break;
              case 3: col.r = col.g = col.b = (uint8_t)(x); break;
            }
            col.a = y;

            dpl.add(RDP::setPrimColor(col))
              .add(RDP::fillRectSize(pos[0], pos[1], rectSize, rectSize));

            pos[0] += rectSize;
          }

          dpl.runSync();
          VI::keepAlive(); // may take a while until the entire test is done

          pos[1] += rectSize;
        }

        assert.surface(surf);
        surf.draw(16, 48);
      });
    }

    return group;
  }
}
