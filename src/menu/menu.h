/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <span>
#include "../framework/testGroup.h"

namespace Menu
{
  extern void draw(const std::span<TestGroup> &tests);

  extern void drawFooter();
  extern void drawOptions();
}
