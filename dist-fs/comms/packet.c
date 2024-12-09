#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "comms.h"
#include "../utils.hpp"

int list_files_command(comm_context_t *comm_ctx) {
  // no payload, just send the header
  uint8_t buffer[DIST_FS_HEADER_SIZE];
  const uint16_t buffer_size = sizeof(buffer);
  const uint16_t timeout_ms = 1000; // 1-second timeout

  int ret = encode_packet(DIST_FS_LIST, nullptr, 0, buffer);
  if (ret < 0) {
    LOG(ERR, "Failed to form packet\n");
    return ret;
  }

  // send packet over
  ret = comm_ctx->driver->write(comm_ctx, buffer, buffer_size, timeout_ms);
  if (ret == 0) {
    LOG(INFO, "LIST command sent");
  } else {
    LOG(ERR, "Failed to send LIST command: %d", ret);
  }
  return ret;
}

int test_packet(comm_context_t *comm_ctx, uint8_t *payload, uint16_t payload_size) {
  int rc = 0;

  const uint16_t timeout_ms  = 1000; // 1-second timeout for reading
  ssize_t bytes_received     = 0;
  
  // write data from UART
  rc = comm_ctx->driver->write(comm_ctx, payload, payload_size, timeout_ms);
  if (rc == 0) {
    LOG(INFO, "Data sent");
  }
  else if (rc == -1 || rc == -ETIMEDOUT) {
    LOG(WARN, "Read timed out. No data received");
  }
  else {
    LOG(ERR, "Error while reading data: %d", rc);
  }

  return rc;
}


int encode_packet(dist_fs_ops_e command,
                uint8_t *payload,
                uint16_t payload_size,
                uint8_t *buffer) {
  LOG(INFO, "Forming packet for command {%d} with size {%d}", command, payload_size);
  int rc = 0;
  dist_fs_packet_t packet;

  // form the header of the packet
  buffer[DIST_FS_PKT_START_1] = DIST_FS_START_BYTE_A;
  buffer[DIST_FS_PKT_START_2] = DIST_FS_START_BYTE_B;
  // set command of the packet
  buffer[DIST_FS_PKT_COMMAND] = command;
  // set payload size
  buffer[DIST_FS_PKT_SIZE_MSB] = (payload_size >> 8) & 0xFF;
  buffer[DIST_FS_PKT_SIZE_LSB] = payload_size & 0xFF;

  LOG(INFO, "Formed packet header: ");
  for (int i = 0; i < DIST_FS_HEADER_SIZE; i++) {
    printf("  0x%X ", buffer[i]);
  }
  printf("\n");

  // fill in payload data
  if (payload && payload_size > 0) {
    memcpy(buffer + DIST_FS_PKT_PAYLOAD, payload, payload_size);
  }


  // payload is dynamic! allocate some memory for it, be sure to free
/*
  if (payload_size > 0 && payload != NULL) {
    packet.payload = (uint8_t *)malloc(payload_size);
    if (packet.payload == NULL) {
      LOG(ERR, "Failed to allocate memory for {payload}");
      return -1;
    }
    // memcopy the allocated payload and the passed in payload
    memcpy(packet.payload, payload, payload_size);
  } 
  else {
    packet.payload = NULL;
  }
*/
  switch (command) {

    case DIST_FS_LIST:
      LOG(INFO, "Forming packet for DIST_FS_LIST");
      break;

    case DIST_FS_UPLOAD:
      LOG(INFO, "Forming packet for DIST_FS_UPLOAD");
      break;

    case DIST_FS_DOWNLOAD:
      LOG(INFO, "Forming packet for DIST_FS_UPLOAD");
      break;

    case DIST_FS_DELETE:
      LOG(INFO, "Forming packet for DIST_FS_UPLOAD");
      break;

    default:
      LOG(ERR, "Unknown command {%d}", command);
      break;

  }

  return rc;
}

int decode_packet(uint8_t *buffer, size_t buffer_size) {
  // check start bytes
  if (buffer[DIST_FS_PKT_START_1] != DIST_FS_START_BYTE_A ||
      buffer[DIST_FS_PKT_START_2] != DIST_FS_START_BYTE_B) {
    LOG(ERR, "Invalid start bytes\n");
    return -1;
  }

  // get command code
  dist_fs_ops_e command = static_cast<dist_fs_ops_e>(buffer[DIST_FS_PKT_COMMAND]);

  // get payload size
  uint16_t payload_size = (buffer[DIST_FS_PKT_SIZE_MSB] << 8) 
                          | buffer[DIST_FS_PKT_SIZE_LSB];

  // validate payload size
  if (buffer_size != DIST_FS_HEADER_SIZE + payload_size) {
    LOG(ERR, "Payload size mismatch: expected %u but got %zu\n", 
        payload_size, buffer_size - DIST_FS_HEADER_SIZE);
    return -1;
  }

  LOG(INFO, "Decoded packet - Command: %d, Payload Size: %u bytes", command, payload_size);

  uint8_t *payload = nullptr;
  if (payload_size > 0) {
    payload = (uint8_t *)malloc(payload_size);
    if (!payload) {
      LOG(ERR, "Memory allocation failed for payload\n");
      return -1;
    }
    memcpy(payload, buffer + DIST_FS_PKT_PAYLOAD, payload_size);
      
    // handle payload if needed (printing here for example)
    for (size_t i = 0; i < payload_size; i++) {
      printf("Payload Byte %zu: 0x%X\n", i, payload[i]);
    } 
    free(payload);
  }

  return 0;
}

