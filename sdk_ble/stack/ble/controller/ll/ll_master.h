/********************************************************************************************************
 * @file    ll_master.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef LL_MASTER__H_
#define LL_MASTER__H_


#include "tl_common.h"
#include "stack/ble/ble_common.h"

/**
 * @brief	irq_handler for BLE stack, process system tick interrupt and RF interrupt
 * @param	none
 * @return	none
 */
void		blm_sdk_irq_handler(void);


/**
 * @brief   main_loop for BLE stack, process data and event
 * @param	none
 * @return	none
 */
int 		blm_sdk_main_loop(void);

/**
 * @brief      this function is used to exchange data length for Central Role.
 * @param[in]	opcode - LL Control PDU Opcode
 * @param[in]	maxTxOct - max TX octets of ACL_CONN
 * @return     status, 0x00:  succeed
 * 					   other: error code
 */
ble_sts_t 	blm_ll_exchangeDataLength (u8 opcode, u16 maxTxOct);

/**
 * @brief   LinkLayer initialization after deepSleep retention wake_up in central role.
 * @param	none
 * @return	none
 */
void 		blm_ll_recoverDeepRetention(void);

#endif /* LL__H_ */
