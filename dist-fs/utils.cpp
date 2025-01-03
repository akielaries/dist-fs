/**
 * some miscellaneous utility functions
 */
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <cstdint>

#include <libgen.h>

#include "utils.hpp"


std::string hex_to_ascii(const std::array<char, 16> &header) {
  std::ostringstream oss;
  for (int i = 0; i < 16; ++i) {
    char ch = header[i];
    oss << (std::isprint(static_cast<unsigned char>(ch)) ? ch : '.');
  }
  return oss.str();
}

std::string hex_to_ascii(uint64_t hex_value) {
  bool big_endian = true;
  std::ostringstream oss;
  for (int i = 0; i < 8; ++i) {
    int shift = big_endian ? (7 - i) * 8 : i * 8;
    char ch   = (hex_value >> shift) & 0xFF;

    // Convert non-printable characters to '.'
    oss << (std::isprint(static_cast<unsigned char>(ch)) ? ch : '.');
  }
  return oss.str();
}

std::string hex_to_ascii(uint32_t hex_value) {
  std::ostringstream oss;
  for (int i = sizeof(hex_value) - 1; i >= 0; --i) {
    char ch = (hex_value >> (i * 8)) & 0xFF;
    if (ch) {
      oss << ch;
    }
  }
  return oss.str();
}

const char *log_to_str(log_level_e level) {
  switch (level) {
    case INFO:
      return "INFO";
    case WARN:
      return "WARN";
    case ERR:
      return "ERRO";
    default:
      return "UNKNOWN";
  }
}

void log(log_level_e level,
         const char *file,
         uint16_t line,
         const char *func,
         const char *msg,
         ...) {
  va_list args;
  va_start(args, msg);

  const char *filename = basename(const_cast<char *>(file));

  std::cout << "[" << log_to_str(level) << "] " << std::setw(40) << std::left
            << (std::string(filename) + ":" + std::to_string(line) + " (" +
                func + ")")
            << " - ";

  vfprintf(stdout, msg, args);
  std::cout << std::endl;

  va_end(args);
}

std::string strip_newline(const char *ctime_str) {
  if (ctime_str == nullptr) {
    return "";
  }
  std::string result(ctime_str);
  if (!result.empty() && result.back() == '\n') {
    result.pop_back();
  }
  return result;
}
