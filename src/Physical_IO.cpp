#include "Physical_IO.h"

Joystick joystick(JOYSTICK_X, JOYSTICK_Y);
AnalogControl zoomControl(ZOOM_PIN);
AnalogControl rotateControl(ROTATE_PIN);

PCF857X pcf857X(PCF857X_ADDR);  // Note that the SCL and SDA pins have already been defined as part of
                                // the touch screen setup.

uint8_t current_joystick_mode = JoystickModeNone;
uint8_t previous_joystick_mode = JoystickModeNone;
long millis_last_joystick_move = 0;

int8_t joystick_mode_pins[4] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const uint8_t HWButton_Pins[NUM_HW_BUTTONS] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_10};
int8_t mode_select_button_pin = BUTTON_0;

uint8_t last_hw_button0_set = 0;
uint8_t last_rotate_knob_turned = 0;
uint8_t control_mode = 0;  // 0 = mouse, 1 = rotate, 2 = move
uint8_t last_control_mode = 0;

PCF857X::DigitalInput pcf857X_inputs;

void init_io()
{
    MSG_INFOLN("[INFO] Init joystick");
    joystick.Init();
    joystick.SetDeadzone(cadconfig.joy_deadzone);
    joystick.SetScale(0, cadconfig.joy_scale_x);
    joystick.SetScale(1, cadconfig.joy_scale_y);

    zoomControl.Init();
    zoomControl.SetDeadzone(cadconfig.zoom_deadzone);
    zoomControl.SetScale(cadconfig.zoom_scale);

    rotateControl.Init();
    rotateControl.SetDeadzone(cadconfig.rotate_deadzone);
    rotateControl.SetScale(cadconfig.rotate_scale);

    MSG_INFOLN("[INFO] Init pcf857X pins");
    // All pins are inputs. Note that the PCF857X library doesn't support pullups, so
    // mode has to be INPUT or OUTPUT, not INPUT_PULLUP.
    for (int i = 0; i < 16; i++) {
        pcf857X.pinMode(i, INPUT);
    }

    pcf857X.begin(false);  // false so as not to start the I2C bus (already done by the touch controller)
}

void set_move_mode()
{
    KeyboardMouseAction(17, 11, 0);  // Release all mouse buttons
    KeyboardMouseAction(5, 9, 0);    // Release all keys
    KeyboardMouseAction(17, 3, 0);   // Set middle mouse button
    control_mode = 2;
}

void set_rotate_mode()
{
    KeyboardMouseAction(17, 11, 0);  // Release all mouse buttons
    KeyboardMouseAction(5, 9, 0);    // Release all keys
    KeyboardMouseAction(5, 2, 0);    // Set left shift key
    KeyboardMouseAction(17, 3, 0);   // Set middle mouse button
    control_mode = 1;
}

void set_mouse_mode()
{
    KeyboardMouseAction(17, 11, 0);  // Release all mouse buttons
    KeyboardMouseAction(5, 9, 0);    // Release all keys
    control_mode = 0;
}

void update_io()
{
    joystick.Update();
    zoomControl.Update();
    rotateControl.Update();
    pcf857X_inputs = pcf857X.digitalReadAll();

    uint8_t hw_button0_set = get_hwbutton(0);
    uint8_t rotate_knob_turned = rotateControl.Value() != 0;

    if (Keyboard.isConnected()) {
        if (hw_button0_set && !last_hw_button0_set) {
            set_move_mode();
        }
        else if (!hw_button0_set && last_hw_button0_set) {
            set_mouse_mode();
        }

        if (rotate_knob_turned & !last_rotate_knob_turned) {
            set_rotate_mode();
        }
        else if (!rotate_knob_turned & last_rotate_knob_turned) {
            set_mouse_mode();
        }

        if (control_mode == 0) {
            Mouse.move(joystick.x() * cadconfig.joy_sensitivity, joystick.y() * cadconfig.joy_sensitivity, 0);
        }
        else if (control_mode == 1) {
            Mouse.move(rotateControl.Value() * cadconfig.rotate_sensitivity, 0, 0);
        }
        else if (control_mode == 2) {
            Mouse.move(joystick.x() * cadconfig.joy_sensitivity, joystick.y() * cadconfig.joy_sensitivity, zoomControl.Value() * cadconfig.zoom_sensitivity);
        }
        last_hw_button0_set = hw_button0_set;
        last_rotate_knob_turned = rotate_knob_turned;
        last_control_mode = control_mode;
    }
    else {
        MSG_WARNLN("[DEBUG] BLE Mouse not connected");
    }
}

