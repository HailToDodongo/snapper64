/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../renderer/dpl.h"
#include "../framework/testGroup.h"
#include "../framework/testSurface.h"
#include <array>

namespace
{
  constexpr int SWEEP_COUNT = 32;
  constexpr fm_vec2_t TEST_SIZE{128, 128};
}

namespace Tests::RDPFillTriSweep
{
  TestGroup create()
  {
    TestGroup group{"RDP Fill Mode Tri (Sweep)", SWEEP_COUNT * SWEEP_COUNT * 2};

    for (const auto [side, sweepY, sweepX] : TestMatrix(2, SWEEP_COUNT, SWEEP_COUNT))
    {
      float sX = ((float)sweepX / (float)SWEEP_COUNT) * 2;
      float sY = ((float)sweepY / (float)SWEEP_COUNT) * 2;

      std::string name = side == 0 ? "Sweep 0 | " : "Sweep 1 | ";
      name += toString(sX, 2) + " | " + toString(sY, 2);

      group.test(name, [side, sX, sY](Assert& assert)
      {
        auto size = TEST_SIZE - fm_vec2_t{2,2};
        TestSurface surf{FMT_RGBA32, (uint16_t)TEST_SIZE.x, (uint16_t)TEST_SIZE.y};
        surf.attachAndClear();

        std::array<RDP::Vertex, 3> vtx{};
        if(side == 0) {
          vtx = {RDP::Vertex
            {.pos = {size.x-0.25f, 1}},
            {.pos = {size.x-0.25f, 1}},
            {.pos = {size.x-0.25f, 1}},
          };
          vtx[2].pos.x -= size.x * sX + 4;
          vtx[0].pos.y += size.y * sY + 4;
        } else {
          vtx = {RDP::Vertex
            {.pos = {1, -size.y}},
            {.pos = {1, size.y}},
            {.pos = {1, size.y}},
          };
          vtx[2].pos.x += size.x * sX + 4;
          vtx[0].pos.y += size.y * sY + 4;
        }

        //vtx[2].pos.x += joypad_get_inputs(JOYPAD_PORT_1).stick_x*5.341f;
        //vtx[0].pos.y -= joypad_get_inputs(JOYPAD_PORT_1).stick_y*5.341f;

       RDP::DPL{16}.add(RDP::setFillColor({0xFF, 0xFF, 0xFF, 0xFF}))
         .add(RDP::triangle(0, vtx[0], vtx[1], vtx[2]))
         .runSync();

        assert.surface(surf);

        surf.draw(32, 48);
      });
    }

    return group;
  }
}
