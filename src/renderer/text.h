/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Text
{
  enum class Align : uint32_t
  {
    LEFT = 0,
    RIGHT = 1,
    CENTER = 2
  };

  void setSpaceSize(int size = 8);
  void setColor(color_t color = {0xFF, 0xFF, 0xFF, 0xFF});
  void setBgColor(color_t color = {0x00, 0x00, 0x00, 0x00});
  void setAlign(Align newAlign);

  int print(int x, int y, const char* str);
  int printf(int x, int y, const char *fmt, ...);

  int printSmall(int x, int y, const char* str);
  int printfSmall(int x, int y, const char *fmt, ...);
}
