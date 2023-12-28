#pragma once
#include "Arduino.h"
#include <stdint.h>

const uint8_t LCDKNOB_MAX_BUTTON = 7;
const uint8_t BUTTON_INVALID = 255;

#define LCDKNOB_EVENT_END '\n'
#define LCDKNOB_EVENT_NONE 'j'
#define LCDKNOB_EVENT_CLICK 'k'
#define LCDKNOB_EVENT_DBL_CLICK 'm'
#define LCDKNOB_EVENT_LONG_PRESS_START 'n'
#define LCDKNOB_EVENT_LONG_PRESS_END 'o'
#define LCDKNOB_EVENT_TOUCH_XY 'p'
#define LCDKNOB_EVENT_BUTTON_STATE 'q'

#define LCDKNOB_SEND_BUTTON_STATE_REQUEST 't'
#define LCDKNOB_SEND_SET_BUTTON_STATE 'u'
#define LCDKNOB_SEND_SET_MENU 'v'

class SerialChannel
{
public:
    //    SerialChannel(HardwareSerial &serial, unsigned long baud, uint32_t config, uint8_t rx, uint8_t tx);
    SerialChannel(unsigned long baud, uint32_t config, uint8_t rx, uint8_t tx);
    ~SerialChannel();
    void Begin();
    void SendClickEvent(uint8_t button_number, uint16_t x, uint16_t y);
    void SendDblClickEvent(uint8_t button_number, uint16_t x, uint16_t y);
    void SendLongPressStartEvent(uint8_t button_number, uint16_t x, uint16_t y);
    void SendLongPressEndEvent(uint8_t button_number, uint16_t x, uint16_t y);
    void SendTouchXY(uint16_t x, uint16_t y);
    void SendButtonState(uint8_t button_number, bool isLatched);
    uint8_t ReceiveData();
    uint8_t GetLastEventType();
    uint8_t GetLastEventButtonNumber();
    uint16_t GetLastEventX();
    uint16_t GetLastEventY();
    uint8_t GetLastEventMenuNumber();
    uint8_t GetLastEventState();

private:
    void SendUInt8(uint8_t data);
    void SendUInt16(uint16_t data);
    void SendUInt32(uint32_t data);
    uint8_t ReadUInt8(char *hex_value);
    uint16_t ReadUInt16(char *hex_value);
    uint32_t ReadUInt32(char *hex_value);

   public:


private:
    unsigned long baudRate;
    uint32_t configParameters;
    uint32_t buttonMask;
    uint32_t lastButtonMask;
    uint8_t pinRx;
    uint8_t pinTx;
    uint8_t button_set[LCDKNOB_MAX_BUTTON] = {0};
    uint8_t longPressActive;
    char buf[64] = {0};
    uint8_t idx;
    uint8_t lastEventType;
    uint8_t lastEventMenuNumber;
    uint8_t lastEventButtonNumber;
    uint8_t lastEventState;
    uint16_t buttonStateMask;
    uint16_t lastEventX;
    uint16_t lastEventY;
};