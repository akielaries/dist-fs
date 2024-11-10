#include <string>
#include <iostream>
#include <sstream>

#include <cstdint>

#include "utils.hpp"

std::string hex_to_ascii(uint32_t hexValue) {
  std::ostringstream oss;
  for (int i = sizeof(hexValue) - 1; i >= 0; --i) {
    char ch = (hexValue >> (i * 8)) & 0xFF;
    if (ch) {
      oss << ch;
    }
  }
  return oss.str();
}

