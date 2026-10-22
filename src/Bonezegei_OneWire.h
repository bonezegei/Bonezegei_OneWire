#ifndef BONEZEGEI_ONEWIRE_H
#define BONEZEGEI_ONEWIRE_H

#include <Arduino.h>

class Bonezegei_OneWire {
public:
    explicit Bonezegei_OneWire(uint8_t pin);

    void begin();

    // Master API
    uint8_t masterReset();
    void masterWriteBit(uint8_t bit);
    uint8_t masterReadBit();
    void masterWrite(uint8_t data);
    uint8_t masterRead();

    // Slave API
    bool slaveWaitReset(uint32_t timeoutMs = 2000);
    void slaveSendPresence();
    void slaveWriteBit(uint8_t bit);
    uint8_t slaveReadBit();
    void slaveWrite(uint8_t data);
    uint8_t slaveRead();

private:
    uint8_t _pin;
};

#endif