/*-------------------------------------*/
/*             Libraries               */
/*-------------------------------------*/
#include <Arduino.h>
#include <EEPROM.h>
#include "../lib/btServer/btServer.h"

/*-------------------------------------*/
/*            Definitions              */
/*-------------------------------------*/
#define PIN_U_BTN   34
#define PIN_D_BTN   35
#define PIN_L_BTN   32
#define PIN_R_BTN   33
#define PIN_LED     5
#define PIN_BAT_LVL 4

#define TMR0_ID     0
#define TMR0_PRE    80
#define TMR0_PERIOD 250000
#define oneSecond   1000

/*-------------------------------------*/
/*              Classes                */
/*-------------------------------------*/
volatile SemaphoreHandle_t timerSemaphore;
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }
hw_timer_t * timer = NULL;

/*-------------------------------------*/
/*             Variables               */
/*-------------------------------------*/
uint16_t measuredVoltage;
bool pressed, type;
uint8_t currentSensor;
uint16_t currentSpeed = 1;

/*-------------------------------------*/
/*            Setup code               */
/*-------------------------------------*/
void setup() {
    //Serial2.begin(115200);

    pressed = true;

    pinMode(PIN_U_BTN,   INPUT);
    pinMode(PIN_D_BTN,   INPUT);
    pinMode(PIN_L_BTN,   INPUT);
    pinMode(PIN_R_BTN,   INPUT);
    pinMode(PIN_BAT_LVL, INPUT);
    pinMode(PIN_LED,     OUTPUT);
    //Serial2.println("Pins set");
/*
    timerSemaphore = xSemaphoreCreateBinary();
    timer = timerBegin(TMR0_ID, TMR0_PRE, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, TMR0_PERIOD, true);
    timerAlarmEnable(timer);
*/
    BLE_Initialize();

    while(digitalRead(PIN_U_BTN)) {;}
}
  
/*-------------------------------------*/
/*           Main program              */
/*-------------------------------------*/
void loop() {
    if(currentSpeed <= 1000) {
        for(int i = 0 ; i < currentSpeed; i++){
            updateSensorInputs(currentSpeed);
            updateBatteryLevel(currentSpeed);
            delay(1000/currentSpeed);
        }
        currentSpeed++;
    }
}