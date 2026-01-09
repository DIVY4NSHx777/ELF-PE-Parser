#include "lef_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace parsers;

static inline std::string trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace((unsigned char)s[a])) ++a;
    size_t b = s.size();
    while (b > a && std::isspace((unsigned char)s[b-1])) --b;
    return s.substr(a, b-a);
}

LEFInfo parseLEFFromStream(std::istream& is) {
    LEFInfo info;
    std::string line;
    enum class State { None, InMacro, InPin } state = State::None;
    LEFMacro currentMacro;

    while (std::getline(is, line)) {
        
        auto posc = line.find('#');
        if (posc != std::string::npos) line = line.substr(0, posc);

        
        auto startC = line.find("/*");
        if (startC != std::string::npos) {
            auto endC = line.find("*/", startC+2);
            if (endC != std::string::npos) {
                line.erase(startC, endC - startC + 2);
            } else {
                line = line.substr(0, startC);
            }
        }

        line = trim(line);
        if (line.empty()) continue;

        
        std::istringstream ss(line);
        std::string tok;
        while (ss >> tok) {
        
            if (!tok.empty() && tok.back() == ';') tok.pop_back();
            if (tok.empty()) continue;

            if (state == State::None) {
                if (tok == "LAYER") {
                    std::string layerName;
                    if (ss >> layerName) {
                        if (!layerName.empty() && layerName.back() == ';') layerName.pop_back();
                        info.layers.push_back(layerName);
                    }
                } else if (tok == "MACRO") {
                    std::string macroName;
                    if (ss >> macroName) {
                        state = State::InMacro;
                        currentMacro = LEFMacro{};
                        currentMacro.name = macroName;
                    } else {
                        
                        if (std::getline(is, line)) {
                            std::istringstream ss2(line);
                            ss2 >> macroName;
                            state = State::InMacro;
                            currentMacro = LEFMacro{};
                            currentMacro.name = macroName;
                        }
                    }
                }
            } else if (state == State::InMacro) {
                if (tok == "PIN") {
                    std::string pinName;
                    if (ss >> pinName) {
                        if (!pinName.empty() && pinName.back() == ';') pinName.pop_back();
                        currentMacro.pins.push_back(pinName);
                        state = State::InPin;
                    }
                } else if (tok == "END") {
                    
                    info.macros.push_back(currentMacro);
                    currentMacro = LEFMacro{};
                    state = State::None;
                } else {
                    
                }
            } else if (state == State::InPin) {
                if (tok == "END") {
                    
                    state = State::InMacro;
                } else {
                    
                }
            }
        }
    }

    
    if (state == State::InMacro && !currentMacro.name.empty()) {
        info.macros.push_back(currentMacro);
    }

    
    std::sort(info.layers.begin(), info.layers.end());
    info.layers.erase(std::unique(info.layers.begin(), info.layers.end()), info.layers.end());

    return info;
}

LEFInfo parseLEFFromFile(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs) return {};
    return parseLEFFromStream(ifs);
}