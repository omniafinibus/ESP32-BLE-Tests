/*
 * File:    btServer.h
 * Author:  Arjan Lemmens
 * Created on August 1, 2022, 10:13 PM
 * Version: 01.0
 * 
 * ToDo:    - Add comments
 *          - Clean up code
 */


#ifndef _BTSERVER_H
#define _BTSERVER_H

#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <Arduino.h>
#include <globalDef.h>
#include <btRemoteClientService.h>

void BLE_Initialize(void);
void startAdvertising(void);
void stopAdvertising(void);
bool getSensorValues(uint8_t * sensorArray, uint8_t * matIDs);
void updateAdvertisementData(BLEAdvertisementData * customAdvData);
void BLE_connectionCheck(void);

#endif //_BTSERVER_H