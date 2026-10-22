#include <Bonezegei_OneWire.h>

#define ONEWIRE_PIN 4

Bonezegei_OneWire ow(ONEWIRE_PIN);

void setup() {
    Serial.begin(115200);
    ow.begin();
    Serial.println("Bonezegei OneWire - Slave MCU initialized");
}

void loop() {
    // Listen for Master's Reset pulse
    if (ow.slaveWaitReset(5000)) {
        // Send Presence pulse back to Master MCU
        ow.slaveSendPresence();

        // Read incoming Command Byte from Master MCU
        uint8_t cmd = ow.slaveRead();
        Serial.print("Slave: Received command -> 0x");
        Serial.println(cmd, HEX);

        if (cmd == 0xA5) {
            // Send Reply Byte back to Master MCU
            ow.slaveWrite(0x7E);
            Serial.println("Slave: Replied with 0x7E");
        }
    }
}