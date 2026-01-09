#include "pe_parser.h"
#include "lef_parser.h"
#include <iostream>
#include <filesystem>

using namespace parsers;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: lef_pe_cli <file>\n";
        return 2;
    }
    std::string path = argv[1];
    if (!std::filesystem::exists(path)) {
        std::cerr << "File not found: " << path << "\n";
        return 3;
    }

    // Decide by extension: .lef -> lef, else try to parse as PE
    std::filesystem::path p(path);
    auto ext = p.extension().string();

    if (ext == ".lef" || ext == ".LEF") {
        auto info = parseLEFFromFile(path);
        std::cout << "LEF parsed: layers=" << info.layers.size() << " macros=" << info.macros.size() << "\n";
        if (!info.layers.empty()) {
            std::cout << "Layers:\n";
            for (auto &l : info.layers) std::cout << "  " << l << "\n";
        }
        if (!info.macros.empty()) {
            std::cout << "Macros:\n";
            for (auto &m : info.macros) {
                std::cout << "  " << m.name << " pins=" << m.pins.size() << "\n";
                for (auto &pn : m.pins) std::cout << "    " << pn << "\n";
            }
        }
        return 0;
    } else {
        // Try PE
        auto info = parsePEFromFile(path);
        if (!info.valid) {
            std::cout << "Not a valid PE or unsupported format.\n";
            return 4;
        }
        std::cout << "PE parsed: machine=0x" << std::hex << info.machine << std::dec
                  << " sections=" << info.numberOfSections << "\n";
        for (auto &s : info.sections) {
            std::cout << "  [" << s.name << "] virtSize=" << s.virtualSize
                      << " virtAddr=0x" << std::hex << s.virtualAddress << std::dec
                      << " rawSize=" << s.sizeOfRawData << " rawPtr=0x" << std::hex << s.pointerToRawData << std::dec << "\n";
        }
        return 0;
    }
}