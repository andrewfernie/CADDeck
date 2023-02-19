/*
 KeyPressed with interrupt in LOW_MEMORY mode
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF857X.h"

// To use in low memory mode and prevent use of 7byte you must decomment the line
// #define PCF857X_LOW_MEMORY
// in the library

// For arduino uno only pin 1 and 2 are interrupted
#define ARDUINO_UNO_INTERRUPTED_PIN 2

// Function interrupt
void keyChangedOnPCF857X();

// Set i2c address
PCF857X pcf857X(0x39, ARDUINO_UNO_INTERRUPTED_PIN, keyChangedOnPCF857X);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);
	delay(1000);

	pcf857X.pinMode(P0, INPUT);
	pcf857X.pinMode(P1, INPUT);
	pcf857X.pinMode(P2, INPUT);
	Serial.print("Init pcf857X...");
	if (pcf857X.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}

	Serial.println("START");

	timeElapsed = millis();
}

bool keyChanged = false;
void loop()
{
	if (keyChanged){
		byte di = pcf857X.digitalReadAll();
		Serial.print("READ VALUE FROM PCF: ");
		Serial.println(di, BIN);
//		delay(5);
		keyChanged= false;
	}
}

void keyChangedOnPCF857X(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyChanged = true;
}
