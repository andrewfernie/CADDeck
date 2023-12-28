#include "SerialChannel.h"

SerialChannel::SerialChannel(
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
}

SerialChannel::~SerialChannel()
{
}

void SerialChannel::Begin()
{
    Serial1.begin(baudRate, configParameters, pinRx, pinTx);
}

void SerialChannel::SendUInt8(uint8_t data)
{
    char hex_value[3] = {0};
    sprintf(hex_value, "%02X", data);
    Serial1.print(hex_value);
}

void SerialChannel::SendUInt16(uint16_t data)
{
    char hex_value[5] = {0};
    sprintf(hex_value, "%04X", data);
    Serial1.print(hex_value);
}

void SerialChannel::SendUInt32(uint32_t data)
{
    char hex_value[9] = {0};

    sprintf(hex_value, "%08X", data);
    Serial1.print(hex_value);
}

void SerialChannel::SendClickEvent(uint8_t button_number, uint16_t x, uint16_t y)
{
    Serial1.write(LCDKNOB_EVENT_CLICK);
    SendUInt8(button_number);
    SendUInt16(x);
    SendUInt16(y);
    Serial1.write(LCDKNOB_EVENT_END);
}

void SerialChannel::SendDblClickEvent(uint8_t button_number, uint16_t x, uint16_t y)
{
    Serial1.write(LCDKNOB_EVENT_DBL_CLICK);
    SendUInt8(button_number);
    SendUInt16(x);
    SendUInt16(y);
    Serial1.write(LCDKNOB_EVENT_END);
}

void SerialChannel::SendLongPressStartEvent(uint8_t button_number, uint16_t x, uint16_t y)
{
    Serial1.write(LCDKNOB_EVENT_LONG_PRESS_START);
    SendUInt8(button_number);
    SendUInt16(x);
    SendUInt16(y);
    Serial1.write(LCDKNOB_EVENT_END);
}

void SerialChannel::SendLongPressEndEvent(uint8_t button_number, uint16_t x, uint16_t y)
{
    Serial1.write(LCDKNOB_EVENT_LONG_PRESS_END);
    SendUInt8(button_number);
    SendUInt16(x);
    SendUInt16(y);
    Serial1.write(LCDKNOB_EVENT_END);
}

void SerialChannel::SendTouchXY(uint16_t x, uint16_t y)
{
    Serial1.write(LCDKNOB_EVENT_TOUCH_XY);
    SendUInt16(x);
    SendUInt16(y);
    Serial1.write(LCDKNOB_EVENT_END);
}

void SerialChannel::SendButtonState(uint8_t button_number, bool isLatched)
{
    Serial1.write(LCDKNOB_EVENT_BUTTON_STATE);
    SendUInt8(button_number);
    Serial1.write(isLatched ? "0" : "1");
    Serial1.write(LCDKNOB_EVENT_END);
}

