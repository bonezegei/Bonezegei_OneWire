#include <Bonezegei_OneWire.h>

#define ONEWIRE_PIN 4

Bonezegei_OneWire wire(ONEWIRE_PIN);

void setup() {
    Serial.begin(115200);
    wire.begin();
    Serial.println("Bonezegei OneWire - DS18B20 Demo");
}

void loop() {
    if (!wire.reset()) {
        Serial.println("No 1-Wire device found on bus!");
        delay(2000);
        return;
    }

    wire.skip();          // Skip ROM command
    wire.write(0x44);     // Start temperature conversion

    delay(750);           // 12-bit conversion time delay

    if (!wire.reset()) return;
    wire.skip();
    wire.write(0xBE);     // Read Scratchpad

    uint8_t data[9];
    for (int i = 0; i < 9; i++) {
        data[i] = wire.read();
    }

    uint8_t crc = Bonezegei_OneWire::crc8(data, 8);
    if (crc != data[8]) {
        Serial.println("CRC Check Failed!");
        delay(2000);
        return;
    }

    int16_t rawTemp = (data[1] << 8) | data[0];
    float celsius = (float)rawTemp / 16.0;

    Serial.print("Temperature: ");
    Serial.print(celsius);
    Serial.println(" °C");

    delay(2000);
}