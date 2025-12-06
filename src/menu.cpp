/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <vector>

#include "main.h"
#include "framework/testGroup.h"
#include "renderer/draw.h"
#include "renderer/text.h"
#include "utils/color.h"
#include "utils/vecMath.h"

namespace
{
  constexpr int posStartY = 30;

  constexpr color_t COL_SELECT{0x66, 0x66, 0xFF};

  constinit int tab = 1;
  constinit int nextDemoSel = -1;

  constexpr std::array<const char*, 3> TAB_NAMES{
    "Failed",
    "All",
    "Options"
  };

  void drawTabs()
  {
    auto center = fm_vec2_t{SCREEN_WIDTH / 2.0f, posStartY-14};
    float tabWidth = 48.0f;
    float startX = center.x - ((TAB_NAMES.size() * tabWidth) / 2.0f);

    constexpr color_t COL_DEF{0x99, 0x99, 0x99};
    Text::setColor(COL_DEF);
    Text::print(70, center.y-1, "<");
    Text::print(SCREEN_WIDTH-70, center.y-1, ">");

    Text::printSmall(70+8, center.y+2, "L");
    Text::printSmall(SCREEN_WIDTH-70-4, center.y+2, "R");


    Text::setAlign(Text::Align::CENTER);
    for(size_t i = 0; i < TAB_NAMES.size(); ++i)
    {
      float posX = startX + (i * tabWidth) + (tabWidth / 2.0f);
      Text::setColor(i == (size_t)tab ? COL_SELECT : COL_DEF);

      Text::print((int)posX, center.y, TAB_NAMES[i]);
    }
    Text::setAlign(Text::Align::LEFT);
  }

