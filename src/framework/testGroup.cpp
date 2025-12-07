/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testGroup.h"

#include "../renderer/text.h"
#include "../renderer/vi.h"
#include "../utils/miMemory.h"

namespace
{
  constexpr std::array<const char*, 6> DIFF_MODES = {
    "Actual (Col)",
    "Diff (Col)",
    "Ref (Col)",
    "Actual (CVG)",
    "Diff (CVG)",
    "Ref (CVG)",
  };
}

bool TestGroup::run()
{
  constexpr bool forceDisplay = true;

  Text::setColor();

  countTested = 0;
  countSuccess = 0;

  if(ctx.autoAdvanceTest)ctx.diffMode = 0;

  for(uint32_t i=0; i<entries.size(); ++i)
  {
    Assert assert{};
    assert.hashGroup = nameHash;
    assert.hashTest = entries[i].nameHash;

    auto &entry = entries[i];

    joypad_poll();
    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);

    if(forceDisplay) {
      MiMem::setU64(ctx.fb->buffer, 0, ctx.fb->height * ctx.fb->stride);
    }

    //auto t = get_ticks();
    entry.func(assert);
    //t = get_ticks() - t;

    entry.errorCount = assert.errorCount;
    //debugf("[Debug] Test %ld us\n", TICKS_TO_US(t));

    if(ctx.autoAdvanceTest) {
      Text::setColor({0x99, 0x99, 0xFF});
      Text::print(16, 16+0, name.c_str());
    }

    Text::setColor();
    if(entries.size() < 100) {
      Text::printf(16, 16+8, "%02d/%02d: %s", i+1, entries.size(), entry.name.c_str());
    } else {
      Text::printf(16, 16+8, "%03d/%03d: %s", i+1, entries.size(), entry.name.c_str());
    }
    //Text::printf(16, 16+16, "UUID: %08X_%08X", nameHash, entry.nameHash);

    if(pressed.start) {
      countTested = 0;
      countSuccess = 0;
      return false;
    }

    bool manualAdvance = pressed.l || pressed.r || held.z;
    if(!ctx.autoAdvanceTest)
    {
      if(pressed.c_left)--ctx.diffMode;
      if(pressed.c_right)++ctx.diffMode;
      if(ctx.diffMode >= DIFF_MODES.size())ctx.diffMode = 0;

      Text::setColor({0,0,0,0});
      Text::setBgColor({0x99, 0xFF, 0xFF, 0xFF});
      Text::print(16, 14, "L/R to continue");

      Text::setAlign(Text::Align::RIGHT);
      Text::printf(SCREEN_WIDTH-16, 14, "C-L/R: %s", DIFF_MODES[ctx.diffMode]);
      Text::setAlign(Text::Align::LEFT);

      Text::setColor();
      Text::setBgColor();

      if(manualAdvance) {
        if(pressed.l)i -= i == 0 ? 1 : 2;
      } else {
        VI::show();
        --i;
        continue;
      }
    }

    if(forceDisplay)VI::show();
  }

  // update counters in group
  countTested = (uint32_t)entries.size();
  countSuccess = 0;
  for(const auto &entry : entries) {
    countSuccess += (entry.errorCount == 0) ? 1 : 0;
  }

  if(forceDisplay)wait_ms(20);

  return true;
}
