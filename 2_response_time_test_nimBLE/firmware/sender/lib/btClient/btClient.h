#ifndef _BTCLIENT_H
#define _BTCLIENT_H

#include <NimBLEDevice.h>
#include <globalDef.h>

void BLE_Initialize(void);
void BLE_StartScanning(void);
void BLE_Connect(void);
bool connectToServer(void);
void BLE_UpdateSensor(uint8_t sensorVal);
void BLE_UpdateBattery(uint8_t batteryLvl);
bool BLE_IsConnected(void);

#endif //_BTCLIENT_H