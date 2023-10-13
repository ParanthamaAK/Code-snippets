#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Maximum number of addresses to be assigned
#define MAX_ASSIGNED_ADDRESSES 5

// Data structure to store assigned addresses
int assigned_addresses[MAX_ASSIGNED_ADDRESSES];
int num_assigned_addresses = 0;

// Function to assign an address to a new device
int assignAddress() {
    // Your logic for assigning a new address goes here
    // For simplicity, we'll generate a random address in this example
    int newAddress;

    // Generate a random address until it's not in the array of assigned addresses
    do {
        newAddress = generateRandomAddress();
    } while (isAddressAssigned(newAddress));

    // Check if the pool is full
    if (num_assigned_addresses >= MAX_ASSIGNED_ADDRESSES) {
        printf("Maximum number of addresses assigned. Cannot assign more.\n");
        return -1; // Pool is full
    }

    // Add the new address to the pool
    assigned_addresses[num_assigned_addresses] = newAddress;
    num_assigned_addresses++;

    return newAddress;
}

// Function to check if an address is already assigned
bool isAddressAssigned(int address) {
    for (int i = 0; i < num_assigned_addresses; i++) {
        if (assigned_addresses[i] == address) {
            return true; // Address is already in the pool
        }
    }
    return false; // Address is not in the pool
}

// Function to generate a random address within a byte value
int generateRandomAddress() {
    // Your logic for generating a random address goes here
    // For simplicity, we'll return a random number between 1 and 255
    return (rand() % 255) + 1;
}

int main() {
    // Simulate assigning addresses to devices
    int assignedAddress1 = assignAddress();
    int assignedAddress2 = assignAddress();
    int assignedAddress3 = assignAddress();
    int assignedAddress4 = assignAddress();

    // Attempt to assign the same address again
    int assignedAddress5 = assignAddress(); // This should fail due to duplicate
    int assignedAddress6 = assignAddress(); // This should fail due to pool being full

    // Print the assigned addresses
    printf("Assigned addresses:\n");
    for (int i = 0; i < num_assigned_addresses; i++) {
        printf("%d\n", assigned_addresses[i]);
    }

    return 0;
}
