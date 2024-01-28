#include <stdio.h>

#include <bsp/board.h>
#include <tusb.h>

#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <pico/stdio.h>

#define LOG_MSG_BASIC 1
#define LOG_MSG_LEVEL 1  // 1=ERROR, 2=ERROR+WARN, 3=ERROR+WARN+INFO
#define LOG_MSG_DEBUG 1
#include "debug_msg.h"

// Note that only certain pins can be used for each GPIO function (e.g. UART)
// Check the Raspberry Pi Pico datasheet for more information
#define SERIAL_MOUSE_UART uart1
#define SERIAL_MOUSE_TX_PIN 8
#define SERIAL_MOUSE_RX_PIN 9
#define SERIAL_MOUSE_CTS_PIN 10
#define SERIAL_MOUSE_RTS_PIN 11

uint16_t trans_report[3];
uint16_t rot_report[3];
uint8_t buttons_report[6];

uint8_t trans_pending = 0;
uint8_t rot_pending = 0;
uint8_t buttons_pending = 0;

uint8_t button_bits[] = { 12, 13, 14, 15, 22, 25, 23, 24, 0, 1, 2, 4, 5, 8, 26, 3, 6, 7, 9 };

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
    uart_init(SERIAL_MOUSE_UART, 115200);
    uart_set_hw_flow(SERIAL_MOUSE_UART, true, true);
    uart_set_translate_crlf(SERIAL_MOUSE_UART, false);
    uart_set_format(SERIAL_MOUSE_UART, 8, 1, UART_PARITY_NONE);  // docs say 2 stop bits

    sleep_ms(500);
    uint8_t init_buf[] = { '\r', 'v', 'Q', '\r', 'm', '3', '\r' };
    uart_write_blocking(SERIAL_MOUSE_UART, init_buf, sizeof(init_buf));

    uint8_t buf[64];
    uint8_t idx = 0;

    long last_trans_time = 0;
    long last_rot_time = 0;
    long last_button_time = 0;

    long update_interval = 20;  // ms
    long time_now_millis = 0;

    while (true) {
        time_now_millis = time_us_64() / 1000;
        tud_task();
        if (time_now_millis - last_trans_time > update_interval) {
            if (trans_pending && tud_hid_ready()) {
                tud_hid_report(1, trans_report, 6);
                last_trans_time = time_now_millis;
                trans_pending = 0;
            }
        }
        if (time_now_millis - last_rot_time > update_interval) {
            if (rot_pending && tud_hid_ready()) {
                tud_hid_report(2, rot_report, 6);
                last_rot_time = time_now_millis;
                rot_pending = 0;
            }
        }

        if (time_now_millis - last_button_time > update_interval) {
            if (buttons_pending && tud_hid_ready()) {
                tud_hid_report(3, buttons_report, 6);
                last_button_time = time_now_millis;
                buttons_pending = 0;
            }
        }

        if (uart_is_readable(SERIAL_MOUSE_UART)) {
            char c = uart_getc(SERIAL_MOUSE_UART);
            buf[idx] = c;
            idx = (idx + 1) % sizeof(buf);

            if (c == '\r') {
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
                        }

                        trans_report[0] = values[0];
                        trans_report[1] = values[1];
                        trans_report[2] = values[2];
                        rot_report[0] = values[4];
                        rot_report[1] = -values[3];
                        rot_report[2] = -values[5];

                        trans_pending = 1;
                        rot_pending = 1;

                        break;
                    }
                    case 'k': {
                        uint8_t data_nibbles = 0;
                        if (idx == 5) {
                            data_nibbles = 3;
                        } else if (idx == 6) {
                            data_nibbles = 4;
                        } else if (idx == 7) {
                            data_nibbles = 5;
                        } else {
                            break;
                        }
#if (LOG_MSG_DEBUG > 0)
                        for (int i = 0; i < data_nibbles + 1; i++) {
                            printf("%c", buf[i]);
                        }
                        printf("\n");
#endif
                        unsigned long int buttons = 0;
                        for (int i = 0; i < data_nibbles; i++) {
                            buttons |= (buf[1 + i] & 0x0f) << (4 * i);
                        }
#if (LOG_MSG_DEBUG > 0)
                        printf("Buttons: %08lX", buttons);
#endif

                        memset(buttons_report, 0, sizeof(buttons_report));

                        for (int i = 0; i < data_nibbles * 4; i++) {
                            if (buttons & (1 << i)) {
                                buttons_report[button_bits[i] / 8] |= 1 << (button_bits[i] % 8);
                            }
                        }
#if (LOG_MSG_DEBUG > 0)
                        printf("buttons_report: %02x %02x %02x %02x %02x %02x \n", buttons_report[0], buttons_report[1], buttons_report[2], buttons_report[3], buttons_report[4], buttons_report[5]);
#endif

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
