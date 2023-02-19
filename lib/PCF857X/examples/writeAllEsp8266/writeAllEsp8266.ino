/*
 KeyPressed with interrupt and digital write all
 from P4 to P7
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF857X.h"

// For arduino uno only pin 1 and 2 are interrupted
#define ARDUINO_UNO_INTERRUPTED_PIN D3

// Function interrupt
void ICACHE_RAM_ATTR  keyPressedOnPCF857X();

// Set i2c address
PCF857X pcf857X(0x38, ARDUINO_UNO_INTERRUPTED_PIN, keyPressedOnPCF857X);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.println("INIT");

	pcf857X.pinMode(P0, INPUT);
	pcf857X.pinMode(P1, INPUT_PULLUP);
	pcf857X.pinMode(P2, INPUT);
	pcf857X.pinMode(P3, INPUT);

	pcf857X.pinMode(P7, OUTPUT);
	pcf857X.pinMode(P6, OUTPUT, HIGH);
	pcf857X.pinMode(P5, OUTPUT, LOW);
	pcf857X.pinMode(P4, OUTPUT, LOW);

	Serial.print("Init pcf857X...");
	if (pcf857X.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}

	Serial.println("START");
	timeElapsed = millis();
}
unsigned long lastSendTime = 0;        // last send time
unsigned long interval = 3000;          // interval between sends

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
		Serial.println(val1);
		Serial.print("P2 ");
		Serial.print(val2);
		Serial.print(" P3 ");
		Serial.println(val3);
		keyPressed= false;


	}

	  if (millis() - lastSendTime > interval) {
			Serial.print("WRITE ALL VALUE FROM P4 TO P7 ");
			Serial.println(startVal);

//			pcf857X.digitalWrite(P7, startVal);
			bool startVal2 = LOW;
			if (startVal==HIGH) {
				startVal = LOW;
				startVal2 = HIGH;
			}else{
				startVal = HIGH;
				startVal2 = LOW;
			}
			PCF857X::DigitalInput digitalInput;
			digitalInput.p4 = startVal2;
			digitalInput.p5 = startVal;
			digitalInput.p6 = startVal2;
			digitalInput.p7 = startVal;

			pcf857X.digitalWriteAll(digitalInput);
			lastSendTime = millis();
	  }
}

void keyPressedOnPCF857X(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyPressed = true;

}
