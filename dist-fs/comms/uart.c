#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h> // For UART settings
#include <sys/ioctl.h>
#include <sys/poll.h> // For poll() function

#include "../utils.hpp"
#include "comms.h"

static int uart_init(comm_context_t *ctx);
static int uart_read_one(comm_context_t *ctx, uint16_t timeout_ms);
static int uart_read(comm_context_t *ctx,
                     uint8_t *rx,
                     uint32_t rx_sz,
                     uint16_t timeout_ms);
static int uart_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms);
static int uart_write(comm_context_t *ctx,
                      uint8_t *tx,
                      uint32_t tx_size,
                      uint16_t timeout_ms);
static int uart_ioctl(comm_context_t *ctx, uint8_t opcode, void *data);

comm_driver_t uart_ops = {
  .init      = uart_init,
  .read_one  = uart_read_one,
  .read      = uart_read,
  .write_one = uart_write_one,
  .write     = uart_write,
  .ioctl     = uart_ioctl,
};

static int uart_fd;

// TODO : most of this implementation will need to be solidified. looks like
// the context for this needs to be fleshed out
static int configure_uart(int fd, uint32_t baud) {
  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    LOG(ERR, "Failed to get UART attributes");
    return -1;
  }

  // Set input/output baud rates
  speed_t baud_rate;
  switch (baud) {
    case 9600:
      baud_rate = B9600;
      break;
    case 19200:
      baud_rate = B19200;
      break;
    case 38400:
      baud_rate = B38400;
      break;
    case 57600:
      baud_rate = B57600;
      break;
    case 115200:
      baud_rate = B115200;
      break;
    default:
      LOG(ERR, "Unsupported baud rate: %u\n", baud);
      return -1;
  }
  cfsetospeed(&tty, baud_rate);
  cfsetispeed(&tty, baud_rate);

  // 8N1 (8-bit data, no parity, 1 stop bit)
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit data
  tty.c_cflag &= ~PARENB;                     // no parity
  tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
  // the rest of these settings are sort of trial/error from other device driver
  // work
  tty.c_cflag |= CREAD | CLOCAL; // enable receiver, ignore modem lines
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // disable line input, echo
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // disable software flow control
  tty.c_oflag &= ~OPOST;                  // disable post-processing
  tty.c_cc[VMIN]  = 0;                    // no minimum characters
  tty.c_cc[VTIME] = 1;                    // 0.1 second timeout

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    LOG(ERR, "Failed to set UART attributes");
    return -1;
  }

  return 0;
}

static int uart_init(comm_context_t *ctx) {
  if (!ctx)
    return -EINVAL;

  LOG(INFO, "Opening UART device: %s", ctx->device);

  int fd = open(ctx->device, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    LOG(ERR, "Failed to open UART device");
    return -errno;
  }

  // configure UART with some termios settings
  if (configure_uart(fd, ctx->baud) < 0) {
    close(fd);
    return -1;
  }

  // set global UART file descriptor
  uart_fd = fd;
  // ctx->driver = (comm_driver_t *)(uintptr_t)fd;
  return 0;
}

static int uart_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
  if (!ctx || !ctx->driver)
    return -EINVAL;

  uint8_t byte      = 0;
  struct pollfd pfd = {.fd = uart_fd, .events = POLLIN, .revents = 0};
  int ret           = poll(&pfd, 1, timeout_ms);

  if (ret > 0 && (pfd.revents & POLLIN)) {
    if (read(uart_fd, &byte, 1) == 1) {
      return byte;
    }
  }

  return (ret == 0) ? -ETIMEDOUT : -errno;
}

static int uart_read(comm_context_t *ctx,
                     uint8_t *rx,
                     uint32_t rx_sz,
                     uint16_t timeout_ms) {
  if (!ctx || !ctx->driver || !rx)
    return -EINVAL;

  struct pollfd pfd  = {.fd = uart_fd, .events = POLLIN, .revents = 0};
  ssize_t bytes_read = 0;

  while (bytes_read < rx_sz) {
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret > 0 && (pfd.revents & POLLIN)) {
      ssize_t r =
        read(uart_fd, ((uint8_t *)rx) + bytes_read, rx_sz - bytes_read);
      if (r > 0) {
        bytes_read += r;
      } else {
        return -errno;
      }
    } else if (ret == 0) {
      break;
    } else {
      return -errno;
    }
  }

  if (bytes_read > 0) {
    return 0;
  } else {
    return -1;
  }
}

static int
uart_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms) {
  if (!ctx || !ctx->driver)
    return -EINVAL;

  uint8_t byte = (uint8_t)tx;

  struct pollfd pfd = {.fd = uart_fd, .events = POLLOUT, .revents = 0};
  int ret           = poll(&pfd, 1, timeout_ms);

  if (ret > 0 && (pfd.revents & POLLOUT)) {
    return (write(uart_fd, &byte, 1) == 1) ? 0 : -errno;
  }

  return (ret == 0) ? -ETIMEDOUT : -errno;
}

static int uart_write(comm_context_t *ctx,
                      uint8_t *tx,
                      uint32_t tx_size,
                      uint16_t timeout_ms) {
  if (!ctx || !ctx->driver || !tx)
    return -EINVAL;

  struct pollfd pfd     = {.fd = uart_fd, .events = POLLOUT, .revents = 0};
  ssize_t bytes_written = 0;

  while (bytes_written < tx_size) {
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret > 0 && (pfd.revents & POLLOUT)) {
      ssize_t w = write(uart_fd,
                        ((uint8_t *)tx) + bytes_written,
                        tx_size - bytes_written);
      if (w > 0) {
        bytes_written += w;
      } else {
        return -errno;
      }
    } else if (ret == 0) {
      break; // Timeout
    } else {
      return -errno;
    }
  }

  if (bytes_written > 0) {
    return 0;
  } else {
    return -1;
  }
}

static int uart_ioctl(comm_context_t *ctx, uint8_t opcode, void *data) {
  if (!ctx || !ctx->driver)
    return -EINVAL;

  return ioctl(uart_fd, opcode, data);
}
