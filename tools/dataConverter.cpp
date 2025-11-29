#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <unordered_map>

const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::vector<unsigned char> base64_decode(const std::string& encoded) {
    std::vector<unsigned char> out;
    std::unordered_map<char, int> T;
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : encoded) {
        if (T.find(c) == T.end()) {
            if (c == '=') break;
            continue;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <testCasePath> <outDir>\n";
        return 1;
    }

    std::string testCasePath = argv[1];
    std::string outDir = argv[2];

    std::ifstream infile(testCasePath);
    if (!infile) {
        std::cerr << "Failed to open input file\n";
        return 1;
    }

    std::string line, testName;
    while (std::getline(infile, line))
    {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;

        if (line.rfind("$FILE=", 0) == 0) {
            testName = line.substr(6);
            testName.erase(0, testName.find_first_not_of(" \t\r\n"));
            testName.erase(testName.find_last_not_of(" \t\r\n") + 1);
        } else if (line.rfind("$DATA=", 0) == 0 && !testName.empty()) {
            std::string b64 = line.substr(6);
            b64.erase(0, b64.find_first_not_of(" \t\r\n"));
            b64.erase(b64.find_last_not_of(" \t\r\n") + 1);

            std::vector<unsigned char> data = base64_decode(b64);
            std::string outPath = outDir + "/" + testName;
            std::ofstream outfile(outPath, std::ios::binary);
            if (outfile) {
                outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
                std::cout << "Wrote " << data.size() << " bytes to " << outPath << "\n";
            }
            testName.clear();
        }
    }
    return 0;
}
