/*
 KeyPressed with interrupt
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF857X.h"

// For arduino uno only pin 1 and 2 are interrupted
#define ARDUINO_UNO_INTERRUPTED_PIN 2

// Function interrupt
void keyPressedOnPCF857X();

// Set i2c address
PCF857X pcf857X(0x39, ARDUINO_UNO_INTERRUPTED_PIN, keyPressedOnPCF857X);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);
	delay(1000);

	pcf857X.pinMode(P0, OUTPUT);
	pcf857X.pinMode(P1, INPUT);
	Serial.print("Init pcf857X...");
	if (pcf857X.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}


	timeElapsed = millis();
}

bool keyPressed = false;
void loop()
{
	if (keyPressed){
		uint8_t val = pcf857X.digitalRead(P1);
		Serial.print("READ VALUE FROM PCF ");
		Serial.println(val);
		keyPressed= false;
	}
}

void keyPressedOnPCF857X(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyPressed = true;

}
