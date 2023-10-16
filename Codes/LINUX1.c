#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int assign_address();
void poll_devices(int serial_fd);
void send_data(int serial_fd, int address, int data);

// Define a pool of available addresses
static int address_pool[] = {1, 2, 3, 4, 5};
static int num_available_addresses = sizeof(address_pool) / sizeof(int);

// Dictionary to map assigned addresses to devices
static struct {
  int address;
  int device_id;
} assigned_addresses[5];

// Function to assign an address to a new device
int assign_address() {
  if (num_available_addresses == 0) {
    return -1;
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
    int address = assigned_addresses[i].address;
    write(serial_fd, request, strlen(request));

    // Read and process the response
    char response[100];
    read(serial_fd, response, sizeof(response));
  }
}

// Data transmission
void send_data(int serial_fd, int address, int data) {
  unsigned char packet[2] = {address, data};
  write(serial_fd, packet, sizeof(packet));
}

int main() {
  // Open the serial port
  int serial_fd = open("/dev/ttyUSB0", O_RDWR);
  if (serial_fd == -1) {
    perror("Error opening serial port");
    return 1;
  }

  // Set serial port settings (assuming 8N1)
  struct termios tty;
  memset(&tty, 0, sizeof(tty));
  if (tcgetattr(serial_fd, &tty) != 0) {
    perror("Error from tcgetattr");
    return 1;
  }
  cfsetospeed(&tty, B115200);
  cfsetispeed(&tty, B115200);
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
    return 1;
  }

  // Address assignment logic for new devices
  int new_address = assign_address();
  while (new_address != -1) {
    // Send a request for address and device identifier
    char request[] = "REQ_ADDRESS\n";
    write(serial_fd, request, strlen(request));

    // Read the response containing the assigned address and device ID
    char response[100];
    read(serial_fd, response, sizeof(response));

    int device_id = atoi(strtok(response, "\n"));

    // Add the new device to the list of assigned addresses
    assigned_addresses[num_available_addresses].address = new_address;
    assigned_addresses[num_available_addresses].device_id = device_id;

    num_available_addresses++;

    // Get the next available address
    new_address = assign_address();
  }

  // Polling logic to check device status
  poll_devices(serial_fd);

  // Data transmission logic
  for (int i = 0; i < num_available_addresses; i++) {
    send_data(serial_fd, assigned_addresses[i].address, 0xFF); // Example data payload
  }

  // Close the serial port when done
  close(serial_fd);

  return 0;
}

