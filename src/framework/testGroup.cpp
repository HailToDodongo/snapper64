/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testGroup.h"

#include "../renderer/text.h"
#include "../renderer/rdp.h"
#include "../renderer/dpl.h"
#include "../renderer/vi.h"

namespace
{
}

bool TestGroup::run()
{
  bool forceDisplay = true;

  Text::setColor();

  for(uint32_t i=0; i<entries.size(); ++i)
  {
    Assert assert{};
    assert.hashGroup = nameHash;
    assert.hashTest = entries[i].nameHash;

    auto &entry = entries[i];

    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
    bool oldDump = ctx.dumpData;
    if(held.b)ctx.dumpData = true;

    if(forceDisplay) {
      // @TODO: use RSP?
      memset(ctx.fb->buffer, 0, ctx.fb->height * ctx.fb->stride);
    }

    auto t = get_ticks();
    entry.func(assert);
    t = get_ticks() - t;
    //debugf("[Debug] Test %ld us\n", TICKS_TO_US(t));

    ctx.dumpData = oldDump;

    Text::setColor({0x99, 0x99, 0xFF});
    Text::print(16, 16+0, name.c_str());
    Text::setColor();
    if(entries.size() < 100) {
      Text::printf(16, 16+8, "%02d/%02d: %s", i+1, entries.size(), entry.name.c_str());
    } else {
      Text::printf(16, 16+8, "%03d/%03d: %s", i+1, entries.size(), entry.name.c_str());
    }
    //Text::printf(16, 16+16, "UUID: %08X_%08X", nameHash, entry.nameHash);

    if(forceDisplay)VI::show();

    joypad_poll();
    if(joypad_get_buttons_pressed(JOYPAD_PORT_1).start)
    {
      // @TODO: abort logic
      return false;
    }

    if(held.z) {
      wait_ms(500);
    }
  }

  if(forceDisplay)wait_ms(20);

  return true;
}
