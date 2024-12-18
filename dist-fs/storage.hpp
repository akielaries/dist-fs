#pragma once

#include <cstdint>

#include <vector>

#include "audio_files.hpp"


#define DIST_FS_SSD_PATTERN_SZ 8
// these are the start bytes for each file in the file system
#define DIST_FS_SSD_HEADER 0xDEADBEEF

// the drive!! TODO: make this configurable, from cmd line? from config file?
#define DEVICE_PATH "/dev/disk/by-id/usb-Seagate_Slim_SL_NA710NYN-0:0"

/** @brief SSD metadata table info */
typedef struct {
  char filename[256]; // name
  off_t start_offset; // offset on ssd
  size_t size;        // file size in bytes
  bool is_directory;  // is the file a directory or not
} storage_metadata_t;


/* hard drive operations */
int drive_info();

std::vector<storage_metadata_t> metadata_table_read(int ssd_fd);

/* main SSD operations */
int upload_file(const char *filename);
int download_file(const char *filename);
int delete_file(const char *filename);
int list_files();

/* SSD test functions */
int ssd_echo(const uint8_t *pattern);
int ssd_reset(off_t offset, size_t size);
