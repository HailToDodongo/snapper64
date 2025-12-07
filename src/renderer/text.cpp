/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include "../main.h"
#include "../renderer/text.h"

namespace {
  constexpr uint32_t FMT_BUFF_SIZE = 128;

  constinit uint32_t currColor = 0xFFFFFF00;
  constinit uint32_t currBgColor = 0x00000000;

  constinit int spaceSize = 8;

  constinit auto align = Text::Align::LEFT;

  #include "font.h"
  #include "font3x5.h"
}

void Text::setSpaceSize(int size)
{
  spaceSize = size;
}

void Text::setColor(color_t color)
{
  currColor = color_to_packed32(color);
}

void Text::setBgColor(color_t color)
{
  currBgColor = color_to_packed32(color);
}

void Text::setAlign(Align newAlign)
{
  align = newAlign;
}

int Text::print(int x, int y, const char *str)
{
  if(align == Align::RIGHT)
  {
    int textLen = strlen(str);
    x -= textLen * 8;
  } else if(align == Align::CENTER)
  {
    int textLen = strlen(str);
    x -= (textLen * 8) / 2;
  }

  auto fbBuff = (uint8_t*)ctx.fb->buffer;

  uint64_t *buffStart = (uint64_t*)&fbBuff[y * ctx.fb->stride + x*4];
  uint64_t col = currColor;
  uint64_t colBg = currBgColor;
  uint64_t val{};

  if(currBgColor != 0)
  {
    for(int i=0; i<9; ++i) {
      buffStart[-1 + (i * (ctx.fb->stride / 8))] = (colBg << 32) | colBg;
    }
  }

  while(*str)
  {
    uint8_t charCode = (uint8_t)*str - ' ';
    uint64_t charData = FONT_8x8_DATA[charCode];
    uint64_t *buff = buffStart;
    ++x;

    if(charCode || colBg)
    {
      for(int y=0; y<8; ++y)
      {
        for(int x=0; x<2; ++x)
        {
          if(colBg || (charData & 0b1111) != 0)
          {
            val  = (charData & 0b0001) ? (col << 32) : (colBg << 32);
            val |= (charData & 0b0010) ? (col <<  0) : (colBg <<  0);
            buff[0] = val;

            val  = (charData & 0b0100) ? (col << 32) : (colBg << 32);
            val |= (charData & 0b1000) ? (col <<  0) : (colBg <<  0);
            buff[1] = val;
          }

          charData >>= 4;
          buff += 2;
        }
        buff += (ctx.fb->stride / 8) - 4;
      }
      // draw extra black line below
      buff[0] = buff[1] = buff[2] = buff[3] = (colBg << 32) | colBg;
      buffStart += 4;
    } else {
      buffStart += spaceSize/2;
    }

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
