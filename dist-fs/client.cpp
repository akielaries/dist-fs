/* this will be running on N client's and will be handling
 * commands to the host
 */
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define UART_DEVICE "/dev/serial0"
#define BAUD_RATE B115200

int main() {
    int uart_fd = open(UART_DEVICE, O_WRONLY | O_NOCTTY | O_SYNC);
    if (uart_fd == -1) {
        std::cerr << "Failed to open UART device: " << UART_DEVICE << std::endl;
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(uart_fd, &tty) != 0) {
        std::cerr << "Error getting terminal attributes" << std::endl;
        return 1;
    }

    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                     // disable break processing
    tty.c_lflag = 0;                            // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN] = 1;                         // read doesn't block
    tty.c_cc[VTIME] = 1;                        // 0.1 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);       // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);     // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting terminal attributes" << std::endl;
        return 1;
    }

    const char *message = "Hello, World";
    if (write(uart_fd, message, strlen(message)) == -1) {
        std::cerr << "Error writing to UART" << std::endl;
        return 1;
    }

    std::cout << "Message sent: " << message << std::endl;

    close(uart_fd);
    return 0;
}

