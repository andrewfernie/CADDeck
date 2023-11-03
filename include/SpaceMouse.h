#pragma once
#include "Arduino.h"
#include <stdint.h>
#include <std_defs.h>

class SpaceMouse
{
public:
    //    SpaceMouse(HardwareSerial &serial, unsigned long baud, uint32_t config, uint8_t rx, uint8_t tx);
    SpaceMouse(unsigned long baud, uint32_t config, uint8_t rx, uint8_t tx);
    ~SpaceMouse();
    void Init();
    void SendViewpointDataPacket(uint16_t x, uint16_t y, uint16_t z, uint16_t a, uint16_t b, uint16_t c);
    void SendKeyPacket(uint8_t);

private:
    void SendUInt16(uint16_t data);
    char EncodeNibble(uint8_t nibble);

public:
private:
    // SpaceMouse variables
    //    HardwareSerial &serialPort;
    unsigned long baudRate;
    uint32_t configParameters;
    uint8_t pinRx;
    uint8_t pinTx;

    uint8_t button_bits[15] = {12, 13, 14, 15, 22, 25, 23, 24, 0, 1, 2, 4, 5, 8, 26};

    char cmd_end;
   
    struct data_packet
    {
        uint8_t x[4];
        uint8_t y[4];
        uint8_t z[4];
        uint8_t a[4];
        uint8_t b[4];
        uint8_t c[4];
    };

    struct key_packet
    {
        uint8_t keys[3];
    };

    struct data_packet view_data;
    struct key_packet key_data;
};