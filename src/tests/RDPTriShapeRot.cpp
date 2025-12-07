/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../renderer/dpl.h"
#include "../framework/testGroup.h"
#include "../framework/testSurface.h"
#include "../renderer/text.h"
#include "../utils/vecMath.h"
#include <array>


namespace
{
  constexpr uint64_t OTHER_MODE_AA_CLAMP = RDP::setOtherModes(RDP::OtherMode()
    .cycleType(RDP::CYCLE::ONE)
    .ditherRGBA(RDP::DitherRGB::DISABLED)
    .ditherAlpha(RDP::DitherAlpha::DISABLED)
    .setAA(true)
    .setImageRead(true)
    .forceBlend(true)
  );

  constexpr uint64_t OTHER_MODE_AA_WRAP = OTHER_MODE_AA_CLAMP
    | RDP::setOtherModes(RDP::OtherMode().setAAMode(1));

  fm_vec2_t circlePos(float angle, float radius, fm_vec2_t center) {
    return center + fm_vec2_t{
      Math::cosf(angle) * radius,
      Math::sinf(angle) * radius
    };
  }
}

namespace Tests::RDPTriShapeRot
{
  TestGroup create()
  {
    TestGroup group{"RDP Tri Shape+CVG - Rotation", 36 * 36 * 3};

    for (auto [vtx, angle, vtxRot] : TestMatrix(3, 36, 36))
    {
      angle *= 10;
      vtxRot *= 10;

      std::string name = "Tri " + std::to_string(angle) + "deg"
        " | V:" + std::to_string(vtx) +
        " | R:" + std::to_string(vtxRot);

      group.test(name, [=](Assert& assert)
      {
        TestSurface surf{FMT_RGBA32, 256, 128};
        surf.attachAndClear({0,0,0,0xFF});

        auto pos = surf.getHalfSize() + fm_vec2_t{-64.0f, 0.0f };

        std::array<RDP::Vertex, 3> triL{RDP::Vertex
          {.pos = circlePos(toRad(angle) +   0.0_deg, 48.0f, pos)},
          {.pos = circlePos(toRad(angle) + 120.0_deg, 48.0f, pos)},
          {.pos = circlePos(toRad(angle) + 240.0_deg, 48.0f, pos)}
        };
        for(int i=0; i<vtx+1; ++i) {
          triL[i].pos += circlePos(toRad(vtxRot*(i+1)), 20.0f, {0,0});
        }

        std::array<RDP::Vertex, 3> triR = triL;
        for(auto &v : triR)v.pos.x += surf.getHalfSize().x;

        RDP::DPL{32}
          .add(RDP::setCC(RDPQ_COMBINER1((0,0,0,PRIM), (0,0,0,1))))
          .add(RDP::setPrimColor({0xFF, 0xAA, 0x33, 0xFF}))
         .add(RDP::setOtherModes(OTHER_MODE_AA_CLAMP))
         .add(RDP::triangle(0, triL))
         .add(RDP::syncPipe())

         .add(RDP::setPrimColor({0xFF, 0xAA, 0x33, 0xFF}))
         .add(RDP::setOtherModes(OTHER_MODE_AA_WRAP))
         .add(RDP::triangle(0, triR))
         .runSync();

        assert.surface(surf);

        surf.draw(32, 48);
        Text::print(100, 48 + surf.getHeight() + 8, "AA-Clamp | AA-Wrap");
      });
    }

    return group;
  }
}
