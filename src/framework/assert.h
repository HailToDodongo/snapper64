/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "testSurface.h"
#include "../main.h"
#include "../renderer/draw.h"

struct Assert
{
  // context to identify a specific assert
  // later used to load / dump buffer
  uint32_t hashGroup{0};
  uint32_t hashTest{0};
  uint32_t hashAssert{0};

  uint32_t successCount{0};
  uint32_t errorCount{0};

  void result(bool isOk)
  {
    if(isOk) {
      ++successCount;
    } else {
      ++errorCount;
    }
  }

  template<typename T>
  Assert &equals(T actual, T expected) {
    result(actual == expected);
    return *this;
  }

  Assert &equals(uint32_t actual, uint32_t expected, const std::string &msg, const Draw::IVec2 &pos);

  Assert &surface(TestSurface &surf, bool hiddenBits = false);
};
