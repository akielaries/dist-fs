#include <iostream>
#include "socket.h"

#define SERVER_IP "192.168.86.56"
#define PACKET 0xDEADBEEF

int main() {
    int sock;
    int result;
    char buffer[4];

    // Initialize the socket
    sock = init_socket(SERVER_IP, PORT);
    if (sock < 0) {
        return -1;
    }

    // Send data to the server
    memcpy(buffer, &PACKET, sizeof(PACKET));
    result = write_socket(sock, buffer, sizeof(buffer));
    if (result < 0) {
        close(sock);
        return -1;
    }

    std::cout << "Packet sent: " << std::hex << PACKET << std::endl;

    close(sock);
    return 0;
}

