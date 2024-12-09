/**
 * defines what dist-fs packets will look like
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

#include "comms.h"

#define DIST_FS_START_BYTE_A 0xDA
#define DIST_FS_START_BYTE_B 0xFF


typedef enum {
  DIST_FS_START_BYTE_SIZE = 2, // start bytes are 2 bytes
  DIST_FS_HEADER_SIZE     = 5, // start bytes (2), command (1), payload size (2)
} dist_fs_sizes_e;

/* @brief enumeration of dist-fs operations */
typedef enum {
  DIST_FS_LIST = 0,
  DIST_FS_UPLOAD,
  DIST_FS_DOWNLOAD,
  DIST_FS_DELETE,
} dist_fs_ops_e;

/* @brief packet offsets within the dist-fs packet */
typedef enum {
  DIST_FS_PKT_START_1  = 0, // First start byte
  DIST_FS_PKT_START_2  = 1, // Second start byte
  DIST_FS_PKT_COMMAND  = 2, // Offset for the command byte
  DIST_FS_PKT_SIZE_MSB = 3, // Payload size MSB
  DIST_FS_PKT_SIZE_LSB = 4, // Payload size LSB
  DIST_FS_PKT_PAYLOAD  = 5, // Offset for the payload data
  DIST_FS_PKT_CHECKSUM = 6, // Offset for the checksum byte
} dist_fs_offsets_e;

/* @brief dist-fs packet structure */
typedef struct {
  uint8_t start[DIST_FS_START_BYTE_SIZE]; // Start bytes (fixed at 0 and 1)
  dist_fs_ops_e command; // Command (e.g., DIST_FS_LIST, DIST_FS_UPLOAD)
  uint32_t payload_size; // Size of the payload data
  uint8_t *payload;      // Pointer to the payload data
  uint8_t checksum;      // Checksum byte for error detection
} dist_fs_packet_t;


/* command functions */
int list_files_command(comm_context_t *comm_ctx);
int upload_files_command(comm_context_t *comm_ctx, const char *filename);
/* packet functions */
int test_packet(comm_context_t *comm_ctx,
                uint8_t *payload,
                uint16_t payload_size);
int encode_packet(dist_fs_ops_e command,
                  uint8_t *payload,
                  uint16_t payload_size,
                  uint8_t *buffer);
int decode_packet(comm_context_t *comm_ctx);
