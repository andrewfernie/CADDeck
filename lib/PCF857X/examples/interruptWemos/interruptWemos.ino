/*
 * PCF857X GPIO Port Expand
 * http://nopnop2002.webcrow.jp/WeMos/WeMos-25.html
 *
 * PCF857X    ----- WeMos
 * A0         ----- GRD
 * A1         ----- GRD
 * A2         ----- GRD
 * VSS        ----- GRD
 * VDD        ----- 5V/3.3V
 * SDA        ----- GPIO_4
 * SCL        ----- GPIO_5
 * INT        ----- GPIO_13
 *
 * P0     ----------------- BUTTON0
 * P1     ----------------- BUTTON1
 * P2     ----------------- BUTTON2
 * P3     ----------------- BUTTON3
 * P4     ----------------- BUTTON4
 * P5     ----------------- BUTTON5
 * P6     ----------------- BUTTON6
 * P7     ----------------- BUTTON7
 *
 */

#include "Arduino.h"
#include "PCF857X.h"  // https://github.com/xreef/PCF857X_library

#define ESP8266_INTERRUPTED_PIN 13

// Set i2c address
PCF857X pcf857X(0x20);

// Function interrupt
bool keyPressed = false;

void ICACHE_RAM_ATTR  keyPressedOnPCF857X(){
//  Serial.println("keyPressedOnPCF857X");
  keyPressed = true;
}

void setup()
{
  Serial.begin(9600);
  delay(1000);

  pinMode(ESP8266_INTERRUPTED_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ESP8266_INTERRUPTED_PIN), keyPressedOnPCF857X, FALLING);

  for(int i=0;i<8;i++) {
    pcf857X.pinMode(i, INPUT);
  }
	Serial.print("Init pcf857X...");
	if (pcf857X.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}
}

void loop()
{
  if (keyPressed){
    PCF857X::DigitalInput val = pcf857X.digitalReadAll();
    if (val.p0==HIGH) Serial.println("KEY0 PRESSED");
    if (val.p1==HIGH) Serial.println("KEY1 PRESSED");
    if (val.p2==HIGH) Serial.println("KEY2 PRESSED");
    if (val.p3==HIGH) Serial.println("KEY3 PRESSED");
    if (val.p4==HIGH) Serial.println("KEY4 PRESSED");
    if (val.p5==HIGH) Serial.println("KEY5 PRESSED");
    if (val.p6==HIGH) Serial.println("KEY6 PRESSED");
    if (val.p7==HIGH) Serial.println("KEY7 PRESSED");
    keyPressed= false;
  }
}
