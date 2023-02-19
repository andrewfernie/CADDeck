/*
 KeyPressed and leds with interrupt
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
PCF857X pcf857X(0x38, ARDUINO_UNO_INTERRUPTED_PIN, keyPressedOnPCF857X);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);

	pcf857X.pinMode(P0, INPUT_PULLUP);
	pcf857X.pinMode(P1, INPUT_PULLUP);
	pcf857X.pinMode(P2, INPUT);
	pcf857X.pinMode(P3, INPUT);

	pcf857X.pinMode(P7, OUTPUT);
	pcf857X.pinMode(P6, OUTPUT, HIGH);
	pcf857X.pinMode(P5, OUTPUT);
	pcf857X.pinMode(P4, OUTPUT, LOW);

	Serial.print("Init pcf857X...");
	if (pcf857X.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}

	timeElapsed = millis();
}
unsigned long lastSendTime = 0;        // last send time
unsigned long interval = 4000;          // interval between sends

bool startVal = HIGH;

bool keyPressed = false;
void loop()
{
	if (keyPressed){
		uint8_t val0 = pcf857X.digitalRead(P0);
		uint8_t val1 = pcf857X.digitalRead(P1);
		uint8_t val2 = pcf857X.digitalRead(P2);
		uint8_t val3 = pcf857X.digitalRead(P3);
		Serial.print("P0 ");
		Serial.print(val0);
		Serial.print(" P1 ");
		Serial.print(val1);
		Serial.print(" P2 ");
		Serial.print(val2);
		Serial.print(" P3 ");
		Serial.println(val3);
		keyPressed= false;
	}

	  if (millis() - lastSendTime > interval) {
			pcf857X.digitalWrite(P7, startVal);
			if (startVal==HIGH) {
				startVal = LOW;
			}else{
				startVal = HIGH;
			}
			lastSendTime = millis();

			Serial.print("P7 ");
			Serial.println(startVal);
	  }
}

void keyPressedOnPCF857X(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyPressed = true;

}
