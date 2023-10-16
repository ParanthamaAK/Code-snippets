#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SLAVES 5

// Define an array to hold the addresses and a flag to track assigned addresses
int slaveAddresses[NUM_SLAVES];
bool addressAssigned[NUM_SLAVES];

// Function to initialize the address pool
void initializeAddressPool() {
    for (int i = 0; i < NUM_SLAVES; i++) {
        slaveAddresses[i] = i + 1; // Start with address 1 and increment
        addressAssigned[i] = false; // Mark addresses as unassigned
    }
}

// Function to assign an address to a slave
int assignAddressToSlave() {
    for (int i = 0; i < NUM_SLAVES; i++) {
        if (!addressAssigned[i]) {
            addressAssigned[i] = true; // Mark the address as assigned
            return slaveAddresses[i]; // Return the assigned address
        }
    }
    return -1; // No available addresses
}

int main() {
    // Initialize the address pool
    initializeAddressPool();

    // Assign addresses to 3 initial online slave devices
    for (int i = 0; i < 3; i++) {
        int address = assignAddressToSlave();
        if (address != -1) {
            printf("Assigned address %d to Slave %d\n", address, i + 1);
        } else {
            printf("No available addresses for Slave %d\n", i + 1);
        }
    }

    // Simulate a new device coming online at a random moment
    srand(time(NULL));
    int randomDeviceIndex = rand() % NUM_SLAVES;
    if (!addressAssigned[randomDeviceIndex]) {
        int address = assignAddressToSlave();
        if (address != -1) {
            printf("Assigned address %d to the new online device\n", address);
        } else {
            printf("No available addresses for the new online device\n");
        }
    }

    return 0;
}
