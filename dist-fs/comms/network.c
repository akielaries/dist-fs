#include <stdint.h>

#include "comms.h"

static int network_init(comm_context_t *ctx);
static int network_read_one(comm_context_t *ctx, uint16_t timeout_ms);
static int network_read(comm_context_t *ctx,
                        uint32_t *rx,
                        uint16_t rx_sz,
                        uint16_t timeout_ms);
static int
network_write_one(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms);
static int network_write(comm_context_t *ctx,
                         uint32_t *tx,
                         uint16_t tx_size,
                         uint16_t timeout_ms);
static int network_ioctl(comm_context_t *ctx, uint32_t opcode, void *data);

comm_driver_t network_ops = {
  .init      = network_init,
  .read_one  = network_read_one,
  .read      = network_read,
  .write_one = network_write_one,
  .write     = network_write,
  .ioctl     = network_ioctl,
};

static int network_init(comm_context_t *ctx) {
  (void)ctx;
  int ret = 0;

  return ret;
}

static int network_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
  (void)ctx;
  (void)timeout_ms;
  int bytes_read = 0;

  return bytes_read;
}

static int network_read(comm_context_t *ctx,
                        uint32_t *rx,
                        uint16_t rx_sz,
                        uint16_t timeout_ms) {
  (void)ctx;
  (void)rx;
  (void)rx_sz;
  (void)timeout_ms;
  int bytes_read = 0;

  return bytes_read;
}


static int
network_write_one(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms) {
  (void)ctx;
  (void)tx;
  (void)timeout_ms;
  int bytes_written = 0;

  return bytes_written;
}

static int network_write(comm_context_t *ctx,
                         uint32_t *tx,
                         uint16_t tx_size,
                         uint16_t timeout_ms) {
  (void)ctx;
  (void)tx;
  (void)tx_size;
  (void)timeout_ms;
  int bytes_written = 0;

  return bytes_written;
}

static int network_ioctl(comm_context_t *ctx, uint32_t opcode, void *data) {
  (void)ctx;
  (void)opcode;
  (void)data;
  int ret = 0;

  return ret;
}
