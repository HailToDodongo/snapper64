/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "assert.h"
#include "../utils/hash.h"
#include <string>
#include <functional>
#include <ranges>


using TestFunc = std::function<void(Assert&)>;

template <typename... R>
  constexpr auto TestMatrix(R... range)
{
  return std::views::cartesian_product(
      std::views::iota(0, range)...);
}

class TestGroup
{
  private:
    struct TestEntry
    {
      std::string name;
      TestFunc func;
      uint32_t nameHash;

      uint32_t errorCount{0};
    };

    std::string name{};
    uint32_t nameHash{};
    std::vector<TestEntry> entries{};

    uint32_t countSuccess{0};
    uint32_t countTested{0};

  public:
    explicit TestGroup(std::string _name)
      : name{std::move(_name)} {
      nameHash = Hash::crc32(name.c_str());
    }

    [[nodiscard]] const std::string& getName() const {
      return name;
    }

    [[nodiscard]] const std::vector<TestEntry>& getTests() const {
      return entries;
    }

    [[nodiscard]] uint32_t getTestCount() const {
      return entries.size();
    }

    [[nodiscard]] uint32_t getCountSuccess() const { return countSuccess; }
    [[nodiscard]] uint32_t getCountTested() const { return countTested; }

    TestGroup& test(const std::string &testName, TestFunc func) {
      entries.push_back({
        testName, std::move(func),
        Hash::crc32(testName.c_str())
      });
      return *this;
    }

    bool run();
};