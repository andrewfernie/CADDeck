#pragma once
#include "LCDKnob.h"

// Define the time, in ms, between clicks to be considered a double click. A reasonable value is
// 500.  Set to zero to disable double click detection. This makes the UI for single clicks 
// more responsive as it doesn't have to wasit for the double click timeout to expire.
const uint16_t DBL_CLICK_TIME = 0;

uint8_t processTouchEvents();
extern uint8_t longPressActive;