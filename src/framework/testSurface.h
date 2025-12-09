/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>
#include "../renderer/rdp.h"
#include "../renderer/dpl.h"

class TestSurface
{
  private:
    friend struct Assert;

    std::string name{};
    bool lastTestSuccess{};
    surface_t surface{};
    uint32_t errorCount{};

  public:
    TestSurface(tex_format_t format, uint16_t width, uint16_t height) {
      surface = surface_alloc(format, width, height);
    }

    ~TestSurface() {
      surface_free(&surface);
    }

    int getWidth() const {
      return surface.width;
    }
    int getHeight() const {
      return surface.height;
    }

    fm_vec2_t getSize() const {
      return { (float)surface.width, (float)surface.height };
    }
    fm_vec2_t getHalfSize() const {
      return { (float)surface.width / 2, (float)surface.height / 2 };
    }

    int getByteSize() const {
      return surface.stride * surface.height;
    }

    void clear()
    {
      name.clear();
      lastTestSuccess = false;
      memset(surface.buffer, 0, getByteSize());
    }

    void setPixel(uint32_t x, uint32_t y, uint32_t col);

    void setPixel(uint32_t x, uint32_t y, color_t col) {
      setPixel(x, y, color_to_packed32(col));
    }

    void attachAndClear(color_t clearColor = {0,0,0,0});

    void convertToRGBA32(bool withCoverage);

    void draw(int x, int y);

    [[nodiscard]] const surface_t& get() const {
      return surface;
    }
};