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


  while (true) {
    decode_packet(comm_ctx);
  }

  return 0;
}
