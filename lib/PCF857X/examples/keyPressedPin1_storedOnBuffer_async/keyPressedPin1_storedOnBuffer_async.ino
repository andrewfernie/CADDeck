/*
 KeyPressed async
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF857X.h"

// Set i2c address
PCF857X pcf857X(0x39);
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

void loop()
{
	// Read and store on buffer all input (pinMode) that are going HIGHT
	pcf857X.readBuffer();
	if (millis()>timeElapsed+2000){
		// read value on buffer than reset value for that pin
		uint8_t val = pcf857X.digitalRead(P1);
		if (val==HIGH) Serial.println("KEY PRESSED STORED ON BUFFER, NOW READED AND RESETTED.");
		timeElapsed = millis();
	}
}
