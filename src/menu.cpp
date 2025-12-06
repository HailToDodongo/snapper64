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
  constexpr int posStartY = 24;

  constexpr color_t COL_SELECT{0x66, 0x66, 0xFF};

  void drawCube(const fm_vec2_t &center, float size, color_t color)
  {
    fm_vec4_t cubeVtx[8] = {
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

    for(auto &v : cubeVtx) {
      fm_mat4_mul_vec4(&v, &modelMat, &v);
      float zInv = 1.0f / (v.z + 3.0f) * size;
      v = fm_vec4_t{v.x * zInv, v.y * zInv};
    }

    for(auto &idx : indices) {
      Draw::lineF(
        center + fm_vec2_t{cubeVtx[idx[0]].x, cubeVtx[idx[0]].y},
        center + fm_vec2_t{cubeVtx[idx[1]].x, cubeVtx[idx[1]].y},
        color
      );
    }
  }

  constinit int tab = 0;
  constinit int nextDemoSel = -1;
  constinit sprite_t *icons{};
}

void demoMenuInit()
{
  icons = sprite_load("rom:/");
}

void demoMenuDraw(const std::vector<TestGroup> &tests)
{
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
  auto press = joypad_get_buttons_pressed(JOYPAD_PORT_1);

  sys_hw_memset64(ctx.fb->buffer, 0, ctx.fb->stride * ctx.fb->height);

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

  Text::setColor(nextDemoSel < 0 ? COL_SELECT : color_t{0xFF, 0xFF, 0xFF});
  Text::print(posX, posY, "Run All");

  Text::setAlignRight();
  Text::setColor({0x99, 0x99, 0x99});
  Text::print(resPosX, posY, "Results");
  Text::setAlignLeft();
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

    Text::setAlignRight();
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
    Text::setAlignLeft();

    posY += 10;
  }

  Draw::line({posX, posY}, {SCREEN_WIDTH-14, posY}, {0x77, 0x77, 0x77});

  posY += 2;
  Text::setAlignRight();

  if((totalPassed + totalFailed) == 0) {
    Text::printf(resPosX, posY, "---/---", totalPassed, totalPassed + totalFailed);
  } else {
    if(totalPassed == (totalPassed + totalFailed))Text::setColor({0x33, 0xFF, 0x33});
    else if(totalPassed == 0)Text::setColor({0xFF, 0x33, 0x33});
    else Text::setColor({0xFF, 0xA7, 0x36});
    Text::printf(resPosX, posY, "%03d/%03d", totalPassed, totalPassed + totalFailed);
  }

  Text::setAlignLeft();


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

  Text::setColor({0xAA, 0xFF, 0xAA});
  Text::print(20, posY, "github.com/HailToDodongo/snapper64");
  posY += 10;
  Text::setColor({0x77, 0x77, 0x99});
  Text::print(20 + 16*4, posY, "(C) 2025 Max Bebok");

  // actually "Bebök", too lazy to make my own name work so just set ü-dots manually here
  uint32_t* buf = (uint32_t*)ctx.fb->buffer;
  buf[(posY+0) * (ctx.fb->stride / 4) + (214) ] = 0x77779900;
  buf[(posY+1) * (ctx.fb->stride / 4) + (214) ] = 0x77779900;
  buf[(posY+0) * (ctx.fb->stride / 4) + (214+2) ] = 0x77779900;
  buf[(posY+1) * (ctx.fb->stride / 4) + (214+2) ] = 0x77779900;

  Text::setColor();
  Text::setSpaceHidden(true);

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

