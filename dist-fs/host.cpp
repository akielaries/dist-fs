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

int main() {
  comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/ttyTHS0", 115200);
  if (!comm_ctx) {
    LOG(ERR, "Failed to initialize UART communication\n");
    return -1;
  }
  if (!comm_ctx->driver) {
    LOG(ERR, "Driver is not initialized\n");
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
        printf("0x%X ", buffer[i]);
      }
      printf("\n");

    } else if (ret == -1) {
      continue;
    } else if (ret == -ETIMEDOUT) {
      LOG(WARN, "Read timed out. No data received");
    } else {
      LOG(ERR, "Error while reading data: %d", ret);
      break;
    }
  }

  return 0;
}

