/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <numeric>
#include <vector>

#include "../main.h"
#include "menu.h"
#include "../framework/testGroup.h"
#include "../renderer/draw.h"
#include "../renderer/text.h"
#include "../utils/miMemory.h"
#include "../utils/vecMath.h"
#include "../utils/sram.h"

namespace
{
  constexpr int posStartY = 30;
  constexpr int VISIBLE_TESTS = 10;

  constexpr color_t COL_WHITE{0xFF, 0xFF, 0xFF};
  constexpr color_t COL_SELECT{0x66, 0x66, 0xFF};
  constexpr color_t COL_INACTIVE{0x77, 0x77, 0x77};
  constexpr color_t COL_SCROLL{0xA7, 0xA7, 0xFF};

  constinit int tab = 1;
  constinit int scrollOffset = 0;

  constexpr int TAB_FAILED = 0;
  constexpr int TAB_ALL    = 1;
  constexpr int TAB_OPTIONS= 2;
  constexpr std::array<const char*, 3> TAB_NAMES{"Failed", "All", "Options"};

  void drawTabs()
  {
    auto center = fm_vec2_t{SCREEN_WIDTH / 2.0f, posStartY-14};
    float tabWidth = 48.0f;
    float startX = center.x - ((TAB_NAMES.size() * tabWidth) / 2.0f);

    Text::setColor(COL_INACTIVE);
    Text::print(70, center.y-1, "<");
    Text::print(SCREEN_WIDTH-70, center.y-1, ">");

    Text::printSmall(70+8, center.y+2, "L");
    Text::printSmall(SCREEN_WIDTH-70-4, center.y+2, "R");

    Text::setAlign(Text::Align::CENTER);
    for(int i = 0; i < (int)TAB_NAMES.size(); ++i)
    {
      float posX = startX + (i * tabWidth) + (tabWidth / 2.0f);
      if(i == tab)
      {
        Text::setColor({0,0,0});
        Text::setBgColor(COL_WHITE);
      } else {
        Text::setColor(COL_INACTIVE);
      }

      Text::print(posX, center.y, TAB_NAMES[i]);
      Text::setBgColor();
    }
    Text::setAlign(Text::Align::LEFT);
  }

