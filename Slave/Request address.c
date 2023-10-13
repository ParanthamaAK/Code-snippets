#include <Arduino.h>

#define Rxpin 
#define TXpin

int assignedAddress = -1; // Initialize with an invalid address

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, Rxpin, TXpin);
}

void loop() {
  char data[64];
  
  if (assignedAddress == -1) {
    char request[] = "Request_Address";
    Serial2.write(request);
    
    // Wait for the master to send the address
    while (Serial2.available() == 0) {
      // Wait for data from the master
    }

    // Read the received address
    int receivedAddress = Serial2.parseInt();
    
    if (receivedAddress > 0) {
      // Store the received address as the assigned address
      assignedAddress = receivedAddress;
      Serial.print("Assigned Address: ");
      Serial.println(assignedAddress);
    } else {
      Serial.println("Invalid Address Received");
    }
  }

  // Your main code logic goes here
  // Handle data reception and processing based on assignedAddress
}
