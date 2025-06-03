#ifndef _BTCLIENT_H
#define _BTCLIENT_H

#include <BLEDevice.h>
#include <globalDef.h>

static void notifyCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
void BLE_ClientInitialize(void);
bool BLE_ConnectToServer();
void BLE_StartScan(void);
void BLE_StopScan(void);

#endif //_BTCLIENT_H