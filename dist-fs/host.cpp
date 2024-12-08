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

/*
int main() {
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY | O_SYNC);
    if (uart_fd == -1) {
        std::cerr << "Failed to open UART device: " << UART_DEVICE << std::endl;
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(uart_fd, &tty) != 0) {
        std::cerr << "Error getting terminal attributes" << std::endl;
        return 1;
    }

    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                     // disable break processing
    tty.c_lflag = 0;                            // no signaling chars, no echo,
no canonical processing tty.c_oflag = 0;                            // no
remapping, no delays tty.c_cc[VMIN] = 1;                         // read doesn't
block tty.c_cc[VTIME] = 1;                        // 0.1 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);       // ignore modem controls, enable
reading tty.c_cflag &= ~(PARENB | PARODD);     // shut off parity tty.c_cflag &=
~CSTOPB; tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting terminal attributes" << std::endl;
        return 1;
    }

    char buffer[256];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(uart_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            std::cout << "Received message: " << buffer << std::endl;
        }
        usleep(100000); // Sleep 100ms to prevent excessive CPU usage
    }

    close(uart_fd);
    return 0;
}
*/
