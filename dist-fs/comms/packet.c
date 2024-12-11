#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "packet.h"
#include "comms.h"
#include "../utils.hpp"


int test_packet(comm_context_t *comm_ctx,
                uint8_t *payload,
                uint16_t payload_size) {
  int rc = 0;

  const uint16_t timeout_ms = 1000; // 1-second timeout for reading

  // write data from UART
  LOG(INFO, "Writing packet");
  rc = comm_ctx->driver->write(comm_ctx, payload, payload_size, timeout_ms);
  if (rc == 0) {
    LOG(INFO, "Data sent");
  } else if (rc == -1 || rc == -ETIMEDOUT) {
    LOG(WARN, "Read timed out. No data received");
  } else {
    LOG(ERR, "Error while reading data: %d", rc);
  }

  return rc;
}

/* packetize operation commands */

int list_files_command(comm_context_t *comm_ctx) {
  // no payload, just send the header
  uint8_t buffer[DIST_FS_HEADER_SIZE];
  const uint16_t buffer_size = sizeof(buffer);
  const uint16_t timeout_ms  = 1000; // 1-second timeout

  int ret = encode_packet(DIST_FS_LIST, nullptr, 0, buffer);
  if (ret < 0) {
    LOG(ERR, "Failed to form packet\n");
    return ret;
  }

  // send packet
  LOG(INFO, "Writing packet");
  ret = comm_ctx->driver->write(comm_ctx, buffer, buffer_size, timeout_ms);
  if (ret == 0) {
    LOG(INFO, "LIST command sent");
  } else {
    LOG(ERR, "Failed to send LIST command: %d", ret);
  }
  return ret;
}

int upload_files_command(comm_context_t *comm_ctx, const char *filename) {
  LOG(INFO, "Uploading file {%s}", filename);
  const uint16_t timeout_ms = 1000; // 1-second timeout
  int fd                    = open(filename, O_RDONLY);
  if (fd == -1) {
    LOG(ERR, "Error opening file {%s}", filename);
    return fd;
  }
  uint32_t payload_size = lseek(fd, 0, SEEK_END);
  if (payload_size == -1) {
    LOG(ERR, "Error seeking to end of file");
    return -1;
  } else {
    LOG(INFO, "Payload size {%d}", payload_size);
  }

  // allocate a buffer based on the size of the filename. this will be the
  // payload
  size_t packet_size     = DIST_FS_HEADER_SIZE + payload_size;
  uint8_t *packet_buffer = (uint8_t *)malloc(packet_size);
  if (!packet_buffer) {
    LOG(ERR, "Memory allocation failed for packet buffer");
    close(fd);
    return -1;
  } else {
    LOG(INFO, "Allocated {%zu} bytes for packet buffer", packet_size);
  }

  // read the file directly into the payload portion of the packet buffer
  lseek(fd, 0, SEEK_SET); // reset file pointer to the beginning
  ssize_t bytes_read =
    read(fd, packet_buffer + DIST_FS_HEADER_SIZE, payload_size);
  if (bytes_read != payload_size) {
    LOG(ERR, "Error reading file, read {%ld} bytes", bytes_read);
    free(packet_buffer);
    close(fd);
    return -1;
  }

  int ret = encode_packet(DIST_FS_UPLOAD,
                          packet_buffer + DIST_FS_HEADER_SIZE,
                          payload_size,
                          packet_buffer);
  if (ret < 0) {
    LOG(ERR, "Failed to form packet\n");
    return ret;
  }

  // send packet
  LOG(INFO, "Writing packet");
  ret =
    comm_ctx->driver->write(comm_ctx, packet_buffer, packet_size, timeout_ms);
  if (ret == 0) {
    LOG(INFO, "UPLOAD command sent");
  } else {
    LOG(ERR, "Failed to send UPLOAD command: %d", ret);
  }

  free(packet_buffer);
  close(fd);

  return 0;
}

