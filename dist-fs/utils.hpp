#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

#define INFO  1
#define WARN  2
#define ERR   3

/** @brief log level macro */
#define LOG(level, fmt, ...) \
  do { \
        const char* level_str; \
        switch (level) { \
            case LVL_INFO:  level_str = "INFO"; break; \
            case LVL_WARN:  level_str = "WARN"; break; \
            case LVL_ERROR: level_str = "ERROR"; break; \
            default: level_str = "LOG"; \
        } \
        auto now = std::chrono::system_clock::now(); \
        std::time_t now_time = std::chrono::system_clock::to_time_t(now); \
        std::tm tm_info = *std::localtime(&now_time); \
        std::ostringstream time_stream; \
        time_stream << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S"); \
        std::cout << "[" << time_stream.str() << "] [" << level_str << "] " << fmt << "\n"; \
    } while (0)


std::string hex_to_ascii(uint32_t hexValue);
