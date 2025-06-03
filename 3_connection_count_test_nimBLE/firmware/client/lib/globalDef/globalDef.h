/*
 * File:    globalDef.h
 * Author:  Arjan Lemmens
 * Created on August 5, 2022, 9:45 PM
 * Version: 0.21
 */

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
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*----------------------------------------------------------------------------------*/
/*                                  Bluetooth LE                                    */
/*      GROUP UUIDS ARE NOT ALLOWED TO START WITH THE SAME 2 HEXADECIMAL VALUES     */
/*----------------------------------------------------------------------------------*/
/*      Name                    UUID                                      Properties                  Data structure*/
#define G0_SERVICE_UUID         "0ccd07db-a16d-4d32-8d29-bad3defb895d"          
#define G1_SERVICE_UUID         "7d6700ae-838b-4c61-8f73-fa48192fbdef"
#define G2_SERVICE_UUID         "8e39dae8-a88d-4b3a-ba63-33c1dc24fa56"
#define G3_SERVICE_UUID         "fa9baeb9-d50b-4e83-8a0c-bf355901e142"
  #define LOCAL_GROUP_UUID        "2a23"                                  //READ | BROADCAST
  #define CONN_DEVS_CHAR_UUID     "e32be9d9-b62c-407c-adcc-1fe45f6c00b3"  //READ | BROADCAST          uint8_t
  #define FREE_DEVS_CHAR_UUID     "4f5361fd-1ac6-4669-a086-e056869fc230"  //READ | BROADCAST | WRITE  uint8_t : Mat8 | Mat7 | Mat6 | Mat5 | Mat4 | Mat3 | Mat2 | Mat1
const std::string groupUUID[] = { G0_SERVICE_UUID,
                                  G1_SERVICE_UUID,
                                  G2_SERVICE_UUID,
                                  G3_SERVICE_UUID};

#define CLIENT_0_SERVICE_UUID   "66d038d4-6853-411a-bc30-8411280ebbd6"
#define CLIENT_1_SERVICE_UUID   "821be378-34f3-4072-bdee-1812c84ce583"
#define CLIENT_2_SERVICE_UUID   "869b751f-2b30-47f1-a002-4c6b2653355a"
#define CLIENT_3_SERVICE_UUID   "ce6960aa-5071-44b8-81f5-b82885fc4780"
#define CLIENT_4_SERVICE_UUID   "d22f8411-2fa6-420a-b116-22d4ae377c83"
#define CLIENT_5_SERVICE_UUID   "bb2a9121-a17b-44f5-b950-b59fe327c0c4"
#define CLIENT_6_SERVICE_UUID   "927bae75-74ba-4584-b699-1ba0f1daf422"
#define CLIENT_7_SERVICE_UUID   "47f665c4-8d67-47da-b06a-0809698cc441"
  #define REMOTE_ID_CHAR_UUID     "2a23"                                  //WRITE | WRITE_NR
  #define SENSOR_CHAR_UUID        "2a57"                                  //WRITE | WRITE_NR
  #define BAT_LVL_CHAR_UUID       "2a19"                                  //WRITE | WRITE_NR
const std::string clientUUID[] = {  CLIENT_0_SERVICE_UUID,
                                    CLIENT_1_SERVICE_UUID,
                                    CLIENT_2_SERVICE_UUID,
                                    CLIENT_3_SERVICE_UUID,
                                    CLIENT_4_SERVICE_UUID,
                                    CLIENT_5_SERVICE_UUID,
                                    CLIENT_6_SERVICE_UUID,
                                    CLIENT_7_SERVICE_UUID};

#define COMPANY_IDENTIFIER_UUID "O" //ffff  
const std::string manufacturerUUID(COMPANY_IDENTIFIER_UUID);

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
#define NUM_OF_GROUPS           4
#define MAX_NUM_OF_MATS         8
#define MAT_BLE_NAME            "SdMat"
#define REC_BLE_NAME            "SdRec"
#define NEW_DATA_BIT            0x80
#define UUID_128_LENGTH         37
#define AMOUNT_DATA_INDEX       27
#define GROUP_DATA_INDEX        28
#define UUID_START_INDEX        26
#define UUID_STOP_INDEX         11
#define CONN_CHECK_BIT          0x80



#endif //_GLOBALDEF_H