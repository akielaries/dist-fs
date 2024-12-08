/* this will be running on N client's and will be handling
 * commands to the host
 */
#include <iostream>
#include <cstring>

#include "utils.hpp"
#include "comms/comms.h"


#define UART_DEVICE "/dev/serial0"
#define BAUD_RATE B115200

int main() {
  comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/serial0", 115200);
  if (!comm_ctx) {
    LOG(ERR, "Failed to initialize UART communication\n");
    return -1;
  }
  if (!comm_ctx->driver) {
    LOG(ERR, "Driver is not initialized\n");
    return -1;
  }

  uint32_t buffer[256]; // Adjust size as needed
  const uint16_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
  const uint16_t timeout_ms  = 1000; // 1-second timeout for reading
  ssize_t bytes_received     = 0;

  while (1) {
    // Read data from UART
    int ret = comm_ctx->driver->read(comm_ctx, buffer, buffer_size, timeout_ms);
    if (ret == 0) { // Successful read
      LOG(INFO, "Data received: %s", buffer);
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
