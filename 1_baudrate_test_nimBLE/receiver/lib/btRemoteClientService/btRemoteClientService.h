#ifndef _BTREMOTECLIENTSERVICE_H
#define _BTREMOTECLIENTSERVICE_H

#include <NimBLEDevice.h>
#include <globalDef.h>

class btRemoteClientService {
private:
    NimBLECharacteristic * pCharRemoteID;
    NimBLECharacteristic * pCharSensorVal;
    NimBLECharacteristic * pCharBatteryLvl;
    bool assigned;
public:
    btRemoteClientService(void);
    void assignService(BLEService* CreatedService);
    uint8_t getRemoteID(void);
    uint8_t getSensor(void);
    uint8_t getBattery(void);
    bool newDataAvailable(void);
    bool isAssigned(void);
    void setAssigned(bool newValue);
    void notificationCheck(void);
    uint8_t getSensorBAUDTEST(void);
    bool newDataAvailableBAUDTEST(void);
};

#endif //_BTREMOTECLIENTSERVICE_H