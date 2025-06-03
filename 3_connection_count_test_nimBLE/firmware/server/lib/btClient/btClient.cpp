/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <btClient.h>

/*-------------------------------------*/
/*               UUIDs                 */
/*-------------------------------------*/
BLEUUID GroupServiceUUID[] = {  NimBLEUUID(groupUUID[0]),
                                NimBLEUUID(groupUUID[1]),
                                NimBLEUUID(groupUUID[2]),
                                NimBLEUUID(groupUUID[3]) };
BLEUUID AssignedDevCharUUID(FREE_DEVS_CHAR_UUID);

BLEUUID ClientServiceUUID[] = { NimBLEUUID(clientUUID[0]),
                                NimBLEUUID(clientUUID[1]),
                                NimBLEUUID(clientUUID[2]),
                                NimBLEUUID(clientUUID[3]),
                                NimBLEUUID(clientUUID[4]),
                                NimBLEUUID(clientUUID[5]),
                                NimBLEUUID(clientUUID[6]),
                                NimBLEUUID(clientUUID[7]) };
BLEUUID MatIDCharUUID(REMOTE_ID_CHAR_UUID);
BLEUUID SnsrCharUUID(SENSOR_CHAR_UUID);
BLEUUID BatLvlCharUUID(BAT_LVL_CHAR_UUID);

/*-------------------------------------*/
/*         BLE related classes         */
/*-------------------------------------*/
static BLEScan * pBLEScan;
BLERemoteService* pRemoteGroupService;
BLERemoteCharacteristic* pRemoteAssignedDev;
BLERemoteService* pRemoteMatService;
BLERemoteCharacteristic* pRemoteID;
BLERemoteCharacteristic* pRemoteSnsrs;
BLERemoteCharacteristic* pRemoteBattery;
BLEAdvertisedDevice* advDevice;

/*-------------------------------------*/
/*             Variables               */
/*-------------------------------------*/
std::string deviceName(MAT_BLE_NAME);
uint8_t connectedDevices[NUM_OF_GROUPS];

bool doConnect = false;
bool connected = false;
uint8_t matChannelUsed;
uint8_t groupToConnect = NUM_OF_GROUPS;
uint8_t matID = 1;
uint8_t amountConnected = 10;

static void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {\
  //Serial2.println("Notification received, changing matID");
  pRemoteID->writeValue(&matID, 1, false);
  //Serial2.println("matID updated");
};

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice *advertisedDevice) {
    bool correctName = true;  //Innocent until proven guilty
    //Serial2.print(" - BLE Advertised Device found: ");
    //Serial2.println(advertisedDevice->toString().c_str());
    
    //Check if the found name matches the name known in globalDef.h
    for(uint8_t crntChar = 0; crntChar < (sizeof(REC_BLE_NAME)/sizeof(REC_BLE_NAME[0])-1); crntChar++) {
      if( advertisedDevice->getName().c_str()[crntChar] != REC_BLE_NAME[crntChar] ) {
        correctName = false; //GUILTY!
        //There is enough evidence that this is an imposter, no need to look further
        crntChar = sizeof(REC_BLE_NAME)/sizeof(REC_BLE_NAME[0]);  
      }
    }

    if(correctName) {
      //Serial2.println("  - Device has correct name");

      /* Set the UUID pointer to the location of the service UUID
       * The UUID in the advertisment packet is read from front to back
       * Thus the stop index is lower than the start index */
      uint8_t * UUID = &advertisedDevice->getPayload()[UUID_STOP_INDEX];
      uint8_t * advAmoutData = &advertisedDevice->getPayload()[AMOUNT_DATA_INDEX];
      uint8_t * groupID = &advertisedDevice->getPayload()[GROUP_DATA_INDEX];

      for(uint8_t i = 0; i < advertisedDevice->getPayloadLength(); i++){
        //Serial2.print("  - b");
        //Serial2.print(i);
        //Serial2.print(": \t0x");
        char buffer[3];
        if (advertisedDevice->getPayload()[i] == 0x00){
          sprintf(buffer, "00");
        }
        else if(advertisedDevice->getPayload()[i] <= 0x0F) {
          sprintf(buffer, "0%X", advertisedDevice->getPayload()[i]);
        }
        else {
          sprintf(buffer, "%X", advertisedDevice->getPayload()[i]);  
        }
        //Serial2.println(buffer);
      }

      //Serial2.print("  - Group ");
      //Serial2.print(*groupID);
      //Serial2.println(" found");
      
      //Serial2.print("  - it has ");
      //Serial2.print(*advAmoutData);
      //Serial2.println(" devices connected");

      //Serial2.print("  - Service UUID in advertisment: ");
      for(int i = (UUID_START_INDEX - UUID_STOP_INDEX); i >= 0 ; i--) {
        char buffer[3];
        if (UUID[i] == 0x00){
          sprintf(buffer, "00");
        }
        else if(UUID[i] <= 0x0F) {
          sprintf(buffer, "0%X", UUID[i]);
        }
        else {
          sprintf(buffer, "%X", UUID[i]);  
        }
        //Serial2.print(buffer);
      }
      //Serial2.println();
      
      //Check if the found group has less connected devices than the previous group
      if(*advAmoutData < amountConnected) {
        amountConnected = *advAmoutData;
        groupToConnect = *groupID;
        //Serial2.print("  - Group ");
        //Serial2.print(groupToConnect);
        //Serial2.println(" set as designated group");
        advDevice = advertisedDevice;
        doConnect = true;
      }
      else{
        //Serial2.println("Group not suitable for connection");        
      }
    }
  }
};

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    amountConnected = 10;
    //Serial2.println("Disconnected");
  }
};

