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
static int network_read_one(comm_context_t *ctx, uint16_t timeout_ms);
static int network_read(comm_context_t *ctx,
                        uint8_t *rx,
                        uint32_t rx_sz,
                        uint16_t timeout_ms);
static int
network_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms);
static int network_write(comm_context_t *ctx,
                         uint8_t *tx,
                         uint32_t tx_size,
                         uint16_t timeout_ms);
static int network_ioctl(comm_context_t *ctx, uint8_t opcode, void *data);

comm_driver_t network_ops = {
  .init      = network_init,
  .read_one  = NULL,
  .read      = network_read,
  .write_one = NULL,
  .write     = network_write,
  .ioctl     = network_ioctl,
};


/** @brief Default initialization function for the network interface */
static int network_init(comm_context_t *ctx) {
    if (!ctx) {
        return -EINVAL;
    }

    ctx->network_ctx.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->network_ctx.socket_fd < 0) {
        perror("Socket creation failed");
        return -errno;
    }

    ctx->network_ctx.server_addr.sin_family = AF_INET;
    ctx->network_ctx.server_addr.sin_port = htons(NETWORK_DEFAULT_PORT);
    ctx->network_ctx.server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(ctx->network_ctx.socket_fd, (struct sockaddr *)&ctx->network_ctx.server_addr, sizeof(ctx->network_ctx.server_addr)) < 0) {
        perror("Connect failed");
        close(ctx->network_ctx.socket_fd);
        return -errno;
    }

    return 0;
}

/** @brief Read a buffer of bytes from the network interface */
static int network_read(comm_context_t *ctx, uint8_t *rx, uint32_t rx_sz, uint16_t timeout_ms) {
    if (!ctx || !rx || rx_sz == 0) {
        return -EINVAL;
    }

    struct timeval tv = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000,
    };

    setsockopt(ctx->network_ctx.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    ssize_t received = recv(ctx->network_ctx.socket_fd, rx, rx_sz, 0);
    if (received < 0) {
        perror("Receive failed");
        return -errno;
    }

    return received;
}

/** @brief Write a buffer of bytes to the network interface */
static int network_write(comm_context_t *ctx, uint8_t *tx, uint32_t tx_sz, uint16_t timeout_ms) {
    if (!ctx || !tx || tx_sz == 0) {
        return -EINVAL;
    }

    struct timeval tv = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000,
    };

    setsockopt(ctx->network_ctx.socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    ssize_t sent = send(ctx->network_ctx.socket_fd, tx, tx_sz, 0);
    if (sent < 0) {
        perror("Send failed");
        return -errno;
    }

    return sent;
}

/** @brief Network-specific IOCTL operation */
static int network_ioctl(comm_context_t *ctx, uint8_t opcode, void *data) {
    // Stub: Add handling of specific operations as needed
    return -ENOTSUP;
}

