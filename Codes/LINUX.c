#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <time.h>


// Define the UART device file and baud rate
#define UART_DEVICE "/dev/ttyS0" // Change this to the appropriate UART device
#define BAUD_RATE B115200        // Change this to your ESP32's baud rate

// Define the static addresses for ESP32 devices
char esp32_addresses[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

// Define the data payloads for each ESP32 device
char data_payloads[5] = {0xF1, 0xAA, 0xFF, 0x00, 0xBC};

// Function to configure the UART port
int configureUART(int fd) {
    struct termios uartConfig;
    if (tcgetattr(fd, &uartConfig) < 0) {
        perror("Error configuring UART");
        return -1;
    }

    // Set baud rate
    cfsetospeed(&uartConfig, BAUD_RATE);
    cfsetispeed(&uartConfig, BAUD_RATE);

    // 8N1 (8 data bits, no parity, 1 stop bit)
    uartConfig.c_cflag &= ~PARENB;
    uartConfig.c_cflag &= ~CSTOPB;
    uartConfig.c_cflag &= ~CSIZE;
    uartConfig.c_cflag |= CS8;

    // Apply the configuration
    if (tcsetattr(fd, TCSANOW, &uartConfig) < 0) {
        perror("Error configuring UART");
        return -1;
    }

    return 0;
}

// Function to open the UART device
int openUART() {
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd == -1) {
        perror("Error opening UART");
    }
    return uart_fd;
}

// Function to check if a device is online
bool checkDeviceOnline(int uart_fd, char address) {
    char requestBuffer[64];
    char responseBuffer[64];
    int bytes;
    int timeout = 5; // Timeout in seconds

    // Construct the "device request" message
    snprintf(requestBuffer, sizeof(requestBuffer), "device request %c", address);

    // Send the "device request" to the ESP32 device
    if (write(uart_fd, requestBuffer, strlen(requestBuffer)) == -1) {
        perror("Error writing to UART");
        return false;
    }

    // Wait for a response from the ESP32 device within the timeout
    time_t start_time = time(NULL);
    while (time(NULL) - start_time < timeout) {
        bytes = read(uart_fd, responseBuffer, sizeof(responseBuffer) - 1);
        if (bytes > 0) {
            responseBuffer[bytes] = '\0';
            if (strcmp(responseBuffer, "online") == 0) {
                return true;
            }
        } else if (bytes == -1) {
            perror("Error reading from UART");
            return false;
        }
    }

    return false; // Device did not respond or is not online
}

// Function to send data to devices
void sendDataToDevices(int uart_fd) {
    for (int i = 0; i < 5; i++) {
        // Send the address byte
        if (write(uart_fd, &esp32_addresses[i], 1) == -1) {
            perror("Error writing address to UART");
            break;
        }
        usleep(100000); // Add a delay between address and data

        // Send the data byte
        if (write(uart_fd, &data_payloads[i], 1) == -1) {
            perror("Error writing data to UART");
            break;
        }
        usleep(100000); // Add a delay between data and next transmission
    }
}

// Function to handle incoming requests or data
void handleIncomingData(int uart_fd) {
    char requestBuffer[64];
    int bytes;

    bytes = read(uart_fd, requestBuffer, sizeof(requestBuffer));
    if (bytes > 0) {
        requestBuffer[bytes] = '\0';

        // Handle incoming requests or data here
        // ...
    } else if (bytes == -1) {
        perror("Error reading from UART");
    }
}

int main() {
    int uart_fd;

    // Open the UART device
    uart_fd = openUART();
    if (uart_fd == -1) {
        return 1;
    }

    // Configure the UART port
    if (configureUART(uart_fd) != 0) {
        close(uart_fd);
        return 1;
    }

    // Main loop (you can adjust the timing as needed)
    while (1) {
        handleIncomingData(uart_fd);
    }

    close(uart_fd);

    return 0;
}
