#pragma once
#include "LCDKnob.h"
uint8_t draw_logo(uint16_t x, uint16_t y, const char *filename, bool erase = false);
uint32_t usedPSRAM();
void drawErrorMessage(String message);
void drawGrid();
bool CopyFile(String FileOriginal, String FileCopy);
uint8_t isValidPageNumber(uint8_t page);
void drawKeypad();
void drawButtonNumber(uint8_t page, uint8_t button_index);
bool isActiveButton(uint8_t page, uint8_t row, uint8_t col);
uint8_t getButtonPressedNumber(uint16_t x, uint16_t y);