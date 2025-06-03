/*----------------------------------------------------------------------------------*/
/* This header files is created to document all global definitions.                 */
/* This means that this file will be used in each custom library                    */
/* And in both the Master receiver firmware as well as the mat controller firmware  */
/*----------------------------------------------------------------------------------*/

#ifndef _GLOBALDEF_H
#define _GLOBALDEF_H

/*----------------------------------------------------------------------------------*/
/*                                Global libraries                                  */
/*----------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <EEPROM.h>

/*----------------------------------------------------------------------------------*/
/*                                    General                                       */
/*----------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION        0.1

/*----------------------------------------------------------------------------------*/
/*                                  Bluetooth LE                                    */
/*----------------------------------------------------------------------------------*/
//Services
#define MAT_G0_SERVICE_UUID     "0ccd07db-a16d-4d32-8d29-bad3defb895d"
#define MAT_G1_SERVICE_UUID     "7d6700ae-838b-4c61-8f73-fa48192fbdef"
#define MAT_G2_SERVICE_UUID     "8e39dae8-a88d-4b3a-ba63-33c1dc24fa56"
#define MAT_G3_SERVICE_UUID     "fa9baeb9-d50b-4e83-8a0c-bf355901e142"
#define BAT_SERVICE_UUID        "180f"

//Characteristics
#define SENSOR_CHAR_UUID        "a611d5ff-7fa4-455d-aae8-a37e4fbd42ed"
#define BAT_LVL_CHAR_UUID       "2a19"
#define SYS_ID_CHAR_UUID        "2a23"
#define DEVICE_NAME_CHAR_UUID   "2a00" 

//Other
#define COMPANY_IDENTIFIER      "ffff"  

/*----------------------------------------------------------------------------------*/
/*                                   Sensors                                        */
/*----------------------------------------------------------------------------------*/
#define BTN_U_BIT               0x08
#define BTN_D_BIT               0x04
#define BTN_L_BIT               0x02
#define BTN_R_BIT               0x01

/*----------------------------------------------------------------------------------*/
/*                                 Networking                                       */
/*----------------------------------------------------------------------------------*/
#define NUMBER_OF_GROUPS        4

#endif //_GLOBALDEF_H