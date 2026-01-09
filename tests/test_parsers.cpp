#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "lef_parser.h"
#include "pe_parser.h"
#include <sstream>

using namespace parsers;

TEST_CASE("LEF: parse layers and macros", "[lef]") {
    std::string lef = R"(
        VERSION 5.7 ;
        LAYER Metal1 ;
        LAYER Poly ;
        MACRO INVX1
          CLASS CORE ;
          PIN A ;
            DIRECTION INPUT ;
          END A
          PIN Z ;
            DIRECTION OUTPUT ;
          END Z
        END INVX1
        MACRO NAND2X1
          PIN A ;
          END A
        END NAND2X1
    )";
    std::istringstream ss(lef);
    LEFInfo info = parseLEFFromStream(ss);
    REQUIRE(info.layers.size() == 2);
    REQUIRE((info.layers[0] == "Metal1" || info.layers[1] == "Metal1"));
    REQUIRE(info.macros.size() == 2);
    bool foundInv = false;
    bool foundNand = false;
    for (auto &m : info.macros) {
        if (m.name == "INVX1") { foundInv = true; REQUIRE(m.pins.size() == 2); }
        if (m.name == "NAND2X1") { foundNand = true; REQUIRE(m.pins.size() == 1); }
    }
    REQUIRE(foundInv);
    REQUIRE(foundNand);
}

static std::vector<uint8_t> buildMinimalPE() {
    // build a minimal valid PE in memory (not a runnable exe, but headers are present)
    std::vector<uint8_t> b(512, 0);

    // DOS header "MZ"
    b[0] = 'M';
    b[1] = 'Z';
    // e_lfanew at 0x3C -> place NT header at offset 0x80
    uint32_t e_lfanew = 0x80;
    b[0x3C] = uint8_t(e_lfanew & 0xFF);
    b[0x3D] = uint8_t((e_lfanew >> 8) & 0xFF);
    b[0x3E] = uint8_t((e_lfanew >> 16) & 0xFF);
    b[0x3F] = uint8_t((e_lfanew >> 24) & 0xFF);

    // PE signature
    b[e_lfanew + 0] = 'P';
    b[e_lfanew + 1] = 'E';
    b[e_lfanew + 2] = 0;
    b[e_lfanew + 3] = 0;

    size_t coff_off = e_lfanew + 4;
    // Machine = 0x14c (Intel 386)
    b[coff_off + 0] = 0x4c;
    b[coff_off + 1] = 0x01;
    // NumberOfSections = 2
    b[coff_off + 2] = 2;
    b[coff_off + 3] = 0;
    // SizeOfOptionalHeader = 224 (0xE0)
    b[coff_off + 16] = 0xE0;
    b[coff_off + 17] = 0x00;

    // Section table after coff_off + 20 + sizeOfOptionalHeader
    size_t section_table = coff_off + 20 + 224;
    // first section name ".text" in 8 bytes
    const char* name1 = ".text";
    for (size_t i = 0; name1[i]; ++i) b[section_table + i] = name1[i];
    // virtualSize at offset 8
    b[section_table + 8] = 0x00;
    b[section_table + 9] = 0x10;
    b[section_table + 10] = 0x00;
    b[section_table + 11] = 0x00;
    // VirtualAddress at offset 12
    b[section_table + 12] = 0x00;
    b[section_table + 13] = 0x10;
    // SizeOfRawData at 16
    b[section_table + 16] = 0x00;
    b[section_table + 17] = 0x20;
    // pointerToRawData at 20
    b[section_table + 20] = 0x00;
    b[section_table + 21] = 0x04;

    // second section ".data"
    size_t sec2 = section_table + 40;
    const char* name2 = ".data";
    for (size_t i = 0; name2[i]; ++i) b[sec2 + i] = name2[i];
    b[sec2 + 8] = 0x00;
    b[sec2 + 9] = 0x08;
    b[sec2 + 12] = 0x20;
    b[sec2 + 16] = 0x00;
    b[sec2 + 17] = 0x10;
    b[sec2 + 20] = 0x24;

    return b;
}

TEST_CASE("PE: parse minimal generated headers", "[pe]") {
    auto bytes = buildMinimalPE();
    PEInfo info = parsePEFromBytes(bytes);
    REQUIRE(info.valid);
    REQUIRE(info.machine == 0x014c);
    REQUIRE(info.numberOfSections == 2);
    REQUIRE(info.sections.size() == 2);
    REQUIRE(info.sections[0].name == ".text");
    REQUIRE(info.sections[1].name == ".data");
}