/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <btClient.h>

/*-------------------------------------*/
/*            Definitions              */
/*-------------------------------------*/
#define TMR0_ID           0
#define TMR0_PRE          80
#define TMR0_PERIOD_INIT  5000000  //5 sec
#define TMR0_PERIOD_LOOP  1000000  //1 sec

#define LED_PIN           5

/*-------------------------------------*/
/*              Classes                */
/*-------------------------------------*/
volatile SemaphoreHandle_t timerSemaphore;
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

/*-------------------------------------*/
/*             Variables               */
/*-------------------------------------*/
uint16_t currentSpeed = 1;
hw_timer_t * timer0 = NULL;
uint8_t sensorValue, batteryLvl;

void setup() {
  Serial2.begin(115200);

  sensorValue = 0x01;
  batteryLvl = 5;

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  BLE_Initialize();
  delay(10);
  BLE_StartScanning();
  
  timerSemaphore = xSemaphoreCreateBinary();
  timer0 = timerBegin(TMR0_ID, TMR0_PRE, true);
  timerAttachInterrupt(timer0, &onTimer, true);
  timerAlarmWrite(timer0, TMR0_PERIOD_INIT, true);
  timerAlarmEnable(timer0);

  while(!(xSemaphoreTake(timerSemaphore, 0) == pdTRUE)) {;}

  timerAlarmDisable(timer0);
  timerAlarmWrite(timer0, TMR0_PERIOD_LOOP, true);
  timerAlarmEnable(timer0);
  BLE_Connect();
  
  Serial2.println("MCU Initialized");
}

void loop() {
/* if(xSemaphoreTake(timerSemaphore, 0) == pdTRUE && BLE_IsConnected()) {
    BLE_UpdateBattery(batteryLvl);
    BLE_UpdateSensor(sensorValue);
    digitalWrite(LED_PIN, HIGH);

    if(batteryLvl >= 100) { batteryLvl = 10; }
    else { batteryLvl += 10; }

    if(sensorValue >= 0x08) { sensorValue = 0x01; }
    else { sensorValue = sensorValue << 1; }
    while(true){;}
  }*/

    if(currentSpeed <= 1000) {
      Serial2.print("Current baudrate: ");
      Serial2.println(currentSpeed);
      for(int i = 0 ; i < currentSpeed; i++){
          BLE_UpdateSensorBAUDTEST(currentSpeed);
          BLE_UpdateBattery(currentSpeed);
          delay(1000/currentSpeed);
      }
      currentSpeed++;
    }
}