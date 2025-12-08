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

  [[maybe_unused]] std::string toHex(uint8_t val)
  {
    char buf[3];
    sprintf(buf, "%02X", val);
    return std::string{buf};
  }

  [[maybe_unused]] std::string toHex(uint16_t val) {
    char buf[5];
    sprintf(buf, "%04X", val);
    return std::string{buf};
  }

  [[maybe_unused]] std::string toHex(uint32_t val) {
    char buf[9];
    sprintf(buf, "%08lX", val);
    return std::string{buf};
  }

  [[maybe_unused]] std::string toHex(uint64_t val)
  {
    char buf[17];
    sprintf(buf, "%016llX", val);
    return std::string{buf};
  }

  [[maybe_unused]] std::string toString(float val, int digits) {
    char buf[32];
    sprintf(buf, "%.*f", digits, val);
    return std::string{buf};
  }

  constexpr float operator "" _deg(long double x) { return static_cast<float>(x * DEG_TO_RAD); }
  constexpr float operator "" _rad(long double x) { return static_cast<float>(x); }

  constexpr float toRad(float deg) { return deg * DEG_TO_RAD; }
}

constexpr uint32_t SAVE_DATA_MAGIC = 0xABCD'02;
struct SaveData
{
  uint32_t magic{SAVE_DATA_MAGIC};
  int selTestGroup{-1};
  uint32_t useSdCard{0};
  uint32_t autoAdvanceTest{1};
};

struct Context
{
  SaveData saveData{};
  surface_t *fb{};

  int nextTest{};
  uint32_t frame{};
  uint32_t diffMode = 0;

  bool hasSdCard{};
  bool dumpData{};
  bool autoAdvanceGroup{};

  void save();
};

namespace DIFF_MODE
{
  constexpr uint32_t ACTUAL_COLOR   = 0;
  constexpr uint32_t DIFF_COLOR     = 1;
  constexpr uint32_t EXPECTED_COLOR = 2;

  constexpr uint32_t ACTUAL_CVG     = 3;
  constexpr uint32_t DIFF_CVG       = 4;
  constexpr uint32_t EXPECTED_CVG   = 5;

}

extern Context ctx;