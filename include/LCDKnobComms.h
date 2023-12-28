#pragma once
#include "Arduino.h"
#include <stdint.h>
#include "std_defs.h"

// These are the codes when receiving a packet from the LCDKnob
// These are used in a case statement that doesn't accept "const", so #define used instead
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

const uint8_t LCDKNOB_MAX_BUTTON = 7;
const uint8_t LCDKNOB_BUTTON_INVALID = 255;

class LCDKnobComms
{
public:
    //    LCDKnobComms(HardwareSerial &serial, unsigned long baud, uint32_t config, uint8_t rx, uint8_t tx);
    LCDKnobComms(unsigned long baud, uint32_t config, uint8_t rx, uint8_t tx);
    ~LCDKnobComms();
    void Begin();
    void SendSetMenu(uint8_t menu_number);
    void SendButtonStateRequest(uint8_t button_number);
    void SendSetButtonState(uint8_t button_number, uint8_t state);
    uint8_t ReceiveData();
    uint8_t GetLastEventType();
    uint8_t GetLastEventButtonNumber();
    uint16_t GetLastEventX();
    uint16_t GetLastEventY();

    void SetButtonMask(uint16_t newButtonMask);
    void SetButton(uint8_t button_index);
    void ClearButtonMask();
    void ClearButton(uint8_t button_index);
    bool GetButtonState(uint8_t button_index);

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
    uint8_t pinRx;
    uint8_t pinTx;
    uint8_t button_set[LCDKNOB_MAX_BUTTON] = {0};
    uint8_t longPressActive;
    char buf[64] = {0};
    uint8_t idx;
    uint8_t lastEventType;
    uint8_t lastEventButtonNumber;
    uint16_t buttonStateMask;
    uint16_t lastEventX;
    uint16_t lastEventY;
};