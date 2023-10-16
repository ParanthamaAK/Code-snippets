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

// Define the predefined 4-bit addresses for five devices
char predefinedAddresses[] = {'A', 'B', 'C', 'D', 'E'};
int availableAddresses[] = {1, 1, 1, 1, 1}; // 1 indicates address is available, 0 indicates assigned

// Function to configure the UART port (same as before)
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

int assignAddress() {
    // Find an available address and mark it as assigned
    for (int i = 0; i < 5; i++) {
        if (availableAddresses[i]) {
            availableAddresses[i] = 0; // Mark address as assigned
            return i; // Return the index of the assigned address
        }
    }
    return -1; // No available address
}

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

void sendDataToDevices(int uart_fd) {
    char dataPacket[5][2]; // 5 devices, each with a 2-byte data packet
    int assignedAddresses[5];

    // Assign addresses to the devices and construct data packets
    for (int i = 0; i < 5; i++) {
        int assignedAddressIndex = assignAddress();
        if (assignedAddressIndex != -1) {
            assignedAddresses[i] = assignedAddressIndex;
            dataPacket[i][0] = (predefinedAddresses[assignedAddressIndex] << 4) | 0x0F; // First byte with address
            dataPacket[i][1] = 0xFFF; // Second byte with 12 bits of data (example: 0xFFF)
        } else {
            printf("No available addresses for data packet %d\n", i);
        }
    }

    // Send data packets to the devices
    for (int i = 0; i < 5; i++) {
        if (!availableAddresses[assignedAddresses[i]]) {
            if (write(uart_fd, dataPacket[i], sizeof(dataPacket[i])) == -1) {
                perror("Error writing to UART");
                break;
            }
        }
    }
}

int main() {
    int uart_fd;

    // Open the UART device
    uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd == -1) {
        perror("Error opening UART");
        return 1;
    }

    // Configure the UART port
    if (configureUART(uart_fd) != 0) {
        close(uart_fd);
        return 1;
    }

    // Main loop
    while (1) {
        char requestBuffer[64];
        int bytes;

        // Read incoming requests from ESP32 devices
        bytes = read(uart_fd, requestBuffer, sizeof(requestBuffer));
        if (bytes > 0) {
            requestBuffer[bytes] = '\0';

            // Check if it's an address assignment request
            if (strcmp(requestBuffer, "assign address") == 0) {
                char assignedAddress = assignAddress();
                if (assignedAddress != -1) {
                    printf("Assigned address: %c\n", predefinedAddresses[assignedAddress]);
                    // Send the assigned address to the ESP32 device
                    if (write(uart_fd, &predefinedAddresses[assignedAddress], 1) == -1) {
                        perror("Error writing to UART");
                        break;
                    }
                } else {
                    printf("No available addresses.\n");
                    // Send an error message or handle as needed
                }
            } else if (strcmp(requestBuffer, "device status") == 0) {
                int onlineCount = 0;
                for (int i = 0; i < 5; i++) {
                    if (!availableAddresses[i] && checkDeviceOnline(uart_fd, predefinedAddresses[i])) {
                        onlineCount++;
                    }
                }
                printf("Total devices online: %d\n", onlineCount);
            } else {
                // Handle other types of requests or data here
            }
        } else if (bytes == -1) {
            perror("Error reading from UART");
            break;
        }

        // Send data to the devices (you can adjust the timing as needed)
        sendDataToDevices(uart_fd);

        // Sleep for a while before reading the next request
        usleep(1000000); // 1 second
    }

    // Close the UART device
    close(uart_fd);

    return 0;
}
