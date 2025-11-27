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
    surface_t surface{};

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

    void attachAndClear() {
      RDP::DPL{64}
        .add(RDP::attachAndClear(surface))
        .runSync();
    }

    void draw(int x, int y);

    [[nodiscard]] const surface_t& get() const {
      return surface;
    }
};