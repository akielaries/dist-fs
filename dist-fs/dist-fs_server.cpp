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



#define CLIENT_IP "192.168.86.54"
#define SERVER_PORT 5000
#define BUFFER_SIZE 1024

int main() {
    // Initialize comms as a receiver
    comm_context_t *ctx = comm_init(COMMS_NETWORK, CLIENT_IP, 0);
    if (!ctx) {
        std::cerr << "Failed to initialize network communication." << std::endl;
        return -1;
    }

    uint8_t buffer[BUFFER_SIZE];

    while (true) {
        int bytes_read = ctx->driver->read(ctx, buffer, sizeof(buffer), 1000);
        if (bytes_read > 0) {
            std::cout << "Received " << bytes_read << " bytes: ";
            for (int i = 0; i < bytes_read; ++i) {
                std::cout << std::hex << (int)buffer[i] << " ";
            }
            std::cout << std::endl;
        } else if (bytes_read == 0) {
            std::cout << "No data received (timeout)." << std::endl;
        } else {
            std::cerr << "Error receiving data." << std::endl;
            break;
        }
    }

    return 0;
}
/*
int main() {
  //comm_context_t *comm_ctx = comm_init(COMMS_UART, "/dev/ttyTHS0", 4000000);
  comm_context_t *comm_ctx = comm_init(COMMS_NETWORK, "192.168.86.56", 0);


  if (!comm_ctx) {
    //LOG(ERR, "Failed to initialize UART communication\n");
    LOG(ERR, "Failed to initialize NETWORK communication\n");
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
*/
