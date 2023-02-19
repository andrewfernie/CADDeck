/*
 Blink led on PIN0
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF857X.h"

// Set i2c address
PCF857X pcf857X(0x39);

void setup()
{
	Serial.begin(115200);
	delay(1000);

	// Set pinMode to OUTPUT
	pcf857X.pinMode(P0, OUTPUT);
	pcf857X.pinMode(P1, INPUT);

	Serial.print("Init pcf857X...");
	if (pcf857X.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}
}

void loop()
{
	pcf857X.digitalWrite(P0, HIGH);
	delay(1000);
	pcf857X.digitalWrite(P0, LOW);
	delay(1000);
}
