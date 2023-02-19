/*
 * PCF857X GPIO Port Expand
 * https://www.mischianti.org/2020/03/13/pcf857X-i2c-digital-i-o-expander-rotary-encoder-part-2/
 *
 * PCF857X    ----- WeMos
 * A0         ----- GRD
 * A1         ----- GRD
 * A2         ----- GRD
 * VSS        ----- GRD
 * VDD        ----- 5V/3.3V
 * SDA        ----- D1(PullUp)
 * SCL        ----- D2(PullUp)
 * INT        ----- INT(PullUp)
 *
 * P0     ----------------- ENCODER PIN A
 * P1     ----------------- ENCODER PIN B
 * P2     ----------------- ENCODER BUTTON
 *
 */
#include "Arduino.h"
#include "PCF857X.h"

int encoderPinA = P0;
int encoderPinB = P1;

#define INTERRUPTED_PIN D7

void ICACHE_RAM_ATTR updateEncoder();

// initialize library
PCF857X pcf857X(0x38, INTERRUPTED_PIN, updateEncoder);

volatile long encoderValue = 0;
uint8_t encoderButtonVal = HIGH;

void setup()
{
	  Serial.begin (9600);
	  delay(500);

	  // encoder pins
	  pcf857X.pinMode(encoderPinA, INPUT_PULLUP);
	  pcf857X.pinMode(encoderPinB, INPUT_PULLUP);
	  // encoder button
	  pcf857X.pinMode(P2, INPUT_PULLUP);

	  // Set low latency with this method or uncomment LOW_LATENCY define in the library
	  // Needed for encoder
	  pcf857X.setLatency(0);

	  // Start library
		Serial.print("Init pcf857X...");
		if (pcf857X.begin()){
			Serial.println("OK");
		}else{
			Serial.println("KO");
		}
}

bool changed = false;

void loop()
{
	if (changed){
		Serial.print("ENCODER --> ");
		Serial.print(encoderValue);
		Serial.print(" - BUTTON --> ");
		Serial.println(encoderButtonVal?"HIGH":"LOW");
		changed = false;
	}
}

uint8_t encoderPinALast = LOW;
uint8_t valPrecEncoderButton = LOW;

void updateEncoder(){
	// Encoder management
	uint8_t n = pcf857X.digitalRead(encoderPinA);
	if ((encoderPinALast == LOW) && (n == HIGH)) {
		if (pcf857X.digitalRead(encoderPinB) == LOW) {
			encoderValue--;
			changed = true; // Chnged the value
		} else {
			encoderValue++;
			changed = true; // Chnged the value
		}
	}
	encoderPinALast = n;

	// Button management
	encoderButtonVal = pcf857X.digitalRead(P2);
	if (encoderButtonVal!=valPrecEncoderButton){
	  changed = true; // Chnged the value of button
	  valPrecEncoderButton = encoderButtonVal;
	}
}
