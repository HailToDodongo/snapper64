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

    if(forceDisplay) {
      // @TODO: use RSP?
      memset(ctx.fb->buffer, 0, ctx.fb->height * ctx.fb->stride);
    }

    entry.func(assert);

    Text::setColor({0x99, 0x99, 0xFF});
    Text::print(16, 16+0, name.c_str());
    Text::setColor();
    Text::printf(16, 16+8, "%03d/%03d: %s", i, entries.size(), entry.name.c_str());
    //Text::printf(16, 16+16, "UUID: %08X_%08X", nameHash, entry.nameHash);

    if(forceDisplay)VI::show();

    joypad_poll();
    if(joypad_get_buttons_pressed(JOYPAD_PORT_1).start)
    {
      // @TODO: abort logic
      return false;
    }

    if(joypad_get_buttons_held(JOYPAD_PORT_1).z)
    {
      wait_ms(500);
    }
  }

  return true;
}
