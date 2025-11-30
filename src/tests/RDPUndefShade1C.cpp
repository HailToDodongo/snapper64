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
#include "../utils/rng.h"

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

namespace Tests::RDPUndefShade1C
{
  TestGroup create()
  {
    TestGroup group{"RDP Undefined Shade 1C"};

    for(int i=0; i<64; ++i)
    {
      group.test("Random " + std::to_string(i), [i](Assert& assert)
      {
        RNG::setSeed(i * 0x123);
        TestSurface surf{FMT_RGBA32, 128, 128};
        surf.attachAndClear();

        std::array<RDP::Vertex, 3> vtx{};
        for(auto &v : vtx) {
          v.pos = RNG::randVec2Signed() * (surf.getHalfSize()*1.5f) + surf.getHalfSize();
        }

        RDP::DPL{16}.add(RDP::setOtherModes(OTHER_MODE))
          .add(RDP::setCC(RDPQ_COMBINER1((0,0,0,SHADE), (0,0,0,1))))
          .add(RDP::triangle(0, vtx))
          .runSync();

        assert.surface(surf);
        surf.draw(32, 48);
      });
    }

    return group;
  }
}
