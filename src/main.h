/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include <string>

#define DEG_TO_RAD 0.01745329252L
#define RAD_TO_DEG 57.295779512896175L

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

  constexpr float operator "" _deg(long double x) { return static_cast<float>(x * DEG_TO_RAD); }
  constexpr float operator "" _rad(long double x) { return static_cast<float>(x); }

  constexpr float toRad(float deg) { return deg * DEG_TO_RAD; }
}

struct Context
{
  surface_t *fb{};

  int countAssertPassed = 0;
  int countAssertFailed = 0;

  // if set, dumps data (e.g. framebuffer) to create reference files
  bool hasSdCard{};
  bool useSdCard{false};
  bool dumpData{};
  bool autoAdvance{};

  void reset() {
    countAssertPassed = 0;
    countAssertFailed = 0;
  }
};

extern Context ctx;