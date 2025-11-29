/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

namespace Text
{
  void setSpaceHidden(bool hidden = true);
  void setColor(color_t color = {0xFF, 0xFF, 0xFF, 0xFF});
  int print(int x, int y, const char* str);
  int printf(int x, int y, const char *fmt, ...);

  int printSmall(int x, int y, const char* str);
  int printfSmall(int x, int y, const char *fmt, ...);
}
