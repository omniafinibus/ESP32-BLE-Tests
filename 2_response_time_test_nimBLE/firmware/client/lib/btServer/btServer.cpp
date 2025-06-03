#include <btServer.h>

static BLEServer* pServer = NULL;

static BLEService * pServiceLocalGroup = NULL;
static BLECharacteristic * pCharDeviceName = NULL;
static BLECharacteristic * pCharAssignedDev = NULL;
static BLECharacteristic * pCharTotalConnDev = NULL;

static btRemoteClientService client[MAX_NUM_OF_MATS];

uint8_t groupID = 0;
uint8_t totalConnDev = 0;
uint8_t assignedDevices = 0;
std::string deviceName(REC_BLE_NAME);

const std::string manufacturerUUID(COMPANY_IDENTIFIER_UUID);

const std::string groupUUID[NUMBER_OF_GROUPS] = { G0_SERVICE_UUID,
                                                  G1_SERVICE_UUID,
                                                  G2_SERVICE_UUID,
                                                  G3_SERVICE_UUID};

const std::string clientUUID[MAX_NUM_OF_MATS] = { CLIENT_0_SERVICE_UUID,
                                                  CLIENT_1_SERVICE_UUID,
                                                  CLIENT_2_SERVICE_UUID,
                                                  CLIENT_3_SERVICE_UUID,
                                                  CLIENT_4_SERVICE_UUID,
                                                  CLIENT_5_SERVICE_UUID,
                                                  CLIENT_6_SERVICE_UUID,
                                                  CLIENT_7_SERVICE_UUID};

class serverCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    pServer->stopAdvertising();
    totalConnDev++;
    Serial2.println("New client connected");
    Serial2.println("Connected clients are:");
    for(uint8_t i = 0; i <= MAX_NUM_OF_MATS; i++) {
      if(client[i].isAssigned()) {
        Serial2.print(" - Device ");
        Serial2.println(client[i].getRemoteID());
      }
    }

    pCharTotalConnDev->setValue(&totalConnDev, 1);

    if(totalConnDev < MAX_NUM_OF_MATS) {
      startAdvertising();
    }
  }

  void onDisconnect(BLEServer* pServer) {
    Serial2.println("Device disconnected");
    digitalWrite(5, LOW);
    totalConnDev--;
    pCharTotalConnDev->setValue(&totalConnDev, 1);
  }
};

void  BLE_Initialize(void) {
  Serial2.println("Initializing Server");
  
  //Create name
  deviceName.append("G", 1);    //Add G to the end of the string
  char buffer[4];            
  itoa(groupID, buffer, 10);    //Convert groupID to * char (Max is 9)
  deviceName.append(buffer, 1); //Attach the number at the end of the string

  //Initilize BLE device
  BLEDevice::init(deviceName);
  Serial2.print(" - BLE device started with name: ");
  Serial2.println(deviceName.c_str());

  //Create server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new serverCallbacks());

  //Setup services and add characteristics
  pServiceLocalGroup = pServer->createService(groupUUID[groupID]);
  pCharDeviceName = pServiceLocalGroup->createCharacteristic(DEV_NAME_CHAR_UUID,
                                                             NIMBLE_PROPERTY::BROADCAST |
                                                             NIMBLE_PROPERTY::READ);
  pCharAssignedDev = pServiceLocalGroup->createCharacteristic(CONN_DEVS_CHAR_UUID,
                                                              NIMBLE_PROPERTY::BROADCAST |
                                                              NIMBLE_PROPERTY::READ);
  pCharTotalConnDev = pServiceLocalGroup->createCharacteristic(FREE_DEVS_CHAR_UUID,
                                                              NIMBLE_PROPERTY::BROADCAST |
                                                              NIMBLE_PROPERTY::READ |
                                                              NIMBLE_PROPERTY::WRITE);
  
  pCharAssignedDev->setValue(&assignedDevices, 1);
  pCharTotalConnDev->setValue(&totalConnDev, 1);
  pCharDeviceName->setValue(deviceName);

  pServiceLocalGroup->start();
  Serial2.print(" - Group service ");
  Serial2.print(groupID);
  Serial2.println(" created");

  for(uint8_t i = 0; i <= 7; i++) {
    client[i].assignService(pServer->createService(clientUUID[i]));
    Serial2.print(" - Client service ");
    Serial2.print(i);
    Serial2.println(" created");
  }

  Serial2.println("Initialization complete");
}

void startAdvertising(void) {
  Serial2.println("Setting up advertisement");

  BLEAdvertising * pAdvertising = BLEDevice::getAdvertising();
/*  BLEAdvertisementData advData;

  advData.setName(deviceName);
  Serial2.print(" - Name set as ");
  Serial2.println(deviceName.c_str());

  advData.setAppearance(0x03C4);
  Serial2.println(" - Appearance set to gamepad");

  advData.setManufacturerData(manufacturerUUID);
  Serial2.print(" - Manufacturer UUID set as: ");
  Serial2.println(manufacturerUUID.c_str());

  advData.setCompleteServices(groupUUID[groupID]);
  //advData.setServiceData(groupUUID[groupID], pCharTotalConnDev->getValue());
  Serial2.print(" - Service UUID: ");
  Serial2.println(groupUUID[groupID].c_str());
  Serial2.print(" - Number of connected devices data as ");
  Serial2.println(pCharTotalConnDev->getValue().c_str());
  
  pAdvertising->setAdvertisementData(advData);
*/
  pAdvertising->addServiceUUID(groupUUID[groupID]);
  pAdvertising->setScanResponse(true);
  Serial2.println(" - Scan responces allowed");

  pAdvertising->setMinInterval(0x640);
  Serial2.println(" - Minimum interval set to 1 second");

  pAdvertising->setMaxInterval(0xC80);
  Serial2.println(" - Maximum interval set to 2 seconds");

  pServer->advertiseOnDisconnect(true);
  Serial2.println(" - Advertise on disconnect enabled");

  pServer->startAdvertising();
  Serial2.println("Advertising started");
}

void stopAdvertising(void) {
  BLEDevice::stopAdvertising();
  Serial2.println("Advertising stopped");
}

bool getSensorValues(uint8_t * sensorArray) {
  bool newData = false;
  for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
    if(client[i].newDataAvailable()) {
      client[i].notificationCheck();
      sensorArray[i] = client[i].getSensor();
      Serial2.print("Mat ");
      Serial2.print(client[i].getRemoteID());
      Serial2.print(" has value ");
      Serial2.println(sensorArray[i]);
      newData = true;
    }
  }
  return newData;
}