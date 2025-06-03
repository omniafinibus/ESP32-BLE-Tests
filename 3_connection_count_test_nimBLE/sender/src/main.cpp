/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <Arduino.h>
#include <btClient.h>

/*-------------------------------------*/
/*            Definitions              */
/*-------------------------------------*/
#define TMR0_ID           0
#define TMR0_PRE          80
#define TMR0_PERIOD_INIT  300000000  //5 Minutes

#define TMR1_ID           1
#define TMR1_PRE          80
#define TMR1_PERIOD_CONN  2000000    //20 Seconds
#define TMR1_PERIOD_LOOP  250000     //250 milliseconds

/*-------------------------------------*/
/*             Variables               */
/*-------------------------------------*/
uint8_t sensorValue, batteryLvl;

/*-------------------------------------*/
/*              Classes                */
/*-------------------------------------*/
volatile SemaphoreHandle_t sleepTimerSemaphore;
void ARDUINO_ISR_ATTR sleepTimerInt() { xSemaphoreGiveFromISR(sleepTimerSemaphore, NULL); }

volatile SemaphoreHandle_t timer1Semaphore;
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timer1Semaphore, NULL); }

hw_timer_t * sleepTimer = NULL;
hw_timer_t * timer1 = NULL;

void setup() {
  //Serial2.begin(115200);

  sensorValue = 0x01;
  batteryLvl = 5;
  
  sleepTimerSemaphore = xSemaphoreCreateBinary();
  sleepTimer = timerBegin(TMR0_ID, TMR0_PRE, true);
  timerAttachInterrupt(sleepTimer, &sleepTimerInt, true);
  timerAlarmWrite(sleepTimer, TMR0_PERIOD_INIT, true);
  timerAlarmEnable(sleepTimer);

  timer1Semaphore = xSemaphoreCreateBinary();
  timer1 = timerBegin(TMR1_ID, TMR1_PRE, true);
  timerAttachInterrupt(timer1, &onTimer, true);
  timerAlarmWrite(timer1, TMR1_PERIOD_CONN, true);

  BLE_Initialize();
  BLE_StartScanning(true);

  do {
    //Wait until at least 1 device is found
    if(xSemaphoreTake(sleepTimerSemaphore, 0) == pdTRUE) {   
      //Serial2.println("Sleep timer triggered"); 
    }
  
    //If at least 1 device is found wait for 10 seconds before connecting
    if(BLE_OneDeviceFound()) {
      if(!timerAlarmEnabled(timer1)) {
        //Serial2.println("  - Device found, starting timer before connecting");
        timerAlarmEnable(timer1);
        timerWrite(timer1, 0);
      }

      if(xSemaphoreTake(timer1Semaphore, 0) == pdTRUE) {
        //Serial2.println("  - 10 seconds passed, attempting to connect");
        BLE_Connect(); 
      }
    }
    else {
      /*If the connection fails it will just start scanning again
     * In the case no other receivers are found
     * the device will try to connect to the previously found receiver again */
      BLE_StartScanning(false);
    }
  } while (!BLE_IsConnected());

  //Stop the sleep timer
  timerAlarmDisable(sleepTimer);
  
  //Prepare timer 1 for the loop
  timerAlarmWrite(timer1, TMR1_PERIOD_LOOP, true);

  //Serial2.println("MCU Initialized");
}

void loop() {
  if(BLE_IsConnected() && (xSemaphoreTake(timer1Semaphore, 0) == pdTRUE)) {
    BLE_UpdateBattery(batteryLvl);
    BLE_UpdateSensor(sensorValue);

    if(batteryLvl >= 100) { batteryLvl = 10; }
    else { batteryLvl += 10; }

    if(sensorValue >= 0x08) { sensorValue = 0x01; }
    else { sensorValue++; }
  }
  else if(!BLE_IsConnected()) {
    if(!timerAlarmEnabled(sleepTimer)) { 
      timerAlarmEnable(sleepTimer);
      timerAlarmWrite(timer1, TMR1_PERIOD_CONN, true);
      timerWrite(sleepTimer, 0);
      BLE_StartScanning(true); 
    }
    
    if(xSemaphoreTake(sleepTimerSemaphore, 0) == pdTRUE) {   
      //Serial2.println("Sleep timer triggered"); 
    }
  
    //If at least 1 device is found wait for 10 seconds before connecting
    if(BLE_OneDeviceFound()) {
      //Start 10 second timer if not started already
      if(!timerAlarmEnabled(timer1)) {
        //Serial2.println("Device found, starting timer before connecting");
        timerAlarmEnable(timer1);
        timerWrite(timer1, 0);
      }

      if(xSemaphoreTake(timer1Semaphore, 0) == pdTRUE) {
        //Serial2.println("10 seconds passed, attempting to connect");
        BLE_Connect(); 
        if(BLE_IsConnected()) {
          timerAlarmDisable(sleepTimer);
          timerAlarmWrite(timer1, TMR1_PERIOD_LOOP, true);
        }
      }
    }
    else {
      /*If the connection fails it will just start scanning again
      * In the case no other receivers are found
      * the device will try to connect to the previously found receiver again */
      BLE_StartScanning(false);
    }
  }
}