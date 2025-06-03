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
uint8_t sensorValues[MAX_NUM_OF_MATS];

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
  //if(
  if(getSensorValues(sensorValues)) {
    digitalWrite(LED_PIN, HIGH);
  }
    /*) {
    Serial2.println("New sensor values: ");
    for(uint8_t i = 0; i < (MAX_NUM_OF_MATS - 1); i++) {
      Serial2.print(sensorValues[i]);
      Serial2.print(" ");
    }
    Serial2.println(sensorValues[MAX_NUM_OF_MATS-1]);
  }*/
}