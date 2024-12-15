#ifndef SOCKET_API_H
#define SOCKET_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Initialize the socket
int init_socket(const char *ip, int port);

// Read data from the socket (server side)
int read_socket(int sockfd, void *buffer, size_t size);

// Write data to the socket (client side)
int write_socket(int sockfd, const void *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif // SOCKET_API_H

