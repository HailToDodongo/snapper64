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
    .cycleType(RDP::CYCLE::TWO)
    .ditherRGBA(RDP::DitherRGB::DISABLED)
    .ditherAlpha(RDP::DitherAlpha::DISABLED)
    .setAA(false)
    .forceBlend(false)
    .setImageRead(false)
    .setDepthWrite(false)
  );

  constexpr uint64_t CC_ENV[2] {
    RDPQ_COMBINER2(
      (0,0,0,ENV),      (0,0,0,1),
      (0,0,0,COMBINED), (0,0,0,1)
    ),
    RDPQ_COMBINER2(
      (0,0,0,0),   (0,0,0,1),
      (0,0,0,ENV), (0,0,0,1)
    )
  };

  void testCase(Assert& assert, int cycle, int i)
  {
    TestSurface surf{FMT_RGBA32, 320-32, 180};
    surf.attachAndClear();

    RDP::DPL dplTri{4000};
    dplTri.add(RDP::syncPipe())
      .add(RDP::setOtherModes(OTHER_MODE))
      .add(RDP::setCC(CC_ENV[cycle]));

    float baseSizeY = (i*6)+1;
    float baseSizeX = 1;
    if(i >= 10)
    {
      baseSizeX = ((i-10)*10)+1;
      baseSizeY = 8;
    }

    float pos[2]{0.0f, 2};
    for(int y=0;  y<16; ++y)
    {
      pos[0] = 2;
      for(int x=0;  x<32; ++x)
      {
        dplTri
          .add(RDP::syncPipe())
          .add(RDP::setEnvColor({0xFF, 0xFF, 0xFF, 0xFF}))
          .add(RDP::fillRectSize(
            pos[0], pos[1], x+baseSizeX, y+baseSizeY
          ))
          .add(RDP::setEnvColor({0xFF, 0x00, 0x00, 0xFF}))
          .add(RDP::setEnvColor({0x00, 0xFF, 0x00, 0xFF}))
          .add(RDP::setEnvColor({0x00, 0x00, 0xFF, 0xFF}))
        ;
        pos[0] += x+baseSizeX+1;

        if(pos[0] + x + baseSizeX + 1 > surf.getWidth()) {
          break;
        }
      }
      pos[1] += y+baseSizeY+1;
      if((pos[1]+y+baseSizeY+1) > surf.getHeight()) {
        break;
      }
    }

    dplTri.runSync();

    assert.surface(surf);
    surf.draw(16, 48);
  }
}

namespace Tests::RDPRectNoSync2C
{
  TestGroup create()
  {
    TestGroup group{"RDP Rect No-Sync-Env 2C"};

    for(int cycle=0; cycle<2; ++cycle)
    {
      for(int i=0; i<20; ++i)
      {
        std::string name = "Rect Env " + std::to_string(i) + " | In Cycle-" + std::to_string(cycle+1);
        group.test(name, [=](Assert &assert) {
          testCase(assert, cycle, i);
        });
      }
    }

    return group;
  }
}
