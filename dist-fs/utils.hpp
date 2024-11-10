#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <format>
#include <cstdarg>

enum log_level_e {
  INFO,
  WARN,
  ERR,
};

void log(log_level_e level, const char* file, uint16_t line, const char* msg, ...);

/** @brief log level macro */
#define LOG(level, msg, ...) log(level, __FILE__, __LINE__, msg, ##__VA_ARGS__)

std::string hex_to_ascii(uint32_t hexValue);
