/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../framework/testGroup.h"
#include "../renderer/text.h"
#include "../utils/miMemory.h"

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

  constexpr color_t CVG_PALETTE[8]
  {
    {40, 40, 40,   0xFF},
    {212, 23, 23,  0xFF},
    {222, 112, 22, 0xFF},
    {222, 219, 22, 0xFF},
    {60, 207, 37,  0xFF},
    {70, 219, 197, 0xFF},
    {37, 65, 207,  0xFF},
    {159, 37, 207, 0xFF},
  };
}

namespace Tests::RAM9thBitRDP
{
  TestGroup create()
  {
    TestGroup group{"RAM 9th Bit - RDP->CPU"};

    for(int i=0; i<32; ++i)
    {
      std::string name = "RDP write - CPU read | " + std::to_string(i);
      group.test(name, [i](Assert& assert)
      {
        TestSurface triSurf{FMT_RGBA16, 64, 64};
        TestSurface bitsSurf{FMT_RGBA32, 64, 64};

        bitsSurf.clear();
        triSurf.attachAndClear();

        std::array<RDP::Vertex, 3> vtx{RDP::Vertex
          {.pos = {30,  0}},
          {.pos = {60, 33}},
          {.pos = {0,  50}},
        };
        float offset = i * 0.03f;
        for(auto &v : vtx)v.pos += {1+offset, 1+offset};

        RDP::DPL{16}.add(RDP::setOtherModes(OTHER_MODE))
         .add(RDP::setCC(RDPQ_COMBINER1((0,0,0,1), (0,0,0,1))))
         .add(RDP::triangle(0, vtx))
         .runSync();

        auto bits = MiMem::ebusReadBits((uint32_t*)triSurf.get().buffer, triSurf.getByteSize());
        assert(bits.size() == (size_t)(triSurf.getWidth() * triSurf.getHeight() * 2));

        uint16_t *src = (uint16_t*)triSurf.get().buffer;
        uint32_t *dst = (uint32_t*)bitsSurf.get().buffer;
        uint32_t *dstEnd = dst + (bitsSurf.getWidth() * (bitsSurf.getHeight()-1));
        uint32_t bitIdx = 0;
        for(; dst < dstEnd; ++dst) {
          uint8_t cvg = bits[bitIdx+0] << 2;
          cvg |= bits[bitIdx+1] << 1;
          cvg |= (*src & 1);

          *dst = color_to_packed32(CVG_PALETTE[cvg]);
          ++src;
          bitIdx += 2;
        }

        triSurf.convertToRGBA32(false);

        assert.surface(triSurf);
        assert.surface(bitsSurf);

        triSurf.draw(24, 64);
        bitsSurf.draw(120, 64);

        Text::setColor();
        Text::print(24, 44, "Surface     3 CVG-Bits");
      });
    }

    return group;
  }
}
