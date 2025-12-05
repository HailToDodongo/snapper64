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

constexpr fm_vec3_t UP_VECTOR{0, 1, 0};
constexpr fm_vec3_t GRAVITY = -UP_VECTOR * 9.81f;

constinit Context ctx{};

extern void demoMenuInit();
extern void demoMenuDraw(const std::vector<TestGroup> &tests);

namespace {
  constexpr int TEST_IDX_MENU = -1;
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

  VI::init();

  VI::setFrameBuffers({
    surface_make((char*)0xA0300000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
    surface_make((char*)0xA0380000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
    surface_make((char*)0xA0400000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
  });

  Text::print(SCREEN_WIDTH/2 - 7*4, SCREEN_HEIGHT/2 - 8, "Loading");
  VI::show();

  ctx.hasSdCard = debug_init_sdfs("sd:/", -1);
  ctx.useSdCard = ctx.hasSdCard;

  TestPack::init();

  #define TEST_ENTRY(X) Tests::X::create(),
  tests = {
    #include "testList.h"
  };
  #undef TEST_ENTRY

  std::ranges::sort(tests, [](const TestGroup &a, const TestGroup &b) {
    return a.getName() < b.getName();
  });

  ctx.frame = 0;
  ctx.nextTest = TEST_IDX_MENU;

  for(;;) 
  {
    ++ctx.frame;
    VI::show();
    joypad_poll();

    Text::setColor();
    Text::setSpaceHidden(false);

    if(ctx.nextTest == TEST_IDX_MENU) {
      demoMenuDraw(tests);
    } else {
      if(!tests[ctx.nextTest].run() || !ctx.autoAdvance)
      {
        ctx.nextTest = TEST_IDX_MENU;
      } else if (ctx.autoAdvance) {
        ++ctx.nextTest;
        if(ctx.nextTest >= (int)tests.size()) {
          ctx.nextTest = TEST_IDX_MENU;
        }
      }

    }
  }
}
