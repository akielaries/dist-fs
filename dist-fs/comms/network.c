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
  .read_one  = network_read_one,
  .read      = network_read,
  .write_one = network_write_one,
  .write     = network_write,
  .ioctl     = network_ioctl,
};

static int network_init(comm_context_t *ctx) {
    if (!ctx || !ctx->path) {
        LOG(ERR, "Invalid context or server address");
        return -1;
    }

    network_context_t *net_ctx = &ctx->network_ctx;

    // Create socket
    net_ctx->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (net_ctx->socket_fd < 0) {
        LOG(ERR, "Socket creation failed: %s", strerror(errno));
        return -1;
    }

    // Configure server address
    memset(&net_ctx->server_addr, 0, sizeof(net_ctx->server_addr));
    net_ctx->server_addr.sin_family = AF_INET;
    net_ctx->server_addr.sin_port = htons(NETWORK_DEFAULT_PORT);

    if (inet_pton(AF_INET, ctx->path, &net_ctx->server_addr.sin_addr) <= 0) {
        LOG(ERR, "Invalid server address: %s", ctx->path);
        close(net_ctx->socket_fd);
        return -1;
    }

    // Connect to server
    if (connect(net_ctx->socket_fd,
                (struct sockaddr *)&net_ctx->server_addr,
                sizeof(net_ctx->server_addr)) < 0) {
        LOG(ERR, "Connection to server failed: %s", strerror(errno));
        close(net_ctx->socket_fd);
        return -1;
    }

    LOG(INFO, "Network initialized and connected to %s:%d", ctx->path, NETWORK_DEFAULT_PORT);
    return 0;
}

static int network_read_one(comm_context_t *ctx, uint16_t timeout_ms) {
    uint8_t byte;
    int ret = network_read(ctx, &byte, 1, timeout_ms);
    return (ret == 1) ? byte : -1;
}

static int network_read(comm_context_t *ctx,
                        uint8_t *rx,
                        uint32_t rx_sz,
                        uint16_t timeout_ms) {
    if (!ctx || !rx) {
        LOG(ERR, "Invalid arguments to network_read");
        return -1;
    }

    network_context_t *net_ctx = &ctx->network_ctx;
    struct timeval timeout = {
        .tv_sec  = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000,
    };
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(net_ctx->socket_fd, &read_fds);

    int ret = select(net_ctx->socket_fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret <= 0) {
        return (ret == 0) ? -ETIMEDOUT : -1;
    }

    ssize_t bytes_read = recv(net_ctx->socket_fd, rx, rx_sz, 0);
    if (bytes_read < 0) {
        LOG(ERR, "Error reading from network: %s", strerror(errno));
        return -1;
    }

    return bytes_read;
}

static int
network_write_one(comm_context_t *ctx, uint8_t tx, uint16_t timeout_ms) {
    return network_write(ctx, &tx, 1, timeout_ms);
}

static int network_write(comm_context_t *ctx,
                         uint8_t *tx,
                         uint32_t tx_size,
                         uint16_t timeout_ms) {
    if (!ctx || !tx) {
        LOG(ERR, "Invalid arguments to network_write");
        return -1;
    }

    network_context_t *net_ctx = &ctx->network_ctx;
    ssize_t bytes_sent = send(net_ctx->socket_fd, tx, tx_size, 0);
    if (bytes_sent < 0) {
        LOG(ERR, "Error writing to network: %s", strerror(errno));
        return -1;
    }

    return bytes_sent;
}

static int network_ioctl(comm_context_t *ctx, uint8_t opcode, void *data) {
    // Implement specific control operations if required
    (void)ctx;
    (void)opcode;
    (void)data;
    return -1;
}
