#include "pe_parser.h"
#include <fstream>
#include <cstring>

using namespace parsers;

static uint16_t read_u16(const std::vector<uint8_t>& b, size_t off) {
    if (off + 2 > b.size()) return 0;
    return uint16_t(b[off]) | (uint16_t(b[off+1]) << 8);
}
static uint32_t read_u32(const std::vector<uint8_t>& b, size_t off) {
    if (off + 4 > b.size()) return 0;
    return uint32_t(b[off]) | (uint32_t(b[off+1]) << 8) | (uint32_t(b[off+2]) << 16) | (uint32_t(b[off+3]) << 24);
}

PEInfo parsePEFromBytes(const std::vector<uint8_t>& bytes) {
    PEInfo info;
    const auto& b = bytes;
    if (b.size() < 64) return info; 

    
    if (b[0] != 'M' || b[1] != 'Z') return info;

    
    uint32_t e_lfanew = read_u32(b, 0x3C);
    if (e_lfanew + 4 > b.size()) return info;

    // Check PE signature
    if (!(b[e_lfanew] == 'P' && b[e_lfanew+1] == 'E' && b[e_lfanew+2] == 0 && b[e_lfanew+3] == 0))
        return info;

    size_t coff_off = e_lfanew + 4;
    if (coff_off + 20 > b.size()) return info;

    uint16_t machine = read_u16(b, coff_off + 0);
    uint16_t num_sections = read_u16(b, coff_off + 2);
    uint16_t size_of_opt_hdr = read_u16(b, coff_off + 16);

    size_t section_table_off = coff_off + 20 + size_of_opt_hdr;
    size_t section_size = 40;
    if (section_table_off + section_size * num_sections > b.size()) {
        
        if (section_table_off > b.size()) return info;
    }

    info.valid = true;
    info.machine = machine;
    info.numberOfSections = num_sections;

    for (uint16_t i = 0; i < num_sections; ++i) {
        size_t off = section_table_off + i * section_size;
        if (off + section_size > b.size()) break;

        std::string name;
        for (size_t n = 0; n < 8; ++n) {
            char c = static_cast<char>(b[off + n]);
            if (c == '\0') break;
            name.push_back(c);
        }
        uint32_t virtualSize = read_u32(b, off + 8);
        uint32_t virtualAddress = read_u32(b, off + 12);
        uint32_t sizeOfRawData = read_u32(b, off + 16);
        uint32_t pointerToRawData = read_u32(b, off + 20);

        PESection s;
        s.name = name;
        s.virtualSize = virtualSize;
        s.virtualAddress = virtualAddress;
        s.sizeOfRawData = sizeOfRawData;
        s.pointerToRawData = pointerToRawData;
        info.sections.push_back(std::move(s));
    }

    return info;
}

PEInfo parsePEFromFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    std::vector<uint8_t> buf;
    if (!ifs) return {};
    ifs.seekg(0, std::ios::end);
    std::streampos sz = ifs.tellg();
    if (sz <= 0) return {};
    buf.resize((size_t)sz);
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
    return parsePEFromBytes(buf);
}