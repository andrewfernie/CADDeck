#include "ProcessTouchscreen.h"

long lastClickTime = 0;
uint8_t lastClickButton = 0;
bool waitingForDblClick = false;
uint8_t longPressActive = BUTTON_INVALID;

uint8_t processTouchEvents()
{
    long timeNow = 0;

    timeNow = millis();
    if(waitingForDblClick) {
        if (timeNow - lastClickTime > DBL_CLICK_TIME)
        {
            waitingForDblClick = false;
            serialChannel.SendClickEvent(lastClickButton, touch.data.x, touch.data.y);
            MSG_INFOLN("Single Click at x:" + String(touch.data.x) + ", y:" + String(touch.data.y));
            MSG_INFOLN("Button number " + String(lastClickButton) + " clicked");
        }
    }

    if (touch.available()) {
        uint8_t button_number = getButtonPressedNumber(touch.data.x, touch.data.y);
 
        if ((touch.data.gestureID == SINGLE_CLICK) && (touch.data.points == 0) && (touch.data.event == 1)) {
 
            uint8_t button_number = getButtonPressedNumber(touch.data.x, touch.data.y);
 
            if (timeNow - lastClickTime < DBL_CLICK_TIME)
            {
                serialChannel.SendDblClickEvent(lastClickButton, touch.data.x, touch.data.y);
                MSG_INFOLN("Double Click at x:" + String(touch.data.x) + ", y:" + String(touch.data.y));
                MSG_INFOLN("Button number " + String(lastClickButton) + " clicked");
                lastClickTime = 0;
                lastClickButton = BUTTON_INVALID;
                waitingForDblClick = false;
            }
            else {
                lastClickTime = timeNow;
                lastClickButton = button_number;
                waitingForDblClick = true;
            }

            if (button_number == 0) {
                menu[pageNum].button[button_number].islatched = !menu[pageNum].button[button_number].islatched;
                // MSG_INFOLN("Button pressed number: " + String(button_number) + " is latched: " + String(menu[pageNum].button[button_number].islatched));
                MSG_INFOLN("Button" + String(button_number) + " latch status " + String(menu[pageNum].button[button_number].islatched));
                drawButtonNumber(pageNum, button_number);
                serialChannel.SendButtonState(button_number, menu[pageNum].button[button_number].islatched);
            }
        }
        else if (touch.data.gestureID == LONG_PRESS) {
            if ((touch.data.points == 1) && (touch.data.event == 2)) {
                if (longPressActive == BUTTON_INVALID) {
                    serialChannel.SendLongPressStartEvent(button_number, touch.data.x, touch.data.y);
                    MSG_INFOLN("Button number " + String(button_number) + " started long press at x:" + String(touch.data.x) + ", y:" + String(touch.data.y));
                }
                longPressActive = button_number;
            }
            else if ((touch.data.points == 0) && (touch.data.event == 1)) {
                if (longPressActive == BUTTON_INVALID) {
                    serialChannel.SendLongPressEndEvent(longPressActive, touch.data.x, touch.data.y);
                    MSG_INFOLN("Button number " + String(longPressActive) + " finished long press.");
                }
                longPressActive = BUTTON_INVALID;
            }
        }
    }
    return 0;
}