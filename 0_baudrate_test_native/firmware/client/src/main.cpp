/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
#include <Arduino.h>
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("7d6700ae-838b-4c61-8f73-fa48192fbdef");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("2a23");
static BLEUUID    snsrCharUUID("a611d5ff-7fa4-455d-aae8-a37e4fbd42ed");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteID;
static BLERemoteCharacteristic* pRemoteSnsrs;
static BLEAdvertisedDevice* myDevice;
uint16_t packetsReceived = 0;
uint16_t previousSpeed = 0;
bool baudSuccess = false;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    char hexVal[64];
    uint16_t receivedSpeed = ( pData[1] << 8 ) | pData[2];
    /*
    sprintf(hexVal, "m %u", pData[0]);
    Serial2.print(hexVal);
    sprintf(hexVal, " d: %u", pData[1]);
    Serial2.print(hexVal);*/
    if(receivedSpeed == packetsReceived) {
      sprintf(hexVal, "mat: %u baud: %u was succesful with %u packets received", pData[0], receivedSpeed, packetsReceived);
      Serial2.println(hexVal);
      baudSuccess = true;
    }
    else if(receivedSpeed != previousSpeed) {
      if(!baudSuccess) {
        sprintf(hexVal, "mat: %u baud: %u was NOT succesful with %u packets received", pData[0], receivedSpeed, packetsReceived);
        Serial2.println(hexVal);
      }
      previousSpeed = receivedSpeed;
      packetsReceived = 2; 
      baudSuccess = false;
    }
    else { packetsReceived++; }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial2.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial2.print("Forming a connection to ");
    Serial2.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial2.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
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
    pRemoteID = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteID == nullptr) {
      Serial2.print("Failed to find our characteristic UUID: ");
      Serial2.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial2.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteID->canRead()) {
      std::string value = pRemoteID->readValue();
      Serial2.print("The characteristic value was: ");
      Serial2.println(value.c_str());
    }

    pRemoteID = pRemoteService->getCharacteristic(snsrCharUUID);

    if(pRemoteID->canNotify())
      pRemoteID->registerForNotify(notifyCallback);

      pRemoteSnsrs = pRemoteService->getCharacteristic(snsrCharUUID);
    if (pRemoteSnsrs == nullptr) {
      Serial2.print("Failed to find our sensor UUID: ");
    }
    else {
        Serial2.println(" - Found sensor characteristic");

        if(pRemoteSnsrs->canRead()) {
        std::string value = pRemoteSnsrs->readValue();
        Serial2.print("The sensor value was: ");
        Serial2.println(value.c_str());
        }
        pRemoteSnsrs = pRemoteService->getCharacteristic(snsrCharUUID);
        if(pRemoteSnsrs->canNotify())
        pRemoteSnsrs->registerForNotify(notifyCallback);
    }

    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial2.print("BLE Advertised Device found: ");
    Serial2.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial2.begin(115200);
  Serial2.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial2.println("We are now connected to the BLE Server.");
    } else {
      Serial2.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    //String newValue = "Time since boot: " + String(millis()/1000);
    //Serial2.println("Times since start: \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    //pRemoteSnsrs->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); // Delay a second between loops.
} // End of loop