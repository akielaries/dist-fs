#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <linux/spi/spidev.h>

#include "comms.h"
#include "../utils.hpp"


static int spi_init(comm_context_t *ctx);
static int spi_read_one(comm_context_t *ctx, uint16_t timeout_ms);
static int
spi_read(comm_context_t *ctx, uint8_t *rx, uint32_t rx_sz, uint16_t timeout_ms);
static int spi_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms);
static int spi_write(comm_context_t *ctx,
                     uint8_t *tx,
                     uint32_t tx_size,
                     uint16_t timeout_ms);
static int spi_ioctl(comm_context_t *ctx, uint8_t opcode, void *data);

comm_driver_t spi_ops = {
  .init      = spi_init,
  .read_one  = spi_read_one,
  .read      = spi_read,
  .write_one = spi_write_one,
  .write     = spi_write,
  .ioctl     = spi_ioctl,
};


static int spi_fd = -1; // SPI file descriptor


static int configure_spi(int fd, uint32_t speed_hz) {
  uint8_t mode          = SPI_MODE_0; // SPI mode 0
  uint8_t bits_per_word = 8;          // 8-bit words

  // Set SPI mode
  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
    LOG(ERR, "Failed to set SPI mode: %s", strerror(errno));
    return -1;
  }

  // Set bits per word
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0) {
    LOG(ERR, "Failed to set bits per word: %s", strerror(errno));
    return -1;
  }

  // Set SPI speed
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz) < 0) {
    LOG(ERR, "Failed to set SPI speed: %s", strerror(errno));
    return -1;
  }

  return 0;
}

static int spi_init(comm_context_t *ctx) {
  if (!ctx)
    return -EINVAL;

  LOG(INFO, "Opening SPI device: %s", ctx->device);

  int fd = open(ctx->device, O_RDWR);
  if (fd < 0) {
    LOG(ERR, "Failed to open SPI device: %s", strerror(errno));
    return -errno;
  }

  if (configure_spi(fd, ctx->baud) < 0) {
    close(fd);
    return -1;
  }

  spi_fd = fd;
  return 0;
}

static int spi_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
  if (!ctx)
    return -EINVAL;

  uint8_t rx_byte   = 0;
  struct pollfd pfd = {.fd = spi_fd, .events = POLLIN, .revents = 0};
  int ret           = poll(&pfd, 1, timeout_ms);

  if (ret > 0 && (pfd.revents & POLLIN)) {
    if (read(spi_fd, &rx_byte, 1) == 1) {
      return rx_byte;
    }
  }

  return (ret == 0) ? -ETIMEDOUT : -errno;
}

static int spi_read(comm_context_t *ctx,
                    uint8_t *rx,
                    uint32_t rx_sz,
                    uint16_t timeout_ms) {
  if (!ctx || !rx)
    return -EINVAL;

  struct pollfd pfd  = {.fd = spi_fd, .events = POLLIN, .revents = 0};
  ssize_t bytes_read = 0;

  while (bytes_read < rx_sz) {
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret > 0 && (pfd.revents & POLLIN)) {
      ssize_t r = read(spi_fd, rx + bytes_read, rx_sz - bytes_read);
      if (r > 0) {
        bytes_read += r;
      } else {
        return -errno;
      }
    } else if (ret == 0) {
      break; // Timeout
    } else {
      return -errno;
    }
  }

  return bytes_read;
}

static int spi_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms) {
  if (!ctx)
    return -EINVAL;

  uint8_t tx_byte   = tx;
  struct pollfd pfd = {.fd = spi_fd, .events = POLLOUT, .revents = 0};
  int ret           = poll(&pfd, 1, timeout_ms);

  if (ret > 0 && (pfd.revents & POLLOUT)) {
    return (write(spi_fd, &tx_byte, 1) == 1) ? 0 : -errno;
  }

  return (ret == 0) ? -ETIMEDOUT : -errno;
}

static int spi_write(comm_context_t *ctx,
                     uint8_t *tx,
                     uint32_t tx_size,
                     uint16_t timeout_ms) {
  if (!ctx || !tx)
    return -EINVAL;

  struct pollfd pfd     = {.fd = spi_fd, .events = POLLOUT, .revents = 0};
  ssize_t bytes_written = 0;

  while (bytes_written < tx_size) {
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret > 0 && (pfd.revents & POLLOUT)) {
      ssize_t w = write(spi_fd, tx + bytes_written, tx_size - bytes_written);
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

  return bytes_written;
}

static int spi_ioctl(comm_context_t *ctx, uint8_t opcode, void *data) {
  if (!ctx)
    return -EINVAL;

  return ioctl(spi_fd, opcode, data);
}
