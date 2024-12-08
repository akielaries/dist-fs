#include <stdint.h>

#include "comms.h"

static int uart_init(comm_context_t *ctx);
static int uart_read_one(comm_context_t *ctx, uint16_t timeout_ms);
static int uart_read(comm_context_t *ctx, uint32_t *rx, uint16_t rx_sz, uint16_t timeout_ms);
static int uart_write_one(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms);
static int uart_write(comm_context_t *ctx, uint32_t *tx, uint16_t tx_size, uint16_t timeout_ms);
static int uart_ioctl(comm_context_t *ctx, uint32_t opcode, void *data);

comm_driver_t uart_ops = { 
  .init       = uart_init,
  .read_one   = uart_read_one,
  .read       = uart_read,
  .write_one  = uart_write_one,
  .write      = uart_write,
  .ioctl      = uart_ioctl,
};

static int uart_init(comm_context_t *ctx) {
  int ret = 0;

  return ret;
}

static int uart_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
  int bytes_read = 0;

  return bytes_read;
}

static int uart_read(comm_context_t *ctx, uint32_t *rx, uint16_t rx_sz, uint16_t timeout_ms) {
  int bytes_read = 0;

  return bytes_read;
}


static int uart_write_one(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms) {
  int bytes_written = 0;

  return bytes_written;
}

static int uart_write(comm_context_t *ctx, uint32_t *tx, uint16_t tx_size, uint16_t timeout_ms) {
  int bytes_written = 0;

  return bytes_written;
}

static int uart_ioctl(comm_context_t *ctx, uint32_t opcode, void *data) {
  int ret = 0;

  return ret;
}

