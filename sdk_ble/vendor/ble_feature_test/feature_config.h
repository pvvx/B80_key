/********************************************************************************************************
 * @file    feature_config.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    12,2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef FEATURE_CONFIG_H_
#define FEATURE_CONFIG_H_



/////////////////// TEST FEATURE SELECTION /////////////////////////////////
//ble link layer test
#define TEST_ADVERTISING_IN_CONN_SLAVE_ROLE				3
#define TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE        4



//power test
#define TEST_POWER_ADV									10 //ADV power


//gatt secure test
#define TEST_GATT_SECURITY								21 //If testing SECURITY, such as Passkey Entry or Numric_Comparison, we use the remote control board for testing

//slave LL data length exchange test
#define TEST_LL_DATA_LENGTH_EXTENSION_SLAVE				22 //Slave LL DLE test

//master LL data length exchange test
#define TEST_LL_DATA_LENGTH_EXTENSION_MASTER			23 //Master LL DLE test

 //phy test
#define TEST_BLE_PHY									32 // BQB PHY_TEST demo

#define TEST_MD_MASTER									33 //Master more data test

#define TEST_MD_SLAVE									34 //Slave more data test


#define	TEST_PHY_CONN					                60 //  2M/Coded PHY used on Legacy_ADV and Connection



//some simple function test
#define TEST_USER_BLT_SOFT_TIMER						100 //software timer


#define TEST_USER_BLT_SOFT_UART							130 //software UART




#define TEST_FEATURE_BACKUP								200




#define FEATURE_TEST_MODE								TEST_MD_MASTER






#endif /* FEATURE_CONFIG_H_ */
