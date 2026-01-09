#pragma once
#include "model.h"
#include <string>
#include <vector>

namespace parsers {

PEInfo parsePEFromFile(const std::string& path);
PEInfo parsePEFromBytes(const std::vector<uint8_t>& bytes);

} 