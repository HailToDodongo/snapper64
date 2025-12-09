/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../framework/testGroup.h"
#include "../utils/rng.h"
#include "../utils/miMemory.h"

namespace
{
  struct TestVal
  {
    uint16_t val{};
    uint16_t bit{};
  };
}

namespace Tests::RAM9thBitCPU
{
  TestGroup create()
  {
    TestGroup group{"RAM 9th Bit - CPU->CPU"};
    for(int test=0; test<4; ++test)
    {
      group.test("Bits - U16-Set - rand " + std::to_string(test), [test](Assert& assert)
      {
        constexpr uint32_t U16_COUNT = 36;
        static_assert(U16_COUNT % 4 == 0);

        RNG::setSeed(0x9876 * test + 0x1234);

        std::array<TestVal, U16_COUNT> values{};
        for(auto &val : values) {
          val = {
            .val = (uint16_t)(RNG::randU32()),
            .bit = (uint16_t)(RNG::randU32() & 1),
          };
        }

        auto data = (volatile uint16_t*)malloc_uncached(U16_COUNT*2);
        for(uint32_t i=0; i<U16_COUNT; ++i) {
          data[i] = 0; // clear hidden bits
        }

        for(uint32_t i=0; i<U16_COUNT; ++i) {
          MiMem::writeHiddenU16(&data[i], values[i].val, values[i].bit);
        }

        auto hiddenBits = MiMem::ebusReadBytes((uint32_t*)data, U16_COUNT*2);

        Draw::IVec2 pos{16,36};
        uint32_t u16Idx = 0;
        for(uint32_t i=0; i<hiddenBits.size(); ++i)
        {
          auto val0 = values[u16Idx+0];
          auto val1 = values[u16Idx+1];
          uint32_t bits = hiddenBits[i];

          // The special u16 write earlier can arbitrarily set the upper hidden of the u16 value.
          // While the lower one is destroyed, or rather set to whatever the LSB of the value written is.
          // So our expected value is a mix of the hidden bits we set independently of the value,
          // and the hidden bit directly derived from the LSB of the value.
          uint32_t expected = 0;
          expected |= val0.bit << 3;
          expected |= (val0.val & 1) << 2;
          expected |= val1.bit << 1;
          expected |= (val1.val & 1) << 0;

          std::string name = toHex(val0.val) + (val0.bit ? "_1 " : "_0 ");
          name += toHex(val1.val) + (val1.bit ? "_1" : "_0");

          assert.equals(bits, expected, name, pos);
          pos.y += 10;
          u16Idx += 2;
        }

        free_uncached((void*)data);
      });
    }
    return group;
  }
}
