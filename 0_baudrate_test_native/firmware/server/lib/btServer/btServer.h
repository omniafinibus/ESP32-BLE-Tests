#ifndef _BTSERVER_H
#define _BTSERVER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> 
#include <globalDef.h>

void BLE_Initialize();
void genRandUUID(uint8_t * bufferUUID);
void startAdvertising(void);
void setAdvertisingMode(esp_ble_adv_type_t newMode);
void updateSensorInputs(uint16_t sensorValue);
void updateBatteryLevel(uint16_t batteryLevel);

#endif //_BTSERVER_H