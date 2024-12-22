#pragma once

#include <cstdint>
#include <vector>

#include "audio_files.hpp"

#include "config.hpp"


#define DIST_FS_SSD_PATTERN_SZ 8
#define DIST_FS_SSD_HEADER_SZ  4
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


// metadata table starts at the very beginning of the drive
constexpr const off_t METADATA_TABLE_OFFSET = 0;
// max files I want to track for now...
constexpr const size_t MAX_FILES = 1024;
// total size of the metadata table
constexpr const size_t METADATA_TABLE_SZ =
  sizeof(storage_metadata_t) * MAX_FILES;

constexpr size_t PACKET_METADATA_SIZE =
  sizeof(file_info_t) + DIST_FS_SSD_HEADER_SZ;

/* hard drive operations */
int drive_provision(config_context_t cfg_ctx);
int drive_info();

std::vector<storage_metadata_t> metadata_table_read(int ssd_fd);

int metadata_table_print(const std::vector<storage_metadata_t> &metadata_table);

/* main SSD operations */
int upload_file(config_context_t cfg_ctx, const char *filename);
int download_file(config_context_t cfg_ctx, const char *filename);
int delete_file(config_context_t cfg_ctx, const char *filename);
int list_files(config_context_t cfg_ctx);

/* SSD test functions */
int ssd_echo(const uint8_t *pattern);
int ssd_reset(off_t offset, size_t size);
