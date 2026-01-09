#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace parsers {

struct PESection {
    std::string name;
    uint32_t virtualSize = 0;
    uint32_t virtualAddress = 0;
    uint32_t sizeOfRawData = 0;
    uint32_t pointerToRawData = 0;
};

struct PEInfo {
    bool valid = false;
    uint16_t machine = 0;
    uint16_t numberOfSections = 0;
    std::vector<PESection> sections;
};

struct LEFMacro {
    std::string name;
    std::vector<std::string> pins;
};

struct LEFInfo {
    std::vector<std::string> layers;
    std::vector<LEFMacro> macros;
};

} // namespace parsers