  void drawCube(const fm_vec2_t &center, float size, color_t color)
  {
    fm_vec4_t vtx[8] = {
      {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
      {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1},
    };
    constexpr uint8_t indices[12][2] = {
      {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6},
      {6,7}, {7,4}, {0,4}, {1,5}, {2,6}, {3,7},
    };

    fm_mat4_t modelMat{};
    fm_vec3_t pos{0,0,0};
    fm_vec3_t rot{0, ctx.frame * 0.011f, ctx.frame * 0.015f};
    fm_mat4_from_rt_euler(&modelMat, rot.v, &pos);

    for(auto &v : vtx) {
      fm_mat4_mul_vec4(&v, &modelMat, &v);
      float zInv = 1.0f / (v.z + 3.0f) * size;
      v = fm_vec4_t{v.x * zInv, v.y * zInv};
    }

    for(auto &idx : indices) {
      Draw::lineF(
        center + fm_vec2_t{vtx[idx[0]].x, vtx[idx[0]].y},
        center + fm_vec2_t{vtx[idx[1]].x, vtx[idx[1]].y},
        color
      );
    }
  }
}

void demoMenuDraw(const std::span<TestGroup> &tests)
{
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
  auto press = joypad_get_buttons_pressed(JOYPAD_PORT_1);

  sys_hw_memset64(ctx.fb->buffer, 0, ctx.fb->stride * ctx.fb->height);

  if(press.l)--tab;
  if(press.r)++tab;
  if(tab < 0)tab = (int)TAB_NAMES.size() - 1;
  if(tab >= (int)TAB_NAMES.size())tab = 0;

  if(press.d_up || press.c_up)--nextDemoSel;
  if(press.d_down || press.c_down)++nextDemoSel;
  if(nextDemoSel < -1)nextDemoSel = tests.size()-1;
  if(nextDemoSel >= (int)tests.size())nextDemoSel = -1;

  if(held.a || held.b) {
    ctx.nextTest = nextDemoSel < 0 ? 0 : nextDemoSel;
    ctx.dumpData = held.b;
    ctx.autoAdvance = nextDemoSel < 0;
    ctx.reset();
  }

  if(press.r && ctx.hasSdCard) {
    ctx.useSdCard = !ctx.useSdCard;
  }

  int posX = 24;
  int resPosX = SCREEN_WIDTH - 14;
  int posY = posStartY;

  drawTabs();

  Text::setColor(nextDemoSel < 0 ? COL_SELECT : color_t{0xFF, 0xFF, 0xFF});
  Text::print(posX, posY, "Run All");

  Text::setAlign(Text::Align::RIGHT);
  Text::setColor({0x99, 0x99, 0x99});
  Text::print(resPosX, posY, "Results");
  Text::setAlign(Text::Align::LEFT);
  posY += 10;
  Draw::line({posX, posY}, {SCREEN_WIDTH-14, posY}, {0x77, 0x77, 0x77});
  posY += 2;

  uint32_t totalPassed = 0;
  uint32_t totalFailed = 0;

  for(int i = 0; i < (int)tests.size(); ++i)
  {
    auto &group = tests[i];
    uint32_t success = group.getCountSuccess();
    uint32_t total = group.getCountTested();

    Text::setColor(i == nextDemoSel ? COL_SELECT : color_t{0xFF, 0xFF, 0xFF});
    Text::print(posX, posY, group.getName().c_str());

    Text::setAlign(Text::Align::RIGHT);
    if(group.getTestCount() != total)
    {
      Text::setColor({0x99, 0x99, 0x99});
      Text::printf(resPosX, posY, "---/%03d", group.getTestCount());
    } else {

      totalPassed += success;
      totalFailed += (total - success);

      if(success == total)Text::setColor({0x33, 0xFF, 0x33});
      else if(success == 0)Text::setColor({0xFF, 0x33, 0x33});
      else Text::setColor({0xFF, 0xA7, 0x36});

      Text::printf(resPosX, posY, "%03d/%03d", success, total);
    }
    Text::setAlign(Text::Align::LEFT);

    posY += 10;
  }

  Draw::line({posX, posY}, {SCREEN_WIDTH-14, posY}, {0x77, 0x77, 0x77});

  posY += 2;
  Text::setAlign(Text::Align::RIGHT);

  if((totalPassed + totalFailed) == 0) {
    Text::printf(resPosX, posY, "---/---", totalPassed, totalPassed + totalFailed);
  } else {
    if(totalPassed == (totalPassed + totalFailed))Text::setColor({0x33, 0xFF, 0x33});
    else if(totalPassed == 0)Text::setColor({0xFF, 0x33, 0x33});
    else Text::setColor({0xFF, 0xA7, 0x36});
    Text::printf(resPosX, posY, "%03d/%03d", totalPassed, totalPassed + totalFailed);
  }

  Text::setAlign(Text::Align::LEFT);

  Text::setColor(COL_SELECT);
  Text::print(posX-10, posStartY + (nextDemoSel+1) * 10 - 1 + (nextDemoSel >= 0 ? 2 : 0), ">");
  Text::setColor();

  posY = 160;
  posX = 24;
  Text::print(posX, posY, "   Select"); posY += 8;
  Text::print(posX, posY, "   Run Test"); posY += 8;
  Text::print(posX, posY, "   Dump Test"); posY += 8;

  if(ctx.hasSdCard) {
    Text::print(posX, posY, ctx.useSdCard ? "R: SD-Card" : "R: Logging"); posY += 8;
  }
  Text::print(posX, posY, "   Stop Test"); posY += 8;

  posY = 160;
  Text::setColor({0xff, 0xd7, 0x36});
  Text::print(posX, posY, "C:"); posY += 8;
  Text::setColor({0x66, 0x66, 0xFF});
  Text::print(posX, posY, "A:"); posY += 8;
  Text::setColor({0x33, 0xFF, 0x33});
  Text::print(posX, posY, "B:"); posY += 8;
  if(ctx.hasSdCard)posY += 8;
  Text::setColor({0xFF, 0x44, 0x44});
  Text::print(posX, posY, "S:"); posY += 8;
  Text::setColor();

  posY = 208;

  Text::setAlign(Text::Align::CENTER);

  Text::setColor({0xAA, 0xFF, 0xAA});
  Text::print(SCREEN_WIDTH/2, posY, "github.com/HailToDodongo/snapper64");
  posY += 10;
  Text::setColor({0x77, 0x77, 0x99});
  Text::print(SCREEN_WIDTH/2, posY, "(C) 2025 Max Bebok");

  // actually "Bebök", too lazy to make my own name work so just set ü-dots manually here
  uint32_t* buf = (uint32_t*)ctx.fb->buffer;
  buf[(posY+0) * (ctx.fb->stride / 4) + (218) ] = 0x77779900;
  buf[(posY+1) * (ctx.fb->stride / 4) + (218) ] = 0x77779900;
  buf[(posY+0) * (ctx.fb->stride / 4) + (218+2) ] = 0x77779900;
  buf[(posY+1) * (ctx.fb->stride / 4) + (218+2) ] = 0x77779900;

  Text::setColor();
  Text::setSpaceHidden(true);
  Text::setAlign(Text::Align::LEFT);

  drawCube({262, 176}, 40.0f, Color::rainbow(ctx.frame * 0.02f));

  // prevents flickering only present in ares, can't be bothered to check why
  wait_ms(16);
}

/*void testText() {
  int posY = 32;
  Text::print(16, posY, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); posY += 8;
  Text::print(16, posY, "abcdefghijklmnopqrstuvwxyz"); posY += 8;
  Text::print(16, posY, "0123456789"); posY += 8;
  Text::print(16, posY, "!@#$%^&*()-_=+[]{};:"); posY += 8;
  Text::print(16, posY, "'\",.<>/?\\|`~"); posY += 8;

  Text::printSmall(128, 128, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  Text::printSmall(128, 128+8, "abcdefghijklmnopqrstuvwxyz");
  Text::printSmall(128, 128+16, "0123456789_");

  posY += 8;
  }
}*/

