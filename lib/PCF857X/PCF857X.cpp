/*
 * PCF857X GPIO Port Expand
 * https://www.mischianti.org/2019/01/02/pcf857X-i2c-digital-i-o-expander-fast-easy-usage/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Renzo Mischianti www.mischianti.org All right reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "PCF857X.h"
#include "Wire.h"

/**
 * Constructor
 * @param address: i2c address
 */
PCF857X::PCF857X(uint8_t address)
{
    _wire = &Wire;

    _address = address;
};

/**
 * Construcor
 * @param address: i2c address
 * @param interruptPin: pin to set interrupt
 * @param interruptFunction: function to call when interrupt raised
 */
PCF857X::PCF857X(uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
{
    _wire = &Wire;

    _address = address;
    _interruptPin = interruptPin;
    _interruptFunction = interruptFunction;
    _usingInterrupt = true;
};

#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(ARDUINO_ARCH_STM32) && !defined(TEENSYDUINO)
/**
 * Constructor
 * @param address: i2c address
 * @param sda: sda pin
 * @param scl: scl pin
 */
PCF857X::PCF857X(uint8_t address, int sda, int scl)
{
    _wire = &Wire;

    _address = address;
    _sda = sda;
    _scl = scl;
};

/**
 * Constructor
 * @param address: i2c address
 * @param sda: sda pin
 * @param scl: scl pin
 * @param interruptPin: pin to set interrupt
 * @param interruptFunction: function to call when interrupt raised
 */
PCF857X::PCF857X(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
{
    _wire = &Wire;

    _address = address;
    _sda = sda;
    _scl = scl;

    _interruptPin = interruptPin;
    _interruptFunction = interruptFunction;

    _usingInterrupt = true;
};
#endif

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD)
/**
 * Constructor
 * @param address: i2c address
 */
PCF857X::PCF857X(TwoWire *pWire, uint8_t address)
{
    _wire = pWire;

    _address = address;
};

/**
 * Construcor
 * @param address: i2c address
 * @param interruptPin: pin to set interrupt
 * @param interruptFunction: function to call when interrupt raised
 */
PCF857X::PCF857X(TwoWire *pWire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
{
    _wire = pWire;

    _address = address;
    _interruptPin = interruptPin;
    _interruptFunction = interruptFunction;
    _usingInterrupt = true;
};
#endif
#if defined(ESP32)
/**
 * Constructor
 * @param address: i2c address
 * @param sda: sda pin
 * @param scl: scl pin
 */
PCF857X::PCF857X(TwoWire *pWire, uint8_t address, int sda, int scl)
{
    _wire = pWire;

    _address = address;
    _sda = sda;
    _scl = scl;
};

/**
 * Constructor
 * @param address: i2c address
 * @param sda: sda pin
 * @param scl: scl pin
 * @param interruptPin: pin to set interrupt
 * @param interruptFunction: function to call when interrupt raised
 */
PCF857X::PCF857X(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
{
    _wire = pWire;

    _address = address;
    _sda = sda;
    _scl = scl;

    _interruptPin = interruptPin;
    _interruptFunction = interruptFunction;

    _usingInterrupt = true;
};
#endif
bool encoderPins[PCF_DATA_SIZE];

void PCF857X::attachInterrupt()
{
    // If using interrupt set interrupt value to pin
    if (_usingInterrupt) {
        for (int i = 0; i < PCF_DATA_SIZE; i++) {
            if (encoderPins[i])
                PCF857X::digitalRead(i);
        }

        DEBUG_PRINTLN("Using interrupt pin (not all pin is interrupted)");
        ::pinMode(_interruptPin, INPUT_PULLUP);
        ::attachInterrupt(digitalPinToInterrupt(_interruptPin), (*_interruptFunction), FALLING);
    }
}
void PCF857X::detachInterrupt()
{
    // If using interrupt set interrupt value to pin
    if (_usingInterrupt) {
        ::detachInterrupt(digitalPinToInterrupt(_interruptPin));
        DEBUG_PRINTLN("Detach interrupt pin");
    }
}

/**
 * wake up i2c controller
 */
bool PCF857X::begin(bool initI2C)
{
    this->transmissionStatus = 4;
    if (initI2C) {
#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(ARDUINO_ARCH_STM32) && !defined(TEENSYDUINO)
        DEBUG_PRINT(F("begin(sda, scl) -> "));
        DEBUG_PRINT(_sda);
        DEBUG_PRINT(F(" "));
        DEBUG_PRINTLN(_scl);
//		_wire->begin(_sda, _scl);
#ifdef ARDUINO_ARCH_STM32
        _wire->begin((uint32_t)_sda, (uint32_t)_scl);
#else
        _wire->begin((int)_sda, (int)_scl);
#endif
#else
        //			Default pin for AVR some problem on software emulation
        //			#define SCL_PIN _scl
        // 			#define SDA_PIN _sda
        _wire->begin();
#endif
    }
    // Check if there are pins to set low
    if (writeMode > 0 || readMode > 0) {
        DEBUG_PRINTLN("Set write mode");
        _wire->beginTransmission(_address);

        DEBUG_PRINT("resetInitial pin ");
#ifdef PCF857X_SOFT_INITIALIZATION
        resetInitial = writeModeUp | readModePullUp;
#else
        resetInitial = writeModeUp | readMode;
#endif
        DEBUG_PRINTLN(resetInitial, BIN);

        _wire->write((uint8_t)resetInitial);
#ifdef PCF8575
        _wire->write((uint8_t)(resetInitial >> 8));
#endif

        initialBuffer = writeModeUp | readModePullUp;
        dataBuffered = initialBuffer;
        writeDataBuffered = writeModeUp;

        DEBUG_PRINTLN("Start end trasmission if stop here check pullup resistor.");
        this->transmissionStatus = _wire->endTransmission();
    }

    PCF857X::attachInterrupt();

    // inizialize last read
    lastReadMillis = millis();

    return this->isLastTransmissionSuccess();
}

/**
 * Set if fin is OUTPUT or INPUT
 * @param pin: pin to set
 * @param mode: mode, supported only INPUT or OUTPUT (to simplify)
 * @param output_start: output_start, for OUTPUT we can set initial value
 */
void PCF857X::pinMode(uint8_t pin, uint8_t mode, uint8_t output_start)
{
    DEBUG_PRINT("Set pin ");
    DEBUG_PRINT(pin);
    DEBUG_PRINT(" as ");
    DEBUG_PRINTLN(mode);

    if (mode == OUTPUT) {
        writeMode = writeMode | bit(pin);
        if (output_start == HIGH) {
            writeModeUp = writeModeUp | bit(pin);
        }

        readMode = readMode & ~bit(pin);
        readModePullDown = readModePullDown & ~bit(pin);
        readModePullUp = readModePullUp & ~bit(pin);

        DEBUG_PRINT("W: ");
        DEBUG_PRINT(writeMode, BIN);
        DEBUG_PRINT(" R ALL: ");
        DEBUG_PRINT(readMode, BIN);

        DEBUG_PRINT(" R Down: ");
        DEBUG_PRINT(readModePullDown, BIN);
        DEBUG_PRINT("R Up: ");
        DEBUG_PRINTLN(readModePullUp, BIN);
    }
    else if (mode == INPUT) {
        writeMode = writeMode & ~bit(pin);

        readMode = readMode | bit(pin);
        readModePullDown = readModePullDown | bit(pin);
        readModePullUp = readModePullUp & ~bit(pin);

        DEBUG_PRINT("W: ");
        DEBUG_PRINT(writeMode, BIN);
        DEBUG_PRINT(" R ALL: ");
        DEBUG_PRINT(readMode, BIN);

        DEBUG_PRINT(" R Down: ");
        DEBUG_PRINT(readModePullDown, BIN);
        DEBUG_PRINT("R Up: ");
        DEBUG_PRINTLN(readModePullUp, BIN);
    }
    else if (mode == INPUT_PULLUP) {
        writeMode = writeMode & ~bit(pin);

        readMode = readMode | bit(pin);
        readModePullDown = readModePullDown & ~bit(pin);
        readModePullUp = readModePullUp | bit(pin);

        DEBUG_PRINT("W: ");
        DEBUG_PRINT(writeMode, BIN);
        DEBUG_PRINT(" R ALL: ");
        DEBUG_PRINT(readMode, BIN);

        DEBUG_PRINT(" R Down: ");
        DEBUG_PRINT(readModePullDown, BIN);
        DEBUG_PRINT("R Up: ");
        DEBUG_PRINTLN(readModePullUp, BIN);
    }
    else {
        DEBUG_PRINTLN("Mode non supported by PCF857X")
    }
};

void PCF857X::encoder(uint8_t pinA, uint8_t pinB)
{
    PCF857X::pinMode(pinA, INPUT_PULLUP);
    PCF857X::pinMode(pinB, INPUT_PULLUP);

    encoderPins[pinA] = true;
    encoderPins[pinB] = true;
}

byte getBit(PCF_DATA_TYPE n, byte position)
{
    return (n >> position) & 1;
}

bool PCF857X::checkProgression(byte oldValA, byte oldValB, byte newValA, byte newValB, byte validProgression)
{
    bool findOldVal = false;
    int posFinded = 0;
    for (int pos = 0; pos < 8; pos = pos + 2) {
        if ((oldValB == ((validProgression & bit(pos + 1)) > 0 ? HIGH : LOW)) && (oldValA == ((validProgression & bit(pos + 0)) > 0 ? HIGH : LOW))) {
            findOldVal = true;
            posFinded = pos;
        }
    }
    if (!findOldVal)
        return false;

    posFinded = posFinded + 2;
    if (posFinded > 8)
        posFinded = 0;

    return ((newValB == ((validProgression & bit(posFinded + 1)) > 0 ? HIGH : LOW)) && (newValA == ((validProgression & bit(posFinded + 0)) > 0 ? HIGH : LOW)));
}

#ifdef BASIC_ENCODER_ALGORITHM
bool PCF857X::readEncoderValue(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation)
{
   PCF857X::detachInterrupt();

    bool changed = false;

    byte na = PCF857X::digitalRead(pinA, true);
    byte nb = PCF857X::digitalRead(pinB, true);

    byte encoderPinALast = (this->encoderValues & bit(pinA)) > 0 ? HIGH : LOW;
    byte encoderPinBLast = (this->encoderValues & bit(pinB)) > 0 ? HIGH : LOW;

    DEBUG_PRINT(pinA);
    DEBUG_PRINT(" TO --> ");
    DEBUG_PRINT(encoderPinALast);
    DEBUG_PRINT(encoderPinBLast);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(na);
    DEBUG_PRINT(nb);
    DEBUG_PRINTLN();

    if ((encoderPinALast != na || encoderPinBLast != nb) && (encoderPinALast == LOW) && (na == HIGH)) {
        if (nb == LOW) {
            *encoderValue = *encoderValue + (!reverseRotation ? +1 : -1);
            changed = true;
        }
        else {
            *encoderValue = *encoderValue + (!reverseRotation ? -1 : +1);
            changed = true;
        }
    }

    this->encoderValues = (encoderPinALast != na) ? this->encoderValues ^ bit(pinA) : this->encoderValues;
    this->encoderValues = (encoderPinBLast != nb) ? this->encoderValues ^ bit(pinB) : this->encoderValues;
    PCF857X::attachInterrupt();
    
    return changed;
}

int8_t PCF857X::readEncoderValue(uint8_t pinA, uint8_t pinB)
{
    volatile long encoderValue = 0;
    PCF857X::readEncoderValue(pinA, pinB, &encoderValue);
    return encoderValue;
}

#endif

#ifdef SEQUENCE_ENCODER_ALGORITHM
bool PCF857X::readEncoderValueSequence(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation)
{
    PCF857X::detachInterrupt();
    bool changed = false;

    delay(100);
    byte na = PCF857X::digitalRead(pinA, true);
    byte nb = PCF857X::digitalRead(pinB, true);

    byte encoderPinALast = (this->encoderValues & bit(pinA)) > 0 ? HIGH : LOW;
    byte encoderPinBLast = (this->encoderValues & bit(pinB)) > 0 ? HIGH : LOW;

    DEBUG_PRINT(pinA);
    DEBUG_PRINT(" TO --> ");
    DEBUG_PRINT(encoderPinALast);
    DEBUG_PRINT(encoderPinBLast);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(na);
    DEBUG_PRINT(nb);
    DEBUG_PRINT(" -- ");

    int encoded = (na << 1) | nb;  // converting the 2 pin value to single number
    int lastEncoded = (encoderPinALast << 1) | encoderPinBLast;
    int sum = (lastEncoded << 2) | encoded;  // adding it to the previous encoded value

    DEBUG_PRINT("sum - ");
    DEBUG_PRINT(sum, BIN);

    DEBUG_PRINT(" enc - ");
    DEBUG_PRINT(*encoderValue);

    if (
        sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        //			  encoderValue ++;
        *encoderValue = *encoderValue + (!reverseRotation ? +1 : -1);
        changed = true;
    }
    if (
        sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        *encoderValue = *encoderValue + (!reverseRotation ? -1 : +1);
        changed = true;
        //			  encoderValue --;
    }

    DEBUG_PRINT(" enc next - ");
    DEBUG_PRINTLN(*encoderValue);

    this->encoderValues = (encoderPinALast != na) ? this->encoderValues ^ bit(pinA) : this->encoderValues;
    this->encoderValues = (encoderPinBLast != nb) ? this->encoderValues ^ bit(pinB) : this->encoderValues;
    PCF857X::attachInterrupt();
    return changed;
}
int8_t PCF857X::readEncoderValueSequence(uint8_t pinA, uint8_t pinB)
{
    volatile long encoderValue = 0;
    PCF857X::readEncoderValueSequence(pinA, pinB, &encoderValue);
    return encoderValue;
}

#endif
#ifdef SEQUENCE_ENCODER_ALGORITHM_REDUCED
bool PCF857X::readEncoderValueSequenceReduced(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation)
{
    PCF857X::detachInterrupt();
    bool changed = false;

    delay(100);
    byte na = PCF857X::digitalRead(pinA, true);
    byte nb = PCF857X::digitalRead(pinB, true);

    byte encoderPinALast = (this->encoderValues & bit(pinA)) > 0 ? HIGH : LOW;
    byte encoderPinBLast = (this->encoderValues & bit(pinB)) > 0 ? HIGH : LOW;

    DEBUG_PRINT(pinA);
    DEBUG_PRINT(" TO --> ");
    DEBUG_PRINT(encoderPinALast);
    DEBUG_PRINT(encoderPinBLast);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(na);
    DEBUG_PRINT(nb);
    DEBUG_PRINT(" -- ");

    int encoded = (na << 1) | nb;  // converting the 2 pin value to single number
    int lastEncoded = (encoderPinALast << 1) | encoderPinBLast;
    int sum = (lastEncoded << 2) | encoded;  // adding it to the previous encoded value

    DEBUG_PRINT("sum - ");
    DEBUG_PRINT(sum, BIN);

    DEBUG_PRINT(" enc - ");
    DEBUG_PRINT(*encoderValue);

    if (
        sum == 0b1101
        // || sum == 0b0100
        || sum == 0b0010
        // || sum == 0b1011
    ) {
        //			  encoderValue ++;
        *encoderValue = *encoderValue + (!reverseRotation ? +1 : -1);
        changed = true;
    }
    if (
        sum == 0b1110
        // || sum == 0b0111
        || sum == 0b0001
        // || sum == 0b1000
    ) {
        *encoderValue = *encoderValue + (!reverseRotation ? -1 : +1);
        changed = true;
        //			  encoderValue --;
    }

    DEBUG_PRINT(" enc next - ");
    DEBUG_PRINTLN(*encoderValue);

    this->encoderValues = (encoderPinALast != na) ? this->encoderValues ^ bit(pinA) : this->encoderValues;
    this->encoderValues = (encoderPinBLast != nb) ? this->encoderValues ^ bit(pinB) : this->encoderValues;
    PCF857X::attachInterrupt();
    return changed;
}
int8_t PCF857X::readEncoderValueSequenceReduced(uint8_t pinA, uint8_t pinB)
{
    volatile long encoderValue = 0;
    PCF857X::readEncoderValueSequenceReduced(pinA, pinB, &encoderValue);
    return encoderValue;
}

#endif
#ifdef MISCHIANTI_ENCODER_ALGORITHM
bool PCF857X::readEncoderValueMischianti(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation)
{
    PCF857X::detachInterrupt();
    bool changed = false;

    byte na = PCF857X::digitalRead(pinA, true);
    byte nb = PCF857X::digitalRead(pinB, true);

    byte encoderPinALast = (this->encoderValues & bit(pinA)) > 0 ? HIGH : LOW;
    byte encoderPinBLast = (this->encoderValues & bit(pinB)) > 0 ? HIGH : LOW;

    if ((encoderPinALast != na || encoderPinBLast != nb) && ((encoderPinALast == LOW) || encoderPinALast == encoderPinBLast) && (na == HIGH)) {
        DEBUG_PRINT("TO --> ");
        DEBUG_PRINT(encoderPinALast);
        DEBUG_PRINT(encoderPinBLast);
        DEBUG_PRINT(" - ");
        DEBUG_PRINT(na);
        DEBUG_PRINT(nb);
        DEBUG_PRINTLN();

        if (nb == LOW && nb != na) {
            *encoderValue = *encoderValue + (!reverseRotation ? +1 : -1);
            changed = true;
        }
        else if (nb == na && encoderPinALast == encoderPinBLast) {
            *encoderValue = *encoderValue + (!reverseRotation ? -1 : +1);
            changed = true;
        }
    }
    //		  encoderValues = encoderValues & (~(bit(pinA) | bit(pinB)));
    //		  if (na == HIGH){
    //			  encoderValues = encoderValues | bit(pinA);
    //		  }
    //		  if (nb == HIGH){
    //			  encoderValues = encoderValues | bit(pinA);
    //		  }

    if (encoderPinALast != na || encoderPinBLast != nb) {
        this->encoderValues = (encoderPinALast != na) ? this->encoderValues ^ bit(pinA) : this->encoderValues;
        this->encoderValues = (encoderPinBLast != nb) ? this->encoderValues ^ bit(pinB) : this->encoderValues;
    }

    PCF857X::attachInterrupt();
    return changed;
}
int8_t PCF857X::readEncoderValueMischianti(uint8_t pinA, uint8_t pinB)
{
    volatile long encoderValue = 0;
    PCF857X::readEncoderValueMischianti(pinA, pinB, &encoderValue);
    return encoderValue;
}

#endif

#ifdef POKI_ENCODER_ALGORITHM
bool PCF857X::readEncoderValuePoki(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation)
{
    PCF857X::detachInterrupt();

    bool changed = false;

    byte na = PCF857X::digitalRead(pinA, true);
    byte nb = PCF857X::digitalRead(pinB, true);

    byte encoderPinALast = (this->encoderValues & bit(pinA)) > 0 ? HIGH : LOW;
    byte encoderPinBLast = (this->encoderValues & bit(pinB)) > 0 ? HIGH : LOW;

    DEBUG_PRINT("TO --> ");
    DEBUG_PRINT(encoderPinALast);
    DEBUG_PRINT(encoderPinBLast);
    DEBUG_PRINT(" - ");
    DEBUG_PRINT(na);
    DEBUG_PRINT(nb);
    DEBUG_PRINTLN();

    if ((encoderPinALast != na || encoderPinBLast != nb) && ((encoderPinALast == LOW) || encoderPinALast == encoderPinBLast) && (na == HIGH)) {
        DEBUG_PRINT("TO --> ");
        DEBUG_PRINT(encoderPinALast);
        DEBUG_PRINT(encoderPinBLast);
        DEBUG_PRINT(" - ");
        DEBUG_PRINT(na);
        DEBUG_PRINT(nb);
        DEBUG_PRINTLN();

        if (na && !nb) {
            if (encoderPinBLast) {
                *encoderValue = *encoderValue + (!reverseRotation ? +1 : -1);
            }
            else {
                *encoderValue = *encoderValue + (!reverseRotation ? -1 : +1);
            }
            changed = true;
        }
    }

    this->encoderValues = (encoderPinALast != na) ? this->encoderValues ^ bit(pinA) : encoderValues;
    this->encoderValues = (encoderPinBLast != nb) ? this->encoderValues ^ bit(pinB) : encoderValues;
    PCF857X::attachInterrupt();

    return changed;
}
int8_t PCF857X::readEncoderValuePoki(uint8_t pinA, uint8_t pinB)
{
    volatile long encoderValue = 0;
    PCF857X::readEncoderValue(pinA, pinB, &encoderValue);
    return encoderValue;
}

#endif

/**
 * Read value from i2c and bufferize it
 * @param force
 */
void PCF857X::readBuffer(bool force)
{
    if (millis() > PCF857X::lastReadMillis + latency || _usingInterrupt || force) {
#ifdef PCF8575
        _wire->requestFrom(_address, (uint8_t)2);  // Begin transmission to PCF857X with the buttons
#else
        _wire->requestFrom(_address, (uint8_t)1);  // Begin transmission to PCF857X with the buttons
#endif
        lastReadMillis = millis();
        if (_wire->available())  // If bytes are available to be recieved
        {
#ifdef PCF8575
            PCF_DATA_TYPE iInput = _wire->read() | (_wire->read() << 8);
#else
            PCF_DATA_TYPE iInput = _wire->read();  // Read a byte
#endif

            if ((iInput & readModePullDown) > 0 and (~iInput & readModePullUp) > 0) {
                //			  if ((iInput & readMode)>0){
                dataBuffered = (dataBuffered & ~readMode) | (PCF_DATA_TYPE)iInput;
            }
        }
    }
}

#ifndef PCF857X_LOW_MEMORY
/**
 * Read value of all INPUT pin
 * Debounce read more fast than 10millis, non managed for interrupt mode
 * @return
 */
PCF857X::DigitalInput PCF857X::digitalReadAll(void)
{
    DEBUG_PRINTLN("Read from buffer");
#ifdef PCF8575
    _wire->requestFrom(_address, (uint8_t)2);  // Begin transmission to PCF857X with the buttons
#else
    _wire->requestFrom(_address, (uint8_t)1);  // Begin transmission to PCF857X with the buttons
#endif
    lastReadMillis = millis();
    if (_wire->available())  // If bytes are available to be received
    {
        DEBUG_PRINTLN("Data ready");
#ifdef PCF8575
        PCF_DATA_TYPE iInput = _wire->read() | (_wire->read() << 8);
#else
        PCF_DATA_TYPE iInput = _wire->read();  // Read a byte
#endif

        if ((readModePullDown & iInput) > 0 or (readModePullUp & ~iInput) > 0) {
            DEBUG_PRINT(" -------- CHANGE --------- ");
            dataBuffered = (dataBuffered & ~readMode) | (PCF_DATA_TYPE)iInput;
        }
    }

    DEBUG_PRINT("Buffer value ");
    DEBUG_PRINTLN(dataBuffered, BIN);

    if ((bit(0) & readMode) > 0)
        digitalInput.p0 = ((dataBuffered & bit(0)) > 0) ? HIGH : LOW;
    if ((bit(1) & readMode) > 0)
        digitalInput.p1 = ((dataBuffered & bit(1)) > 0) ? HIGH : LOW;
    if ((bit(2) & readMode) > 0)
        digitalInput.p2 = ((dataBuffered & bit(2)) > 0) ? HIGH : LOW;
    if ((bit(3) & readMode) > 0)
        digitalInput.p3 = ((dataBuffered & bit(3)) > 0) ? HIGH : LOW;
    if ((bit(4) & readMode) > 0)
        digitalInput.p4 = ((dataBuffered & bit(4)) > 0) ? HIGH : LOW;
    if ((bit(5) & readMode) > 0)
        digitalInput.p5 = ((dataBuffered & bit(5)) > 0) ? HIGH : LOW;
    if ((bit(6) & readMode) > 0)
        digitalInput.p6 = ((dataBuffered & bit(6)) > 0) ? HIGH : LOW;
    if ((bit(7) & readMode) > 0)
        digitalInput.p7 = ((dataBuffered & bit(7)) > 0) ? HIGH : LOW;
#ifdef PCF8575
    if ((bit(8) & readMode) > 0)
        digitalInput.p8 = ((dataBuffered & bit(8)) > 0) ? HIGH : LOW;
    if ((bit(9) & readMode) > 0)
        digitalInput.p9 = ((dataBuffered & bit(9)) > 0) ? HIGH : LOW;
    if ((bit(10) & readMode) > 0)
        digitalInput.p10 = ((dataBuffered & bit(10)) > 0) ? HIGH : LOW;
    if ((bit(11) & readMode) > 0)
        digitalInput.p11 = ((dataBuffered & bit(11)) > 0) ? HIGH : LOW;
    if ((bit(12) & readMode) > 0)
        digitalInput.p12 = ((dataBuffered & bit(12)) > 0) ? HIGH : LOW;
    if ((bit(13) & readMode) > 0)
        digitalInput.p13 = ((dataBuffered & bit(13)) > 0) ? HIGH : LOW;
    if ((bit(14) & readMode) > 0)
        digitalInput.p14 = ((dataBuffered & bit(14)) > 0) ? HIGH : LOW;
    if ((bit(15) & readMode) > 0)
        digitalInput.p15 = ((dataBuffered & bit(15)) > 0) ? HIGH : LOW;
#endif

    if ((bit(0) & writeMode) > 0)
        digitalInput.p0 = ((writeDataBuffered & bit(0)) > 0) ? HIGH : LOW;
    if ((bit(1) & writeMode) > 0)
        digitalInput.p1 = ((writeDataBuffered & bit(1)) > 0) ? HIGH : LOW;
    if ((bit(2) & writeMode) > 0)
        digitalInput.p2 = ((writeDataBuffered & bit(2)) > 0) ? HIGH : LOW;
    if ((bit(3) & writeMode) > 0)
        digitalInput.p3 = ((writeDataBuffered & bit(3)) > 0) ? HIGH : LOW;
    if ((bit(4) & writeMode) > 0)
        digitalInput.p4 = ((writeDataBuffered & bit(4)) > 0) ? HIGH : LOW;
    if ((bit(5) & writeMode) > 0)
        digitalInput.p5 = ((writeDataBuffered & bit(5)) > 0) ? HIGH : LOW;
    if ((bit(6) & writeMode) > 0)
        digitalInput.p6 = ((writeDataBuffered & bit(6)) > 0) ? HIGH : LOW;
    if ((bit(7) & writeMode) > 0)
        digitalInput.p7 = ((writeDataBuffered & bit(7)) > 0) ? HIGH : LOW;
#ifdef PCF8575
    if ((bit(8) & writeMode) > 0)
        digitalInput.p8 = ((writeDataBuffered & bit(8)) > 0) ? HIGH : LOW;
    if ((bit(9) & writeMode) > 0)
        digitalInput.p9 = ((writeDataBuffered & bit(9)) > 0) ? HIGH : LOW;
    if ((bit(10) & writeMode) > 0)
        digitalInput.p10 = ((writeDataBuffered & bit(10)) > 0) ? HIGH : LOW;
    if ((bit(11) & writeMode) > 0)
        digitalInput.p11 = ((writeDataBuffered & bit(11)) > 0) ? HIGH : LOW;
    if ((bit(12) & writeMode) > 0)
        digitalInput.p12 = ((writeDataBuffered & bit(12)) > 0) ? HIGH : LOW;
    if ((bit(13) & writeMode) > 0)
        digitalInput.p13 = ((writeDataBuffered & bit(13)) > 0) ? HIGH : LOW;
    if ((bit(14) & writeMode) > 0)
        digitalInput.p14 = ((writeDataBuffered & bit(14)) > 0) ? HIGH : LOW;
    if ((bit(15) & writeMode) > 0)
        digitalInput.p15 = ((writeDataBuffered & bit(15)) > 0) ? HIGH : LOW;
#endif

    dataBuffered = (initialBuffer & readMode) | (dataBuffered & ~readMode);  //~readMode & dataBuffered;

    DEBUG_PRINT("Buffer hight value readed set readed ");
    DEBUG_PRINTLN(dataBuffered, BIN);
    //}
    DEBUG_PRINT("Return value ");
    return digitalInput;
};
#else
/**
 * Read value of all INPUT pin in byte format for low memory usage
 * Debounce read more fast than 10millis, non managed for interrupt mode
 * @return
 */
byte PCF857X::digitalReadAll(void)
{
    DEBUG_PRINTLN("Read from buffer");
#ifdef PCF8575
    _wire->requestFrom(_address, (uint8_t)2);  // Begin transmission to PCF857X with the buttons
#else
    _wire->requestFrom(_address, (uint8_t)1);  // Begin transmission to PCF857X with the buttons
#endif
    lastReadMillis = millis();
    if (_wire->available())  // If bytes are available to be recieved
    {
        DEBUG_PRINTLN("Data ready");
#ifdef PCF8575
        PCF_DATA_TYPE iInput = _wire->read() | (_wire->read() << 8);
#else
        PCF_DATA_TYPE iInput = _wire->read();  // Read a byte
#endif

        if ((readModePullDown & iInput) > 0 or (readModePullUp & ~iInput) > 0) {
            DEBUG_PRINT(" -------- CHANGE --------- ");
            dataBuffered = (dataBuffered & ~readMode) | (PCF_DATA_TYPE)iInput;
        }
    }

    DEBUG_PRINT("Buffer value ");
    DEBUG_PRINTLN(dataBuffered, BIN);

    PCF_DATA_TYPE dataRead = dataBuffered | writeDataBuffered;

    dataBuffered = (initialBuffer & readMode) | (dataBuffered & ~readMode);  //~readMode & dataBuffered;
    DEBUG_PRINT("Buffer hight value readed set readed ");
    DEBUG_PRINTLN(dataBuffered, BIN);
    //}
    DEBUG_PRINT("Return value ");
    return dataRead;
};
#endif

/**
 * Read value of specified pin
 * Debounce read more fast than 10millis, non managed for interrupt mode
 * @param pin
 * @return
 */
uint8_t PCF857X::digitalRead(uint8_t pin, bool forceReadNow)
{
    uint8_t value = (bit(pin) & readModePullUp) ? HIGH : LOW;
    DEBUG_PRINT("Read pin ");
    DEBUG_PRINT(pin);
    // Check if pin already HIGH than read and prevent reread of i2c
    //	DEBUG_PRINTLN("----------------------------------")
    //	DEBUG_PRINT("readModePullUp   ");
    //	DEBUG_PRINTLN(readModePullUp, BIN);
    //	DEBUG_PRINT("readModePullDown ");
    //	DEBUG_PRINTLN(readModePullDown, BIN);
    //	DEBUG_PRINT("dataBuffered     ");
    //	DEBUG_PRINTLN(dataBuffered, BIN);

    if ((((bit(pin) & (readModePullDown & dataBuffered)) > 0) or (bit(pin) & (readModePullUp & ~dataBuffered)) > 0)) {
        DEBUG_PRINTLN(" ...Pin already set");
        if ((bit(pin) & dataBuffered) > 0) {
            value = HIGH;
        }
        else {
            value = LOW;
        }
    }
    else if (forceReadNow || (millis() > PCF857X::lastReadMillis + latency)) {
        DEBUG_PRINT(" ...Read from buffer... ");
#ifdef PCF8575
        _wire->requestFrom(_address, (uint8_t)2);  // Begin transmission to PCF857X with the buttons
#else
        _wire->requestFrom(_address, (uint8_t)1);  // Begin transmission to PCF857X with the buttons
#endif
        lastReadMillis = millis();
        if (_wire->available())  // If bytes are available to be recieved
        {
            DEBUG_PRINTLN(" Data ready");
#ifdef PCF8575
            PCF_DATA_TYPE iInput = _wire->read() | (_wire->read() << 8);
#else
            PCF_DATA_TYPE iInput = _wire->read();  // Read a byte
#endif
            DEBUG_PRINT("Input ");
            DEBUG_PRINT((PCF_DATA_TYPE)iInput, BIN);

            if ((readModePullDown & iInput) > 0 or (readModePullUp & ~iInput) > 0) {
                DEBUG_PRINT(" -------- CHANGE --------- ");
                dataBuffered = (dataBuffered & ~readMode) | (PCF_DATA_TYPE)iInput;
                if ((bit(pin) & dataBuffered) > 0) {
                    value = HIGH;
                }
                else {
                    value = LOW;
                }
            }
        }
    }
    DEBUG_PRINT(" ..Buffer value ");
    DEBUG_PRINT(dataBuffered, BIN);
    // If HIGH set to low to read buffer only one time
    if ((bit(pin) & readModePullDown) and value == HIGH) {
        dataBuffered = bit(pin) ^ dataBuffered;
        DEBUG_PRINT(" ...Buffer hight value readed set readed ");
        DEBUG_PRINT(dataBuffered, BIN);
    }
    else if ((bit(pin) & readModePullUp) and value == LOW) {
        dataBuffered = bit(pin) ^ dataBuffered;
        DEBUG_PRINT(" ...Buffer low value readed set readed ");
        DEBUG_PRINT(dataBuffered, BIN);
    }
    else if (bit(pin) & writeDataBuffered) {
        value = HIGH;
    }
    DEBUG_PRINT(" ...Return value ");
    DEBUG_PRINTLN(value);
    return value;
};

/**
 * Write on pin
 * @param pin
 * @param value
 */
bool PCF857X::digitalWrite(uint8_t pin, uint8_t value)
{
    DEBUG_PRINTLN("Begin trasmission");
    _wire->beginTransmission(_address);  // Begin the transmission to PCF857X
    DEBUG_PRINT("Value ");
    DEBUG_PRINT(value);
    DEBUG_PRINT(" Write data pre ");
    DEBUG_PRINT(writeDataBuffered, BIN);

    if (value == HIGH) {
        writeDataBuffered = writeDataBuffered | bit(pin);
        dataBuffered = writeDataBuffered | bit(pin);
    }
    else {
        writeDataBuffered = writeDataBuffered & ~bit(pin);
        dataBuffered = writeDataBuffered & ~bit(pin);
    }
    DEBUG_PRINT("Write data ");
    DEBUG_PRINT(writeDataBuffered, BIN);
    DEBUG_PRINT(" for pin ");
    DEBUG_PRINT(pin);
    DEBUG_PRINT(" bin value ");
    DEBUG_PRINT(bit(pin), BIN);
    DEBUG_PRINT(" value ");
    DEBUG_PRINT(value);

    dataBuffered = (writeDataBuffered & writeMode) | (resetInitial & readMode);

    DEBUG_PRINT(" dataBuffered ");
    DEBUG_PRINTLN(dataBuffered, BIN);

    DEBUG_PRINT("Going to write data ");
    DEBUG_PRINTLN(writeDataBuffered, BIN);

    _wire->write((uint8_t)dataBuffered);
#ifdef PCF8575
    _wire->write((uint8_t)(dataBuffered >> 8));
#endif

    dataBuffered = (writeDataBuffered & writeMode) | (initialBuffer & readMode);

    DEBUG_PRINTLN("Start end trasmission if stop here check pullup resistor.");

    this->transmissionStatus = _wire->endTransmission();

    return this->isLastTransmissionSuccess();
};

#ifndef PCF857X_LOW_MEMORY
/**
 * Read value of all INPUT pin
 * Debounce read more fast than 10millis, non managed for interrupt mode
 * @return
 */
void PCF857X::setVal(uint8_t pin, uint8_t value)
{
    if (value == HIGH) {
        writeDataBuffered = writeDataBuffered | bit(pin);
        dataBuffered = writeDataBuffered | bit(pin);
    }
    else {
        writeDataBuffered = writeDataBuffered & ~bit(pin);
        dataBuffered = writeDataBuffered & ~bit(pin);
    }
}
bool PCF857X::digitalWriteAll(PCF857X::DigitalInput digitalInput)
{
    setVal(P0, digitalInput.p0);
    setVal(P1, digitalInput.p1);
    setVal(P2, digitalInput.p2);
    setVal(P3, digitalInput.p3);
    setVal(P4, digitalInput.p4);
    setVal(P5, digitalInput.p5);
    setVal(P6, digitalInput.p6);
    setVal(P7, digitalInput.p7);
#ifdef PCF8575
    setVal(P8, digitalInput.p8);
    setVal(P9, digitalInput.p9);
    setVal(P10, digitalInput.p10);
    setVal(P11, digitalInput.p11);
    setVal(P12, digitalInput.p12);
    setVal(P13, digitalInput.p13);
    setVal(P14, digitalInput.p14);
    setVal(P15, digitalInput.p15);
#endif

    return digitalWriteAllBytes(writeDataBuffered);
}
#else
bool PCF857X::digitalWriteAll(PCF_DATA_TYPE digitalInput)
{
    return digitalWriteAllBytes(digitalInput);
}
#endif

bool PCF857X::digitalWriteAllBytes(PCF_DATA_TYPE allpins)
{
    _wire->beginTransmission(_address);  // Begin the transmission to PCF857X

    writeDataBuffered = allpins;
    dataBuffered = (writeDataBuffered & writeMode) | (resetInitial & readMode);

    DEBUG_PRINT(" dataBuffered ");
    DEBUG_PRINTLN(dataBuffered, BIN);

    DEBUG_PRINT("Going to write data ");
    DEBUG_PRINTLN(writeDataBuffered, BIN);

    _wire->write((uint8_t)dataBuffered);
#ifdef PCF8575
    _wire->write((uint8_t)(dataBuffered >> 8));
#endif

    dataBuffered = (writeDataBuffered & writeMode) | (initialBuffer & readMode);

    DEBUG_PRINTLN("Start end trasmission if stop here check pullup resistor.");

    this->transmissionStatus = _wire->endTransmission();

    return this->isLastTransmissionSuccess();
}
