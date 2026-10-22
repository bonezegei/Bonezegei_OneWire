#ifndef BONEZEGEI_ONEWIRE_H
#define BONEZEGEI_ONEWIRE_H

#include <Arduino.h>

class Bonezegei_OneWire {
public:
    explicit Bonezegei_OneWire(uint8_t pin);

    void begin();
    uint8_t reset();
    void write(uint8_t v);
    void writeBit(uint8_t v);
    uint8_t read();
    uint8_t readBit();
    void select(const uint8_t rom[8]);
    void skip();
    static uint8_t crc8(const uint8_t *addr, uint8_t len);

private:
    uint8_t _pin;
};

#endif