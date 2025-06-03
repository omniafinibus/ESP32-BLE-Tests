/*
 * File:    stepSensor.h
 * Author:  Arjan Lemmens
 * Created on August 9, 2022, 3:09 PM
 * Version: 0.1
 * 
 * ToDo:    - Add comments
 *          - Clean up code
 */

#ifndef SENSOR_H
#define	SENSOR_H

#include <globalDef.h>

#define THRES_SAMPLE_SIZE   50
#define SAMPLE_PERIOD_mS    5

class stepSensor {
    private:
        void (*pCallback)(void);
        uint16_t adc_result;
        uint16_t max_threshold;
        uint16_t min_threshold;
        uint8_t function_id;
        uint8_t pin_number;
        bool digital_state;
        bool pin_assinged;
        bool int_attached;
    public:
        stepSensor(uint8_t sensorFunctionID, uint8_t inputPin);
        stepSensor(uint8_t sensorFunctionID, uint8_t inputPin, void (&ISR)(void));
        ~stepSensor();
        bool getDigitalValue(void);
        bool getAnalogValue(void);
        void getAnalogValue(uint8_t & valueBuffer);
        void setFunctionID(uint8_t newFunctionValue);
        uint8_t getFunctionID(void);
        void setMinThreshold(uint16_t newThreshold);
        void setMaxThreshold(uint16_t newThreshold);
        void calibrateMinThreshold(void);
        void calibrateMaxThreshold(void);
        void setPin(uint8_t newPinNumber, bool keepInterrupt) ;
        void setPin(uint8_t newPinNumber);
        uint8_t getPin(void);
        void setIntOnChange(void (&ISR)(void));
};

#endif	/* SENSOR_H */