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
  void ensureRDPNotCrashed()
  {
    RDP::DPL dplTest{8};
    dplTest.add(RDP::syncPipe())
      .add(RDP::syncFull())
      .runAsyncUnsafe();

    bool isCrashed = false;
    uint64_t ticksWaitEnd = get_ticks() + TICKS_FROM_MS(250);
    while(*DP_STATUS & DP_STATUS_PIPE_BUSY)
    {
      if(get_ticks() > ticksWaitEnd) {
        isCrashed = true;
        break;
      }
    }

    if(isCrashed)
    {
      int posY = 64;
      Text::setColor({0xFF, 0x22, 0x22});
      Text::printf(64, posY, "!!! RDP HAS CRASHED !!!"); posY += 8;
      Text::setColor();
      Text::printf(64, posY, "Please Power Cycle"); posY += 16;

      Text::printf(64, posY, "DP_CLCK: %08X", *DP_CLOCK); posY += 8;
      Text::printf(64, posY, "DP_BUSY: %08X", *DP_BUSY); posY += 8;
      Text::printf(64, posY, "DP_CURR: %08X", *DP_CURRENT); posY += 8;
      Text::printf(64, posY, "DP_END : %08X", *DP_END); posY += 8;

      for(;;) { wait_ms(1); }
    }
  }
}

bool TestGroup::run()
{
  bool forceDisplay = true;
  ensureRDPNotCrashed();

  Text::setColor();

  Assert assert{};
  for(uint32_t i=0; i<entries.size(); ++i)
  {
    auto &entry = entries[i];

    if(forceDisplay) {
      memset(ctx.fb->buffer, 0x22, ctx.fb->height * ctx.fb->stride);
    }

    entry.func(assert);

    Text::print(16, 16+0, name.c_str());
    Text::printf(16, 16+8, "%03d/%03d: %s", i, entries.size(), entry.name.c_str());

    if(forceDisplay)VI::show();

    joypad_poll();
    if(joypad_get_buttons_pressed(JOYPAD_PORT_1).start)
    {
      // @TODO: abort logic
      return false;
    }
  }

  return true;
}
