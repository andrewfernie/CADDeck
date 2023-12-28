#include "ProcessTouchscreen.h"

long lastClickTime = 0;
uint8_t lastClickButton = 0;
bool waitingForDblClick = false;
uint8_t longPressActive = BUTTON_INVALID;
uint32_t redraw_timer = 0;
const uint32_t REDRAW_INTERVAL = 1000;
bool redraw_latch = false;
uint8_t redraw_button = BUTTON_INVALID;

uint8_t processTouchEvents()
{
    long timeNow = 0;

    timeNow = millis();
    if (waitingForDblClick)
    {
        if (timeNow - lastClickTime > DBL_CLICK_TIME)
        {
            waitingForDblClick = false;
            serialChannel.SendClickEvent(lastClickButton, touch.data.x, touch.data.y);
            redraw_timer = timeNow;
            redraw_latch = true;
            redraw_button = lastClickButton;
            MSG_DEBUGLN("Single Click at x:" + String(touch.data.x) + ", y:" + String(touch.data.y));
            MSG_DEBUGLN("Button number " + String(lastClickButton) + " clicked");
        }
    }

    if (redraw_latch)
    {
        if (timeNow - redraw_timer > REDRAW_INTERVAL)
        {
            redraw_latch = false;
            drawButtonNumber(pageNum, redraw_button);
            drawGrid();
        }
    }

    if (touch.available())
    {
        uint8_t button_number = getButtonPressedNumber(touch.data.x, touch.data.y);

        if ((touch.data.gestureID == SINGLE_CLICK) && (touch.data.points == 0) && (touch.data.event == 1))
        {

            uint8_t button_number = getButtonPressedNumber(touch.data.x, touch.data.y);
            drawButtonCircle(button_number, TFT_RED);

            if (timeNow - lastClickTime < DBL_CLICK_TIME)
            {
                serialChannel.SendDblClickEvent(lastClickButton, touch.data.x, touch.data.y);
                MSG_DEBUGLN("Double Click at x:" + String(touch.data.x) + ", y:" + String(touch.data.y));
                MSG_DEBUGLN("Button number " + String(lastClickButton) + " clicked");
                drawButtonNumber(pageNum, lastClickButton);
                lastClickTime = 0;
                lastClickButton = BUTTON_INVALID;
                waitingForDblClick = false;
            }
            else
            {
                lastClickTime = timeNow;
                lastClickButton = button_number;
                waitingForDblClick = true;
            }

            if (button_number == 0)
            {
                menu[pageNum].button[button_number].islatched = !menu[pageNum].button[button_number].islatched;
                MSG_DEBUGLN("Button pressed number: " + String(button_number) + " is latched: " + String(menu[pageNum].button[button_number].islatched));
                MSG_DEBUGLN("Button" + String(button_number) + " latch status " + String(menu[pageNum].button[button_number].islatched));
                drawButtonNumber(pageNum, button_number);
                serialChannel.SendButtonState(button_number, menu[pageNum].button[button_number].islatched);
            }
        }
        else if (touch.data.gestureID == LONG_PRESS)
        {
            if ((touch.data.points == 1) && (touch.data.event == 2))
            {
                if (longPressActive == BUTTON_INVALID)
                {
                    drawButtonCircle(button_number, TFT_RED);
                    serialChannel.SendLongPressStartEvent(button_number, touch.data.x, touch.data.y);
                    MSG_DEBUGLN("Button number " + String(button_number) + " started long press at x:" + String(touch.data.x) + ", y:" + String(touch.data.y));
                }
                longPressActive = button_number;
            }
            else if ((touch.data.points == 0) && (touch.data.event == 1))
            {
                if (longPressActive != BUTTON_INVALID)
                {
                    redraw_timer = timeNow;
                    redraw_latch = true;
                    redraw_button = longPressActive;
                    serialChannel.SendLongPressEndEvent(longPressActive, touch.data.x, touch.data.y);
                    MSG_DEBUGLN("Button number " + String(longPressActive) + " finished long press.");
                }
                longPressActive = BUTTON_INVALID;
            }
        }
    }
    return 0;
}