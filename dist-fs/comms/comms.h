#ifndef COMMS_H
#define COMMS_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>


typedef struct comm_driver_t comm_driver_t;

typedef enum {
  COMMS_SPI = 0,
  COMMS_NETWORK,
  COMMS_UART,
  COMMS_I2C,
  COMMS_END,
  COMMS_NUM_TYPES = COMMS_END - 1,
} comm_types_e;

typedef struct {
  int socket_fd;
  struct sockaddr_in server_addr;
} network_context_t;

typedef struct {
  uint8_t type;
  uint32_t baud;
  char device[128];
  comm_driver_t *driver;
  network_context_t network_ctx;
} comm_context_t;

typedef struct comm_driver_t {
  /**
   * @brief initialize comms + type
   */
  int (*init)(comm_context_t *ctx);
  /**
   * @brief read one byte from the comms stream
   *
   * @param[in] ctx         comm context pointer
   * @param[in] timeout_ms  timeout after given time
   *
   * @return byte read, some meaningful errno on failure
   */
  int (*read_one)(comm_context_t *ctx, uint16_t timeout_ms);
  /**
   * @brief read a buffer of bytes from the comms stream
   *
   * @param[in] ctx         comm context pointer
   * @param[in] rx          receive buffer
   * @param[in] rx_sz       receive buffer size
   * @param[in] timeout_ms  timeout after given time
   *
   * @return bytes read, < 0 on failure
   */
  int (*read)(comm_context_t *ctx,
              uint8_t *rx,
              uint32_t rx_sz,
              uint16_t timeout_ms);
  /**
   * @brief read a buffer of random bytes from the stream
   *
   * @param[in] ctx       comm context pointer
   * @param[in] tx        transfer single byte
   * @param[in] timout_ms timeout after given time
   *
   * @return bytes written, < 0 on failure
   */
  int (*write_one)(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms);
  /**
   * @brief write a buffer of bytes to the comms stream
   *
   * @param[in] ctx       comm context pointer
   * @param[in] tx        transfer buffer
   * @param[in] tx_sz     transfer buffer size
   * @param[in] timout_ms timeout after given time
   *
   * @return bytes written, < 0 on failure
   */
  int (*write)(comm_context_t *ctx,
               uint8_t *tx,
               uint32_t tx_sz,
               uint16_t timeout_ms);
  /**
   * @brief some comms specific ioctl operation
   *
   * @param[in]     ctx     comm context pointer
   * @param[in]     opcode  ioctl operation code
   * @param[in/out] data    return data
   */
  int (*ioctl)(comm_context_t *ctx, uint8_t opcode, void *data);
} comm_driver_t;

/**
 * @brief top level comms initialize function. this sets up the driver and
 * context
 */
comm_context_t *comm_init(comm_types_e type, const char *device, uint32_t baud);

#endif
