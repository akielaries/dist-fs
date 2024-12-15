#include <iostream>
#include "socket.h"

#define BUFFER_SIZE 1024
#define SERVER_IP   "192.168.86.56"

int main() {
  int sockfd;
  int new_socket;
  char buffer[BUFFER_SIZE] = {0};

  // Initialize the socket
  sockfd = init_socket(SERVER_IP, PORT);
  if (sockfd < 0) {
    return -1;
  }

  std::cout << "Server listening on port " << PORT << std::endl;

  // Accept incoming connections
  new_socket = accept(sockfd, (struct sockaddr *)NULL, NULL);
  if (new_socket < 0) {
    perror("Accept failed");
    close(sockfd);
    return -1;
  }

  // Read data from the client
  int valread = read_socket(new_socket, buffer, sizeof(buffer));
  if (valread < 0) {
    close(new_socket);
    close(sockfd);
    return -1;
  }

  std::cout << "Received packet: " << std::hex
            << *reinterpret_cast<int *>(buffer) << std::endl;

  close(new_socket);
  close(sockfd);
  return 0;
}
