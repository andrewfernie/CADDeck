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
    if (Serial1.available() > 0)
    {
        char c = Serial1.read();
        buf[idx] = c;
        Serial.print(c,HEX);
        Serial.print(' ');
        Serial.println(idx);
        idx = (idx + 1) % sizeof(buf);

        if (c == LCDKNOB_EVENT_END)
        {
            switch (buf[0])
            {
            case LCDKNOB_SEND_SET_BUTTON_STATE:
            {
                if (idx != 6)
                {
                    break;
                }
                lastEventType = LCDKNOB_SEND_SET_BUTTON_STATE;
                lastEventButtonNumber = ReadUInt8(&buf[1]);
                lastEventState = ReadUInt8(&buf[3]);

                Serial.println("Set button state received for button " + String(lastEventButtonNumber) + " state " + String(lastEventState));

                buf[idx] = 0;
                break;
            }
            case LCDKNOB_SEND_BUTTON_STATE_REQUEST:
            {
                if (idx != 4)
                {

                    break;
                }
                lastEventType = LCDKNOB_SEND_BUTTON_STATE_REQUEST;
                lastEventButtonNumber = ReadUInt8(&buf[1]);
                Serial.println("Button state request received for button " + String(lastEventButtonNumber));

                buf[idx] = 0;
                break;
            }
            case LCDKNOB_SEND_SET_MENU:
            {
                if (idx != 4)
                {
                    break;
                }
                lastEventType = LCDKNOB_SEND_SET_MENU;
                lastEventMenuNumber = ReadUInt8(&buf[1]);
                Serial.println("Set menu received for menu " + String(lastEventMenuNumber));

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
uint8_t SerialChannel::ReadUInt8(char *hex_value)
{
    uint8_t data = 0;
    sscanf(hex_value, "%2X", &data);
    return data;
}

uint16_t SerialChannel::ReadUInt16(char *hex_value)
{
    uint16_t data = 0;
    sscanf(hex_value, "%4X", &data);
    return data;
}

uint32_t SerialChannel::ReadUInt32(char *hex_value)
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

uint8_t SerialChannel::GetLastEventMenuNumber()
{
    return lastEventMenuNumber;
}

uint8_t SerialChannel::GetLastEventState()
{
    return lastEventState;
}

uint16_t SerialChannel::GetLastEventX()
{
    return lastEventX;
}

uint16_t SerialChannel::GetLastEventY()
{
    return lastEventY;
}


