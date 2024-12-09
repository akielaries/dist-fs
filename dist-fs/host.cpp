/* this is the interaction between the host and the drive
 * so commands coming from a client should be handled here
 */
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "utils.hpp"
#include "comms/comms.h"
#include "comms/packet.h"

#define UART_DEVICE "/dev/ttyTHS0"
#define BAUD_RATE   B115200


void handle_packet(const dist_fs_packet_t *packet) {
    if (!packet) return;

    switch (packet->command) {
        case DIST_FS_LIST:
            LOG(INFO, "Handling LIST command\n");
            // TODO: Add functionality to fetch and send metadata back
            break;

        case DIST_FS_UPLOAD:
            LOG(INFO, "Handling UPLOAD command\n");
            break;

        case DIST_FS_DOWNLOAD:
            LOG(INFO, "Handling DOWNLOAD command\n");
            break;

        case DIST_FS_DELETE:
            LOG(INFO, "Handling DELETE command\n");
            break;

        default:
            LOG(WARN, "Unknown command received: %d\n", packet->command);
            break;
    }
}

int main() {
  comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/ttyTHS0", 115200);
  if (!comm_ctx) {
    LOG(ERR, "Failed to initialize UART communication");
    return -1;
  }
  if (!comm_ctx->driver) {
    LOG(ERR, "Driver is not initialized");
    return -1;
  }

  uint8_t buffer[256]; // Adjust size as needed
  const uint16_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
  const uint16_t timeout_ms  = 1000; // 1-second timeout for reading
  ssize_t bytes_received     = 0;

  while (1) {
    // Read data from UART
    int ret = comm_ctx->driver->read(comm_ctx, buffer, buffer_size, timeout_ms);
    if (ret == 0) { // Successful read
      LOG(INFO, "Got %d bytes", buffer_size);
      for (int i = 0; i < buffer_size; i++) {
        printf("0x%X\n", buffer[i]);
      }
      
      dist_fs_packet_t packet;
      
      if (dist_fs_packet_parse(buffer, bytes_received, &packet) == 0) {
        handle_packet(&packet);
        dist_fs_packet_free(&packet); // Free dynamically allocated payload
      } 
      else {
        LOG(WARN, "Failed to parse received packet");
      }
    } 
    else if (ret == -1) {
      continue;
    } 
    else if (ret == -ETIMEDOUT) {
      LOG(WARN, "Read timed out. No data received");
    } 
    else {
      LOG(ERR, "Error while reading data: %d", ret);
      break;
    }
  }

  return 0;
}

