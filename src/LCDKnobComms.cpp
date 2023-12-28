#include "LCDKnobComms.h"

LCDKnobComms::LCDKnobComms(
    //    HardwareSerial &serial,
    unsigned long baud,
    uint32_t config,
    uint8_t rx,
    uint8_t tx)
{
    //    serialPort = serial;
    baudRate = baud;
    configParameters = config;
    pinRx = rx;
    pinTx = tx;
    idx = 0;
    ClearButtonMask();
}

LCDKnobComms::~LCDKnobComms()
{
}

void LCDKnobComms::Begin()
{
    Serial2.begin(baudRate, configParameters, pinRx, pinTx);
}

void LCDKnobComms::SendUInt8(uint8_t data)
{
    char hex_value[3] = {0};
    sprintf(hex_value, "%02X", data);
    Serial2.print(hex_value);
}

void LCDKnobComms::SendUInt16(uint16_t data)
{
    char hex_value[5] = {0};
    sprintf(hex_value, "%04X", data);
    Serial2.print(hex_value);
}

void LCDKnobComms::SendUInt32(uint32_t data)
{
    char hex_value[9] = {0};

    sprintf(hex_value, "%08X", data);
    Serial2.print(hex_value);
}

void LCDKnobComms::SendSetMenu(uint8_t menu_number)
{
    Serial2.write(LCDKNOB_SEND_SET_MENU);
    SendUInt8(menu_number);
    Serial2.write(LCDKNOB_EVENT_END);
}

void LCDKnobComms::SendButtonStateRequest(uint8_t button_number)
{
    Serial2.write(LCDKNOB_SEND_BUTTON_STATE_REQUEST);
    SendUInt8(button_number);
    Serial2.write(LCDKNOB_EVENT_END);
}

void LCDKnobComms::SendSetButtonState(uint8_t button_number, uint8_t state)
{
    Serial2.write(LCDKNOB_SEND_SET_BUTTON_STATE);
    SendUInt8(button_number);
    SendUInt8(state);
    Serial2.write(LCDKNOB_EVENT_END);
}

uint8_t LCDKnobComms::ReceiveData()
{
    lastEventType = LCDKNOB_EVENT_NONE;
    if (Serial2.available() > 0) {
        char c = Serial2.read();
        buf[idx] = c;
        // MSG_PORT.print(c,HEX);
        // MSG_PORT.print(' ');
        // MSG_PORT.println(idx);
        idx = (idx + 1) % sizeof(buf);

        if (c == LCDKNOB_EVENT_END) {
            switch (buf[0]) {
                case LCDKNOB_EVENT_CLICK: {
                    if (idx != 12) {
                        break;
                    }
                    lastEventButtonNumber = ReadUInt8(&buf[1]);
                    lastEventX = ReadUInt16(&buf[3]);
                    lastEventY = ReadUInt16(&buf[7]);
                    lastEventType = LCDKNOB_EVENT_CLICK;

                    // MSG_PORT.println("Click event received for button " + String(lastEventButtonNumber));
                    buf[idx] = 0;
                    break;
                }
                case LCDKNOB_EVENT_DBL_CLICK: {
                    if (idx != 12) {
                        break;
                    }
                    lastEventButtonNumber = ReadUInt8(&buf[1]);
                    lastEventX = ReadUInt16(&buf[3]);
                    lastEventY = ReadUInt16(&buf[7]);
                    lastEventType = LCDKNOB_EVENT_DBL_CLICK;

                    // MSG_DEBUGLN("Double click event received for button " + String(button_number));
                    buf[idx] = 0;
                    break;
                }
                case LCDKNOB_EVENT_LONG_PRESS_START: {
                    if (idx != 12) {
                        break;
                    }
                    lastEventButtonNumber = ReadUInt8(&buf[1]);
                    lastEventX = ReadUInt16(&buf[3]);
                    lastEventY = ReadUInt16(&buf[7]);
                    lastEventType = LCDKNOB_EVENT_LONG_PRESS_START;

                    // MSG_DEBUGLN("Long press start event received for button " + String(button_number));
                    buf[idx] = 0;
                    break;
                }
                case LCDKNOB_EVENT_LONG_PRESS_END: {
                    if (idx != 12) {
                        break;
                    }
                    lastEventButtonNumber = ReadUInt8(&buf[1]);
                    lastEventX = ReadUInt16(&buf[3]);
                    lastEventY = ReadUInt16(&buf[7]);
                    lastEventType = LCDKNOB_EVENT_LONG_PRESS_END;

                    // MSG_DEBUGLN("Long press end event received for button " + String(button_number));
                    buf[idx] = 0;
                    break;
                }
                case LCDKNOB_EVENT_TOUCH_XY: {
                    if (idx != 10) {
                        break;
                    }
                    lastEventX = ReadUInt16(&buf[1]);
                    lastEventY = ReadUInt16(&buf[5]);
                    lastEventType = LCDKNOB_EVENT_TOUCH_XY;

                    // MSG_DEBUGLN("TouchXY event received for button " + String(button_number));
                    buf[idx] = 0;
                    break;
                }
                case LCDKNOB_EVENT_BUTTON_STATE: {
                    if (idx != 5) {
                        break;
                    }
                    lastEventType = LCDKNOB_EVENT_BUTTON_STATE;
                    lastEventButtonNumber = ReadUInt8(&buf[1]);
                    char state = buf[3];

                    // Serial.println("Button state event received for button " + String(lastEventButtonNumber) + " state " + state);

                    if (state == '1') {
                        // Serial.println("SetButton for button " + String(lastEventButtonNumber));
                        SetButton(lastEventButtonNumber);
                    }
                    else {
                        // Serial.println("ClearButton for button " + String(lastEventButtonNumber));
                        ClearButton(lastEventButtonNumber);
                    }

 
                    buf[idx] = 0;
                    break;
                }

                default:
                    break;
            }
            idx = 0;
        }
    }
    return lastEventType;
}

