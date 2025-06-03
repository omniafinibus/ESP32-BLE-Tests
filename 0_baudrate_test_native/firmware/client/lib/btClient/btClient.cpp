#include <btClient.h>

static BLEUUID serviceUUID(MAT_G1_SERVICE_UUID); // The remote service we wish to connect to.
static BLEUUID charUUID(SYS_ID_CHAR_UUID); // The characteristic of the remote service we are interested in.

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice * matServer;
std::string advertiserServiceData;
uint8_t receiverGroup = 1;
std::string groupUUID[NUMBER_OF_GROUPS] = { MAT_G0_SERVICE_UUID,
                                            MAT_G1_SERVICE_UUID,
                                            MAT_G2_SERVICE_UUID,
                                            MAT_G3_SERVICE_UUID };

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial2.println("Connected");
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial2.println("Disconnected");
  }
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial2.print("BLE Advertised Device found: ");
    Serial2.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      advertiserServiceData = advertisedDevice.getServiceData();
      Serial2.print("Found device with appended data:");
      Serial2.println(advertiserServiceData.c_str());
      BLEDevice::getScan()->stop();
      matServer = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    Serial2.print("Notify callback for characteristic ");
    Serial2.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial2.print(" of data length ");
    Serial2.println(length);
    Serial2.print("data: ");
    Serial2.println((char*)pData);
}                                        

void BLE_ClientInitialize(void) {
  char deviceName[15];
  sprintf(deviceName, "Reciever0%u", receiverGroup);
  //serviceUUID.fromString(groupUUID[( receiverGroup - 1 )]);

  BLE_StartScan();
  Serial2.println("Scanning started");

  BLE_ConnectToServer();
  Serial2.println("Connected to server");
}

bool BLE_ConnectToServer() {    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial2.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remote BLE Server.
    //pClient->connect(matServer);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial2.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial2.print("Failed to find our service UUID: ");
      Serial2.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial2.print("The characteristic value was: ");
      Serial2.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify()) {
      pRemoteCharacteristic->registerForNotify(notifyCallback);
    }

    connected = true;
    return true;
}

void BLE_StartScan(void) {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void BLE_StopScan(void) {
  BLEDevice::getScan()->stop();
}