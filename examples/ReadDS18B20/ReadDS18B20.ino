#include <Bonezegei_OneWire.h>

#define ONEWIRE_PIN 4

Bonezegei_OneWire ow(ONEWIRE_PIN);

// Helper function to calculate Dallas 1-Wire CRC8
uint8_t calculateCRC8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    while (len--) {
        uint8_t inbyte = *data++;
        for (uint8_t i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

void setup() {
    Serial.begin(115200);
    ow.begin();
    Serial.println("Bonezegei OneWire - DS18B20 Sensor Reading Example");
}

void loop() {
    // 1. Send Reset pulse and check for DS18B20 presence
    if (!ow.masterReset()) {
        Serial.println("Error: No DS18B20 sensor found on bus!");
        delay(2000);
        return;
    }

    // 2. Skip ROM (0xCC) and trigger temperature conversion (0x44)
    ow.masterWrite(0xCC);
    ow.masterWrite(0x44);

    // 3. Wait for 12-bit resolution temperature conversion (750ms max)
    delay(750);

    // 4. Send Reset pulse to prepare for reading
    if (!ow.masterReset()) {
        Serial.println("Error: Sensor lost response.");
        delay(2000);
        return;
    }

    // 5. Skip ROM (0xCC) and request Scratchpad data (0xBE)
    ow.masterWrite(0xCC);
    ow.masterWrite(0xBE);

    // 6. Read 9 bytes of scratchpad memory from DS18B20
    uint8_t scratchpad[9];
    for (uint8_t i = 0; i < 9; i++) {
        scratchpad[i] = ow.masterRead();
    }

    // 7. Validate CRC (Byte 8 contains the sensor's calculated CRC)
    if (calculateCRC8(scratchpad, 8) != scratchpad[8]) {
        Serial.println("Error: Data corrupted (CRC Check Failed)!");
        delay(2000);
        return;
    }

    // 8. Convert raw bytes to temperature values
    int16_t rawTemp = (scratchpad[1] << 8) | scratchpad[0];
    float tempC = (float)rawTemp / 16.0;
    float tempF = (tempC * 1.8) + 32.0;

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print(" °C | ");
    Serial.print(tempF);
    Serial.println(" °F");

    delay(2000);
}