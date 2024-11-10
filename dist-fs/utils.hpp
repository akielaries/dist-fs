#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <format>

#define INFO 1
#define WARN 2
#define ERR  3

/** @brief log level macro */
#define LOG(level, fmt, ...)                                         \
  do {                                                               \
    const char* level_str;                                           \
    switch (level) {                                                 \
      case INFO:  level_str = "INFO"; break;                         \
      case WARN:  level_str = "WARN"; break;                         \
      case ERR:   level_str = "ERR"; break;                          \
      default:    level_str = "LOG";                                 \
    }                                                                \
    std::cout << "[" << level_str << "] "                            \
              << std::vformat(fmt, std::make_format_args(__VA_ARGS__)) \
              << "\n";                                               \
  } while (0)


std::string hex_to_ascii(uint32_t hexValue);
