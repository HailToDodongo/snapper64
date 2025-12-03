/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>

/**
 * Helper to load and uncompress test files from the "tests.pack" file.
 */
namespace TestPack
{
  void init();

  void* load(uint32_t groupId, uint32_t testId, uint8_t assertId);
}