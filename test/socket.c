#include "socket.h"

// Initialize the socket and return the socket file descriptor
int init_socket(const char *ip, int port) {
  int sockfd;
  struct sockaddr_in server_addr;

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation error");
    return -1;
  }

  // Configure server address struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_port   = htons(port);
  inet_pton(AF_INET, ip, &server_addr.sin_addr);

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Connection failed");
    close(sockfd);
    return -1;
  }

  return sockfd;
}

// Read data from the socket
int read_socket(int sockfd, void *buffer, size_t size) {
  int valread;
  valread = read(sockfd, buffer, size);
  if (valread < 0) {
    perror("Read failed");
    return -1;
  }
  return valread;
}

// Write data to the socket
int write_socket(int sockfd, const void *buffer, size_t size) {
  if (send(sockfd, buffer, size, 0) < 0) {
    perror("Send failed");
    return -1;
  }
  return 0;
}
