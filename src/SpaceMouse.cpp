#include "SpaceMouse.h"

SpaceMouse::SpaceMouse(
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
    cmd_end = '\r';
}

SpaceMouse::~SpaceMouse()
{
}

void SpaceMouse::Init()
{ 
    Serial1.begin(baudRate, configParameters, pinRx, pinTx);
}

void SpaceMouse::SendViewpointDataPacket(uint16_t x, uint16_t y, uint16_t z, uint16_t a, uint16_t b, uint16_t c)
{
    Serial1.write('d');
    SendUInt16(x);
    SendUInt16(y);
    SendUInt16(z);
    SendUInt16(a);
    SendUInt16(b);
    SendUInt16(c);
    Serial1.write(cmd_end);
}

void SpaceMouse::SendUInt16(uint16_t data)
{
    Serial1.write(EncodeNibble((uint8_t)(data >> (12) & 0x000f)));
    Serial1.write(EncodeNibble((uint8_t)(data >> (8) & 0x000f)));
    Serial1.write(EncodeNibble((uint8_t)(data >> (4) & 0x000f)));
    Serial1.write(EncodeNibble((uint8_t)(data & 0x000f)));
}

void SpaceMouse::SendKeyPacket(uint8_t button_index)
{
    Serial1.write('k');
    uint8_t nibble = 0;
    char encoded_zero = EncodeNibble(0);
    if (button_index == 0) {
        Serial1.write(encoded_zero);
        Serial1.write(encoded_zero);
        Serial1.write(encoded_zero);
    }
    else if (button_index < 5) {
        nibble = 1 << (button_index - 1);
        Serial1.write(EncodeNibble(nibble));
        Serial1.write(encoded_zero);
        Serial1.write(encoded_zero);
    }
    else if (button_index < 9) {
        Serial1.write(encoded_zero);
        nibble = 1 << (button_index - 5);
        Serial1.write(EncodeNibble(nibble));
        Serial1.write(encoded_zero);
    }
    else if (button_index < 13) {
        Serial1.write(encoded_zero);
        Serial1.write(encoded_zero);
        nibble = 1 << (button_index - 9);
        Serial1.write(EncodeNibble(nibble));
    }
    else {
        MSG_ERRORLN("[ERROR] Invalid button index");
        Serial1.write(encoded_zero);
        Serial1.write(encoded_zero);
        Serial1.write(encoded_zero);
    }

    Serial1.write(cmd_end);
}

char SpaceMouse::EncodeNibble(uint8_t nibble)
{
    //    Hex equivalents of char_codes are provided for convenience during debugging
    //
    //                   {0x30, 0x41, 0x42, 0x33, 0x44, 0x35,
    //                    0x36, 0x47, 0x48, 0x39, 0x3a, 0x4b,
    //                    0x3c, 0x4d, 0x4e, 0x3f};

    char char_codes[] = {'0', 'A', 'B', '3', 
                         'D', '5', '6', 'G', 
                         'H', '9', ':', 'K',
                         '<', 'M', 'N', '?'};

    char encoded_nibble = char_codes[nibble & 0x0f];

    return encoded_nibble;
}
/*


#include <bsp/board.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <tusb.h>

#define SERIAL_MOUSE_RX_PIN 21
#define SERIAL_MOUSE_TX_PIN 20
#define SERIAL_MOUSE_CTS_PIN 26
#define SERIAL_MOUSE_RTS_PIN 27
#define SERIAL_MOUSE_UART uart1

uint16_t trans_report[3];
uint16_t rot_report[3];
uint8_t buttons_report[6];

uint8_t trans_pending = 0;
uint8_t rot_pending = 0;
uint8_t buttons_pending = 0;

uint8_t button_bits[] = { 12, 13, 14, 15, 22, 25, 23, 24, 0, 1, 2, 4, 5, 8, 26 };

int main() {
    board_init();
    tusb_init();
    stdio_init_all();

    // while(true){
        printf("hello\n");
    //     sleep_ms(1000);
    // }

    gpio_set_function(SERIAL_MOUSE_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_MOUSE_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_MOUSE_CTS_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_MOUSE_RTS_PIN, GPIO_FUNC_UART);
    uart_init(SERIAL_MOUSE_UART, 9600);
    uart_set_hw_flow(SERIAL_MOUSE_UART, true, true);
    uart_set_translate_crlf(SERIAL_MOUSE_UART, false);
    uart_set_format(SERIAL_MOUSE_UART, 8, 1, UART_PARITY_NONE);  // docs say 2 stop bits

    sleep_ms(500);
    uint8_t init_buf[] = { '\r', 'v', 'Q', '\r', 'm', '3', '\r' };
    uart_write_blocking(SERIAL_MOUSE_UART, init_buf, sizeof(init_buf));

    uint8_t buf[64];
    uint8_t idx = 0;

    while (true) {
        tud_task();
        if (trans_pending && tud_hid_ready()) {
            tud_hid_report(1, trans_report, 6);
            trans_pending = 0;
        }
        if (rot_pending && tud_hid_ready()) {
            tud_hid_report(2, rot_report, 6);
            rot_pending = 0;
        }
        if (buttons_pending && tud_hid_ready()) {
            tud_hid_report(3, buttons_report, 6);
            buttons_pending = 0;
        }

        if (uart_is_readable(SERIAL_MOUSE_UART)) {
            char c = uart_getc(SERIAL_MOUSE_UART);
            buf[idx] = c;
            idx = (idx + 1) % sizeof(buf);
            printf("%c", c);
            if (c == '\r') {
                printf("\n");

                switch (buf[0]) {
                    case 'd': {
                        if (idx != 26) {
                            break;
                        }

                        int16_t values[6];
                        for (int i = 0; i < 6; i++) {
                            values[i] = -32768;
                            for (int j = 0; j < 4; j++) {
                                values[i] += (buf[1 + i * 4 + 3 - j] & 0xf) << (4 * j);
                            }

                            printf("%d %d ", i, values[i]);
                        }
                        printf("\n");

                        trans_report[0] = values[0];
                        trans_report[1] = values[2];
                        trans_report[2] = -values[1];
                        rot_report[0] = values[3];
                        rot_report[1] = values[5];
                        rot_report[2] = -values[4];

                        trans_pending = 1;
                        rot_pending = 1;

                        break;
                    }
                    case 'k': {
                        if (idx != 5) {
                            break;
                        }
                        uint16_t buttons = 0;
                        for (int i = 0; i < 3; i++) {
                            buttons |= (buf[1 + i] & 0x0f) << (4 * i);
                        }
                        printf("%04x\n", buttons);

                        memset(buttons_report, 0, sizeof(buttons_report));

                        for (int i = 0; i < 12; i++) {
                            if (buttons & (1 << i)) {
                                buttons_report[button_bits[i] / 8] |= 1 << (button_bits[i] % 8);
                            }
                        }

                        buttons_pending = 1;
                    }
                    default:
                        break;
                }

                idx = 0;
            }
        }
    }

    return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    return 0;
}


*/