uint8_t SerialChannel::ReceiveData()
{
    lastEventType = LCDKNOB_EVENT_NONE;
    if (Serial1.available() > 0) {
        char c = Serial1.read();
        buf[idx] = c;
        // MSG_PORT.print(c,HEX);
        // MSG_PORT.print(' ');
        // MSG_PORT.println(idx);
        idx = (idx + 1) % sizeof(buf);

        if (c == LCDKNOB_EVENT_END) {
            // switch (buf[0]) {
            //     case LCDKNOB_EVENT_CLICK: {
            //         if (idx != 12) {
            //             break;
            //         }
            //         lastEventButtonNumber = ReadUInt8(&buf[1]);
            //         lastEventX = ReadUInt16(&buf[3]);
            //         lastEventY = ReadUInt16(&buf[7]);
            //         lastEventType = LCDKNOB_EVENT_CLICK;

            //         // MSG_PORT.println("Click event received for button " + String(lastEventButtonNumber));
            //         buf[idx] = 0;
            //         break;
            //     }
            //     case LCDKNOB_EVENT_DBL_CLICK: {
            //         if (idx != 12) {
            //             break;
            //         }
            //         lastEventButtonNumber = ReadUInt8(&buf[1]);
            //         lastEventX = ReadUInt16(&buf[3]);
            //         lastEventY = ReadUInt16(&buf[7]);
            //         lastEventType = LCDKNOB_EVENT_DBL_CLICK;

            //         // MSG_DEBUGLN("Double click event received for button " + String(button_number));
            //         buf[idx] = 0;
            //         break;
            //     }
            //     case LCDKNOB_EVENT_LONG_PRESS_START: {
            //         if (idx != 12) {
            //             break;
            //         }
            //         lastEventButtonNumber = ReadUInt8(&buf[1]);
            //         lastEventX = ReadUInt16(&buf[3]);
            //         lastEventY = ReadUInt16(&buf[7]);
            //         lastEventType = LCDKNOB_EVENT_LONG_PRESS_START;

            //         // MSG_DEBUGLN("Long press start event received for button " + String(button_number));
            //         buf[idx] = 0;
            //         break;
            //     }
            //     case LCDKNOB_EVENT_LONG_PRESS_END: {
            //         if (idx != 12) {
            //             break;
            //         }
            //         lastEventButtonNumber = ReadUInt8(&buf[1]);
            //         lastEventX = ReadUInt16(&buf[3]);
            //         lastEventY = ReadUInt16(&buf[7]);
            //         lastEventType = LCDKNOB_EVENT_LONG_PRESS_END;

            //         // MSG_DEBUGLN("Long press end event received for button " + String(button_number));
            //         buf[idx] = 0;
            //         break;
            //     }
            //     case LCDKNOB_EVENT_TOUCH_XY: {
            //         if (idx != 10) {
            //             break;
            //         }
            //         lastEventX = ReadUInt16(&buf[1]);
            //         lastEventY = ReadUInt16(&buf[5]);
            //         lastEventType = LCDKNOB_EVENT_TOUCH_XY;

            //         // MSG_DEBUGLN("TouchXY event received for button " + String(button_number));
            //         buf[idx] = 0;
            //         break;
            //     }
            //     case LCDKNOB_EVENT_BUTTON_STATE: {
            //         if (idx != 5) {
            //             break;
            //         }
            //         lastEventType = LCDKNOB_EVENT_BUTTON_STATE;
            //         lastEventButtonNumber = ReadUInt8(&buf[1]);
            //         char state = buf[3];

            //         if (state == '1') {
            //             SetButtonMask(lastEventButtonNumber);
            //         }
            //         else {
            //             ClearButton(lastEventButtonNumber);
            //         }

            //         Serial.println("Button state event received for button " + String(lastEventButtonNumber) + " state " + state);
            //         buf[idx] = 0;
            //         break;
            //     }

            //     default:
            //         break;
            // }
            idx = 0;
        }
    }
    return lastEventType;
}
uint8_t SerialChannel::ReadUInt8(char* hex_value)
{
    uint8_t data = 0;
    sscanf(hex_value, "%2X", &data);
    return data;
}

uint16_t SerialChannel::ReadUInt16(char* hex_value)
{
    uint16_t data = 0;
    sscanf(hex_value, "%4X", &data);
    return data;
}

uint32_t SerialChannel::ReadUInt32(char* hex_value)
{
    uint32_t data = 0;
    sscanf(hex_value, "%8X", &data);
    return data;
}

uint8_t SerialChannel::GetLastEventType()
{
    return lastEventType;
}

uint8_t SerialChannel::GetLastEventButtonNumber()
{
    return lastEventButtonNumber;
}

uint16_t SerialChannel::GetLastEventX()
{
    return lastEventX;
}

uint16_t SerialChannel::GetLastEventY()
{
    return lastEventY;
}

void SerialChannel::SetButtonMask(uint16_t newButtonMask)
{
    buttonStateMask = newButtonMask;
}

void SerialChannel::SetButton(uint8_t button_index)
{
    // if ((button_index < LCDKNOB_MAX_BUTTON) && (button_index > 0)) {
    //     buttonStateMask |= (1 << (button_index - 1));
    // }
    // else {
    //     MSG_ERRORLN("[ERROR] SerialChannel::SetButton invalid button index");
    // }
}

void SerialChannel::ClearButtonMask()
{
    buttonStateMask = 0;
}

void SerialChannel::ClearButton(uint8_t button_index)
{
    // if ((button_index < LCDKNOB_MAX_BUTTON) && (button_index > 0)) {
    //     buttonStateMask &= ~(1 << (button_index - 1));
    // }
    // else {
    //     MSG_ERRORLN("[ERROR] SerialChannel::ClearButton invalid button index");
    // }
}
bool SerialChannel::GetButtonState(uint8_t button_index)
{
    // if ((button_index < LCDKNOB_MAX_BUTTON) && (button_index >= 0)) {
    //     return (buttonStateMask & (1 << (button_index)));
    // }
    // else {
    //     MSG_ERROR1("[ERROR] SerialChannel::GetButtonState invalid button index ", button_index);
    //     return false;
    // }

    return false;
}
