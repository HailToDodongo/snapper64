/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "testSurface.h"
#include "../main.h"

struct Assert
{
  // context to identify a specific assert
  // later used to load / dump buffer
  uint32_t hashGroup{0};
  uint32_t hashTest{0};
  uint32_t hashAssert{0};

  void result(bool isOk)
  {
    if(isOk) {
      ++ctx.countAssertPassed;
    } else {
      ++ctx.countAssertFailed;
    }
  }

  template<typename T>
  Assert &equals(T a, T b) {
    result(a == b);
    return *this;
  }

  Assert &surface(TestSurface &surf, bool hiddenBits = false);
};