void calibrate_scale_analog_controls()
{
    // calibrate the analog controls
    int max_value_x = -10000;
    int min_value_x = 10000;
    int max_value_y = -10000;
    int min_value_y = 10000;
    int max_value_r = -10000;
    int min_value_r = 10000;
    int max_value_s = -10000;
    int min_value_s = 10000;

    long timestart = millis();

    while (millis() - timestart < CALIBRATION_TIME_MS) {
        joystick.Update();
        int value_int_x = joystick.RawX();
        int value_int_y = joystick.RawY();
        
        if (value_int_x > max_value_x) {
            max_value_x = value_int_x;
        }
        else if (value_int_x < min_value_x) {
            min_value_x = value_int_x;
        }

        if (value_int_y > max_value_y) {
            max_value_y = value_int_y;
        }
        else if (value_int_y < min_value_y) {
            min_value_y = value_int_y;
        }

        zoomControl.Update();   
        int value_int_r = rotateControl.RawValue();
        if (value_int_r > max_value_r) {
            max_value_r = value_int_r;
        }
        else if (value_int_r < min_value_r) {
            min_value_r = value_int_r;
        }

        rotateControl.Update();
        int value_int_s = zoomControl.RawValue();
        if (value_int_s > max_value_s) {
            max_value_s = value_int_s;
        }
        else if (value_int_s < min_value_s) {
            min_value_s = value_int_s;
        }

    }
    float scale_x = 2.0 / (float)(max_value_x - min_value_x) * 4095.0;
    float scale_y = 2.0 / (float)(max_value_y - min_value_y) * 4095.0;
    float scale_r = 2.0 / (float)(max_value_r - min_value_r) * 4095.0;
    float scale_s = 2.0 / (float)(max_value_s - min_value_s) * 4095.0;

    joystick.SetScale(0, scale_x);
    joystick.SetScale(1, scale_y);
    rotateControl.SetScale(scale_r);
    zoomControl.SetScale(scale_s);
}

