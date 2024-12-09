/**
 * defines what dist-fs packets will look like
 */
#pragma once

#include <stdint.h>
#include <stddef.h>


#define DIST_FS_START_BYTES 0xDAFF


typedef enum {
  DIST_FS_START_BYTE_SIZE = 2,   // Start bytes are 2 bytes
  DIST_FS_HEADER_SIZE = 5,       // Header: start bytes (2), command (1), size (2)
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
  DIST_FS_PKT_CHECKSUM = 5, // Offset for the checksum byte
} dist_fs_offsets_e;

/* @brief dist-fs packet structure */
typedef struct {
  uint8_t start[DIST_FS_START_BYTE_SIZE]; // Start bytes (fixed at 0 and 1)
  dist_fs_ops_e command; // Command (e.g., DIST_FS_LIST, DIST_FS_UPLOAD)
  uint16_t payload_size; // Size of the payload data
  uint8_t *payload;      // Pointer to the payload data
  uint8_t checksum;      // Checksum byte for error detection
} dist_fs_packet_t;

/**
 * @brief Initializes a dist-fs packet structure.
 *
 * @param packet Pointer to the packet structure to initialize.
 * @param command Command to set in the packet.
 * @param payload Pointer to the payload data.
 * @param payload_size Size of the payload data.
 * @return 0 on success, -1 on failure.
 */
int dist_fs_packet_init(dist_fs_packet_t *packet, dist_fs_ops_e command, const uint8_t *payload, uint16_t payload_size);

/**
 * @brief Serializes a dist-fs packet into a byte buffer.
 *
 * @param packet Pointer to the packet structure to serialize.
 * @param buffer Pointer to the output buffer.
 * @param buffer_size Size of the output buffer.
 * @return Number of bytes written to the buffer, or -1 on failure.
 */
int dist_fs_packet_serialize(const dist_fs_packet_t *packet, uint8_t *buffer, size_t buffer_size);

/**
 * @brief Parses a byte buffer into a dist-fs packet structure.
 *
 * @param buffer Pointer to the input buffer containing the packet data.
 * @param buffer_size Size of the input buffer.
 * @param packet Pointer to the packet structure to populate.
 * @return 0 on success, -1 on failure.
 */
int dist_fs_packet_parse(const uint8_t *buffer, size_t buffer_size, dist_fs_packet_t *packet);

/**
 * @brief Calculates a checksum for the packet data.
 *
 * @param buffer Pointer to the data buffer.
 * @param size Size of the data buffer.
 * @return Calculated checksum.
 */
uint8_t dist_fs_packet_calculate_checksum(const uint8_t *buffer, size_t size);

/**
 * @brief Frees dynamically allocated resources in a dist-fs packet.
 *
 * @param packet Pointer to the packet structure to free.
 */
void dist_fs_packet_free(dist_fs_packet_t *packet);

