/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "vi.h"

#include "text.h"
#include "../main.h"

namespace
{
  constinit std::array<surface_t, 3> fbs{};
  constinit uint32_t frame{0};

  volatile int64_t lastAliveTime{};

  volatile int freeFB = 3;
  volatile int viFrameIdx = 0;

  [[noreturn]]
  void rdpCrashScreen(uint64_t ticksDiff)
  {
    //vi_wait_vblank();
    for(;;)
    {
      int posY = 64;
      Text::setColor({0xFF, 0x22, 0x22});
      Text::printf(64, posY, "!!! RDP HAS CRASHED !!!"); posY += 8;
      Text::setColor();
      Text::printf(64, posY, "Please Power Cycle"); posY += 8;
      posY += 8;
      Text::printf(64, posY, "DP_CLCK: %08X", *DP_CLOCK); posY += 8;
      Text::printf(64, posY, "DP_BUSY: %08X", *DP_BUSY); posY += 8;
      Text::printf(64, posY, "DP_CURR: %08X", *DP_CURRENT); posY += 8;
      Text::printf(64, posY, "DP_END : %08X", *DP_END); posY += 8;
      posY += 8;
      Text::printf(64, posY, "Last keep-alive: %ld us", TICKS_TO_US(ticksDiff)); posY += 8;

      //vi_show(ctx.fb);
      *VI_ORIGIN = (uint32_t)(ctx.fb->buffer);
      wait_ms(10);
    }
  }

  void onViInterrupt()
  {
    disable_interrupts();
    if(freeFB < 3)freeFB += 1;
    viFrameIdx += 1;

    int64_t ticksDiff = (int64_t)C0_COUNT() - lastAliveTime;
    if(ticksDiff > TICKS_FROM_MS(3'000)) {
      rdpCrashScreen(ticksDiff);
    }

    enable_interrupts();
  }
}

void VI::init()
{
  vi_init();
  vi_write_begin();
    vi_set_dedither(false);
    vi_set_aa_mode(VI_AA_MODE_NONE);
    vi_set_interlaced(false);
    vi_set_divot(false);
    vi_set_gamma(VI_GAMMA_DISABLE);
  vi_write_end();
  wait_ms(14);

  disable_interrupts();
    register_VI_handler(onViInterrupt);
    set_VI_interrupt(1, VI_V_CURRENT_VBLANK);
  enable_interrupts();
}

void VI::show()
{
  while(freeFB == 0) {
    vi_wait_vblank();
  }

  disable_interrupts();
  freeFB -= 1;
  enable_interrupts();

  keepAlive();

  vi_show(ctx.fb);

  ++frame;
  ctx.fb = &fbs[frame % 3];
}

void VI::keepAlive()
{
  disable_interrupts();
  lastAliveTime = C0_COUNT();
  enable_interrupts();
}

void VI::setFrameBuffers(std::array<surface_t, 3> fb)
{
  fbs = fb;
}
