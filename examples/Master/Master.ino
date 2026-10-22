#include <Bonezegei_OneWire.h>

#define ONEWIRE_PIN 4

Bonezegei_OneWire ow(ONEWIRE_PIN);

void setup() {
    Serial.begin(115200);
    ow.begin();
    Serial.println("Bonezegei OneWire - Master MCU initialized");
}

void loop() {
    Serial.println("Master: Resetting bus...");
    if (ow.masterReset()) {
        Serial.println("Master: Slave detected!");

        // Send Command Byte 0xA5 to Slave MCU
        ow.masterWrite(0xA5);

        // Read response byte back from Slave MCU
        uint8_t response = ow.masterRead();
        Serial.print("Master: Received from Slave -> 0x");
        Serial.println(response, HEX);
    } else {
        Serial.println("Master: No Slave presence detected.");
    }

    delay(3000);
}