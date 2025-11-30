/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include <array>
#include <libdragon.h>

namespace VI
{
  void init();
  void show();

  void keepAlive();

  void setFrameBuffers(std::array<surface_t, 3> fb);
}