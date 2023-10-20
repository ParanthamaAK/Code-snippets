#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// Define a pool of available addresses
static int address_pool[] = {1, 2, 3, 4, 5};
static int num_available_addresses = sizeof(address_pool) / sizeof(int);

// Function to assign an address to a new device
int assign_address() {
    if (num_available_addresses == 0) {
        return -1; // No available addresses
    }
	
    int address = address_pool[0];
    num_available_addresses--;

    for (int i = 0; i < num_available_addresses; i++) {
        address_pool[i] = address_pool[i + 1];
    }
	
    return address;
}

// Polling logic
void poll_devices(int serial_fd) {
    // Iterate through the list of assigned addresses
    for (int i = 0; i < num_available_addresses; i++) {
        // Send a status request
        char request[] = "REQ_STATUS\n";
        int address = address_pool[i];
        write(serial_fd, request, strlen(request));

        // Read and process the response
        char response[100];
        read(serial_fd, response, sizeof(response));
        printf("Device at Address %d Status: %s", address, response);
    }
}

// Data transmission
void send_data(int serial_fd, int address, int data) {
    unsigned char packet[2] = {address, data};
    write(serial_fd, packet, sizeof(packet));
}

// Function to open and configure the serial port
int openSerialPort(const char *portName, speed_t baudRate) {
    int serial_fd = open(portName, O_RDWR);
    if (serial_fd == -1) {
        perror("Error opening serial port");
        exit(1);
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty);
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

int main() {
    // Open and configure the serial port
    const char *portName = "/dev/ttyUSB0"; // Change to your port name
    speed_t baudRate = B115200; // Change to your baud rate
    int serial_fd = openSerialPort(portName, baudRate);

    // Initialize the list of assigned addresses
    int assigned_addresses[num_available_addresses];

    while (1) {
        // Wait for a request for address
        char request[100];
        read(serial_fd, request, sizeof(request));

        if (strcmp(request, "REQ_ADDRESS\n") == 0) {
            int new_address = assign_address();
            if (new_address != -1) {
                // Send the assigned address as a response
                char response[100];
                snprintf(response, sizeof(response), "%d\n", new_address);
                write(serial_fd, response, strlen(response));
            }
        } else {
            // Handle other types of requests or data here
        }

        // Polling logic to check device status
        poll_devices(serial_fd);

        // Data transmission logic
        for (int i = 0; i < num_available_addresses; i++) {
            send_data(serial_fd, address_pool[i], 0xFF); // Example data payload
        }
    }

    // Close the serial port when done (this won't be reached)
    close(serial_fd);

    return 0;
}