uint8_t LCDKnobComms::ReadUInt8(char* hex_value)
{
    uint8_t data = 0;
    sscanf(hex_value, "%2X", &data);
    return data;
}

uint16_t LCDKnobComms::ReadUInt16(char* hex_value)
{
    uint16_t data = 0;
    sscanf(hex_value, "%4X", &data);
    return data;
}

uint32_t LCDKnobComms::ReadUInt32(char* hex_value)
{
    uint32_t data = 0;
    sscanf(hex_value, "%8X", &data);
    return data;
}

uint8_t LCDKnobComms::GetLastEventType()
{
    return lastEventType;
}

uint8_t LCDKnobComms::GetLastEventButtonNumber()
{
    return lastEventButtonNumber;
}

uint16_t LCDKnobComms::GetLastEventX()
{
    return lastEventX;
}

uint16_t LCDKnobComms::GetLastEventY()
{
    return lastEventY;
}

void LCDKnobComms::SetButtonMask(uint16_t newButtonMask)
{
    buttonStateMask = newButtonMask;
}

void LCDKnobComms::SetButton(uint8_t button_index)
{
    if ((button_index < LCDKNOB_MAX_BUTTON) && (button_index >= 0)) {
        buttonStateMask |= (1 << (button_index));
    }
    else {
        MSG_ERRORLN("[ERROR] LCDKnobComms::SetButton invalid button index");
    }
}

void LCDKnobComms::ClearButtonMask()
{
    buttonStateMask = 0;
}

void LCDKnobComms::ClearButton(uint8_t button_index)
{
    if ((button_index < LCDKNOB_MAX_BUTTON) && (button_index >= 0)) {
        buttonStateMask &= ~(1 << (button_index ));
    }
    else {
        MSG_ERRORLN("[ERROR] LCDKnobComms::ClearButton invalid button index");
    }
}
bool LCDKnobComms::GetButtonState(uint8_t button_index)
{
    if ((button_index < LCDKNOB_MAX_BUTTON) && (button_index >= 0)) {
        return (buttonStateMask & (1 << (button_index)));
    }
    else {
        MSG_ERROR1("[ERROR] LCDKnobComms::GetButtonState invalid button index ", button_index);
        return false;
    }
}