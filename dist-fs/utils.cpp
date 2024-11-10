#include <string>
#include <iostream>
#include <sstream>

#include <cstdint>

#include <libgen.h>

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

const char* log_to_str(log_level_e level) {
  switch (level) {
    case INFO:
      return "INFO";
    case WARN:
      return "WARNING";
    case ERR:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

void log(log_level_e level, const char* file, uint16_t line , const char* msg, ...) {
  va_list args;
  va_start(args, msg);

  // just the file name, not full path...
  const char* filename = basename(const_cast<char*>(file));
    
    std::cout << "[" << log_to_str(level) << "] "
              << filename << ":" << line << " - ";

  vfprintf(stdout, msg, args);
  std::cout << std::endl;

  va_end(args);
}
