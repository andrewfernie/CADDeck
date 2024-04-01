/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jacek Fedorynski
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <arduino.h>
#include <tusb.h>
#include "descriptors_spm_hid.h"

int count = 0;
uint16_t trans_report[3];
uint16_t rot_report[3];
uint8_t buttons_report[6];

uint8_t trans_pending = false;
uint8_t rot_pending = false;
uint8_t buttons_pending = false;

uint8_t button_bits[] = {12, 13, 14, 15, 22, 25, 23, 24, 0, 1, 2, 4, 5, 8, 26, 3, 6, 7, 9};
uint8_t init_buf[] = {'\r', 'v', 'Q', '\r', 'm', '3', '\r'};

uint8_t spm_buf[64];
long spm_last_trans_time = 0;
long spm_last_rot_time = 0;
long spm_last_button_time = 0;
long spm_last_keyboard_time = 0;
long spm_last_mouse_time = 0;

long spm_update_interval = 20;  // ms

// 3Dconnexion SpaceMouse Pro
#define USB_SPM_VID 0x046D
#define USB_SPM_PID 0xC62B

// SpaceMouse Pro
const uint8_t report_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x08,        // Usage (Multi-axis Controller)
    0xA1, 0x01,        // Collection (Application)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x01,        //     Report ID (1)
    0x16, 0xA2, 0xFE,  //     Logical Minimum (-350)
    0x26, 0x5E, 0x01,  //     Logical Maximum (350)
    0x36, 0x88, 0xFA,  //     Physical Minimum (-1400)
    0x46, 0x78, 0x05,  //     Physical Maximum (1400)
    0x55, 0x0C,        //     Unit Exponent (-4)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x32,        //     Usage (Z)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x02,        //     Report ID (2)
    0x09, 0x33,        //     Usage (Rx)
    0x09, 0x34,        //     Usage (Ry)
    0x09, 0x35,        //     Usage (Rz)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x03,        //     Report ID (3)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x03,        //     Usage Maximum (0x03)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x35, 0x00,        //     Physical Minimum (0)
    0x45, 0x01,        //     Physical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x05,        //     Usage Minimum (0x05)
    0x29, 0x06,        //     Usage Maximum (0x06)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x09,        //     Usage (0x09)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x0D,        //     Usage Minimum (0x0D)
    0x29, 0x10,        //     Usage Maximum (0x10)
    0x95, 0x04,        //     Report Count (4)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //     Report Count (6)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x17,        //     Usage Minimum (0x17)
    0x29, 0x1B,        //     Usage Maximum (0x1B)
    0x95, 0x05,        //     Report Count (5)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x15,        //     Report Count (21)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x04,        //     Report ID (4)
    0x05, 0x08,        //     Usage Page (LEDs)
    0x09, 0x4B,        //     Usage (Generic Indicator)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x01,        //     Report Size (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x07,        //     Report Size (7)
    0x91, 0x03,        //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,        //   Usage (0x01)
    0xA1, 0x02,        //   Collection (Logical)
    0x15, 0x80,        //     Logical Minimum (-128)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x09, 0x3A,        //     Usage (0x3A)
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x05,        //       Report ID (5)
    0x09, 0x20,        //       Usage (0x20)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x06,        //       Report ID (6)
    0x09, 0x21,        //       Usage (0x21)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x07,        //       Report ID (7)
    0x09, 0x22,        //       Usage (0x22)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x08,        //       Report ID (8)
    0x09, 0x23,        //       Usage (0x23)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x09,        //       Report ID (9)
    0x09, 0x24,        //       Usage (0x24)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x0A,        //       Report ID (10)
    0x09, 0x25,        //       Usage (0x25)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x0B,        //       Report ID (11)
    0x09, 0x26,        //       Usage (0x26)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x13,        //       Report ID (19)
    0x09, 0x2E,        //       Usage (0x2E)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x14,        //       Report ID (20)
    0x09, 0x2F,        //       Usage (0x2F)
    0x95, 0x04,        //       Report Count (4)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x15,        //       Report ID (21)
    0x09, 0x30,        //       Usage (0x30)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x16,        //       Report ID (22)
    0x19, 0x01,        //       Usage Minimum (0x01)
    0x29, 0x1F,        //       Usage Maximum (0x1F)
    0x15, 0x00,        //       Logical Minimum (0)
    0x25, 0x01,        //       Logical Maximum (1)
    0x35, 0x00,        //       Physical Minimum (0)
    0x45, 0x01,        //       Physical Maximum (1)
    0x75, 0x01,        //       Report Size (1)
    0x95, 0x1F,        //       Report Count (31)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //       Report Count (1)
    0x81, 0x03,        //       Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //     End Collection
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_SPM_VID,
    .idProduct = USB_SPM_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x00,

    .bNumConfigurations = 0x01,
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID 0x81

uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LEN, 0, 500)

    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    ,
    TUD_HID_DESCRIPTOR(ITF_NUM_SPACEMOUSE, 0, HID_ITF_PROTOCOL_NONE, sizeof(report_descriptor), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 1)};

char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "Fake",                      // 1: Manufacturer
    "SpaceMouse Pro",            // 2: Product
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb()
{
    return (uint8_t const *)&desc_device;
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    return desc_configuration;
}

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf)
{
    return report_descriptor;
}

static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    }
    else {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            return NULL;

        const char *str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31)
            chr_count = 31;

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    return 0;
}

void spm_hid_task()
{
    long time_now_millis = 0;
    uint8_t idx = 0;

    time_now_millis = millis();
    tud_task();
    // if (time_now_millis - spm_last_trans_time > spm_update_interval) {
    //     if (trans_pending && tud_hid_ready()) {
    //         tud_hid_report(1, trans_report, 6);
    //         spm_last_trans_time = time_now_millis;
    //         trans_pending = 0;
    //     }
    // }
    // if (time_now_millis - spm_last_rot_time > spm_update_interval) {
    //     if (rot_pending && tud_hid_ready()) {
    //         tud_hid_report(2, rot_report, 6);
    //         spm_last_rot_time = time_now_millis;
    //         rot_pending = 0;
    //     }
    // }

    // if (time_now_millis - spm_last_button_time > spm_update_interval) {
    //     if (buttons_pending && tud_hid_ready()) {
    //         tud_hid_report(3, buttons_report, 6);
    //         spm_last_button_time = time_now_millis;
    //         buttons_pending = 0;
    //     }
    // }

    if (Serial1.available() > 0) {
        char c = Serial1.read();
        spm_buf[idx] = c;
        idx = (idx + 1) % sizeof(spm_buf);

        if (c == '\r') {
            switch (spm_buf[0]) {
                case 'd': {
                    if (idx != 26) {
                        break;
                    }

                    int16_t values[6];
                    for (int i = 0; i < 6; i++) {
                        values[i] = -32768;
                        for (int j = 0; j < 4; j++) {
                            values[i] += (spm_buf[1 + i * 4 + 3 - j] & 0xf) << (4 * j);
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
                    }
                    else if (idx == 6) {
                        data_nibbles = 4;
                    }
                    else if (idx == 7) {
                        data_nibbles = 5;
                    }
                    else {
                        break;
                    }
#if (LOG_MSG_DEBUG > 0)
                    for (int i = 0; i < data_nibbles + 1; i++) {
                        printf("%c", spm_buf[i]);
                    }
                    printf("\n");
#endif
                    unsigned long int buttons = 0;
                    for (int i = 0; i < data_nibbles; i++) {
                        buttons |= (spm_buf[1 + i] & 0x0f) << (4 * i);
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
