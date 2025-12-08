/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../main.h"
#include "../renderer/rdp.h"
#include "../framework/testGroup.h"
#include "../utils/rng.h"

namespace
{
  constexpr uint32_t SPAN_WORD_STEP = 128;
  constexpr int SPAN_WORD_PAGES = 8;
  constexpr uint32_t SPAN_WORD_COUNT = SPAN_WORD_PAGES * SPAN_WORD_STEP;

  constexpr uint32_t maskValue(int i, uint32_t val)
  {
    i = i % 4;
    if(i == 2)return val & 0xFF;
    if(i == 3)return 0;
    return val;
  }
}

namespace Tests::RDPTestModeRW
{
  TestGroup create()
  {
    TestGroup group{"RDP Test-Mode - Span R/W"};
    group.setHighRes(true);

    for(auto [valType, page] : TestMatrix(4, SPAN_WORD_PAGES))
    {
      uint32_t regStart = page * SPAN_WORD_STEP;
      uint32_t regEnd = regStart + SPAN_WORD_STEP;

      std::string name = "R/W Span " + std::to_string(regStart) + "-" + std::to_string(regEnd-1);
      name += valType == 0 ? " (FF)" : (" (Rand " + std::to_string(valType) + ")");

      group.test(name, [regStart, regEnd, valType](Assert& assert)
      {
        std::array<uint32_t, SPAN_WORD_COUNT> testData{};

        if(valType == 0)
        {
          testData.fill(0xFFFFFFFF);
        } else {
          RNG::setSeed(valType * 0x1234);
          for(uint32_t i=0; i<testData.size(); ++i) {
            testData[i] = RNG::randU32();
          }
        }

        RDP::TestMode::enable();
        RDP::TestMode::spanClear();

        // to test splits properly, always set the entire spanbuffers
        for(uint32_t i=0; i<SPAN_WORD_COUNT; ++i) {
          RDP::TestMode::spanWrite(i, testData[i]);
        }

        Draw::IVec2 pos{16,36};
        for(uint16_t i=regStart; i<regEnd; ++i)
        {
          uint32_t val = RDP::TestMode::spanRead(i);

          // RDP registers wrap around after 128 regs, check the last written values instead
          uint32_t actualIdx = testData.size() - RDP::TestMode::SPAN_WORDS;
          actualIdx += i % RDP::TestMode::SPAN_WORDS;

          assert.equals(val, maskValue(i, testData[actualIdx]), toHex(i), pos);

          pos.y += 10;
          if(pos.y > 460) {
            pos.x += 190; pos.y = 36;
          }
        }

        RDP::TestMode::disable();
      });
    }

    return group;
  }
}
