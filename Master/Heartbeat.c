#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Define the addresses for the slave devices
char esp32_addresses[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

// Define a list to track devices that have responded
int devices_responded[5] = {0}; // Initialized to all zeros

// Function to send a heartbeat and receive status from all devices
void sendHeartbeat(int uart_fd) {
    // Iterate through the addresses of slave devices
    for (int i = 0; i < 5; i++) {
        char address = esp32_addresses[i];
        char requestBuffer[64];
        char responseBuffer[64];
        int bytes;
        int timeout = 5; // Timeout in seconds

        // Check if this device has already responded
        if (devices_responded[i]) {
            continue;
        }

        // Construct the "heartbeat request" message
        snprintf(requestBuffer, sizeof(requestBuffer), "heartbeat request %c", address);

        // Send the "heartbeat request" to the ESP32 device
        if (write(uart_fd, requestBuffer, strlen(requestBuffer)) == -1) {
            perror("Error writing to UART");
            break;
        }

        // Wait for a response from the ESP32 device within the timeout
        time_t start_time = time(NULL);
        while (time(NULL) - start_time < timeout) {
            bytes = read(uart_fd, responseBuffer, sizeof(responseBuffer) - 1);
            if (bytes > 0) {
                responseBuffer[bytes] = '\0';

                // Check if the response contains status information
                if (strstr(responseBuffer, "status:") != NULL) {
                    printf("Received status from device at address %c: %s\n", address, responseBuffer);
                    devices_responded[i] = 1; // Mark this device as responded
                    break; // Exit the loop upon receiving status
                }
            } else if (bytes == -1) {
                perror("Error reading from UART");
                break;
            }
        }
    }

    // Remove unresponsive devices from the list of assigned addresses
    for (int i = 0; i < 5; i++) {
        if (!devices_responded[i]) {
            printf("Device at address %c did not respond. Removing from the list.\n", esp32_addresses[i]);
            // You can add code here to remove the address from your assigned addresses list
        }
    }
}
