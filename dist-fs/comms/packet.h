/**
 * defines what dist-fs packets will look like
 */
#pragma once

#define DIST_FS_START_BYTES 0xDAFF


typedef enum {
  DIST_FS_START_BYTE_SIZE = 4,
} dist_fs_sizes_e;

/* @brief enumeration of dist-fs operations */
typedef enum {
  DIST_FS_LIST = 0,
  DIST_FS_UPLOAD,
  DIST_FS_DOWNLOAD,
  DIST_FS_DELETE,
} dist_fs_ops_e;

/* @brief packet offsets within the dist-fs packet */
enum {
  DIST_FS_PKT_START_1  = 0, // First start byte
  DIST_FS_PKT_START_2  = 1, // Second start byte
  DIST_FS_PKT_COMMAND  = 2, // Offset for the command byte
  DIST_FS_PKT_SIZE     = 3, // Offset for the payload size (2 bytes)
  DIST_FS_PKT_PAYLOAD  = 5, // Offset for the payload data
  DIST_FS_PKT_CHECKSUM = 5, // Offset for the checksum byte
};

/* @brief dist-fs packet structure */
typedef struct {
  uint8_t start[DIST_FS_START_BYTE_SIZE]; // Start bytes (fixed at 0 and 1)
  dist_fs_ops_e command; // Command (e.g., DIST_FS_LIST, DIST_FS_UPLOAD)
  uint16_t payload_size; // Size of the payload data
  uint8_t *payload;      // Pointer to the payload data
  uint8_t checksum;      // Checksum byte for error detection
} dist_fs_packet_t;
