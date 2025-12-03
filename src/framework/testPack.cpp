/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testPack.h"
#include <libdragon.h>

namespace
{
  constinit FILE* f{};

  struct PackHeader
  {
    char magic[4]; // "PACK"
    uint32_t fileCount;
  };

  struct PackEntry
  {
    uint64_t id{};
    uint32_t offset{};
    int32_t size{};
  };

  constinit PackHeader header{};
  PackEntry *packTable{};
}

void TestPack::init()
{
  f = fopen("rom:/tests.pack", "rb");
  fread(&header, sizeof(PackHeader), 1, f);
  packTable = new PackEntry[header.fileCount];
  fread(packTable, sizeof(PackEntry), header.fileCount, f);

  // debugf("Header: %.4s, fileCount=%u\n", header.magic, header.fileCount);
}

void* TestPack::load(uint32_t groupId, uint32_t testId, uint8_t assertId)
{
  //uint64_t t = get_ticks();
  uint64_t id = ((uint64_t)groupId << 32) | (uint64_t)testId;

  int left = 0;
  int right = header.fileCount - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (packTable[mid].id == id) {
      // get to first entry with the same ID
      while (mid > 0 && packTable[mid-1].id == id) {
        --mid;
      }

      // assertID is an index, there are N amount of the same ID entries
      mid += assertId-1;
      if(packTable[mid].id != id)break;

      fseek(f, packTable[mid].offset, SEEK_SET);

      //t = get_ticks() - t;
      /*debugf("Loading pack entry ID=%016llX, offset=%u, size=%d | %u us\n",
        packTable[mid].id,
        packTable[mid].offset,
        packTable[mid].size,
        0 //TICKS_TO_US(t)
      );*/

      int size = packTable[mid].size;
      return asset_loadf(f, &size);
    }
    if (packTable[mid].id < id) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  debugf("Pack entry not found: ID=%016llX_%02X\n", id, assertId);
  return nullptr;
}
