/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
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

extern void demoMenuInit();
extern void demoMenuDraw(const std::vector<TestGroup> &tests);

namespace {
  constinit std::vector<TestGroup> tests{};
}

[[noreturn]]
int main()
{
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

  Text::print(32, 32, "Loading...");

  VI::show();

  #define TEST_ENTRY(X) Tests::X::create(),
  tests = {
    #include "testList.h"
  };
  #undef TEST_ENTRY

  std::ranges::sort(tests, [](const TestGroup &a, const TestGroup &b) {
    return a.getName() < b.getName();
  });

  ctx.frame = 0;
  for(;;) 
  {
    ++ctx.frame;
    VI::show();
    joypad_poll();

    Text::setColor();
    Text::setSpaceHidden(false);

    if(ctx.nextDemo < 0) {
      demoMenuDraw(tests);
    } else {
      if(!tests[ctx.nextDemo].run() || !ctx.autoAdvance)
      {
        ctx.nextDemo = -1;
      } else if (ctx.autoAdvance) {
        ++ctx.nextDemo;
        if(ctx.nextDemo >= (int)tests.size()) {
          ctx.nextDemo = -1;
        }
      }

    }
  }
}
