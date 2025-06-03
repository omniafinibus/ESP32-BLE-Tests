#include <btServer.h>

#define TMR0_ID     0
#define TMR0_PRE    80
#define TMR0_PERIOD 5000000  //10 seconds

static BLEServer* pServer = NULL;

static BLEService * pServiceLocalGroup = NULL;
static BLECharacteristic * pCharLocalID = NULL;
static BLECharacteristic * pCharAssignedDev = NULL;
static BLECharacteristic * pCharTotalConnDev = NULL;

static btRemoteClientService client[MAX_NUM_OF_MATS];

hw_timer_t * timer = NULL;

uint8_t groupID = 0;
uint8_t totalConnDev = 0;
uint8_t assignedDevices = 0;
bool clientLost = false;
std::string deviceName(REC_BLE_NAME);

volatile SemaphoreHandle_t timerSemaphore;
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

class serverCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    //Stop advertising to focus on the connection with the current device
    stopAdvertising();
    Serial1.println("New client connected");

    //Update the amount of device connected
    totalConnDev++;
    pCharTotalConnDev->setValue(&totalConnDev, 1);
    Serial1.print("Total amount of connections: ");
    Serial1.println(pCharTotalConnDev->getValue().c_str());
    

    /* The client will have control of the characteristics
     * This server only reads them when necessary 
     * This is also the case for the pCharAssignedDev characteristic */
    Serial1.println("Connected clients are:");
    for(uint8_t i = 0; i <= MAX_NUM_OF_MATS; i++) {
      if(client[i].isAssigned()) {
        Serial1.print(" - Device ");
        Serial1.println(client[i].getRemoteID());
      }
    }

    //Check the value of the assigned devices characteristic for debugging purposes
    Serial1.print("Connected channels: ");
    Serial1.println(pCharAssignedDev->getValue().c_str());

    //pServer->getConnectedCount()
    //pServer->getPeerInfo()

    //If there are still non assigned clients, continue advertising
    if(totalConnDev < MAX_NUM_OF_MATS) {
      startAdvertising();
    }
  }

  void onDisconnect(BLEServer* pServer) {
    timerAlarmDisable(timer);
    timerWrite(timer, 0);
    /* Check which clients are still connected to update the assigned devices characteristic
     * Since the disconnected client does not say when it disconnects and does not update the characteristic 
     * because a clean disconnect is usually not the case, 
     * it is the servers responsability to clean the clients mess up */
    Serial1.println("Device disconnected");
    clientLost = true;
    totalConnDev--;
    pCharTotalConnDev->setValue(&totalConnDev, 1);
    Serial1.println(" - Updated total number of connected devices");
    //Toggle advertisement to update the advertized data
    stopAdvertising();
    startAdvertising();
  }
};

void BLE_Initialize(void) {
  Serial1.println("Initializing Server");

  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(TMR0_ID, TMR0_PRE, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, TMR0_PERIOD, true);
  
  //Create name
  deviceName.append("G", 1);    //Add G to the end of the string
  char buffer[4];            
  itoa(groupID, buffer, 10);    //Convert groupID to * char (Max is 9)
  deviceName.append(buffer, 1); //Attach the number at the end of the string

  //Initilize BLE device
  BLEDevice::init(deviceName);
  Serial1.print(" - BLE device started with name: ");
  Serial1.println(deviceName.c_str());

  //Create server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new serverCallbacks());

  //Setup services and add characteristics
  pServiceLocalGroup = pServer->createService(groupUUID[groupID]);
  pCharLocalID = pServiceLocalGroup->createCharacteristic(LOCAL_GROUP_UUID,
                                                          NIMBLE_PROPERTY::BROADCAST |
                                                          NIMBLE_PROPERTY::READ);
  pCharTotalConnDev = pServiceLocalGroup->createCharacteristic(CONN_DEVS_CHAR_UUID,
                                                              NIMBLE_PROPERTY::BROADCAST |
                                                              NIMBLE_PROPERTY::READ);
  pCharAssignedDev = pServiceLocalGroup->createCharacteristic(FREE_DEVS_CHAR_UUID,
                                                              NIMBLE_PROPERTY::WRITE |
                                                              NIMBLE_PROPERTY::BROADCAST |
                                                              NIMBLE_PROPERTY::READ);
  
  //Assign predefined or retreived values
  pCharLocalID->setValue(&groupID, 1);
  pCharAssignedDev->setValue(&assignedDevices, 1);
  pCharTotalConnDev->setValue(&totalConnDev, 1);

  //Start the local group services
  pServiceLocalGroup->start();
  Serial1.print(" - Group service ");
  Serial1.print(pServiceLocalGroup->getUUID().toString().c_str());
  Serial1.println(" created");

  //Create the client services
  for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
    client[i].assignService(pServer->createService(clientUUID[i]));
    Serial1.print(" - Client service ");
    Serial1.print(clientUUID[i].c_str());
    Serial1.println(" created");
  }

  Serial1.println("Initialization complete");
}

