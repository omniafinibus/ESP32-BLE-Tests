#include <btRemoteClientService.h>

btRemoteClientService::btRemoteClientService(void) {
    assigned = false;
    pClientService = NULL;
    pCharRemoteID = NULL;
    pCharSensorVal = NULL;
    pCharBatteryLvl = NULL;
}

btRemoteClientService::~btRemoteClientService(void) {
    pClientService->~NimBLEService();
    delete pClientService;
    pCharRemoteID->~NimBLECharacteristic();
    delete pCharRemoteID;
    pCharSensorVal->~NimBLECharacteristic();
    delete pCharSensorVal;    
    pCharBatteryLvl->~NimBLECharacteristic();
    delete pCharBatteryLvl;
}

void btRemoteClientService::assignService(BLEService* CreatedService) {
    pClientService = CreatedService;
    uint8_t defaultValue = 0x00;    //no sensors pressed
    uint8_t defaultID = 0x01;       //1
    uint8_t defaultBatLvl = 0x64;   //100%

    pCharRemoteID = pClientService->createCharacteristic( REMOTE_ID_CHAR_UUID, 
                                                          NIMBLE_PROPERTY::READ |
                                                          NIMBLE_PROPERTY::NOTIFY | 
                                                          NIMBLE_PROPERTY::WRITE |    
                                                          NIMBLE_PROPERTY::WRITE_NR );
    pCharSensorVal = pClientService->createCharacteristic( SENSOR_CHAR_UUID, 
                                                           NIMBLE_PROPERTY::READ | 
                                                           NIMBLE_PROPERTY::WRITE |    
                                                           NIMBLE_PROPERTY::WRITE_NR );
    pCharBatteryLvl = pClientService->createCharacteristic( BAT_LVL_CHAR_UUID,
                                                            NIMBLE_PROPERTY::READ |  
                                                            NIMBLE_PROPERTY::WRITE |    
                                                            NIMBLE_PROPERTY::WRITE_NR );
    pCharRemoteID->setValue(&defaultValue, 1);
    pCharSensorVal->setValue(&defaultID, 1);
    pCharBatteryLvl->setValue(&defaultBatLvl, 1);

    pClientService->start();
}

uint8_t btRemoteClientService::getRemoteID(void) {
    return *pCharRemoteID->getValue().data();
}

uint8_t btRemoteClientService::getSensor(void) {
    uint8_t intValue = *pCharSensorVal->getValue().data();  //Read the value and set convert it to an integer
    intValue = intValue & ~NEW_DATA_BIT;                    //Remove the new data bit
    pCharSensorVal->setValue(&intValue, 1);                 //Update the characterisitc
    return intValue;
}

uint8_t btRemoteClientService::getBattery(void) {
    return *pCharBatteryLvl->getValue().data();
}

bool btRemoteClientService::newDataAvailable(void) {
    if(( *pCharSensorVal->getValue().data() & NEW_DATA_BIT ) == NEW_DATA_BIT) { return true; } 
    else { return false; }
}

bool btRemoteClientService::isAssigned(void) {
    return assigned;
}

void btRemoteClientService::setAssigned(bool newValue) {
    assigned = newValue;
}

void btRemoteClientService::setRemoteID(uint8_t newID) {
    pCharRemoteID->setValue(&newID, 1);
    pCharRemoteID->notify();
}