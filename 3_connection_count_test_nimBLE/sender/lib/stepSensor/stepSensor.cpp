/*
 * File:    stepSensor.c
 * Author:  Arjan Lemmens
 * Created on August 9, 2022, 3:09 PM
 * Version: 0.1
 * 
 * ToDo:    - Test schmitt trigger with velostat
 *          - Make stepSensor function changeable
 *          - Add EEPROM functionality
 */

#include <stepSensor.h>

stepSensor::stepSensor(uint8_t sensorFunctionID, uint8_t inputPin) {
    adc_result = 0x0000;
    max_threshold = MAX_THRESHOLD_10BIT;
    min_threshold = MIN_THRESHOLD_10BIT;
    function_id = sensorFunctionID;

    if (inputPin != NULL) { 
        pin_number = inputPin;
        pinMode(pin_number, INPUT);
        pin_assinged = true;
        digital_state = digitalRead(pin_number);
    }
    else { 
        pin_assinged = false; 
        digital_state = false;
    }
}

stepSensor::stepSensor(uint8_t sensorFunctionID, uint8_t inputPin, void (&ISR)(void)) {
    adc_result = 0x0000;
    max_threshold = MAX_THRESHOLD_10BIT;
    min_threshold = MIN_THRESHOLD_10BIT;
    function_id = sensorFunctionID;

    if (inputPin != NULL) { 
        pin_number = inputPin;
        pCallback = *ISR;
        pinMode(pin_number, INPUT);
        attachInterrupt(pin_number, pCallback, CHANGE);
        digital_state = digitalRead(pin_number);
        pin_assinged = true;
        int_attached = true;
    }
    else { 
        int_attached = false;
        pin_assinged = false; 
        digital_state = false;
    }
}

stepSensor::~stepSensor() {
    detachInterrupt(pin_number);
}

bool stepSensor::getDigitalValue(void) {
    digital_state = digitalRead(pin_number);
    return digital_state;
}

bool stepSensor::getAnalogValue(void) {
    adc_result = analogRead(pin_number);

    //Software defined schmitt trigger
    if (digital_state && adc_result <= min_threshold) {
        digital_state = false;
    }
    else if (!digital_state && adc_result >= max_threshold) {
        digital_state = true;
    }

    return digital_state;
}

void stepSensor::getAnalogValue(uint8_t & valueBuffer) {
    valueBuffer = analogRead(pin_number);
}

void stepSensor::setFunctionID(uint8_t newFunctionValue) {
    function_id = newFunctionValue;
}

uint8_t stepSensor::getFunctionID(void) {
    return function_id;
}

void stepSensor::setMinThreshold(uint16_t newThreshold) {
    min_threshold = newThreshold;
}

void stepSensor::setMaxThreshold(uint16_t newThreshold) {
    max_threshold = newThreshold;
}

void stepSensor::calibrateMinThreshold(void) {
    uint32_t totalMeasured;

    for(uint8_t crntSample = 0; crntSample < THRES_SAMPLE_SIZE; crntSample++) {
        totalMeasured += analogRead(pin_number);
        delay(SAMPLE_PERIOD_mS);
    }

    min_threshold = (uint16_t)(totalMeasured / THRES_SAMPLE_SIZE);

    if(min_threshold > MIN_THRESHOLD_10BIT) { max_threshold = MIN_THRESHOLD_10BIT; }
}

void stepSensor::calibrateMaxThreshold(void) {
    uint32_t totalMeasured;

    for(uint8_t crntSample = 0; crntSample < THRES_SAMPLE_SIZE; crntSample++) {
        totalMeasured += analogRead(pin_number);
        delay(SAMPLE_PERIOD_mS);
    }

    max_threshold = (uint16_t)(totalMeasured / THRES_SAMPLE_SIZE);

    if(max_threshold < MAX_THRESHOLD_10BIT) { max_threshold = MAX_THRESHOLD_10BIT; }
}

void stepSensor::setPin(uint8_t newPinNumber, bool keepInterrupt) {
    if (int_attached && keepInterrupt) {
        detachInterrupt(pin_number);
        attachInterrupt(newPinNumber, pCallback, CHANGE);
    }

    pin_number = newPinNumber;
    pinMode(pin_number, INPUT);
}

void stepSensor::setPin(uint8_t newPinNumber) {
    pin_number = newPinNumber;
    pinMode(pin_number, INPUT);
}

uint8_t stepSensor::getPin(void) {
    return pin_number;
}

void stepSensor::setIntOnChange(void (&ISR)(void)) {
    pCallback = *ISR;
    int_attached = true;
    attachInterrupt(pin_number, pCallback, CHANGE);
}