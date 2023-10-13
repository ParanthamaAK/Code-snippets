#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to generate a random address within a byte value
int generateRandomAddress() {
    // Seed the random number generator with the current time
    srand(time(NULL));
    
    // Generate a random number between 1 and 255 (inclusive)
    return (rand() % 255) + 1;
}

// Function to assign an address to a new device
int assignAddress() {
    int newAddress = generateRandomAddress();
    
    // Check if the generated address is already assigned
    // You should replace this with your own logic to check against the pool of assigned addresses
    // For simplicity, we'll assume that the address is unique here
    return newAddress;
}

int main() {
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
            printf("Assigned address %d to the slave device.\n", assignedAddress);
        } else {
            printf("No available addresses.\n");
        }
    } else {
        printf("Received an unknown message from the slave device.\n");
    }

    return 0;
}
