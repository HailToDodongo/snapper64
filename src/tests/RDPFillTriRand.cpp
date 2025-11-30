/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../renderer/dpl.h"
#include "../framework/testGroup.h"
#include "../framework/testSurface.h"
#include "../utils/rng.h"
#include "../utils/vecMath.h"
#include <array>

namespace Tests::RDPFillTriRand
{
  TestGroup create()
  {
    TestGroup group{"RDP Fill Mode Tri (Random)"};

    for(uint32_t seed : {
      0x3809113eu, 0x0a5212bbu, 0xe58b24beu,
      0x7be66493u, 0x9f605ee7u, 0x336a3a0au,
      0xa3a56459u,
    })
    {
      group.test("Random 0x" + toHex(seed), [seed](Assert& assert)
      {
        RNG::setSeed(seed);
        TestSurface surf{FMT_RGBA32, 250, 160};
        surf.attachAndClear();

        RDP::Vertex vtx[3]{};
        for(auto &v : vtx) {
          v.pos = (RNG::randVec2Signed() * surf.getWidth()) + surf.getHalfSize();
        }

       RDP::DPL{16}.add(RDP::setFillColorRaw(RNG::randU32()))
         .add(RDP::triangle(0, vtx[0], vtx[1], vtx[2]))
         .runSync();

        assert.surface(surf);
        assert.equals(1, 1);

        surf.draw(32, 48);
      });
    }

    return group;
  }
}
