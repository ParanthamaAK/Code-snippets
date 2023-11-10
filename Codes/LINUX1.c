#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// Define the maximum number of available addresses
#define MAX_AVAILABLE_ADDRESSES 5

// Data structure to store assigned addresses and device IDs
struct AssignedAddress {
    int address;
    int device_id;
};

// Function to open and configure the serial port
int setupSerialPort(const char *portName, speed_t baudRate) {
    int serial_fd = open(portName, O_RDWR);
    if (serial_fd == -1) {
        perror("Error opening serial port");
        exit(1);
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(serial_fd, &tty) != 0) {
        perror("Error from tcgetattr");
        exit(1);
    }

    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        exit(1);
    }

    return serial_fd;
}

// Function to assign an address to a new device
int assignAddress(int *addressPool, int *numAvailableAddresses) {
    if (*numAvailableAddresses == 0) {
        return -1;
    }

    int address = addressPool[0];
    (*numAvailableAddresses)--;

    for (int i = 0; i < *numAvailableAddresses; i++) {
        addressPool[i] = addressPool[i + 1];
    }

    return address;
}

// Function to poll devices and check their status
void pollDevices(int serial_fd, struct AssignedAddress *assignedAddresses, int numAvailableAddresses) {
    for (int i = 0; i < numAvailableAddresses; i++) {
        // Send a status request
        char request[] = "REQ_STATUS\n";
        int address = assignedAddresses[i].address;
        write(serial_fd, request, strlen(request));
        // Read and process the response
        char response[100];
        read(serial_fd, response, sizeof(response));
    }
}

// Function to send data to devices
void sendData(int serial_fd, int address, int data) {
    unsigned char packet[2] = {address, data};
    write(serial_fd, packet, sizeof(packet));
}

int main() {
    // Open and configure the serial port
    const char *portName = "/dev/ttyUSB0"; // Change to your port name
    speed_t baudRate = B115200; // Change to your baud rate
    int serial_fd = setupSerialPort(portName, baudRate);
    
    // Initialize the list of assigned addresses and device IDs
    struct AssignedAddress assignedAddresses[MAX_AVAILABLE_ADDRESSES];
    int addressPool[MAX_AVAILABLE_ADDRESSES] = {1, 2, 3, 4, 5};
    int numAvailableAddresses = MAX_AVAILABLE_ADDRESSES;

    // Address assignment logic for new devices
    int newAddress = assignAddress(addressPool, &numAvailableAddresses);
    while (newAddress != -1) {
        // Send a request for address and device identifier
        char request[] = "REQ_ADDRESS\n";
        write(serial_fd, request, strlen(request));

        // Read the response containing the assigned address and device ID
        char response[100];
        read(serial_fd, response, sizeof(response));

        int deviceID = atoi(strtok(response, "\n"));

        // Add the new device to the list of assigned addresses
        assignedAddresses[numAvailableAddresses].address = newAddress;
        assignedAddresses[numAvailableAddresses].device_id = deviceID;
        numAvailableAddresses++;

        // Get the next available address
        newAddress = assignAddress(addressPool, &numAvailableAddresses);
    }

    // Polling logic to check device status
    pollDevices(serial_fd, assignedAddresses, numAvailableAddresses);

    // Data transmission logic
    for (int i = 0; i < numAvailableAddresses; i++) {
        sendData(serial_fd, assignedAddresses[i].address, 0xFF); // Example data payload
    }

    // Close the serial port when done
    close(serial_fd);

    return 0;
}