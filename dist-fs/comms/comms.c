#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "../utils.hpp"
#include "comms.h"

/** @brief forward declarations of available comm drivers */
extern comm_driver_t uart_ops;
extern comm_driver_t i2c_ops;
extern comm_driver_t spi_ops;
extern comm_driver_t network_ops;

/** @brief registry struct */
typedef struct {
  comm_types_e type;
  comm_driver_t *ops;
} comm_driver_entry_t;

/** @brief driver registry table */
static comm_driver_entry_t comm_driver_registry[] = {
  {COMMS_SPI, &spi_ops},
  {COMMS_UART, &uart_ops},
  {COMMS_I2C, &i2c_ops},
  {COMMS_NETWORK, &network_ops},
  {COMMS_END, NULL}, // termination
};

/** @brief static comm ctx */
static comm_context_t gl_comm_ctx;

/** @brief device driver registry lookup */
static comm_driver_t *find_comm_driver(comm_types_e type) {
  LOG(INFO, "finding comm driver type {%d} from registry...", type);

  for (int i = 0; comm_driver_registry[i].type != COMMS_END; ++i) {
    if (comm_driver_registry[i].type == type) {
      return comm_driver_registry[i].ops;
    }
  }
  return NULL;
}

/** brief top level initialize function */
comm_context_t *comm_init(comm_types_e type, const char *device, uint32_t baud) {
  LOG(INFO, "Initializing comm driver type {%d} on device {%s} with baud {%u}...", 
      type, device, baud);

  // find the driver for the given comm type
  comm_driver_t *driver = find_comm_driver(type);
  if (!driver || !driver->init) {
    LOG(ERR, "error, no driver to init");
    return NULL; // driver not found or missing init function
  }

  // init the static context
  memset(&gl_comm_ctx, 0, sizeof(gl_comm_ctx));
  gl_comm_ctx.type = type;
  gl_comm_ctx.driver = driver;
  strncpy(gl_comm_ctx.device, device, sizeof(gl_comm_ctx.device) - 1);
  gl_comm_ctx.baud = baud;


  // ready to call the driver-specific init function
  if (driver->init(&gl_comm_ctx) < 0) {
    LOG(ERR, "error calling underlying init function");
    return NULL;
  }
  return &gl_comm_ctx;
}
