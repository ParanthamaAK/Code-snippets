#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Define the addresses and data payloads
char esp32_addresses[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
char data_payloads[5] = {0xFF, 0xAA, 0xFF, 0x00, 0xBC};

// Function to send data to devices
void sendDataToDevices(int uart_fd) {
    // Send data to each ESP32 device separately
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
