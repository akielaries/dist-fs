#pragma once

#include <cstdint>

#include "audio_files.hpp"


#define DIST_FS_SSD_PATTERN_SZ 8
// these are the start bytes for each file in the file system
#define DIST_FS_SSD_HEADER 0xDEADBEEF

int upload_file(const char *filename);
int download_file(const char *filename);
int delete_file(const char *filename);
int list_files();
int ssd_echo(const uint8_t *pattern);
