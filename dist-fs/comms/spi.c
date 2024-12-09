#include <stdint.h>

#include "comms.h"

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

static int spi_init(comm_context_t *ctx) {
  (void)ctx;
  int ret = 0;

  return ret;
}

static int spi_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
  (void)ctx;
  (void)timeout_ms;
  int bytes_read = 0;

  return bytes_read;
}

static int spi_read(comm_context_t *ctx,
                    uint8_t *rx,
                    uint32_t rx_sz,
                    uint16_t timeout_ms) {
  (void)ctx;
  (void)rx;
  (void)rx_sz;
  (void)timeout_ms;
  int bytes_read = 0;

  return bytes_read;
}


static int spi_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms) {
  (void)ctx;
  (void)tx;
  (void)timeout_ms;
  int bytes_written = 0;

  return bytes_written;
}

static int spi_write(comm_context_t *ctx,
                     uint8_t *tx,
                     uint32_t tx_size,
                     uint16_t timeout_ms) {
  (void)ctx;
  (void)tx;
  (void)tx_size;
  (void)timeout_ms;
  int bytes_written = 0;

  return bytes_written;
}

static int spi_ioctl(comm_context_t *ctx, uint8_t opcode, void *data) {
  (void)ctx;
  (void)opcode;
  (void)data;
  int ret = 0;

  return ret;
}
