/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <btClient.h>

/*-------------------------------------*/
/*               UUIDs                 */
/*-------------------------------------*/
BLEUUID GroupServiceUUID[] = { NimBLEUUID(G0_SERVICE_UUID), 
                               NimBLEUUID(G1_SERVICE_UUID), 
                               NimBLEUUID(G2_SERVICE_UUID), 
                               NimBLEUUID(G3_SERVICE_UUID) };
BLEUUID NameCharUUID(DEV_NAME_CHAR_UUID);
BLEUUID TotalConnDevCharUUID(CONN_DEVS_CHAR_UUID);
BLEUUID AssignedDevCharUUID(FREE_DEVS_CHAR_UUID);

BLEUUID ClientServiceUUID[] = { NimBLEUUID(CLIENT_0_SERVICE_UUID),
                                NimBLEUUID(CLIENT_1_SERVICE_UUID),
                                NimBLEUUID(CLIENT_2_SERVICE_UUID),
                                NimBLEUUID(CLIENT_3_SERVICE_UUID),
                                NimBLEUUID(CLIENT_4_SERVICE_UUID),
                                NimBLEUUID(CLIENT_5_SERVICE_UUID),
                                NimBLEUUID(CLIENT_6_SERVICE_UUID),
                                NimBLEUUID(CLIENT_7_SERVICE_UUID) };
BLEUUID MatIDCharUUID(REMOTE_ID_CHAR_UUID);
BLEUUID SnsrCharUUID(SENSOR_CHAR_UUID);
BLEUUID BatLvlCharUUID(BAT_LVL_CHAR_UUID);

/*-------------------------------------*/
/*         BLE related classes         */
/*-------------------------------------*/
static BLEScan * pBLEScan;
BLERemoteService* pRemoteGroupService;
BLERemoteCharacteristic* pRemoteName;
BLERemoteCharacteristic* pRemoteTotalConnDev;
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
uint8_t groupToConnect = NUM_OF_GROUPS + 1;
uint8_t matID = 1;

static void notifyCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {

};

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice *advertisedDevice) {
    Serial2.print(" - BLE Advertised Device found: ");
    Serial2.println(advertisedDevice->toString().c_str());

    if(advertisedDevice->haveServiceUUID()) { //Does the device have a service UUID?
      Serial2.println(" - Device contains service UUID in advertisement");
      
      //Does the UUID match with any known service UUID?
      for(uint8_t i = 0; i < NUM_OF_GROUPS; i++) {
        //Notify the class that its corresponding group has been found

        if(advertisedDevice->isAdvertisingService(GroupServiceUUID[i])) {
          advDevice = advertisedDevice;
          uint8_t * advData = advertisedDevice->getPayload();
          uint8_t advDataSize = advertisedDevice->getAdvLength();
          groupToConnect = i;
          doConnect = true;

          Serial2.print(" - Group ");
          Serial2.print(i);
          Serial2.println(" found");
          Serial2.println(" - Data in advertisment: ");
          
          for(uint8_t crntByte; crntByte < advDataSize; crntByte++) {
            Serial2.print(" - b1: ");
            Serial2.println(advData[i]);
          }
        }
      } 
    }
  }
};

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial2.println("Connected");
  }

  void onDisconnect(BLEClient* pclient) {
    //connected = false;
    Serial2.println("Disconnected");
  }
};

void BLE_Initialize(void) {
  for(uint8_t i = 0; i < NUM_OF_GROUPS; i++) {
    connectedDevices[i] = MAX_NUM_OF_MATS;
    Serial2.print(" - Group ");
    Serial2.print(i);
    Serial2.print(" service UUID: ");
    Serial2.println(GroupServiceUUID[i].toString().c_str());
  }

  Serial2.print("  - Device name characterisitic UUID: ");
  Serial2.println(NameCharUUID.toString().c_str());

  Serial2.print("  - Total connected devices characterisitic UUID: ");
  Serial2.println(TotalConnDevCharUUID.toString().c_str());

  Serial2.print("  - Assigned device characterisitic UUID: ");
  Serial2.println(AssignedDevCharUUID.toString().c_str());

  for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
    Serial2.print(" - Client ");
    Serial2.print(i);
    Serial2.print(" service UUID: ");
    Serial2.println(ClientServiceUUID[i].toString().c_str());
  }

  Serial2.print("  - Mat ID characterisitic UUID: ");
  Serial2.println(MatIDCharUUID.toString().c_str());

  Serial2.print("  - Sensor value characterisitic UUID: ");
  Serial2.println(SnsrCharUUID.toString().c_str());

  Serial2.print("  - Battery level characterisitic UUID: ");
  Serial2.println(BatLvlCharUUID.toString().c_str());

  Serial2.println("Starting BLE Client application...");
  char buffer[4];
  sprintf(buffer, "%u.3", matID);    //Convert matID to * char
  deviceName.append(buffer, 3); //Attach the number at the end of the string

  BLEDevice::init(deviceName);
}

