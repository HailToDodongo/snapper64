/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include "../main.h"
#include "../renderer/text.h"
#include "../utils/miMemory.h"

namespace {
  constexpr uint32_t FMT_BUFF_SIZE = 128;

  constinit uint32_t currColor = 0xFFFFFF00;
  constinit uint8_t ignoreChar = 0;
  constinit bool alignLeft = true;

  #include "font.h"
  #include "font3x5.h"
}

void Text::setSpaceHidden(bool hidden)
{
  ignoreChar = hidden ? 0 : 0xFF;
}

void Text::setColor(color_t color)
{
  currColor = color_to_packed32(color);
}

void Text::setAlignLeft() {
  alignLeft = true;
}

void Text::setAlignRight() {
  alignLeft = false;
}

int Text::print(int x, int y, const char *str) {
  if(!alignLeft)
  {
    int textLen = strlen(str);
    x -= textLen * 8;
  }

  auto fbBuff = (uint8_t*)ctx.fb->buffer;

  uint64_t *buffStart = (uint64_t*)&fbBuff[y * ctx.fb->stride + x*4];
  uint64_t col = (uint64_t)currColor;

  while(*str)
  {
    uint8_t charCode = (uint8_t)*str - ' ';
    uint64_t charData = FONT_8x8_DATA[charCode];
    uint64_t *buff = buffStart;
    ++x;

    if(charCode != ignoreChar)
    {
      for(int y=0; y<8; ++y) {
        for(int x=0; x<2; ++x) {
          uint64_t val = (charData & 0b0001) ? (col << 32) : 0;
          val |= (charData & 0b0010) ? (col <<  0) : 0;
          buff[0] = val;
          val  = (charData & 0b0100) ? (col << 32) : 0;
          val |= (charData & 0b1000) ? (col <<  0) : 0;

          buff[1] = val;

          charData >>= 4;
          buff += 2;
        }
        buff += (ctx.fb->stride / 8) - 4;
      }
      // draw extra black line below
      buff[0] = 0;
      buff[1] = 0;
      buff[2] = 0;
      buff[3] = 0;
    }

    buffStart += 4;
    ++str;
  }
  //uint64_t charData = FONT_8x8_DATA[0];

  return x;
}

int Text::printSmall(int x, int y, const char* str)
{
  auto fbBuff = (uint8_t*)ctx.fb->buffer;

  auto *buffStart = (uint64_t*)&fbBuff[y * ctx.fb->stride + x*4];

  while(*str)
  {
    uint8_t charCode = (uint8_t)*str - '-';
    if(charCode >= ('a' - '-'))charCode -= ('a' - 'A');

    uint16_t charData = FONT_3x5_DATA[charCode];
    auto buff = buffStart;
    ++x;

    for(int y=0; y<5; ++y) {
      uint64_t col = (charData & 0b001) ? currColor : 0;
      col <<= 32;
      buff[0] = col | ((charData & 0b010) ? currColor : 0);

      col = (charData & 0b100) ? currColor : 0;
      buff[1] = col << 32;

      buff += (ctx.fb->stride / 8);
      charData >>= 3;
    }
    // draw extra black line below
    buff[0] = 0;
    buff[1] = 0;

    buffStart += 2;
    ++str;
  }
  //uint64_t charData = FONT_8x8_DATA[0];

  return x;
}

int Text::printf(int x, int y, const char *fmt, ...) {
  char buffer[FMT_BUFF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, FMT_BUFF_SIZE, fmt, args);
  va_end(args);
  return print(x, y, buffer);
}

int Text::printfSmall(int x, int y, const char* fmt, ...)
{
  char buffer[FMT_BUFF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, FMT_BUFF_SIZE, fmt, args);
  va_end(args);
  return printSmall(x, y, buffer);
}
