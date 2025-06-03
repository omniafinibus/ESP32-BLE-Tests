#ifndef _BTSERVER_H
#define _BTSERVER_H

#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <globalDef.h>
#include <btRemoteClientService.h>

void BLE_Initialize(void);
void startAdvertising(void);
void stopAdvertising(void);
bool getSensorValues(uint8_t * sensorArray);
bool getSensorValuesBAUDTEST(uint16_t * sensorArray);

#endif //_BTSERVER_H