/**
 * @file storage.hpp
 * @brief Contains definitions and declarations for SSD operations, metadata
 * handling, and file management
 */

#pragma once

#include <cstdint>
#include <vector>

#include "audio_files.hpp"
#include "config.hpp"

#define DIST_FS_SSD_PATTERN_SZ                                                 \
  8 /**< Size of the SSD pattern used for echo testing */
#define DIST_FS_SSD_HEADER_SZ 4 /**< Size of the SSD header */

/**
 * @def DIST_FS_SSD_HEADER
 * @brief Magic number indicating the start bytes for each file in the file
 * system
 */
#define DIST_FS_SSD_HEADER 0xDEADBEEF

/**
 * @def DEVICE_PATH
 * @brief Default path to the SSD device
 * @note This is configurable (via config file)
 */
#define DEVICE_PATH "/dev/disk/by-id/usb-Seagate_Slim_SL_NA710NYN-0:0"


/**
 * @struct storage_metadata_t
 * @brief Structure to hold metadata information for files on the SSD
 */
typedef struct {
  char filename[256];     /**< File name (including directories) */
  off_t start_offset;     /**< Offset on the SSD where the file begins */
  size_t size;            /**< File size in bytes */
  bool is_directory;      /**< Flag indicating if the entry is a directory */
  size_t index;           /**< Index in the metadata table */
  file_times_t file_time; /**< File timestamps */
} storage_metadata_t;

/** @brief Offset where the metadata table begins on the SSD */
constexpr const off_t METADATA_TABLE_OFFSET = 0;

/** @brief Maximum number of files that can be tracked in the metadata table */
constexpr const size_t MAX_FILES = 1024;

/** @brief Total size of the metadata table */
constexpr const size_t METADATA_TABLE_SZ =
  sizeof(storage_metadata_t) * MAX_FILES;

/** @brief Combined size of a file header and SSD header */
constexpr size_t PACKET_METADATA_SIZE =
  sizeof(file_info_t) + DIST_FS_SSD_HEADER_SZ;

/**
 * @brief Provisions the SSD with the initial magic numbers and info
 * @param cfg_ctx Configuration context for the SSD
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int drive_provision(config_context_t cfg_ctx);

/**
 * @brief Displays information about the SSD, such as capacity and current
 * usage
 * @param cfg_ctx Configuration context for the SSD
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int drive_info(config_context_t cfg_ctx);

/**
 * @brief Reads the metadata table from the SSD
 * @param ssd_fd File descriptor for the SSD
 * @return A vector containing the metadata table entries
 */
std::vector<storage_metadata_t> md_table_read(int ssd_fd);

/**
 * @brief Prints the contents of the metadata table to the console
 * @param md_table Reference to the metadata table
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int md_table_print(const std::vector<storage_metadata_t> &md_table);

/**
 * @brief Uploads a file or directory to the SSD
 * @param cfg_ctx Configuration context for the SSD
 * @param filename Path to the file or directory to upload
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int upload_file(config_context_t cfg_ctx, const char *filename);

/**
 * @brief Downloads a file from the SSD to the local filesystem
 * @param cfg_ctx Configuration context for the SSD
 * @param filename Name of the file to download
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int download_file(config_context_t cfg_ctx, const char *filename);

/**
 * @brief Deletes a file or directory from the SSD
 * @param cfg_ctx Configuration context for the SSD
 * @param filename Name of the file or directory to delete
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int delete_file(config_context_t cfg_ctx, const char *filename);

/**
 * @brief Lists all files and directories stored on the SSD
 * @param cfg_ctx Configuration context for the SSD
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int list_files(config_context_t cfg_ctx);

/**
 * @brief Performs an echo test on the SSD by writing and reading a pattern
 * @param pattern Pointer to the pattern data
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int ssd_echo(const uint8_t *pattern);

/**
 * @brief Resets a portion of the SSD by overwriting it with zeros
 * @param offset Starting offset of the region to reset
 * @param size Size of the region to reset, in bytes
 * @return Returns 0 on success, or a non-zero error code on failure
 */
int ssd_reset(off_t offset, size_t size);
