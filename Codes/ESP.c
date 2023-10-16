#include <HardwareSerial.h>

// Define the ESP32's static address (0x01 to 0x05)
char DEVICE_ADDRESS = 0x01; // Change this address for each ESP32

// Define the serial communication object
HardwareSerial SerialESP32(2); // Use Serial2 for ESP32 (adjust as needed)

void setup() {
  SerialESP32.begin(115200); // Set the baud rate to match the master
  SerialESP32.setTimeout(100); // Set a timeout for reading serial data
  SerialESP32.print("Assigned address: 0x");
  SerialESP32.println(DEVICE_ADDRESS, HEX);
}

void loop() {
  // Continue to listen for data packets
  while (1) {
    // Wait for and read the data packet from the master
    if (SerialESP32.available() >= 2) {
      char receivedData[2];
      SerialESP32.readBytes(receivedData, 2);

      // Extract the received address (8 bits)
      char receivedAddress = receivedData[0];

      // Check if the received address matches this device's address
      if (receivedAddress == DEVICE_ADDRESS) {
        // Extract the data (8 bits)
        char receivedDataValue = receivedData[1];

        // Process the received data here
        SerialESP32.print("Received Data: 0x");
        SerialESP32.println(receivedDataValue, HEX);
      }
    }
    
    // Add a delay or other processing as needed
    delay(1000);
  }
}
