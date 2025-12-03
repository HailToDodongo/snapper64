/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <utility>
#include <vector>
#include <algorithm>
#include "renderer/text.h"
#include "main.h"

#include "framework/testGroup.h"
#include "framework/testPack.h"

typedef TestGroup (*TestCreateFunc)();

#define TEST_ENTRY(X) namespace Tests::X { TestGroup create(); }
#include "testList.h"
#include "renderer/vi.h"
#undef TEST_ENTRY

constinit Context ctx{};

namespace {
  constinit std::vector<TestGroup> tests{};

  /*void testText()
  {
    int posY = 32;
    for(int i=0; i<4; ++i) {
      Text::print(16, posY, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); posY += 8;
      Text::print(16, posY, "abcdefghijklmnopqrstuvwxyz"); posY += 8;
      Text::print(16, posY, "0123456789"); posY += 8;
      Text::print(16, posY, "!@#$%^&*()-_=+[]{};:"); posY += 8;
      Text::print(16, posY, "'\",.<>/?\\|`~"); posY += 8;
      posY += 8;
    }
  }*/

  constinit uint64_t frameTime = 0;
  constinit int nextDemo = -1;

  uint32_t frame = 0;
  int nextDemoSel = -1;

  void demoMenuDraw()
  {
    // draw checkerboard pattern
    uint32_t *buff = (uint32_t*)ctx.fb->buffer;
    int offset = frame / 4;

    for(int y=0; y<ctx.fb->height; ++y)
    {
      if(y > ctx.fb->height-40) {
        sys_hw_memset64(buff, 0, ctx.fb->stride);
        buff += ctx.fb->stride/4;
        continue;
      }

      for(int x=0; x<ctx.fb->width; ++x) {
        bool isEven = (((x+offset) / 16) + ((y+offset) / 16)) % 2 == 0;
        if(y < 30 || y > (int)(40 + (tests.size()+1) * 10))
        {
          *buff++ = isEven
            ? color_to_packed32(color_t{0x33, 0x33, 0x33})
            : color_to_packed32(color_t{0x22, 0x22, 0x22});
        } else {
          *buff++ = isEven ? color_to_packed32(color_t{0x11, 0x11, 0x11}) : 0;
        }
      }
      buff += (ctx.fb->stride/4 - ctx.fb->width);
    }

    auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
    auto press = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if(press.d_up || press.c_up)--nextDemoSel;
    if(press.d_down || press.c_down)++nextDemoSel;
    if(nextDemoSel < -1)nextDemoSel = tests.size()-1;
    if(nextDemoSel >= (int)tests.size())nextDemoSel = -1;

    if(held.a || held.b) {
      nextDemo = nextDemoSel < 0 ? 0 : nextDemoSel;
      ctx.dumpData = held.b;
      ctx.autoAdvance = nextDemoSel < 0;
      ctx.reset();
    }

    if(press.r && ctx.hasSdCard) {
      ctx.useSdCard = !ctx.useSdCard;
    }

    constexpr color_t colSel{0x66, 0x66, 0xFF};

    int posY = 36;
    Text::setColor(nextDemoSel < 0 ? colSel : color_t{0xFF, 0xFF, 0xFF});
    Text::print(40, posY, "-Run All-"); posY += 10;

    for(int i = 0; i < (int)tests.size(); ++i) {
      Text::setColor(i == nextDemoSel ? colSel : color_t{0xFF, 0xFF, 0xFF});
      Text::print(40, posY, tests[i].getName().c_str());
      posY += 10;
    }

    Text::setColor(colSel);
    Text::print(28, 36 + (nextDemoSel+1) * 10 - 1, ">");
    Text::setColor();

    posY = 160;
    Text::print(30, posY, "Asserts:"); posY += 8;
    if(ctx.countAssertPassed == 0)
    {
      Text::setColor({0xAA, 0xAA, 0xAA});
      Text::print(30, posY, "Passed: -");
    } else {
      Text::setColor({0x33, 0xFF, 0x33});
      Text::printf(30, posY, "Passed: %d", ctx.countAssertPassed);
    }
    posY += 8;

    if(ctx.countAssertFailed == 0) {
      Text::setColor({0xAA, 0xAA, 0xAA});
      Text::print(30, posY, "Failed: -");
    } else {
      Text::setColor({0xFF, 0x33, 0x33});
      Text::printf(30, posY, "Failed: %d", ctx.countAssertFailed);
    }
    Text::setColor();

    posY = 160;
    int posX = 180;
    Text::print(posX, posY, "   Select"); posY += 8;
    Text::print(posX, posY, "   Run Test"); posY += 8;
    Text::print(posX, posY, "   Dump Test"); posY += 8;

    if(ctx.hasSdCard)
    {
      Text::print(posX, posY, ctx.useSdCard ? "R: (SD-Card)" : "R: (Logging)");
    } else {
      Text::print(posX, posY, "   (Logging)");
    }

    posY = 160;
    Text::setColor({0xff, 0xd7, 0x36});
    Text::print(posX, posY, "C:"); posY += 8;
    Text::setColor({0x66, 0x66, 0xFF});
    Text::print(posX, posY, "A:"); posY += 8;
    Text::setColor({0x33, 0xFF, 0x33});
    Text::print(posX, posY, "B:"); posY += 8;

    posY = 208;

    Text::setColor({0xAA, 0xFF, 0xAA});
    Text::print(20, posY, "Repo: <WIP>");
    posY += 10;
    Text::setColor({0x77, 0x77, 0x99});
    Text::print(20, posY, "(C) 2025 Max Beboek (HailToDodongo)"); posY += 10;
    Text::setColor();

    //Text::printSmall(128, 128, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    //Text::printSmall(128, 128+8, "abcdefghijklmnopqrstuvwxyz");
    //Text::printSmall(128, 128+16, "0123456789_");

    Text::setSpaceHidden(true);
  }
}

[[noreturn]]
int main()
{
  #define TEST_ENTRY(X) Tests::X::create(),
  tests = {
    #include "testList.h"
  };
  #undef TEST_ENTRY

  std::ranges::sort(tests, [](const TestGroup &a, const TestGroup &b) {
    return a.getName() < b.getName();
  });

  debug_init_isviewer();
  debug_init_usblog();

  dfs_init(DFS_DEFAULT_LOCATION);
  asset_init_compression(3);

  joypad_init();

  ctx.hasSdCard = debug_init_sdfs("sd:/", -1);
  ctx.useSdCard = ctx.hasSdCard;

  TestPack::init();
  VI::init();

  assertf(get_tv_type() == TV_NTSC, "Please run ROM in NTSC mode!");

  VI::setFrameBuffers({
    surface_make((char*)0xA0300000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
    surface_make((char*)0xA0380000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
    surface_make((char*)0xA0400000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
  });

  frame = 0;
  for(;;) 
  {
    ++frame;
    VI::show();
    joypad_poll();

    Text::setColor();
    Text::setSpaceHidden(false);

    if(nextDemo < 0) {
      demoMenuDraw();
    } else {
      memset(ctx.fb->buffer, 0x33, ctx.fb->height * ctx.fb->stride);
      if(!tests[nextDemo].run() || !ctx.autoAdvance)
      {
        nextDemo = -1;
      } else if (ctx.autoAdvance) {
        ++nextDemo;
        if(nextDemo >= (int)tests.size()) {
          nextDemo = -1;
        }
      }

    }
  }
}
