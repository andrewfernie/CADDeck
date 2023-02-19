/*
 KeyPressed on PIN1
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF857X.h"

TwoWire aWire(&sercom3, 20, 21);
// Set i2c address
PCF857X pcf857X(&aWire, 0x38);

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
}

void loop()
{
	uint8_t val = pcf857X.digitalRead(P1);
	if (val==HIGH) Serial.println("KEY PRESSED");
	delay(50);
}
