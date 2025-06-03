#include <U8g2lib.h>
#include <Wire.h>
#include <OLED.h>

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1327_MIDAS_128X128_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

char writeString[CHAR_PER_MAT-1] = {'0', '0'};
char header[] = "Connected Mats:";
bool matConnected[MAX_MATS];
bool matPressed[MAX_MATS];
uint8_t matPower[MAX_MATS];
uint8_t crntX, crntY, crntMat;

uint8_t x_offset = 0;
uint8_t y_offset = 0;
uint8_t CHAR_H = 13;
uint8_t CHAR_W = 9;
bool crnt_font = true;
uint8_t numberOfConnectedMats = 0;
uint8_t interval = 0;

void OLED_Initialize(void) {
  for(uint8_t i = 0; i < MAX_MATS; i++) { 
    matPower[i] = i*3;
    matPressed[i] = (bool)(i % 2);
  }
  
  //Increase I2C baudrate
  //u8g2.setBusClock(2560000);
  u8g2.begin();
  u8g2.clear();
}

void loop(void) {
  x_offset = (rand()%3);
  y_offset = (rand()%3);
  for(uint8_t i = 0; i < MAX_MATS; i++) { matConnected[i] = false; }
  numberOfConnectedMats = 0;
  if  (interval<MAX_MATS-1) {interval++;}
  else {interval = 0;}
  for(uint8_t i = 0; i < interval; i++) { 
    matConnected[i] = true;
    if (matConnected[i]) { numberOfConnectedMats++; }
  }

  //Move pixels around to improve OLED longevity
  crntY = HEADER_CHAR_H + y_offset;
  crntX = x_offset;
  crntMat = 0;

  //Prepare buffer
  u8g2.clearBuffer();
  
  //Write header and line
  u8g2.setFont(u8g2_font_7x14B_mr);
  u8g2.drawStr(crntX, crntY, header);
  u8g2.drawHLine(crntX, (crntY + 2), 128);
  crntY += 3;

  //Check which font is required
  OLED_SetFontBasedOnMats(numberOfConnectedMats);

  //Send all data to screen
  do {
    crntY += CHAR_H;
    crntX = x_offset;
    do {

      //Check if mat is connected
      if(matConnected[crntMat]){
        //Prepare string, pad singular decimal numbers with an extra 0
        if(crntMat+1 < 10) { sprintf(writeString, "0%u", crntMat+1); }
        else { sprintf(writeString, "%u", crntMat+1); } 

        //Write the string
        u8g2.drawStr(crntX, crntY-1, writeString);
        OLED_DrawBattery(crntX, crntY, matPower[crntMat],false);
        OLED_PressCheck(crntX, crntY, matPressed[crntMat]);
        //Go to the next character cell
        crntX += ( CHAR_W * CHAR_PER_MAT ) + SPACING;
      }
      //Go to the next mat
      crntMat++;
    //Repeat until last mat has been checked/written or until end of line   
    } while ((SCREEN_W-crntX)/(CHAR_W*CHAR_PER_MAT) >= 1 && crntMat < MAX_MATS);
  } while ((SCREEN_H-crntY)/(CHAR_H) >= 1 && crntMat < MAX_MATS);

  
  u8g2.sendBuffer();// transfer internal memory to the display
  delay(250);
}

void OLED_DrawBattery(uint8_t x, uint8_t y, uint8_t batteryPercentage, bool charging) {
//  if (batteryPercentage <= 25) {
//    u8g2.drawTriangle(x+(CHAR_W/5)+(2*CHAR_W), y+3-CHAR_H, x+(CHAR_W*3/5)+(2*CHAR_W), y+3-CHAR_H, x+(2*CHAR_W)+(CHAR_W/2), y+3-((CHAR_H*3)/5));
//    u8g2.drawCircle(x+(2*CHAR_W)+(CHAR_W/2), y-(CHAR_H/10)-2, (CHAR_H/10));
//  }
//  else {
    u8g2.drawFrame((x + 2*CHAR_W),  ((y-CHAR_H) + 3 ), (CHAR_W-2), (CHAR_H-3));
    u8g2.drawBox((x + 2*CHAR_W),  ((y-CHAR_H) + 4 + (((CHAR_H-2)*(101-batteryPercentage))/100)), (CHAR_W-2), (((CHAR_H-4)*batteryPercentage)/100));
//  }
}

void OLED_SetFontBasedOnMats(uint8_t numberOfMats) {
  switch(numberOfMats) {
    case 0: 
      u8g2.clear();
      delay(1000);
      break;
    case 1: case 2: case 3: case 4: case 5: case 6: //0-6
      u8g2.setFont(u8g2_font_inr16_mn);
      CHAR_H = 18;
      CHAR_W = 13;
      break;
    case 7: case 8: case 9: case 10: case 11: case 12://7-12
      u8g2.setFont(u8g2_font_t0_14_mn);
      CHAR_H = 15;
      CHAR_W = 9;
      break;
    case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: //12-24
      u8g2.setFont(u8g2_font_6x10_mn);
      CHAR_H = 11;
      CHAR_W = 6;
      break;
    case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: //25-32
      u8g2.setFont(u8g2_font_5x8_mn);
      CHAR_H = 9;
      CHAR_W = 6;
      break;
  }
}

void OLED_PressCheck(uint8_t x, uint8_t y, uint8_t pressed) {
  if(pressed) {
    u8g2.drawLine(x, y-1, 2*CHAR_W+x-2, y-1);
  }
}