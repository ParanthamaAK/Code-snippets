#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>


void uart_config(int uart_fd) {
    struct termios tty;

    tcgetattr(serial_port, &tty);

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~(PARENB, CSTOPB, CSIZE);
    tty.c_cflag |= CS8;
    tty.c_cflag |= (CREAD | CLOCAL);

    tcsetattr(uart_fd , TCSANOW, &tty);
}

void send_binary_data(int uart_fd, const void* data, size_t size){
    write(uart_fd, data, size);
}

int main() {
    const char* serial_port = "/dev/ttyS0";

    int uart_fd = open(serial_port, O_RDWR | O_NOCTTY);

    if(uart_fd == -1){
        perror("Failed to open serial port");
    }

    uart_config(uart_fd);
    
    const unsigned char bin_data[] = {0x01, 0x02, 0x03, 0x04};
    size_t data_size = sizeof(bin_data);

    send_binary_data(uart_fd, bin_data, data_size);

    close(uart_fd);
    return 0;
}

 