#include <HardwareSerial.h>

// Define the ESP32's predefined address (A, B, C, D, or E)
char DEVICE_ADDRESS = ' ';

// Define the serial communication object
HardwareSerial SerialESP32(2); // Use Serial2 for ESP32 (adjust as needed)

void setup() {
  SerialESP32.begin(115200); // Set the baud rate to match the master
  SerialESP32.setTimeout(100); // Set a timeout for reading serial data
  SerialESP32.println("Waiting for address assignment from master...");
}

void loop() {
  // Request address assignment from the master
  SerialESP32.println("assign address");

  // Wait for and read the assigned address from the master
  while (SerialESP32.available() < 1);
  DEVICE_ADDRESS = SerialESP32.read();

  // Print the assigned address
  SerialESP32.print("Assigned address: ");
  SerialESP32.println(DEVICE_ADDRESS);

  // Continue to listen for data packets and device status requests
  while (1) {
    if (SerialESP32.available() > 0) {
      // Read the incoming command from the master
      String command = SerialESP32.readStringUntil('\n');
      command.trim();

      if (command.equals("device status")) {
        // Send an "online" reply to the master
        SerialESP32.println("online");
      } else if (command.equals("data request")) {
        // Process data request (replace this with your data handling code)
        processDataRequest();
      }
    }

    // Wait for and read the data packet from the master
    if (SerialESP32.available() >= 2) {
      char receivedData[2];
      SerialESP32.readBytes(receivedData, 2);

      // Extract the received address (first 4 bits)
      char receivedAddress = (receivedData[0] >> 4) & 0x0F;

      // Check if the received address matches this device's address
      if (receivedAddress == (DEVICE_ADDRESS - 'A')) {
        // Extract the 12 bits of data
        int receivedValue = (((receivedData[0] & 0x0F) << 8) | receivedData[1]) & 0xFFF;

        // Process the received data here
        SerialESP32.print("Received Data: ");
        SerialESP32.println(receivedValue, HEX);
      }
    }

    // Add a delay or other processing as needed
    delay(1000);
  }
}

void processDataRequest() {
  // Replace this function with your data handling code
  // For example, you can send data back to the master
  // when it requests data from this device.
  //SerialESP32.println("Sending data to master...");
}
