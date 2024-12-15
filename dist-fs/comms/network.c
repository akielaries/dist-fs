#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

#include "comms.h"
#include "../utils.hpp"


#define NETWORK_DEFAULT_PORT 5000
#define NETWORK_BUFFER_SIZE  1024


static int network_init(comm_context_t *ctx);
static int network_read(comm_context_t *ctx,
                        uint8_t *rx,
                        uint32_t rx_sz,
                        uint16_t timeout_ms);
static int network_write(comm_context_t *ctx,
                         uint8_t *tx,
                         uint32_t tx_size,
                         uint16_t timeout_ms);
static int network_ioctl(comm_context_t *ctx, uint8_t opcode, void *data);

comm_driver_t network_ops = {
  /*
  .init      = network_init,
  .read_one  = NULL,
  .read      = network_read,
  .write_one = NULL,
  .write     = network_write,
  .ioctl     = network_ioctl,
  */
};