void BLE_StartScanning(void) {
  Serial2.println("Preparing BLE scan...");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  Serial2.println("Starting BLE scan...");
  pBLEScan->start(5, false);
}

void BLE_Connect(void) {
  BLEDevice::getScan()->stop();
  if(groupToConnect != NUM_OF_GROUPS + 1) {
    Serial2.print(" - Connecting to group ");
    Serial2.println(groupToConnect);

    if(doConnect == true) {
      if (connectToServer()) { 
        Serial2.println("We are now connected to the BLE Server.");
      } 
      else { 
        Serial2.println("We have failed to connect to the server; there is nothin more we will do.");
        while(true){;}
      }
      doConnect = false;
    }
  }
}

bool connectToServer(void) {
    Serial2.print("Forming a connection to ");
    Serial2.println(advDevice->getAddress().toString().c_str());

    BLEClient* pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());
    Serial2.println(" - Created client");

    // Connect to the remote BLE Server.
    pClient->connect(advDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial2.println(" - Connected to server");
    
    //Request and save the group servers information
    pRemoteGroupService = pClient->getService(GroupServiceUUID[groupToConnect]);
    if (pRemoteGroupService == nullptr) {
      Serial2.print("Failed to find our service UUID: ");
      Serial2.println(GroupServiceUUID[groupToConnect].toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found group service");

    pRemoteName = pRemoteGroupService->getCharacteristic(NameCharUUID);
    if (pRemoteName == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(NameCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found name characteristic");

    pRemoteTotalConnDev = pRemoteGroupService->getCharacteristic(TotalConnDevCharUUID);
    if (pRemoteTotalConnDev == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(TotalConnDevCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found number of connected devices characteristic");

    if (pRemoteTotalConnDev->canRead()) {
      Serial2.print(" - Group has ");
      Serial2.print(pRemoteTotalConnDev->readValue<uint8_t>());
      Serial2.println(" active connections");
    }
    
    pRemoteAssignedDev = pRemoteGroupService->getCharacteristic(AssignedDevCharUUID);
    if (pRemoteAssignedDev == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(AssignedDevCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }

    if (pRemoteAssignedDev->canRead()) {
      uint8_t freeMats = pRemoteAssignedDev->readValue<uint8_t>();
      if(freeMats == 0xFF) {
        Serial2.println("No space available");
        pClient->disconnect();
        return false;
      }

      for(int i = 0; i < MAX_NUM_OF_MATS; i++) {
        if(( freeMats & ( 2^i )) > 0 ) {
          Serial2.print(" - Connecting to mat channel ");
          Serial2.println( i + 1 );

          freeMats = freeMats & !( 2^i );
          if(pRemoteAssignedDev->canWrite()) {
            pRemoteAssignedDev->writeValue(&freeMats, 1, true);
          }
          else{
            Serial2.println("Cannot write to assigned device characterisitc");
            pClient->disconnect();
            return false;
          }
          pRemoteMatService = pClient->getService(ClientServiceUUID[i]);
          i = MAX_NUM_OF_MATS;
        }
      }
    }
    else {
      Serial2.println("Cannot retreive and or update free mats byte");
      pClient->disconnect();
      return false;
    }
    
    pRemoteID = pRemoteMatService->getCharacteristic(MatIDCharUUID);
    if (pRemoteID == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(MatIDCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found mat ID characteristic");

    if (pRemoteID->canWrite()) {
      pRemoteID->writeValue(&matID, 1, false);
    }
    
    pRemoteSnsrs = pRemoteMatService->getCharacteristic(SnsrCharUUID);
    if (pRemoteSnsrs == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(SnsrCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found sensor value characteristic");

    /*if (pRemoteSnsrs->canNotify())
    {
      pRemoteSnsrs->setNotify(notifyCallback);
    }*/

    pRemoteBattery = pRemoteMatService->getCharacteristic(BatLvlCharUUID);
    if (pRemoteBattery == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(BatLvlCharUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found battery level characteristic");

    connected = true;
    return true;
}

void BLE_UpdateSensor(uint8_t sensorVal) {
  sensorVal = sensorVal | 0x80;
  pRemoteSnsrs->writeValue(&sensorVal, 1, true);
}

void BLE_UpdateBattery(uint8_t batteryLvl) {
  pRemoteBattery->writeValue(&batteryLvl, 1, true);
}

bool BLE_IsConnected(void){
  return connected;
}