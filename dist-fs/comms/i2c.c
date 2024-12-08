#include <stdint.h>

#include "comms.h"

static int i2c_init(comm_context_t *ctx);
static int i2c_read_one(comm_context_t *ctx, uint16_t timeout_ms);
static int i2c_read(comm_context_t *ctx,
                    uint32_t *rx,
                    uint16_t rx_sz,
                    uint16_t timeout_ms);
static int i2c_write_one(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms);
static int i2c_write(comm_context_t *ctx,
                     uint32_t *tx,
                     uint16_t tx_size,
                     uint16_t timeout_ms);
static int i2c_ioctl(comm_context_t *ctx, uint32_t opcode, void *data);


comm_driver_t i2c_ops = {
  .init      = i2c_init,
  .read_one  = i2c_read_one,
  .read      = i2c_read,
  .write_one = i2c_write_one,
  .write     = i2c_write,
  .ioctl     = i2c_ioctl,
};

static int i2c_init(comm_context_t *ctx) {
  (void)ctx;
  int ret = 0;

  return ret;
}

static int i2c_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
  (void)ctx;
  (void)timeout_ms;
  int bytes_read = 0;

  return bytes_read;
}

static int i2c_read(comm_context_t *ctx,
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
i2c_write_one(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms) {
  (void)ctx;
  (void)tx;
  (void)timeout_ms;
  int bytes_written = 0;

  return bytes_written;
}

static int i2c_write(comm_context_t *ctx,
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

static int i2c_ioctl(comm_context_t *ctx, uint32_t opcode, void *data) {
  (void)ctx;
  (void)opcode;
  (void)data;
  int ret = 0;

  return ret;
}