void startAdvertising(void) {
  Serial1.println("Setting up advertisement");
  BLEAdvertising * pAdvertising = BLEDevice::getAdvertising();
  BLEAdvertisementData pAdvData;

  //Setup and custom attach advertisment data
  updateAdvertisementData(&pAdvData);
  pAdvertising->setAdvertisementData(pAdvData);

  //Setup advertisment settings
  pAdvertising->setScanResponse(true);
  Serial1.println(" - Scan responces allowed");

  pAdvertising->setMinInterval(0x640);
  Serial1.println(" - Minimum interval set to 1 second");

  pAdvertising->setMaxInterval(0xC80);
  Serial1.println(" - Maximum interval set to 2 seconds");

  pServer->advertiseOnDisconnect(true);
  Serial1.println(" - Advertise on disconnect enabled");

  pServer->startAdvertising();
  Serial1.println("Advertising started");
}

void stopAdvertising(void) {
  BLEDevice::stopAdvertising();
  Serial1.println("Advertising stopped");
}

bool getSensorValues(uint8_t * sensorArray, uint8_t * matIDs) {
  bool newData = false;
  for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
    matIDs[i] = client[i].getRemoteID();
    if(client[i].newDataAvailable()) {
      sensorArray[i] = client[i].getSensor();
      newData = true;
    }
  }
  return newData;
}

void updateAdvertisementData(BLEAdvertisementData * customAdvData) {
  std::string dataToAppend;
  Serial1.println(" - Setting up custom adertisement data");

  customAdvData->setName(deviceName);
  Serial1.print(" - Set device name: ");
  Serial1.println(deviceName.c_str());
  
  //Save required data to a string
  dataToAppend.append(pCharTotalConnDev->getValue().operator std::__cxx11::string());
  dataToAppend.append(pCharLocalID->getValue().operator std::__cxx11::string());
  Serial1.print(" - Data string: ");
  Serial1.println(dataToAppend.c_str());
  
  //Add service UUID, total connected devices value and local ID value
  customAdvData->setServiceData(pServiceLocalGroup->getUUID(), dataToAppend);
  Serial1.print(" - Service UUID ");
  Serial1.print(pServiceLocalGroup->getUUID().toString().c_str());
  Serial1.println(" appended");
  Serial1.print(" - With data values: ");
  Serial1.println(dataToAppend.c_str());

  //Add manufacturer data
  customAdvData->setManufacturerData(manufacturerUUID);
  Serial1.print(" - Manufactor data ");
  Serial1.print(manufacturerUUID.c_str());
  Serial1.println(" appended");

  //Check payload
  //Serial1.print(" - Full payload: ");
  //Serial1.println(customAdvData->getPayload().c_str());
}

void BLE_connectionCheck(void) {
  if(clientLost) {
    if(!timerAlarmEnabled(timer)) {
      assignedDevices = *pCharAssignedDev->getValue().data();
      for(uint16_t i = 0x01; i <= 0x80; i *= 2) {
        if(( assignedDevices & i ) != 0 ) {
          uint8_t clientIndex = (uint8_t)log2(i);
          Serial1.print("  - Checking channel ");
          Serial1.println(clientIndex);
          client[clientIndex].setRemoteID(client[clientIndex].getRemoteID() | CONN_CHECK_BIT);
        }
      }
      timerWrite(timer, 0);
      timerAlarmEnable(timer);
    }

    if(xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
      Serial1.println("  - Checking clients");

      for(uint16_t i = 0x01; i <= 0x80; i *= 2) {
        uint8_t clientIndex = (uint8_t)log2(i);
        if(( assignedDevices & i ) != 0 ) {
          if (client[clientIndex].getRemoteID() >= CONN_CHECK_BIT) {
            client[clientIndex].setAssigned(false);
            client[clientIndex].setRemoteID(0x00);
            Serial1.print("   - Client channel ");
            Serial1.print(clientIndex);
            Serial1.println(" has not repsonded, assuming it is unassigned");
            assignedDevices = assignedDevices & ~i;
          }
          else {
            Serial1.print("   - Client channel ");
            Serial1.print(clientIndex);
            Serial1.println(" has repsonded");
          }
        }
      }
      clientLost = false;
      pCharAssignedDev->setValue(&assignedDevices, 1);
    }
  }
}