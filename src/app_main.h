/********************************************************************************************************
 * @file    app.h
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
#ifndef _APP_H
#define _APP_H

// bits: wrk.ble_connected
enum {
	CONNECTED_FLG_ENABLE = 0,
	CONNECTED_FLG_PAR_UPDATE = 1,	// see app_event_handler()
	CONNECTED_FLG_BONDING = 2,
	CONNECTED_FLG_DATA_LENGTH_EXCHANGE =3, // see app_event_handler()
	CONNECTED_FLG_RESET_OF_DISCONNECT = 7
} CONNECTED_FLG_BITS_e;

typedef struct {
	u32 measureTick;
	u32 measureInterval;
#ifdef GPIO_KEY
	u32 timerWakeUpTick; // timer max 256 sec (2pow32/CLOCK_16M_SYS_TIMER_CLK_1US - 12000000, 268.435456 - 12.0 sec)
	u32 keyCounter;
#endif
#if SWS_PRINTF // debug
	u16 advCnt;
#endif
	u8  startMeasure;
	u8  otaIsWorking;
	u8	bleConnected;
#ifdef GPIO_KEY
	u8  keyPressed;
#endif
} work_t;

extern work_t wrk;

/**
 * @brief		user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */

void user_init_normal(void);

/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
void user_init_deepRetn(void);

/**
 * @brief      flash protection operation, including all locking & unlocking for application
 * 			   handle all flash write & erase action for this demo code. use should add more more if they have more flash operation.
 * @param[in]  flash_op_evt - flash operation event, including application layer action and stack layer action event(OTA write & erase)
 * 			   attention 1: if you have more flash write or erase action, you should should add more type and process them
 * 			   attention 2: for "end" event, no need to pay attention on op_addr_begin & op_addr_end, we set them to 0 for
 * 			   			    stack event, such as stack OTA write new firmware end event
 * @param[in]  op_addr_begin - operating flash address range begin value
 * @param[in]  op_addr_end - operating flash address range end value
 * 			   attention that, we use: [op_addr_begin, op_addr_end)
 * 			   e.g. if we write flash sector from 0x10000 to 0x20000, actual operating flash address is 0x10000 ~ 0x1FFFF
 * 			   		but we use [0x10000, 0x20000):  op_addr_begin = 0x10000, op_addr_end = 0x20000
 * @return     none
 */
void app_flash_protection_operation(u8 flash_op_evt, u32 op_addr_begin, u32 op_addr_end);

/**
 * @brief     BLE main loop
 * @param[in]  none.
 * @return     none.
 */
void main_loop (void);

/**
 * @brief      write callback of Attribute of TelinkSppDataClient2ServerUUID
 * @param[in]  para - rf_packet_att_write_t
 * @return     0
 */
int app_onWriteData(void *para);
//int app_onReadData(void *para);

#endif /* APP_H_ */
