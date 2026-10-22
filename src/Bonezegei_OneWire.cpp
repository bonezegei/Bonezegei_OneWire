#include "Bonezegei_OneWire.h"

Bonezegei_OneWire::Bonezegei_OneWire(uint8_t pin) {
    _pin = pin;
}

void Bonezegei_OneWire::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

// --- MASTER METHODS ---

uint8_t Bonezegei_OneWire::masterReset() {
    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(2);

    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(480);

    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(70);
    uint8_t presence = !digitalRead(_pin);
    delayMicroseconds(410);

    return presence;
}

void Bonezegei_OneWire::masterWriteBit(uint8_t bit) {
    noInterrupts();
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    if (bit & 1) {
        delayMicroseconds(6);
        pinMode(_pin, INPUT_PULLUP);
        delayMicroseconds(64);
    } else {
        delayMicroseconds(60);
        pinMode(_pin, INPUT_PULLUP);
        delayMicroseconds(10);
    }
    interrupts();
}

uint8_t Bonezegei_OneWire::masterReadBit() {
    noInterrupts();
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(6);
    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(9);
    uint8_t r = digitalRead(_pin);
    delayMicroseconds(55);
    interrupts();
    return r;
}

void Bonezegei_OneWire::masterWrite(uint8_t data) {
    for (uint8_t bitMask = 0x01; bitMask != 0; bitMask <<= 1) {
        masterWriteBit((bitMask & data) ? 1 : 0);
    }
}

uint8_t Bonezegei_OneWire::masterRead() {
    uint8_t data = 0;
    for (uint8_t bitMask = 0x01; bitMask != 0; bitMask <<= 1) {
        if (masterReadBit()) {
            data |= bitMask;
        }
    }
    return data;
}

// --- SLAVE METHODS ---

bool Bonezegei_OneWire::slaveWaitReset(uint32_t timeoutMs) {
    pinMode(_pin, INPUT_PULLUP);
    uint32_t start = millis();

    // Wait until line goes LOW driven by Master reset
    while (digitalRead(_pin) == HIGH) {
        if (millis() - start > timeoutMs) return false;
    }

    uint32_t lowStart = micros();
    // Wait until line returns HIGH
    while (digitalRead(_pin) == LOW) {
        if (micros() - lowStart > 1000) break; 
    }
    uint32_t lowDuration = micros() - lowStart;

    // Standard 1-Wire reset pulse is ~480us (valid if > 300us)
    return (lowDuration >= 300);
}

void Bonezegei_OneWire::slaveSendPresence() {
    delayMicroseconds(30);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delayMicroseconds(120);
    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(300);
}

uint8_t Bonezegei_OneWire::slaveReadBit() {
    // Wait for Master to initiate time-slot (pull line LOW)
    while (digitalRead(_pin) == HIGH);
    
    delayMicroseconds(15); // Sample near middle of slot
    uint8_t bit = digitalRead(_pin);
    
    // Wait for slot completion
    while (digitalRead(_pin) == LOW);
    return bit;
}

void Bonezegei_OneWire::slaveWriteBit(uint8_t bit) {
    // Wait for Master to initiate time-slot
    while (digitalRead(_pin) == HIGH);

    if (bit == 0) {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
        delayMicroseconds(45);
        pinMode(_pin, INPUT_PULLUP);
    }
    while (digitalRead(_pin) == LOW);
}

void Bonezegei_OneWire::slaveWrite(uint8_t data) {
    for (uint8_t bitMask = 0x01; bitMask != 0; bitMask <<= 1) {
        slaveWriteBit((data & bitMask) ? 1 : 0);
    }
}

uint8_t Bonezegei_OneWire::slaveRead() {
    uint8_t data = 0;
    for (uint8_t bitMask = 0x01; bitMask != 0; bitMask <<= 1) {
        if (slaveReadBit()) {
            data |= bitMask;
        }
    }
    return data;
}