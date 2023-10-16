#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial Serial1(1); // UART1 on ESP32

int assigned_address = -1;

void setup() {
    Serial1.begin(115200);
}

void loop() {
    // Request an address from the Linux master
    char request[] = "REQ_ADDRESS\n";
    Serial1.print(request);

    // Read the assigned address from the Linux master
    if (Serial1.available()) {
        String response = Serial1.readStringUntil('\n');
        assigned_address = response.toInt();
    }

    // Check if an address is assigned
    if (assigned_address != -1) {
        // Send status (ONLINE) to the Linux master
        String status = String(assigned_address) + "\n";
        Serial1.print(status);

        // Continue with data reception and processing
        for(int i=0; i<3; i++){
			receiveAndProcessData();
    }	
  }
}

// Data reception and processing
void receiveAndProcessData() {
    while (Serial1.available() >= 2) {
        // Read the incoming address and data bytes
        int incoming_address = Serial1.read();
        int payload = Serial1.read();

        // Check if the received address matches the assigned address
        if (incoming_address == assigned_address) {
            // Process the data based on the assigned address and payload
            processReceivedData(assigned_address, payload);
            
            // Send an acknowledgment byte to the master
            Serial1.write(payload); // Acknowledgment with the same payload
        } else {
            // Ignore data with mismatched address
            Serial.println("Ignoring Data with Mismatched Address");
        }
    }
}

// Function to process received data based on address and payload
void processReceivedData(int address, int payload) {
    // Add your code to handle the received data here
    Serial.print("Received Data - Address: ");
    Serial.print(address);
    Serial.print(", Payload: ");
    Serial.println(payload);
}

int main() {
    init();
    setup();

    while (true) {
        loop();
    }

    return 0;
}
