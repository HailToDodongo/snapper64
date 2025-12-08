/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "testPack.h"
#include <libdragon.h>

namespace
{
  constexpr uint32_t ODD_SIZE_FLAG = (1 << 31);
  constinit FILE* f{};

  struct PackEntry
  {
    uint32_t idGroup{};
    uint32_t idTest{};
    uint32_t offset{};

    uint64_t getId() const {
      return ((uint64_t)idGroup << 32) | (uint64_t)idTest;
    }
  };

  struct PackIndex
  {
    uint32_t fileCount{};
    PackEntry entries[];
  };

  PackIndex *table{};
}

void TestPack::init()
{
  table = (PackIndex*)asset_load("rom:/tests.pack.idx", nullptr);
  f = fopen("rom:/tests.pack", "rb");
  // debugf("Header: %.4s, fileCount=%u\n", header.magic, header.fileCount);
}

void* TestPack::load(uint32_t groupId, uint32_t testId, uint8_t assertId)
{
  //uint64_t t = get_ticks();
  uint64_t id = ((uint64_t)groupId << 32) | (uint64_t)testId;

  int left = 0;
  int right = table->fileCount - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (table->entries[mid].getId() == id) {
      // get to first entry with the same ID
      while (mid > 0 && table->entries[mid-1].getId() == id) {
        --mid;
      }

      // assertID is an index, there are N amount of the same ID entries
      mid += assertId-1;
      if(table->entries[mid].getId() != id)break;

      uint32_t offset = table->entries[mid].offset & ~ODD_SIZE_FLAG;
      uint32_t nextOffset = table->entries[mid+1].offset & ~ODD_SIZE_FLAG;

      fseek(f, offset, SEEK_SET);

      int size = nextOffset - offset;
      if(table->entries[mid].offset & ODD_SIZE_FLAG) {
        size -= 1;
      }

      //t = get_ticks() - t;
      /*debugf("Loading pack entry ID=%016llX, offset=%u, size=%d | %u us\n",
        packTable[mid].id,
        packTable[mid].offset,
        packTable[mid].size,
        0 //TICKS_TO_US(t)
      );*/

      return asset_loadf(f, &size);
    }
    if (table->entries[mid].getId() < id) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  debugf("Pack entry not found: ID=%016llX_%02X\n", id, assertId);
  return nullptr;
}
