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

        // Send data to the Linux master (replace with your data)
        send_data(0xFF); // Example data payload

        // Continue with other tasks
    }
}

// Data transmission
void send_data(int data) {
    Serial1.write(assigned_address); // Send the assigned address
    Serial1.write(data); // Send the data
}

int main() {
    init();
    setup();

    while (true) {
        loop();
    }

    return 0;
}
