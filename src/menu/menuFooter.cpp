/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "menu.h"
#include "../renderer/text.h"
#include "../renderer/draw.h"
#include "../utils/color.h"

namespace
{
  void drawCube(const fm_vec2_t &center, float size, color_t color)
  {
    fm_vec4_t vtx[8] = {
      {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
      {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1},
    };
    constexpr uint8_t indices[12][2] = {
      {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6},
      {6,7}, {7,4}, {0,4}, {1,5}, {2,6}, {3,7},
    };

    fm_mat4_t modelMat{};
    fm_vec3_t pos{0,0,0};
    fm_vec3_t rot{0, ctx.frame * 0.011f, ctx.frame * 0.015f};
    fm_mat4_from_rt_euler(&modelMat, rot.v, &pos);

    for(auto &v : vtx) {
      fm_mat4_mul_vec4(&v, &modelMat, &v);
      float zInv = 1.0f / (v.z + 3.0f) * size;
      v = fm_vec4_t{v.x * zInv, v.y * zInv};
    }

    for(auto &idx : indices) {
      Draw::lineF(
        center + fm_vec2_t{vtx[idx[0]].x, vtx[idx[0]].y},
        center + fm_vec2_t{vtx[idx[1]].x, vtx[idx[1]].y},
        color
      );
    }
  }
}

void Menu::drawFooter()
{
  int posY = 208;
  Text::setAlign(Text::Align::CENTER);

  Text::setColor({0xAA, 0xFF, 0xAA});
  Text::print(SCREEN_WIDTH/2, posY, "github.com/HailToDodongo/snapper64");
  posY += 10;
  Text::setColor({0x77, 0x77, 0x99});
  Text::print(SCREEN_WIDTH/2, posY, "(C) 2025 Max Bebok");

  // actually "Bebök", too lazy to make my own name work so just set ü-dots manually here
  uint32_t* buf = (uint32_t*)ctx.fb->buffer;
  buf[(posY+0) * (ctx.fb->stride / 4) + (218) ] = 0x77779900;
  buf[(posY+1) * (ctx.fb->stride / 4) + (218) ] = 0x77779900;
  buf[(posY+0) * (ctx.fb->stride / 4) + (218+2) ] = 0x77779900;
  buf[(posY+1) * (ctx.fb->stride / 4) + (218+2) ] = 0x77779900;

  Text::setColor();
  Text::setAlign(Text::Align::LEFT);

  drawCube({272, 180}, 36.0f, Color::rainbow(ctx.frame * 0.02f));

  /*
    auto t = get_ticks();
    posY = 32;
    Text::print(16, posY, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); posY += 8;
    Text::print(16, posY, "abcdefghijklmnopqrstuvwxyz"); posY += 8;
    Text::print(16, posY, "0123456789"); posY += 8;
    Text::print(16, posY, "!@#$%^&*()-_=+[]{};:"); posY += 8;
    Text::print(16, posY, "'\",.<>/?\\|`~"); posY += 8;
    t = get_ticks() - t;
    debugf("[Debug] Text render time: %lld us\n", TICKS_TO_US(t));
  */
}

