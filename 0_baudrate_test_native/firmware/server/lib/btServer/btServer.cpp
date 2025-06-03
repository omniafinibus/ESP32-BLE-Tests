#include "btServer.h"

BLEServer* pServer = NULL;

std::string matServiceUUID;
static BLEService *pMatService = NULL;
static BLECharacteristic* pMatId = NULL;
static BLECharacteristic* pMatSnsrs = NULL;
static BLEService *pBatService = NULL;
static BLECharacteristic* pBattery = NULL;
static BLEAdvertising *pAdvertising = NULL;

bool deviceConnected = false;
uint8_t matID = 1;
uint8_t sensorVal[2];

const std::string groupUUID[NUMBER_OF_GROUPS] = { MAT_G0_SERVICE_UUID,
                                                  MAT_G1_SERVICE_UUID,
                                                  MAT_G2_SERVICE_UUID,
                                                  MAT_G3_SERVICE_UUID };

class serverCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    //Serial2.println("Device connected");
    deviceConnected = true;
    BLEDevice::stopAdvertising();
  };

  void onDisconnect(BLEServer* pServer) {
    //Serial2.println("Device disconnected");
    deviceConnected = false;
    BLEDevice::startAdvertising();
  }
};

void  BLE_Initialize() {
  char deviceName[15];
  if (matID < 10) { sprintf(deviceName, "Controller0%u", matID); }
  else { sprintf(deviceName, "Controller%u", matID); }
  matServiceUUID = groupUUID[1];
/*
  for(uint8_t i = 0; i < NUMBER_OF_GROUPS; i++) {
    if(( ( matID - i ) % NUMBER_OF_GROUPS ) == 0) {
      matServiceUUID = groupUUID[i];
    }
  }*/
  //Serial2.println("BLE variables set");

  BLEDevice::init(deviceName);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new serverCallbacks());
  //Serial2.println("BLE server started");

  pMatService = pServer->createService(matServiceUUID);
  pMatId = pMatService->createCharacteristic(SYS_ID_CHAR_UUID,
                                             BLECharacteristic::PROPERTY_BROADCAST |
                                             BLECharacteristic::PROPERTY_READ);
  pMatId->addDescriptor(new BLE2902());
  pMatSnsrs = pMatService->createCharacteristic(SENSOR_CHAR_UUID,
                                                BLECharacteristic::PROPERTY_NOTIFY | 
                                                BLECharacteristic::PROPERTY_READ);
  pMatSnsrs->addDescriptor(new BLE2902());
  //Serial2.println("BLE mat service created");
  pMatId->setValue(&matID, 1);
  sensorVal[0] = matID;
  sensorVal[1] = 0x00;
  pMatSnsrs->setValue(sensorVal, 2);
  pMatService->start();
  //Serial2.println("BLE mat service started");

  pBatService = pServer->createService(BAT_SERVICE_UUID);
  pBattery = pBatService->createCharacteristic(BAT_LVL_CHAR_UUID,
                                               BLECharacteristic::PROPERTY_NOTIFY | 
                                               BLECharacteristic::PROPERTY_READ);
  pBattery->addDescriptor(new BLE2902());
  //Serial2.println("BLE battery service created");
  pBatService->start();
  //Serial2.println("BLE battery service started");

  startAdvertising();
}

void genRandUUID(uint8_t * bufferUUID) {

}

void startAdvertising(void) {
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(matServiceUUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->setAppearance(0x03C4);
  BLEDevice::startAdvertising();
  //Serial2.println("BLE start advertising");
}

void setAdvertisingMode(esp_ble_adv_type_t newMode) {
  BLEDevice::stopAdvertising();
  pAdvertising->setAdvertisementType(newMode);
  BLEDevice::startAdvertising();
}

void updateSensorInputs(uint16_t sensorValue) {
  sensorVal[1] = (uint8_t)(( sensorValue & 0xFF00 ) >> 8 );
  sensorVal[2] = (uint8_t)( sensorValue & 0x00FF );
  pMatSnsrs->setValue(sensorVal, 3);
  pMatSnsrs->notify();
}

void updateBatteryLevel(uint16_t batteryLevel) {
  pBattery->setValue(batteryLevel);
  pBattery->notify();
}