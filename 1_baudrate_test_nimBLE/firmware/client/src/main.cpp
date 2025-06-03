/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <Arduino.h>
#include <btServer.h>

/*-------------------------------------*/
/*            Definitions              */
/*-------------------------------------*/
#define TMR0_ID     0
#define TMR0_PRE    80
#define TMR0_PERIOD 10000000  //10 sec
#define LED_PIN     5

/*-------------------------------------*/
/*              Classes                */
/*-------------------------------------*/
volatile SemaphoreHandle_t timerSemaphore;
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

/*-------------------------------------*/
/*             Variables               */
/*-------------------------------------*/
hw_timer_t * timer = NULL;
uint16_t sensorValues[MAX_NUM_OF_MATS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t previousSpeed[MAX_NUM_OF_MATS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t packetsReceived[MAX_NUM_OF_MATS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool baudSuccess = false;

void setup() {
  Serial2.begin(115800);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(TMR0_ID, TMR0_PRE, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, TMR0_PERIOD, true);
  timerAlarmEnable(timer);

  BLE_Initialize();

  startAdvertising();
}

void loop() {
  if(getSensorValuesBAUDTEST(sensorValues)) {
    for( uint8_t i = 0; i < MAX_NUM_OF_MATS; i++) {
      char buffer[64];
      if(sensorValues[i] == packetsReceived[i]) {
        sprintf(buffer, "mat: %u baud: %u was succesful with %u packets received", i, sensorValues[i], packetsReceived);
        Serial2.println(buffer);
        baudSuccess = true;
      }
      else if(sensorValues[i] != previousSpeed[i]) {
        if(!baudSuccess) {
          sprintf(buffer, "mat: %u baud: %u was NOT succesful with %u packets received", i, sensorValues[i], packetsReceived);
          Serial2.println(buffer);
        }
        previousSpeed[i] = sensorValues[i];
        packetsReceived[i] = 2; 
        baudSuccess = false;
      }
      else { packetsReceived[i]++; }
    }
  }
}