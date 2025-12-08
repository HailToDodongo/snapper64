#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <bit>

namespace fs = std::filesystem;

struct FileEntry {
  uint32_t idGroup;
  uint32_t idTest;
  uint8_t idAssert;
  std::string path;
  uint32_t size;
};

struct TableEntry {
 uint32_t offset;
 uint32_t size;
 uint32_t idGroup;
 uint32_t idTest;
 uint8_t idAssert;
};

uint32_t align16(uint32_t offset) {
  return (offset + 15) & ~0xF;
}

uint32_t align2(uint32_t offset) {
  return (offset + 1) & ~0x1;
}

bool parseFileName(const std::string& filename, FileEntry& entry) {
  size_t pos1 = filename.find('_');
  size_t pos2 = filename.find('_', pos1 + 1);
  size_t pos3 = filename.find(".test");
  if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) return false;

  entry.idGroup = std::stoul(filename.substr(0, pos1), nullptr, 16);
  entry.idTest = std::stoul(filename.substr(pos1 + 1, pos2 - pos1 - 1), nullptr, 16);
  entry.idAssert = static_cast<uint8_t>(std::stoul(filename.substr(pos2 + 1, pos3 - pos2 - 1), nullptr, 16));

  return true;
}

int main(int argc, char* argv[])
{
  if (argc < 3) {
    std::cerr << "Usage: data_bundler <input_dir> <output_file>\n";
    return 1;
  }

  std::string inDir = argv[1];
  std::string outFile = argv[2];
  std::string outFileIdx = outFile + ".idx";

  std::vector<FileEntry> files;
  for (const auto& entry : fs::directory_iterator(inDir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".test") {
      FileEntry fe{};
      if (parseFileName(entry.path().filename().string(), fe)) {
        fe.path = entry.path().string();
        fe.size = fs::file_size(entry.path());
        files.push_back(fe);
      }
    }
  }

  std::ranges::sort(files, [](const FileEntry& a, const FileEntry& b) {
    if (a.idGroup != b.idGroup) return a.idGroup < b.idGroup;
    if (a.idTest != b.idTest) return a.idTest < b.idTest;
    return a.idAssert < b.idAssert;
  });

  // end marker dummy file
  files.push_back({0xFFFFFFFF, 0xFFFFFFFF, 0, "", 2});

  std::vector<TableEntry> table{};
  uint32_t dataOffset{0};

  for (const auto& fe : files) {
    table.push_back({dataOffset, fe.size, fe.idGroup, fe.idTest, fe.idAssert});
    dataOffset = align2(dataOffset + fe.size);
  }

  std::ofstream outPack{outFile, std::ios::binary};
  std::ofstream outIdx{outFileIdx, std::ios::binary};


  auto writeU32 = [&](uint32_t val) {
    uint32_t be = std::byteswap(val);
    outIdx.write((const char*)(&be), sizeof(be));
  };

  writeU32(files.size());
  for (const auto& te : table) {
    writeU32(te.idGroup);
    writeU32(te.idTest);

    uint32_t offset = te.offset;
    if(te.size % 2 != 0) {
      offset |= 1 << 31;
    }
    writeU32(offset);
  }
  outIdx.close();

  for (size_t i = 0; i < files.size(); ++i) {
    outPack.seekp(table[i].offset, std::ios::beg);
    std::vector<char> buffer(files[i].size);

    if(!files[i].path.empty())
    {
      std::ifstream in(files[i].path, std::ios::binary);
      in.read(buffer.data(), files[i].size);
    } else
    {
      buffer.push_back(0);
      buffer.push_back(0);
    }
    outPack.write(buffer.data(), files[i].size);
    uint64_t pad = align16(files[i].size) - files[i].size;
    if (pad) {
      std::vector<char> padding(pad, 0);
      outPack.write(padding.data(), pad);
    }
  }

  outPack.close();
  return 0;
}