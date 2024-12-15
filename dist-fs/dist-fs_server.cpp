/* this is the interaction between the host and the drive
 * so commands coming from a client should be handled here
 */
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <chrono>

#include "utils.hpp"
#include "comms/comms.h"
#include "comms/packet.h"

#define UART_DEVICE "/dev/ttyTHS0"
#define BAUD_RATE   B115200

int main() {
  //comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/ttyTHS0", 4000000);
  comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/ttyTHS0", 4000000);

  if (!comm_ctx) {
    LOG(ERR, "Failed to initialize UART communication\n");
    return -1;
  }
  if (!comm_ctx->driver) {
    LOG(ERR, "Driver is not initialized\n");
    return -1;
  }


  // start the timer
  auto start_time = std::chrono::steady_clock::now();

  while (true) {
    decode_packet(comm_ctx);

    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
                          current_time - start_time)
                          .count();
    if (elapsed_time >= 10) {
      LOG(INFO, "Timeout reached after 10 seconds");
      break; // Exit the loop
    }
  }


  return 0;
}
