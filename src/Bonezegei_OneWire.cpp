#include "Bonezegei_OneWire.h"

Bonezegei_OneWire::Bonezegei_OneWire(uint8_t pin) {
    _pin = pin;
}

void Bonezegei_OneWire::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

uint8_t Bonezegei_OneWire::reset() {
    uint8_t presence = 0;
    uint8_t retries = 125;

    pinMode(_pin, INPUT_PULLUP);

    while (!digitalRead(_pin)) {
        if (--retries == 0) return 0;
        delayMicroseconds(2);
    }

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(480);

    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(70);
    presence = !digitalRead(_pin);
    delayMicroseconds(410);

    return presence;
}

void Bonezegei_OneWire::writeBit(uint8_t v) {
    if (v & 1) {
        noInterrupts();
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
        delayMicroseconds(6);
        pinMode(_pin, INPUT_PULLUP);
        delayMicroseconds(64);
        interrupts();
    } else {
        noInterrupts();
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
        delayMicroseconds(60);
        pinMode(_pin, INPUT_PULLUP);
        delayMicroseconds(10);
        interrupts();
    }
}

uint8_t Bonezegei_OneWire::readBit() {
    uint8_t r;

    noInterrupts();
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(6);
    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(9);
    r = digitalRead(_pin);
    delayMicroseconds(55);
    interrupts();

    return r;
}

void Bonezegei_OneWire::write(uint8_t v) {
    for (uint8_t bitMask = 0x01; bitMask != 0; bitMask <<= 1) {
        writeBit((bitMask & v) ? 1 : 0);
    }
}

uint8_t Bonezegei_OneWire::read() {
    uint8_t r = 0;

    for (uint8_t bitMask = 0x01; bitMask != 0; bitMask <<= 1) {
        if (readBit()) {
            r |= bitMask;
        }
    }

    return r;
}

void Bonezegei_OneWire::select(const uint8_t rom[8]) {
    write(0x55); // Match ROM
    for (uint8_t i = 0; i < 8; i++) {
        write(rom[i]);
    }
}

void Bonezegei_OneWire::skip() {
    write(0xCC); // Skip ROM
}

uint8_t Bonezegei_OneWire::crc8(const uint8_t *addr, uint8_t len) {
    uint8_t crc = 0;

    while (len--) {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}