#pragma once

#include <cstdint>

#include "audio_files.hpp"


#define DIST_FS_SSD_PATTERN_SZ 8

typedef struct {
  uint16_t id;               // file ID
  char *name;                // file name
  uint32_t size;             // file size
  dist_fs_file_types_e type; // file type
  uint64_t offset;           // file offset in the drive

} file_info_t;

int upload_file(const char *filename);
int download_file(const char *filename);
int delete_file(const char *filename);
int list_files();
int ssd_echo(const uint8_t *pattern);
