#include <btRemoteClientService.h>

btRemoteClientService::btRemoteClientService(void) {
    assigned = false;
}

void btRemoteClientService::assignService(BLEService* CreatedService) {
    uint8_t defaultValue = 0x00;
    uint8_t defaultID = 0x01;
    uint8_t defaultBatLvl = 0x64;

    //pServiceRemoteClient = CreatedService;
    pCharRemoteID = CreatedService->createCharacteristic( REMOTE_ID_CHAR_UUID, 
                                                          NIMBLE_PROPERTY::WRITE |    
                                                          NIMBLE_PROPERTY::WRITE_NR );
    pCharSensorVal = CreatedService->createCharacteristic( SENSOR_CHAR_UUID, 
                                                           NIMBLE_PROPERTY::WRITE |    
                                                           NIMBLE_PROPERTY::WRITE_NR | 
                                                           NIMBLE_PROPERTY::NOTIFY );
    pCharBatteryLvl = CreatedService->createCharacteristic( BAT_LVL_CHAR_UUID, 
                                                            NIMBLE_PROPERTY::WRITE |    
                                                            NIMBLE_PROPERTY::WRITE_NR );
    CreatedService->start();

    pCharRemoteID->setValue(&defaultValue, 1);
    pCharSensorVal->setValue(&defaultID, 1);
    pCharBatteryLvl->setValue(&defaultBatLvl, 1);
}

uint8_t btRemoteClientService::getRemoteID(void) {
    return *pCharRemoteID->getValue().data();
}

uint8_t btRemoteClientService::getSensor(void) {
    uint8_t intValue = *pCharSensorVal->getValue().data();    //Read the value and set convert it to an integer
    intValue = intValue & 0x7F;                             //Remove the new data bit
    pCharSensorVal->setValue(&intValue, 1);                 //Update the characterisitc
    return intValue;
}

uint8_t btRemoteClientService::getSensorBAUDTEST(void) {
    uint8_t pData[2];
    uint16_t data;
    pData[0] = pCharSensorVal->getValue().data()[0];
    pData[1] = pCharSensorVal->getValue().data()[1];
    pData[1] = pData[1] & 0x7F;                         //Remove the new data bit
    pCharSensorVal->setValue(pData, 2);                 //Update the characterisitc
    return (uint16_t)(pData[1] << 8 & pData[0]);
}

bool btRemoteClientService::newDataAvailableBAUDTEST(void) {
    //Check if new data bits are set
    uint8_t pData[2];
    pData[0] = pCharSensorVal->getValue().data()[0];
    pData[1] = pCharSensorVal->getValue().data()[1];
    if(( pData[1] & 0x80 ) == 0x80 ) { return true; } 
    else { return false; }
}


uint8_t btRemoteClientService::getBattery(void) {
    return *pCharBatteryLvl->getValue().data();
}

bool btRemoteClientService::newDataAvailable(void) {
    //Check if new data bits are set
    if(( *pCharSensorVal->getValue().data() & 0x80 ) == 0x80) { return true; } 
    else { return false; }
}

bool btRemoteClientService::isAssigned(void) {
    return assigned;
}

void btRemoteClientService::setAssigned(bool newValue) {
    assigned = newValue;
}

void btRemoteClientService::notificationCheck(void) {
    pCharSensorVal->notify();
}