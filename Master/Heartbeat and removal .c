#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Define the addresses for the slave devices
char esp32_addresses[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

// Define a list to track devices that have responded
int devices_responded[5] = {0}; // Initialized to all zeros

// Define a list of connected devices (initialized to all zeros)
int connected_devices[5] = {0};

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

                    // Update the list of connected devices
                    connected_devices[i] = 1;

                    break; // Exit the loop upon receiving status
                }
            } else if (bytes == -1) {
                perror("Error reading from UART");
                break;
            }
        }
    }

    // Remove unresponsive devices from the list of connected devices and assigned addresses
    for (int i = 0; i < 5; i++) {
        if (!devices_responded[i]) {
            printf("Device at address %c did not respond. Removing from the list.\n", esp32_addresses[i]);
            // You can add code here to remove the address from your assigned addresses list
            // Also, mark the device as disconnected in the list of connected devices
            connected_devices[i] = 0;
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Define the addresses for the slave devices
char esp32_addresses[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

// Define a list to track devices that have responded
int devices_responded[5] = {0}; // Initialized to all zeros

// Define a list of connected devices (initialized to all zeros)
int connected_devices[5] = {0};

// Function to generate a random address within a byte value
int generateRandomAddress() {
    // Generate a random number between 1 and 255 (inclusive)
    return (rand() % 255) + 1;
}

// Function to send a heartbeat request
void sendHeartbeatRequest(int uart_fd, char address) {
    char requestBuffer[64];
    // Construct the "heartbeat request" message
    snprintf(requestBuffer, sizeof(requestBuffer), "heartbeat request %c", address);
    if (write(uart_fd, requestBuffer, strlen(requestBuffer)) == -1) {
        perror("Error writing to UART");
    }
}

// Function to receive and process the response
int receiveAndProcessResponse(int uart_fd, char address) {
    char responseBuffer[64];
    int bytes;
    int timeout = 5; // Timeout in seconds

    time_t start_time = time(NULL);
    while (time(NULL) - start_time < timeout) {
        bytes = read(uart_fd, responseBuffer, sizeof(responseBuffer) - 1);
        if (bytes > 0) {
            responseBuffer[bytes] = '\0';

            // Check if the response contains status information
            if (strstr(responseBuffer, "status:")   NULL) {
                printf("Received status from device at address %c: %s\n", address, responseBuffer);
                return 1; // Mark this device as responded
            }
        } else if (bytes == -1) {
            perror("Error reading from UART");
        }
    }
    return 0;
}

// Function to handle device status
void handleDeviceStatus(int index) {
    devices_responded[index] = 1; // Mark this device as responded
    connected_devices[index] = 1;
}

// Function to cleanup unresponsive devices
void cleanupUnresponsiveDevices() {
    for (int i = 0; i < 5; i++) {
        if (!devices_responded[i]) {
            printf("Device at address %c did not respond. Removing from the list.\n", esp32_addresses[i]);
            // You can add code here to remove the address from your assigned addresses list
            // Also, mark the device as disconnected in the list of connected devices 
            connected_devices[i] = 0;
        }
    }
}


// Function to send heartbeats to all devices and process their responses
void sendHeartbeats(int uart_fd) {
    for (int i = 0; i < 5; i++) {
        char address = esp32_addresses[i];
        if (devices_responded[i]) {
            continue;
        }

        sendHeartbeatRequest(uart_fd, address);

        if (receiveAndProcessResponse(uart_fd, address)) {
            handleDeviceStatus(i);
        }
    }

    cleanupUnresponsiveDevices();
}

int main() {
    // Initialize the random number generator
    srand(time(NULL));

    // Simulate receiving a message from a slave requesting an address
    char requestMessage[] = "ADDRESS_REQ";

    // Check if the received message is "ADDRESS_REQ"
    if (strcmp(requestMessage, "ADDRESS_REQ") == 0) {
        // The slave device has requested an address

        // Example of sending heartbeats and processing responses
        sendHeartbeats(uart_fd);
    } else {
        printf("Received an unknown message from the slave device.\n");
    }

    return 0;
}
