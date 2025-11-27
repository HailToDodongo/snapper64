/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "testSurface.h"
#include "../main.h"

struct Assert
{
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

  Assert &surface(const TestSurface &surf, bool hiddenBits = false)
  {
    return *this;
  }
};
