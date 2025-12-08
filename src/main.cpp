/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include "main.h"
#include "renderer/text.h"
#include "framework/testGroup.h"
#include "framework/testPack.h"
#include "renderer/vi.h"
#include "menu/menu.h"

#include <unordered_set>
#include <vector>
#include <algorithm>

#include "utils/miMemory.h"

typedef TestGroup (*TestCreateFunc)();

#define TEST_ENTRY(X) namespace Tests::X { TestGroup create(); }
#include "testList.h"
#undef TEST_ENTRY

constinit Context ctx{};

namespace {

  #define TEST_ENTRY(X) + 1
  constexpr size_t TEST_GROUP_COUNT = 0
    #include "testList.h"
  ;
  #undef TEST_ENTRY

  constexpr int TEST_IDX_MENU = -1;
  constinit std::array<TestGroup, TEST_GROUP_COUNT> tests{};

  /*void validateTests()
  {
    std::unordered_set<uint32_t> groupNames{};
    groupNames.reserve(tests.size());

    for(const auto &group : tests) {
      uint32_t hash = group.getNameHash();
      assertf(groupNames.insert(hash).second, "Duplicate test group name hash detected: %s (hash %08lX)", group.getName().c_str(), hash);

      std::unordered_set<uint32_t> testNames{};
      testNames.reserve(group.getTestCount());
      for(const auto &test : group.getTests()) {
        uint32_t testHash = test.nameHash;
        assertf(testNames.insert(testHash).second, "Duplicate test name detected in group %s: %s (hash %08lX)", group.getName().c_str(), test.name.c_str(), testHash);
      }
    }
  }*/
}

[[noreturn]]
int main()
{
  debug_init_isviewer();
  debug_init_usblog();

  dfs_init(DFS_DEFAULT_LOCATION);
  asset_init_compression(3);

  joypad_init();

  assert_memory_expanded();

  MiMem::checkSupport();
  VI::init();

  VI::setFrameBuffers({
    surface_make((char*)0xA0300000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
    surface_make((char*)0xA0380000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
    surface_make((char*)0xA0400000, FMT_RGBA32, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH*4),
  });

  Text::setAlign(Text::Align::CENTER);
    Text::print(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 8, "Loading");
    //Text::print(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 8, "(MI-Repeat not supported)");
  Text::setAlign(Text::Align::LEFT);
  VI::show();

  ctx.hasSdCard = debug_init_sdfs("sd:/", -1);
  ctx.useSdCard = ctx.hasSdCard;
  ctx.autoAdvanceTest = true;

  TestPack::init();

  auto tLoad = get_ticks();
  #define TEST_ENTRY(X) Tests::X::create(),
  tests = {
    #include "testList.h"
  };

  #undef TEST_ENTRY
  tLoad = get_ticks() - tLoad;
  debugf("[Debug] Loaded tests in %lld ms (%lld us)\n", TICKS_TO_MS(tLoad), TICKS_TO_US(tLoad));

  std::ranges::sort(tests, [](const TestGroup &a, const TestGroup &b) {
    return a.getName() < b.getName();
  });

  /*auto t = get_ticks();
  validateTests();
  t = get_ticks() - t;
  debugf("Test init: %lldms\n", TICKS_TO_MS(t));*/

  ctx.frame = 0;
  ctx.nextTest = TEST_IDX_MENU;

  for(;;) 
  {
    ++ctx.frame;
    if(ctx.frame > 0xFFFF)ctx.frame = 0;

    VI::show();
    joypad_poll();

    Text::setColor();

    if(ctx.nextTest == TEST_IDX_MENU) {
      Menu::draw(tests);
    } else {
      if(!tests[ctx.nextTest].run() || !ctx.autoAdvanceGroup)
      {
        ctx.nextTest = TEST_IDX_MENU;
      } else if (ctx.autoAdvanceGroup) {
        ++ctx.nextTest;
        if(ctx.nextTest >= (int)tests.size()) {
          ctx.nextTest = TEST_IDX_MENU;
        }
      }

    }
  }
}
