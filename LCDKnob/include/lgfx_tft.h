#pragma once
#define LGFX_USE_V1
#include <Arduino.h>
#include <LovyanGFX.hpp>

class LGFX_TFT : public lgfx::LGFX_Device
{
    lgfx::Panel_GC9A01 _panel_instance;

    lgfx::Bus_SPI _bus_instance;

    uint8_t _bl_pin;

public:
    LGFX_TFT(uint16_t SCREEN_WIDTH, uint16_t SCREEN_HEIGHT, uint8_t bl_pin=-1);
    void BLOn();
    void BLOff();
};