  void drawTestSelection(const std::span<TestGroup> &tests, const std::vector<int> &filteredTests)
  {
    int posX = 24;
    int resPosX = SCREEN_WIDTH - 14;
    int posY = posStartY;

    int arrowWobble = Math::sinf(ctx.frame * 0.1f) * 2;

    int totalTests = filteredTests.size();
    int startIdx = scrollOffset;
    int endIdx = std::min(scrollOffset + VISIBLE_TESTS, totalTests);

    // scrolling up arrow
    if (scrollOffset > 0) {
      Text::setColor(COL_SCROLL);
      Text::print(SCREEN_WIDTH / 2, posY + 3 - arrowWobble, "^");
      Text::setColor();
    }

    Text::setColor(ctx.saveData.selTestGroup < 0 ? COL_SELECT : COL_WHITE);
    Text::print(posX, posY, "Run All");

    Text::setAlign(Text::Align::RIGHT);
    Text::setColor(COL_INACTIVE);
    Text::print(resPosX, posY, "Results");
    Text::setAlign(Text::Align::LEFT);
    posY += 10;
    Draw::line({posX, posY}, {SCREEN_WIDTH-14, posY}, COL_INACTIVE);
    posY += 2;

    uint32_t totalExist = 0;
    uint32_t totalPassed = 0;
    uint32_t totalFailed = 0;

    for(auto i : filteredTests) {
      auto &group = tests[i];
      totalExist += group.getTestCount();
      if(group.getTestCount() == group.getCountTested()) {
        totalPassed += group.getCountSuccess();
        totalFailed += group.getCountTested() - group.getCountSuccess();
      }
    }

    for (int i = startIdx; i < endIdx; ++i)
    {
      auto &group = tests[filteredTests[i]];
      uint32_t success = group.getCountSuccess();
      uint32_t total = group.getCountTested();

      Text::setSpaceSize(4);
      Text::setColor(i == ctx.saveData.selTestGroup ? COL_SELECT : COL_WHITE);
      Text::print(posX, posY, group.getName().c_str());

      Text::setSpaceSize();
      Text::setAlign(Text::Align::RIGHT);
      if(group.getTestCount() != total)
      {
        Text::setColor(COL_INACTIVE);
        Text::printf(resPosX, posY, "----/%04d", group.getTestCount());
      } else {
        if(success == total)Text::setColor({0x33, 0xFF, 0x33});
        else if(success == 0)Text::setColor({0xFF, 0x33, 0x33});
        else Text::setColor({0xFF, 0xA7, 0x36});

        Text::printf(resPosX, posY, "%04d/%04d", success, total);
      }
      Text::setAlign(Text::Align::LEFT);

      posY += 10;
    }

    if (endIdx < totalTests) {
      Text::setColor(COL_SCROLL);
      Text::print(SCREEN_WIDTH / 2, posY + arrowWobble, "v");
      Text::setColor();
    }

    Draw::line({posX, posY}, {SCREEN_WIDTH-14, posY}, COL_INACTIVE);

    posY += 2;
    Text::setAlign(Text::Align::RIGHT);

    uint32_t totalRan = totalPassed + totalFailed;
    if(totalRan == 0) {
      Text::setColor(COL_INACTIVE);
      Text::printf(resPosX, posY, "----/----", totalPassed, totalPassed + totalFailed);
    } else {
      if(totalPassed == totalExist)Text::setColor({0x33, 0xFF, 0x33});
      else if(totalPassed == 0)Text::setColor({0xFF, 0x33, 0x33});
      else Text::setColor({0xFF, 0xA7, 0x36});
      Text::printf(resPosX, posY, "%04d/%04d", totalPassed, totalExist);
    }

    Text::setAlign(Text::Align::LEFT);

    Text::setColor(COL_SELECT);
    int cursorPos = posStartY + (ctx.saveData.selTestGroup+1) * 10 - 1 + (ctx.saveData.selTestGroup >= 0 ? 2 : 0);
    cursorPos -= scrollOffset * 10;
    Text::print(posX-10, cursorPos, ">");
    Text::setColor();

    posX = 24; posY = 160;
    Text::print(posX, posY, "   Select"   ); posY += 8;
    Text::print(posX, posY, "   Run Test" ); posY += 8;
    Text::print(posX, posY, "   Dump Test"); posY += 8;
    Text::print(posX, posY, "   Stop Test"); posY += 8;

    if(!MiMem::isSupported)
    {
      posY += 2;
      Text::setBgColor({0xFF, 0x33, 0x33});
      Text::setColor({0,0,0});
      Text::setSpaceSize(2);
      Text::print(posX, posY, "MI-Rep. not emulated, using fallback");
      Text::setSpaceSize();
      Text::setBgColor();
      Text::setColor();
    }

    posY = 160;
    Text::setColor({0xff, 0xd7, 0x36}); Text::print(posX, posY, "C:"); posY += 8;
    Text::setColor({0x66, 0x66, 0xFF}); Text::print(posX, posY, "A:"); posY += 8;
    Text::setColor({0x33, 0xFF, 0x33}); Text::print(posX, posY, "B:"); posY += 8;
    Text::setColor({0xFF, 0x44, 0x44}); Text::print(posX, posY, "S:"); posY += 8;
    Text::setColor();
  }
}

void Menu::draw(const std::span<TestGroup> &tests)
{
  auto &selGroup = ctx.saveData.selTestGroup;
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
  auto press = joypad_get_buttons_pressed(JOYPAD_PORT_1);

  MiMem::setU64(ctx.fb->buffer, 0, ctx.fb->stride * ctx.fb->height);

  if(press.l)--tab;
  if(press.r)++tab;
  if(tab < 0)tab = (int)TAB_NAMES.size() - 1;
  if(tab >= (int)TAB_NAMES.size())tab = 0;

  std::vector<int> filteredTests{};
  filteredTests.reserve(tests.size());

  if(tab == TAB_FAILED) {
    for(int i = 0; i < (int)tests.size(); ++i) {
      auto &group = tests[i];
      if(group.getCountSuccess() < group.getCountTested()) {
        filteredTests.push_back(i);
      }
    }
  } else {
    for(int i=0; i<(int)tests.size(); ++i) {
      filteredTests.push_back(i);
    }
  }

  drawTabs();

  if(tab == TAB_OPTIONS) {
    drawOptions();
  } else {
    if(press.d_up || press.c_up)--selGroup;
    if(press.d_down || press.c_down)++selGroup;
    if(selGroup < -1)selGroup = filteredTests.size()-1;
    if(selGroup >= (int)filteredTests.size())selGroup = -1;

    if (selGroup >= 0) {
      if (selGroup < scrollOffset) scrollOffset = selGroup;
      if (selGroup >= scrollOffset + VISIBLE_TESTS) scrollOffset = selGroup - VISIBLE_TESTS + 1;
    } else {
      scrollOffset = 0;
    }

    if(held.a || held.b) {
      ctx.nextTest = selGroup < 0 ? 0 : filteredTests[selGroup];
      ctx.dumpData = held.b;
      ctx.autoAdvanceGroup = selGroup < 0;
      ctx.save();
    }

    drawTestSelection(tests, filteredTests);
  }
  drawFooter();
  // prevents flickering only present in ares, can't be bothered to check why
  wait_ms(16);
}