int encode_packet(dist_fs_ops_e command,
                  uint8_t *payload,
                  uint32_t payload_size,
                  uint8_t *buffer) {
  LOG(INFO,
      "Forming packet for command {%d} with size {%d}",
      command,
      payload_size + DIST_FS_HEADER_SIZE);
  int rc = 0;

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

  // form the header of the packet
  buffer[DIST_FS_PKT_START_1] = DIST_FS_START_BYTE_A;
  buffer[DIST_FS_PKT_START_2] = DIST_FS_START_BYTE_B;
  // set command of the packet
  buffer[DIST_FS_PKT_COMMAND] = command;
  // set payload size
  buffer[DIST_FS_PKT_SIZE_MSB] = (payload_size >> 8) & 0xFF;
  buffer[DIST_FS_PKT_SIZE_LSB] = payload_size & 0xFF;

  LOG(INFO,
      "Formed packet header: 0x%X 0x%X 0x%X 0x%X 0x%X",
      buffer[DIST_FS_PKT_START_1],
      buffer[DIST_FS_PKT_START_2],
      buffer[DIST_FS_PKT_COMMAND],
      buffer[DIST_FS_PKT_SIZE_MSB],
      buffer[DIST_FS_PKT_SIZE_LSB]);

  // fill in payload data
  if (payload && payload_size > 0) {
    memcpy(buffer + DIST_FS_PKT_PAYLOAD, payload, payload_size);
    LOG(INFO, "memcpy complete");
  }

  return rc;
}

int decode_packet(comm_context_t *comm_ctx) {
  uint8_t buffer[DIST_FS_HEADER_SIZE];
  const uint16_t buffer_size = sizeof(buffer);
  const uint16_t timeout_ms  = 1000; // 1-second timeout

  // read header first
  int ret =
    comm_ctx->driver->read(comm_ctx, buffer, DIST_FS_HEADER_SIZE, timeout_ms);
  if (ret == 0) {
    LOG(INFO, "Read :");
    for (int i = 0; i < DIST_FS_HEADER_SIZE; i++) {
      printf("0x%X ", buffer[i]);
    }
    printf("\n");

    // validate start bytes
    if (buffer[DIST_FS_PKT_START_1] != DIST_FS_START_BYTE_A ||
        buffer[DIST_FS_PKT_START_2] != DIST_FS_START_BYTE_B) {
      LOG(ERR, "Invalid start bytes");
      return -1;
    }

    // get command
    dist_fs_ops_e command =
      static_cast<dist_fs_ops_e>(buffer[DIST_FS_PKT_COMMAND]);

    // get payload size
    uint32_t payload_size =
      (buffer[DIST_FS_PKT_SIZE_MSB] << 8) | buffer[DIST_FS_PKT_SIZE_LSB];

    // validate payload size
    if (buffer_size != DIST_FS_HEADER_SIZE + payload_size) {
      LOG(ERR,
          "Payload size mismatch: expected %u but got %zu",
          payload_size,
          buffer_size - DIST_FS_HEADER_SIZE);
      return -1;
    }

    // print header information
    LOG(INFO,
        "Start Bytes: 0x%X 0x%X",
        buffer[DIST_FS_PKT_START_1],
        buffer[DIST_FS_PKT_START_2]);
    LOG(INFO, "Command: %d", command);
    LOG(INFO, "Payload Size: %u bytes", payload_size);

    // allocate memory for payload if needed
    uint8_t *payload = nullptr;
    if (payload_size > 0) {
      payload = (uint8_t *)malloc(payload_size);
      if (!payload) {
        LOG(ERR, "Memory allocation failed for payload");
        return -1;
      }

      // read payload data of the packet
      ret = comm_ctx->driver->read(comm_ctx, payload, payload_size, timeout_ms);
      if (ret == 0) {
        // handle payload if needed (printing here for example)
        for (size_t i = 0; i < payload_size; i++) {
          LOG(INFO, "Payload Byte %zu: 0x%X", i, payload[i]);
        }
      } else if (ret == -1) {
      } else {
        LOG(ERR, "Error reading payload data: %d", ret);
      }

      // free memory after usage
      free(payload);
    }

    return 0;
  } else if (ret == -ETIMEDOUT) {
    LOG(WARN, "Read timed out. No data received");
  } else if (ret == -1) {
  } else {
    LOG(ERR, "Error while reading data: %d", ret);
  }
  return -1;
}
