/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "assert.h"
#include "../utils/hash.h"
#include <string>
#include <functional>


using TestFunc = std::function<void(Assert&)>;

class TestGroup
{
  private:
    struct TestEntry
    {
      std::string name;
      TestFunc func;
      uint32_t nameHash;
    };

    std::string name{};
    uint32_t nameHash{};
    std::vector<TestEntry> entries{};

  public:
    explicit TestGroup(std::string _name)
      : name{std::move(_name)} {
      nameHash = Hash::crc32(name.c_str());
    }

    [[nodiscard]] const std::string& getName() const {
      return name;
    }

    [[nodiscard]] uint32_t getTestCount() const {
      return entries.size();
    }

    TestGroup& test(const std::string &testName, TestFunc func) {
      entries.push_back({
        testName, std::move(func),
        Hash::crc32(testName.c_str())
      });
      return *this;
    }

    bool run();
};