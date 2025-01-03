/* this will be running on N client's and will be handling
 * commands to the host
 */
#include <iostream>
#include <cstring>

#include "utils.hpp"
#include "comms/comms.h"
#include "comms/packet.h"


int main() {
  // comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/serial0", 4000000);

  comm_context_t *comm_ctx = comm_init(COMMS_NETWORK, "192.168.86.56", 0);

  if (!comm_ctx) {
    // LOG(ERR, "Failed to initialize UART communication\n");
    LOG(ERR, "Failed to initialize NETWORK communication\n");
    return -1;
  }
  if (!comm_ctx->driver) {
    LOG(ERR, "Driver is not initialized\n");
    return -1;
  }

  /*
    // send a test packet first
    uint8_t buffer[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    const uint16_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
    test_packet(comm_ctx, buffer, buffer_size);
  */

  int rc = list_files_command(comm_ctx);
  LOG(INFO, "list_files_command() rc: %d", rc);

  // rc = upload_files_command(comm_ctx,
  // "../test_files/wavs/SPRING_ARC_VOL.3(120BPM)_v4.wav"); LOG(INFO,
  // "upload_files_command() rc: %d", rc);

  return 0;
}
