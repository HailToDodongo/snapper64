/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "assert.h"

#include "testPack.h"
#include "../renderer/vi.h"

namespace
{
  constexpr char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
  constexpr int mod_table[] = {0, 2, 1};

  char* base64Encode(char* out, const uint8_t* data, size_t inputLength) {
    size_t outputLength = 4 * ((inputLength + 2) / 3);
    for (size_t i = 0, j = 0; i < inputLength;) {
      uint32_t octet_a = i < inputLength ? data[i++] : 0;
      uint32_t octet_b = i < inputLength ? data[i++] : 0;
      uint32_t octet_c = i < inputLength ? data[i++] : 0;

      uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

      out[j++] = encoding_table[(triple >> 18) & 0x3F];
      out[j++] = encoding_table[(triple >> 12) & 0x3F];
      out[j++] = encoding_table[(triple >> 6) & 0x3F];
      out[j++] = encoding_table[triple & 0x3F];
    }

    for (int i = 0; i < mod_table[inputLength % 3]; i++)
      out[outputLength - 1 - i] = '=';

    out[outputLength] = '\n';
    out[outputLength+1] = '\0';

    return out;
  }

  /*void hexByte(char* out, uint8_t value) {
    out[0] = "0123456789ABCDEF"[value >> 4];
    out[1] = "0123456789ABCDEF"[value & 0xF];
  }*/
}

Assert& Assert::surface(TestSurface &surf, bool hiddenBits)
{
  [[maybe_unused]] auto t = get_ticks();
  ++hashAssert;

  std::string fileName{};
  fileName.resize(32);
  int size = sprintf(fileName.data(), "%08lX_%08lX_%02lX.test", hashGroup, hashTest, hashAssert);
  fileName.resize(size);
  auto romPath = "rom:/" + fileName;

  surf.name = fileName.substr(0, fileName.size() - 5); // remove .test
  surf.errorCount = 0;

  if(ctx.dumpData)
  {
    if(ctx.useSdCard)
    {
      std::string sdPath = "sd:/tests/" + fileName;
      debugf("[DEBUG] Dumping test surface to %s\n", sdPath.c_str());
      FILE *f = fopen(sdPath.c_str(), "wb");
      if(!f) {
        debugf("[DEBUG] Failed to open file for writing: %s\n", sdPath.c_str());
        result(false);
        surf.lastTestSuccess = false;
        return *this;
      }

      VI::keepAlive();

      fwrite(surf.get().buffer, 1, surf.getByteSize(), f);
      fclose(f);

      VI::keepAlive();
    } else
    {
      debugf("$FILE=%s\n", fileName.c_str());
      uint8_t *buf = (uint8_t*)surf.get().buffer;
      uint32_t byteSize = surf.get().stride * surf.get().height;

      std::vector<char> hexBuf{};
      hexBuf.resize(byteSize * 2 + 2);

      VI::keepAlive();
      base64Encode(hexBuf.data(), buf, byteSize);
      VI::keepAlive();

      fputs("$DATA=", stderr);
      fputs(hexBuf.data(), stderr);
    }

    result(false); // avoid false-positives in dumping mode
    surf.lastTestSuccess = false;
  } else
  {
    VI::keepAlive();
    auto t2 = get_ticks();
    //auto refData = (uint64_t*)asset_load(romPath.c_str(), &refDataSize);
    auto refData = (uint64_t*)TestPack::load(hashGroup, hashTest, hashAssert);
    t2 = get_ticks() - t2;
    VI::keepAlive();

    if(!refData) {
      debugf("[DEBUG] Test file not found: %s\n", fileName.c_str());
      result(false);
      surf.lastTestSuccess = false;
      return *this;
    }

    auto t3 = get_ticks();
    bool success = true;

    if(ctx.diffMode == DIFF_MODE::DIFF_COLOR || ctx.diffMode == DIFF_MODE::DIFF_CVG)
    {
      auto buf = (uint32_t*)surf.get().buffer;
      auto buffSteps = surf.get().stride * surf.get().height / 4;

      for(uint32_t i=0; i < (uint32_t)buffSteps; ++i)
      {
        uint32_t refVal = ((uint32_t*)refData)[i];
        if(buf[i] != refVal)
        {
          success = false;
          ++surf.errorCount;

          uint32_t col = refVal == 0 ? 0xFFFFFF00 : 0xFF000000;
          if(ctx.diffMode == DIFF_MODE::DIFF_CVG && (buf[i] & 0xFF) != (refVal & 0xFF)) {
            col = 0xFFFFFFFF;
          }
          buf[i] = col;
        } else {
          buf[i] = 0;
        }
      }
    } else {
      auto buf = (uint64_t*)surf.get().buffer;
      auto buffSteps = surf.get().stride * surf.get().height / 8;

      for(uint32_t i=0; i < (uint32_t)buffSteps; ++i)
      {
        if(buf[i] != refData[i])success = false;
      }
    }

    if(ctx.diffMode == DIFF_MODE::EXPECTED_COLOR || ctx.diffMode == DIFF_MODE::EXPECTED_CVG) {
      memcpy(surf.get().buffer, refData, surf.getByteSize());
    }

    t3 = get_ticks() - t3;

    free(refData);

    surf.lastTestSuccess = success;
    result(success);

    if(!success) {
      debugf("[Debug] Surface-Test %s: FAILED!\n", fileName.c_str());
    }
/*
    t = get_ticks() - t;
    debugf("[DEBUG] assert time: %lu us\n", TICKS_TO_US(t));
    debugf("[DEBUG]  - asset_load time: %lu us\n", TICKS_TO_US(t2));
    debugf("[DEBUG]  - memcmp time: %lu us\n", TICKS_TO_US(t3));
    */
  }

  return *this;
}
