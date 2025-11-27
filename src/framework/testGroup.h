/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "assert.h"
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
    };

    std::string name{};
    std::vector<TestEntry> entries{};

  public:
    explicit TestGroup(std::string _name)
      : name{std::move(_name)}
    {}

    [[nodiscard]] const std::string& getName() const {
      return name;
    }

    [[nodiscard]] uint32_t getTestCount() const {
      return entries.size();
    }

    TestGroup& test(const std::string &testName, TestFunc func) {
      entries.push_back(TestEntry{testName, std::move(func)});
      return *this;
    }

    bool run();
};