void BLE_Initialize(void) {
  //Write UUIDs to serial for debugging purposses
  for(uint8_t i = 0; i < NUM_OF_GROUPS; i++) {
    connectedDevices[i] = MAX_NUM_OF_MATS;
    //Serial2.print(" - Group ");
    //Serial2.print(i);
    //Serial2.print(" service UUID: ");
    //Serial2.println(GroupServiceUUID[i].toString().c_str());
  }

  //Serial2.print("  - Assigned device characterisitic UUID: ");
  //Serial2.println(AssignedDevCharUUID.toString().c_str());

  for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
    //Serial2.print(" - Client ");
    //Serial2.print(i);
    //Serial2.print(" service UUID: ");
    //Serial2.println(ClientServiceUUID[i].toString().c_str());
  }

  //Serial2.print("  - Mat ID characterisitic UUID: ");
  //Serial2.println(MatIDCharUUID.toString().c_str());

  //Serial2.print("  - Sensor value characterisitic UUID: ");
  //Serial2.println(SnsrCharUUID.toString().c_str());

  //Serial2.print("  - Battery level characterisitic UUID: ");
  //Serial2.println(BatLvlCharUUID.toString().c_str());

  //Serial2.println("Starting BLE Client application...");
  char buffer[4];
  sprintf(buffer, "%u.3", matID); //Convert matID to * char
  deviceName.append(buffer, 3); //Attach the number at the end of the string

  BLEDevice::init(deviceName);
  
}

void BLE_StartScanning(bool clearPreviousScan) {
  if(clearPreviousScan) { 
    //Serial2.println("Preparing BLE scan..."); 
    doConnect = false; 
  }
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  if(clearPreviousScan) { 
    //Serial2.println("Starting BLE scan..."); 
  }
  pBLEScan->start(5, !clearPreviousScan);
}

void BLE_Connect(void) {
  BLEDevice::getScan()->stop();
  if(groupToConnect < NUM_OF_GROUPS && groupToConnect >= 0 ) {
    //Serial2.print("Connecting to group ");
    //Serial2.println(groupToConnect);

    if(doConnect == true) {
      if (connectToServer()) { 
        //Serial2.println("Client is now connected to the BLE Server.");
        doConnect = false;
      } 
      else {
        //Serial2.println("Client has failed to connect to the server; restarting scan.");
        BLE_StartScanning(false);
      }
    }
  }
  else { 
    //Serial2.println("Group to connect to has a incorrect value; restarting scan.");
    BLE_StartScanning(false); 
  }
}

