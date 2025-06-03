/*
 * File:    btClient.h
 * Author:  Arjan Lemmens
 * Created on August 1, 2022, 10:32 PM
 * Version: 1.0
 * 
 * ToDo:    - Add comments
 *          - Clean up code
 */


#ifndef _BTREMOTECLIENTSERVICE_H
#define _BTREMOTECLIENTSERVICE_H

#include <NimBLEDevice.h>
#include <globalDef.h>

class btRemoteClientService {
private:
    NimBLEService* pClientService;
    NimBLECharacteristic * pCharRemoteID;
    NimBLECharacteristic * pCharSensorVal;
    NimBLECharacteristic * pCharBatteryLvl;
    bool assigned;
public:
    ~btRemoteClientService(void);
    btRemoteClientService(void);
    void assignService(BLEService* CreatedService);
    uint8_t getRemoteID(void);
    uint8_t getSensor(void);
    uint8_t getBattery(void);
    bool newDataAvailable(void);
    bool isAssigned(void);
    void setAssigned(bool newValue);
    void notificationCheck(void);
    void setRemoteID(uint8_t newID);
};

#endif //_BTREMOTECLIENTSERVICE_H