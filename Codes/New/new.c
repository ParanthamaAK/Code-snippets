#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

#define UART_PORT "/dev/ttyS0"
#define BAUD_RATE B115200

int uart_fd;

int uart_config(){
    uart_fd = open(UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY);

    if(uart_fd == -1){
        perror("Unable to open uart port");
        return -1;
    }

    struct termios tty;
    tcgetattr(uart_fd, &tty);
    tty.c_cflag |= (BAUD_RATE | CS8| CLOCAL | CREAD);
    tty.c_cflag &= ~(IGNPAR);
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    tcsetattr(uart_config, TCSANOW, &tty);
    
    
 

}