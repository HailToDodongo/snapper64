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
}

namespace Tests::RDPRectTri
{
  TestGroup create()
  {
    TestGroup group{"RDP Rect-Tri - Slopes", 250};
    constexpr int32_t SLOPE_STEP = 0x333;

    for(int i=0; i<250; ++i)
    {
      group.test("Tri-Rect ISL " + std::to_string(i), [i](Assert& assert)
      {
        TestSurface surf{FMT_RGBA32, 260, 80};
        surf.attachAndClear();

        auto vtx = std::to_array<RDP::Vertex>({{{0,0}}, {{64,0}}, {{0, 64}}});
        for(auto &v : vtx)v.pos += {8,8};

        auto triData = RDP::triangleGen(0, vtx);
        triData.isl = i*SLOPE_STEP;

        RDP::DPL{16}.add(RDP::setOtherModes(OTHER_MODE))
          .add(RDP::setCC(RDPQ_COMBINER1((0,0,0,PRIM), (0,0,0,1))))
          .add(RDP::setPrimColor({0xFF, 0xFF, 0xFF, 0xFF}))
          .add(RDP::triangleWrite(triData))
          .runSync();

        assert.surface(surf);
        surf.draw(32, 48);

        triData.isl = i*SLOPE_STEP*1000;

        surf.clear();
        RDP::DPL{16}.add(RDP::triangleWrite(triData)).runSync();

        assert.surface(surf);
        surf.draw(32, 48 + surf.getHeight() + 12);
      });
    }

    return group;
  }
}