void calibrate_zero_analog_controls()
{
    joystick.CalibrateZeroAll();
    zoomControl.CalibrateZero();
    rotateControl.CalibrateZero();
}
    // // Handle the joystick driving the mouse
    // uint8_t set_mouse_buttons()
    // {
    //     current_joystick_mode = check_joystick_mode();

    //     if (current_joystick_mode != previous_joystick_mode) {
    //         //        MSG_DEBUG1("[DEBUG] Joystick mode changed to ", current_joystick_mode);
    //         switch (current_joystick_mode) {
    //             case JoystickModeNone:
    //                 KeyboardMouseAction(17, 11, 0);  // Release all mouse buttons
    //                 KeyboardMouseAction(5, 9, 0);
    //                 break;

    //             case JoystickModePan:
    //                 for (int i = 0; i < 3; i++) {
    //                     if (cadprogramconfig[cadconfig.current_program].pan[i].action != 0) {
    //                         KeyboardMouseAction(cadprogramconfig[cadconfig.current_program].pan[i].action,
    //                                             cadprogramconfig[cadconfig.current_program].pan[i].value,
    //                                             cadprogramconfig[cadconfig.current_program].pan[i].symbol);
    //                     }
    //                 }
    //                 break;

    //             case JoystickModeTilt:
    //                 for (int i = 0; i < 3; i++) {
    //                     if (cadprogramconfig[cadconfig.current_program].tilt[i].action != 0) {
    //                         KeyboardMouseAction(cadprogramconfig[cadconfig.current_program].tilt[i].action,
    //                                             cadprogramconfig[cadconfig.current_program].tilt[i].value,
    //                                             cadprogramconfig[cadconfig.current_program].tilt[i].symbol);
    //                     }
    //                 }
    //                 break;

    //             case JoystickModeZoom:
    //                 for (int i = 0; i < 3; i++) {
    //                     if (cadprogramconfig[cadconfig.current_program].zoom[i].action != 0) {
    //                         KeyboardMouseAction(cadprogramconfig[cadconfig.current_program].zoom[i].action,
    //                                             cadprogramconfig[cadconfig.current_program].zoom[i].value,
    //                                             cadprogramconfig[cadconfig.current_program].zoom[i].symbol);
    //                     }
    //                 }
    //                 break;

    //             case JoystickModeRotate:
    //                 for (int i = 0; i < 3; i++) {
    //                     if (cadprogramconfig[cadconfig.current_program].rotate[i].action != 0) {
    //                         KeyboardMouseAction(cadprogramconfig[cadconfig.current_program].rotate[i].action,
    //                                             cadprogramconfig[cadconfig.current_program].rotate[i].value,
    //                                             cadprogramconfig[cadconfig.current_program].rotate[i].symbol);
    //                     }
    //                 }
    //                 break;

    //             case JoystickModeMouse:
    //                 KeyboardMouseAction(17, 11, 0);  // Release all mouse buttons
    //                 KeyboardMouseAction(5, 9, 0);
    //                 break;

    //             default:
    //                 break;
    //         }
    //     }
    //     previous_joystick_mode = current_joystick_mode;
    //     return current_joystick_mode;
    // }

    // uint8_t check_joystick_mode()
    // {
    //     uint8_t mode = 0;

    //     if (fabs(joystick.x()) < 0.001 && fabs(joystick.y()) < 0.001) {
    //         if (millis() - millis_last_joystick_move > cadconfig.joy_steady_time) {
    //             mode = JoystickModeNone;
    //         }
    //         else {
    //             mode = previous_joystick_mode;
    //         }
    //     }
    //     else if (pan_button()) {
    //         mode = JoystickModePan;
    //         millis_last_joystick_move = millis();
    //     }
    //     else if (tilt_button()) {
    //         mode = JoystickModeTilt;
    //         millis_last_joystick_move = millis();
    //     }
    //     else if (zoom_button()) {
    //         mode = JoystickModeZoom;
    //         millis_last_joystick_move = millis();
    //     }
    //     else if (rotate_button()) {
    //         mode = JoystickModeRotate;
    //         millis_last_joystick_move = millis();
    //     }
    //     else {
    //         mode = cadprogramconfig[cadconfig.current_program].default_joystick_mode;
    //         millis_last_joystick_move = millis();
    //     }

    //     return mode;
    // }

    // uint8_t pan_button()
    // {
    //     uint8_t state;
    //     uint8_t pin = joystick_mode_pins[JoystickModePan - 1];

    //     state = !get_pcf857X_bit(pcf857X_inputs, pin);

    //     return state;
    // }

    // uint8_t tilt_button()
    // {
    //     uint8_t state;
    //     uint8_t pin = joystick_mode_pins[JoystickModeTilt - 1];

    //     state = !get_pcf857X_bit(pcf857X_inputs, pin);

    //     return state;
    // }

    // uint8_t zoom_button()
    // {
    //     uint8_t state;
    //     uint8_t pin = joystick_mode_pins[JoystickModeZoom - 1];

    //     state = !get_pcf857X_bit(pcf857X_inputs, pin);

    //     return state;
    // }

    // uint8_t rotate_button()
    // {
    //     uint8_t state;
    //     uint8_t pin = joystick_mode_pins[JoystickModeRotate - 1];

    //     state = !get_pcf857X_bit(pcf857X_inputs, pin);

    //     return state;
    // }

    uint8_t get_hwbutton(uint8_t button)
    {
        uint8_t state;
        if (button >= 0 && button < NUM_HW_BUTTONS) {
            state = !get_pcf857X_bit(pcf857X_inputs, HWButton_Pins[button]);
        }
        else {
            state = 0;
        }
        return state;
    }

    uint8_t get_pcf857X_bit(PCF857X::DigitalInput dinputs, uint8_t pin)
    {
        uint8_t state;
        switch (pin) {
            case P0:
                state = dinputs.p0;
                break;
            case P1:
                state = dinputs.p1;
                break;
            case P2:
                state = dinputs.p2;
                break;
            case P3:
                state = dinputs.p3;
                break;
            case P4:
                state = dinputs.p4;
                break;
            case P5:
                state = dinputs.p5;
                break;
            case P6:
                state = dinputs.p6;
                break;
            case P7:
                state = dinputs.p7;
                break;
            case P8:
                state = dinputs.p8;
                break;
            case P9:
                state = dinputs.p9;
                break;
            case P10:
                state = dinputs.p10;
                break;
            case P11:
                state = dinputs.p11;
                break;
            case P12:
                state = dinputs.p12;
                break;
            case P13:
                state = dinputs.p13;
                break;
            case P14:
                state = dinputs.p14;
                break;
            case P15:
                state = dinputs.p15;
                break;
            default:
                state = 0;
                break;
        }
        return state;
    }