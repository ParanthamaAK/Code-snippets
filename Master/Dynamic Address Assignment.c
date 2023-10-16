#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Define the maximum number of addresses
#define MAX_ADDRESSES 255

// Array to track assigned addresses
bool assignedAddresses[MAX_ADDRESSES];

// Function to generate a random address within a byte value
int generateRandomAddress() {
    // Generate a random number between 1 and 255 (inclusive)
    return (rand() % 255) + 1;
}

// Function to assign a new address
int assignAddress() {
    // Attempt to assign an address up to MAX_ADDRESSES times
    for (int i = 0; i < MAX_ADDRESSES; i++) {
        int newAddress = generateRandomAddress();
        if (!assignedAddresses[newAddress]) {
            assignedAddresses[newAddress] = true;
            return newAddress;
        }
    }
    // All addresses are assigned
    return -1;
}

// Function to release an address
void releaseAddress(int address) {
    if (address >= 1 && address <= 255) {
        assignedAddresses[address] = false;
    }
}

int main() {
    // Initialize the random number generator
    srand(time(NULL));

    // Simulate receiving a message from a slave requesting an address
    char requestMessage[] = "ADDRESS_REQ";

    // Check if the received message is "ADDRESS_REQ"
    if (strcmp(requestMessage, "ADDRESS_REQ") == 0) {
        // The slave device has requested an address

        // Example of assigning an address to the slave device
        int assignedAddress = -1;
        int newAddress = assignAddress();

        if (newAddress != -1) {
            // Assign the new address to the slave device
            assignedAddress = newAddress;
            printf("Assigned address %d to the slave device.\n");
        } else {
            printf("No available addresses.\n");
        }

        // Simulate releasing an address (for example, when a device disconnects)
        if (assignedAddress != -1) {
            releaseAddress(assignedAddress);
            printf("Released address %d.\n", assignedAddress);
        }
    } else {
        printf("Received an unknown message from the slave device.\n");
    }

    return 0;
}
