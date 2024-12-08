#ifndef COMMS_H
#define COMMS_H

#include <stdint.h>

#include <stdio.h>
#include <time.h>

#define LVL_INFO  1
#define LVL_WARN  2
#define LVL_ERROR 3

#define LOG(level, fmt, ...)                                                   \
  do {                                                                         \
    const char *level_str;                                                     \
    switch (level) {                                                           \
      case LVL_INFO:                                                           \
        level_str = "INFO";                                                    \
        break;                                                                 \
      case LVL_WARN:                                                           \
        level_str = "WARN";                                                    \
        break;                                                                 \
      case LVL_ERROR:                                                          \
        level_str = "ERROR";                                                   \
        break;                                                                 \
      default:                                                                 \
        level_str = "LOG";                                                     \
    }                                                                          \
    time_t now         = time(NULL);                                           \
    struct tm *tm_info = localtime(&now);                                      \
    char time_buffer[26];                                                      \
    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);                   \
    printf("[%s] [%s] " fmt "\n", time_buffer, level_str, ##__VA_ARGS__);      \
  } while (0)


typedef enum {
  COMMS_SPI = 0,
  COMMS_ANALOG,
  COMMS_UART,
  COMMS_I2C,
  COMMS_END,
  COMMS_NUM_TYPES = COMMS_END - 1,
} comm_types_e;

typedef struct {
  uint8_t type;
  uint32_t baud;
} comm_context_t;

typedef struct {
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
              uint32_t *rx,
              uint16_t rx_sz,
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
  int (*write_one)(comm_context_t *ctx, uint32_t tx, uint16_t timeout_ms);
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
               uint32_t *tx,
               uint16_t tx_sz,
               uint16_t timeout_ms);
  /**
   * @brief some comms specific ioctl operation
   *
   * @param[in]     ctx     comm context pointer
   * @param[in]     opcode  ioctl operation code
   * @param[in/out] data    return data
   */
  int (*ioctl)(comm_context_t *ctx, uint32_t opcode, void *data);
} comm_driver_t;

#endif
