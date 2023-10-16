#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial Serial1(1); // UART1 on ESP32

int assigned_address = -1;

void setup() {
    Serial1.begin(115200);
}

void loop() {
    // Check for address request from the master
    if (Serial1.available()) {
        String request = Serial1.readStringUntil('\n');
        if (request == "REQ_ADDRESS\n") {
            // Assign an address to this device and respond
            assigned_address = assignAddress();
            Serial1.print(assigned_address);
            Serial1.print('\n');
        }
    }

    // Polling logic to report device status (ONLINE)
    if (assigned_address != -1) {
        String status = "ONLINE\n";
        Serial1.print(assigned_address);
        Serial1.print(":");
        Serial1.print(status);
    }

    // Check for data reception
    if (Serial1.available() >= 2) {
        byte receivedAddress = Serial1.read();
        byte data = Serial1.read();

        // Process the received data if it matches the assigned address
        if (receivedAddress == assigned_address) {
            // Implement your data processing logic here
        }
    }
}

int assignAddress() {
    // You can implement your own logic to assign unique addresses.
    // For simplicity, we'll use a static address for demonstration purposes.
    return 1;
}
