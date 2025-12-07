/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "menu.h"
#include "../renderer/text.h"
#include "../renderer/draw.h"
#include "../utils/color.h"

namespace
{
  constexpr color_t COL_WHITE{0xFF, 0xFF, 0xFF};
  constexpr color_t COL_SELECT{0x66, 0x66, 0xFF};

  struct Option
  {
    const char* name{};
    std::function<void()> action{};
    std::function<std::string()> getValue{};
  };

  std::array<Option, 2> OPTIONS = {{
    {
      "Advance Tests:",
      []{ ctx.autoAdvanceTest = !ctx.autoAdvanceTest; },
      []{ return ctx.autoAdvanceTest ? "Auto" : "L/R to Step"; }
    }, {
      "Dump-Method:",
      []{ ctx.useSdCard = !ctx.useSdCard; },
      []{ return ctx.useSdCard ? "SD-Card" : "Logging"; }
    },
  }};

  uint32_t selOption = 0;
}

void Menu::drawOptions()
{
  auto press = joypad_get_buttons_pressed(JOYPAD_PORT_1);

  int posX = 24;
  int posY = 30;

  if(press.c_up || press.d_up)--selOption;
  if(press.c_down || press.d_down)++selOption;
  if(selOption >= 0xFFFF) selOption = OPTIONS.size() - 1;
  if(selOption >= OPTIONS.size()) selOption = 0;

  bool wantsToggle = press.a || press.c_left || press.c_right || press.d_left || press.d_right;

  Text::setAlign(Text::Align::LEFT);
  Text::setColor();
  Text::setBgColor();

  posY += 16;
  for(uint32_t i=0; i<OPTIONS.size(); ++i)
  {
    const auto &opt = OPTIONS[i];
    Text::setColor(i == selOption ? COL_SELECT : COL_WHITE);
    Text::print(posX, posY, opt.name);
    Text::setAlign(Text::Align::RIGHT);
    Text::print(SCREEN_WIDTH - 24, posY, opt.getValue().c_str());
    Text::setAlign(Text::Align::LEFT);
    posY += 10;

    if(wantsToggle && i == selOption) {
      opt.action();
    }
  }

  Text::setColor(COL_SELECT);
  Text::print(posX - 10, selOption * 10 + 45, ">");
  Text::setColor();

  posY = 120;
  Text::setColor();
  Text::print(posX, posY, "System-Info"); posY += 10;
  Draw::line({posX, posY}, {SCREEN_WIDTH - 24, posY}, COL_WHITE); posY += 4;

  heap_stats_t h{};
  sys_get_heap_stats(&h);
  Text::printf(posX, posY, "Used-Heap : %.2fkb", (double)h.used / 1024); posY += 10;

  uint32_t miVer = *MI_VERSION;
  Text::printf(posX, posY, "MI_VERSION: %08X", miVer); posY += 10;

  uint32_t cprRegPRId{};
  uint32_t cprRegConfig{};
  asm volatile("mfc0 %0, $15":"=r"(cprRegPRId));
  asm volatile("mfc0 %0, $16":"=r"(cprRegConfig));

  Text::printf(posX, posY, "COP0-PRId : %08X", cprRegPRId); posY += 10;
  Text::printf(posX, posY, "COP0-Conf : %08X", cprRegConfig); posY += 10;


}