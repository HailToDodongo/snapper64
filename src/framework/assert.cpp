/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "assert.h"

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

  void hexByte(char* out, uint8_t value) {
    out[0] = "0123456789ABCDEF"[value >> 4];
    out[1] = "0123456789ABCDEF"[value & 0xF];
  }
}

Assert & Assert::surface(TestSurface &surf, bool hiddenBits)
{
  ++hashAssert;

  std::string fileName{};
  fileName.resize(32);
  int size = sprintf(fileName.data(), "%08lX_%08lX_%02lX.test", hashGroup, hashTest, hashAssert);
  fileName.resize(size);
  auto romPath = "rom:/" + fileName;

  surf.name = fileName.substr(0, fileName.size() - 5); // remove .test

  if(ctx.dumpData)
  {
    debugf("$FILE=%s\n", fileName.c_str());
    uint8_t *buf = (uint8_t*)surf.get().buffer;
    uint32_t byteSize = surf.get().stride * surf.get().height;

    std::vector<char> hexBuf{};
    hexBuf.resize(byteSize * 2 + 2);
    base64Encode(hexBuf.data(), buf, byteSize);

    fputs("$DATA=", stderr);
    fputs(hexBuf.data(), stderr);

    result(false); // avoid false-positives in dumping mode
    surf.lastTestSuccess = false;
  } else
  {
    int refDataSize{};

    if(dfs_rom_addr(fileName.c_str()) == 0) {
      debugf("[DEBUG] Test file not found: %s\n", fileName.c_str());
      result(false);
      surf.lastTestSuccess = false;
      return *this;
    }

    //dfs_open() @TODO: do load into to avoid "rom:/" prefix
    auto refData = (uint8_t*)asset_load(romPath.c_str(), &refDataSize);

    /*assertf(refDataSize == surf.getByteSize(), "Incorrect reference data size: %d != %d",
      refDataSize, surf.getByteSize());
*/
    auto buf = (uint8_t*)surf.get().buffer;
    bool success = memcmp(buf, refData, surf.get().stride * surf.get().height) == 0;

    free(refData);

    surf.lastTestSuccess = success;
    result(success);
    debugf("[Debug] Surface-Test %s: %s\n", fileName.c_str(), success ? "Pass" : "FAIL!");
  }

  return *this;
}
