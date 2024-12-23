#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <format>
#include <cstdarg>
#include <vector>

enum log_level_e {
  INFO,
  WARN,
  ERR,
};

void log(log_level_e level,
         const char *file,
         uint16_t line,
         const char *func,
         const char *msg,
         ...);

/** @brief log level macro */
#define LOG(level, msg, ...)                                                   \
  log(level, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)


std::string hex_to_ascii(const std::array<char, 16> &header);
std::string hex_to_ascii(uint64_t hexValue);
std::string hex_to_ascii(uint32_t hexValue);

std::string strip_newline(const char *ctime_str);
