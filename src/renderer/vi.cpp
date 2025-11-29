/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "vi.h"
#include "../main.h"

namespace
{
  constinit std::array<surface_t, 3> fbs{};
  constinit uint32_t frame{0};

  volatile int freeFB = 3;
  volatile int viFrameIdx = 0;
  void onViInterrupt()
  {
    disable_interrupts();
    if(freeFB < 3)freeFB += 1;
    viFrameIdx += 1;
    enable_interrupts();
  }
}

void VI::init()
{
  vi_init();
  vi_set_dedither(false);
  //vi_set_aa_mode(VI_AA_MODE_RESAMPLE);
  vi_set_aa_mode(VI_AA_MODE_NONE);
  vi_set_interlaced(false);
  vi_set_divot(false);
  vi_set_gamma(VI_GAMMA_DISABLE);
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

  vi_show(ctx.fb);

  ++frame;
  ctx.fb = &fbs[frame % 3];
}

void VI::setFrameBuffers(std::array<surface_t, 3> fb)
{
  fbs = fb;
}