bool connectToServer(void) {
    //Serial2.print(" - Forming a connection to ");
    //Serial2.println(advDevice->getAddress().toString().c_str());
    
    //Create a client in the BLE stack
    BLEClient* pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());
    //Serial2.println(" - Created client");

    // Connect to the remote BLE Server.
    pClient->connect(advDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    //Serial2.println(" - Connected to server");
    
    //Request and save the group service
    pRemoteGroupService = pClient->getService(GroupServiceUUID[groupToConnect]);
    if (pRemoteGroupService == nullptr) {
      //Serial2.print("Failed to find our service UUID: ");
      //Serial2.println(GroupServiceUUID[groupToConnect].toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial2.print(" - Found group service UUID: ");
    //Serial2.println(GroupServiceUUID[groupToConnect].toString().c_str());

    //Request and save assigned channel characteristic
    pRemoteAssignedDev = pRemoteGroupService->getCharacteristic(AssignedDevCharUUID);
    if (pRemoteAssignedDev == nullptr) {
      //Serial2.print("Failed to find our characteristic UUID: ");
      //Serial2.println(AssignedDevCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial2.print("  - Found assigned channel characteristic UUID: ");
    //Serial2.println(AssignedDevCharUUID.toString().c_str());

    //Check which channels are available and assign the client to the first open one
    if (pRemoteAssignedDev->canRead()) {
      uint8_t freeMats = *pRemoteAssignedDev->readValue().data();
      //Serial2.print("   - Assigned channels are: ");
      //Serial2.println(freeMats);
      //When there is no space available, disconnect from the server
      if(freeMats == 0xFF) {
        //Serial2.println("No space available");
        pClient->disconnect();
        return false;
      }

      //Check each channel until a free channel is found
      for(uint8_t i = 0x01; i <= 0x80; i *= 2) {
        if(( ~freeMats & i ) != 0 ) {
          //Serial2.print(" - Connecting to mat channel ");
          //Serial2.println((uint8_t)log2(i));

          //Update assigned channel data
          if(pRemoteAssignedDev->canWrite()) {
            freeMats = freeMats | i;
            pRemoteAssignedDev->writeValue(&freeMats, 1, true);
            //Serial2.print("  - Remote connected devices characteristic update to ");
            //Serial2.println(freeMats);
          }
          else{
            //If unwriteable, disconnect
            //Serial2.println("Cannot write to assigned device characterisitc");
            pClient->disconnect();
            return false;
          }

          //Save correct channels and exit the for loop
          pRemoteMatService = pClient->getService(ClientServiceUUID[(uint8_t)log2(i)]);
          //Serial2.print("  - Client service UUID set to ");
          //Serial2.println(pRemoteMatService->getUUID().toString().c_str());
          i = 0xFF;
        }
      }
    }
    else {
      //Serial2.println("Cannot retreive and or update free mats byte");
      pClient->disconnect();
      return false;
    }
    
    //Request and save remote mat ID characteristic
    pRemoteID = pRemoteMatService->getCharacteristic(MatIDCharUUID);
    if (pRemoteID == nullptr) {
      //Serial2.print("Failed to find our characteristic UUID: ");
      //Serial2.println(MatIDCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial2.println(" - Found mat ID characteristic");

    //Update the remote mat ID
    if (pRemoteID->canWrite()) {
      pRemoteID->writeValue(&matID, 1, false);
    }

    //Connect to interrupt
    if (pRemoteID->canNotify()) {
      pRemoteID->subscribe(true, notifyCallback, false);
    }
    
    //Request and save remote sensor characteristic
    pRemoteSnsrs = pRemoteMatService->getCharacteristic(SnsrCharUUID);
    if (pRemoteSnsrs == nullptr) {
      //Serial2.print("Failed to find our characteristic UUID: ");
      //Serial2.println(SnsrCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial2.println(" - Found sensor value characteristic");

    //Request and save remote battery characteristic
    pRemoteBattery = pRemoteMatService->getCharacteristic(BatLvlCharUUID);
    if (pRemoteBattery == nullptr) {
      //Serial2.print("Failed to find our characteristic UUID: ");
      //Serial2.println(BatLvlCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //Serial2.println(" - Found battery level characteristic");

    pClient->updateConnParams(1, 0xFFFF, 0, 50);

    connected = true;
    return true;
}

void BLE_UpdateSensor(uint8_t sensorVal) {
  //Set the MSB to 1 together with the sensor value to notify the server that a new value is available
  sensorVal = sensorVal | NEW_DATA_BIT;
  pRemoteSnsrs->writeValue(&sensorVal, 1, true);
}

void BLE_UpdateBattery(uint8_t batteryLvl) {
  pRemoteBattery->writeValue(&batteryLvl, 1, true);
}

bool BLE_IsConnected(void){
  return connected;
}

bool BLE_OneDeviceFound(void) {
  return doConnect;
}