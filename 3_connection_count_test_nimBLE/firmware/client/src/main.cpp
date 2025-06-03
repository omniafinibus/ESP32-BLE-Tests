/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <Arduino.h>
#include <btServer.h>

/*-------------------------------------*/
/*            Definitions              */
/*-------------------------------------*/

/*-------------------------------------*/
/*              Classes                */
/*-------------------------------------*/

/*-------------------------------------*/
/*             Variables               */
/*-------------------------------------*/

uint8_t sensorValues[MAX_NUM_OF_MATS];
uint8_t matID[MAX_NUM_OF_MATS];

void setup() {
  Serial1.begin(115800);

  BLE_Initialize();

  startAdvertising();
}

void loop() {
  if(getSensorValues(sensorValues, matID)) {
    Serial1.println("--------------------------------------------------------------------------");
    Serial1.print("Mat ID: ");
    for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
      Serial1.print("\t");
      Serial1.print(matID[i]);
    }
    Serial1.println();
     
    Serial1.print("Value: \t");
    for(uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
      Serial1.print("\t");
      Serial1.print(sensorValues[i]);
    } 
    Serial1.println();
  }

  BLE_connectionCheck();
}