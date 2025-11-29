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

namespace Tests::RDPFillTri
{
  TestGroup create()
  {
    TestGroup group{"RDP Fill Mode Tri (Slopes)"};
    constexpr int32_t SLOPE_STEP = 0x333;

    for(int i=0; i<500; ++i)
    {
      group.test("Tri-Rect ISL " + std::to_string(i), [i](Assert& assert)
      {
        TestSurface surf{FMT_RGBA32, 260, 80};
        surf.attachAndClear();

        auto vtx = std::to_array<RDP::Vertex>({{{0,0}}, {{64,0}}, {{0, 64}}});
        for(auto &v : vtx)v.pos += {8,8};

        auto triData = RDP::triangleGen(0, vtx);
        triData.isl = i*SLOPE_STEP;

        RDP::DPL{16}.add(RDP::setFillColor({0xFF, 0xFF, 0xFF}))
          .add(RDP::triangleWrite(triData))
          .runSync();

        assert.surface(surf);
        surf.draw(32, 48);

        vtx = std::to_array<RDP::Vertex>({{{0,0}}, {{64,0}}, {{0, 64}}});
        for(auto &v : vtx)v.pos += {(float)surf.getWidth() - 64 - 8, 8};

        triData = RDP::triangleGen(0, vtx);
        triData.isl = -i*SLOPE_STEP;

        surf.attachAndClear();
        RDP::DPL{16}.add(RDP::setFillColor({0xFF, 0xFF, 0xFF}))
          .add(RDP::triangleWrite(triData))
          .runSync();

        assert.surface(surf);
        surf.draw(32, 48 + surf.getHeight() + 12);
      });
    }

    return group;
  }
}
