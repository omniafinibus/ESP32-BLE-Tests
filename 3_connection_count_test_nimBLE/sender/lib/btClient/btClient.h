/*
 * File:    btClient.h
 * Author:  Arjan Lemmens
 * Created on August 1, 2022, 8:46 PM
 * Version: 1.0
 * 
 * ToDo:    - Add EEPROM functionality
 *          - Add comments
 *          - Clean up code
 */

#ifndef _BTCLIENT_H
#define _BTCLIENT_H

#include <NimBLEDevice.h>
#include <globalDef.h>

void BLE_Initialize(void);
void BLE_StartScanning(bool clearPreviousScan);
void BLE_Connect(void);
bool connectToServer(void);
void BLE_UpdateSensor(uint8_t sensorVal);
void BLE_UpdateBattery(uint8_t batteryLvl);
bool BLE_IsConnected(void);
bool BLE_OneDeviceFound(void);

#endif //_BTCLIENT_H