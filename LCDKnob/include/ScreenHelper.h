#pragma once
#include "LCDKnob.h"
unsigned long convertHTMLtoRGB888(char *html);
unsigned int convertRGB888ToRGB565(unsigned long rgb);
void convertRGB888toHTML(unsigned long rgb, char *html, unsigned int htmlsize);
unsigned long convertRGB565ToRGB888(unsigned int rgb);
unsigned int convertHTMLtoRGB565(char *html);
void convertRGB565toHTML(unsigned int rgb, char *html, unsigned int htmlsize);