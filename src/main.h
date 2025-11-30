/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <string>

namespace {
  constexpr uint32_t SCREEN_WIDTH = 320;
  constexpr uint32_t SCREEN_HEIGHT = 240;

  std::string toHex(uint32_t val) {
    char buf[9];
    sprintf(buf, "%08lX", val);
    return std::string{buf};
  }
  std::string toHex(uint64_t val)
  {
    char buf[17];
    sprintf(buf, "%016llX", val);
    return std::string{buf};
  }
}

struct Context
{
  surface_t *fb{};

  int countAssertPassed = 0;
  int countAssertFailed = 0;

  // if set, dumps data (e.g. framebuffer) to create reference files
  bool dumpData{};
  bool autoAdvance{};

  void reset() {
    countAssertPassed = 0;
    countAssertFailed = 0;
  }
};

extern Context ctx;