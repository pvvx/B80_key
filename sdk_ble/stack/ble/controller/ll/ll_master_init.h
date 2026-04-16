/********************************************************************************************************
 * @file    ll_master_init.h
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
#ifndef LL_INIT_H_
#define LL_INIT_H_


extern int blm_create_connection;


/******************************* User Interface  ************************************/

/**
 * @brief      this function is used to initiate link layer initiating state .
 * @param	   none
 * @return     none
 */
void	 	blc_ll_initInitiating_module(void);


/**
 * @brief      this function is used to start connection process .
 * @param[in]  scan_interval - scan interval
 * @param[in]  scan_window - scan window
 * @param[in]  initiator_filter_policy - filter policy
 * @param[in]  adr_type - ADV type
 * @param[in]  mac - MAC
 * @param[in]  own_adr_type - own type
 * @param[in]  conn_min - minimum connection interval
 * @param[in]  connHandle - connect handle
 * @param[in]  conn_min - minimum connection interval
 * @param[in]  conn_max - maximum connection interval
 * @param[in]  conn_latency - connection latency
 * @param[in]  timeout - connection timeout
 * @param[in]  ce_min - not supported
 * @param[in]  ce_max - not supported
 * @return     status, 0x00:  succeed
 * 					   other: failed
 */
ble_sts_t 	blc_ll_createConnection (u16 scan_interval, u16 scan_window, init_fp_t initiator_filter_policy,
							  u8 adr_type, u8 *mac, u8 own_adr_type,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );


/**
 * @brief      this function is used to to cancel create connection process .
 * @param	   none
 * @return     status, 0x00:  succeed
 * 					   other: failed
 */
ble_sts_t 	blc_ll_createConnectionCancel (void);


/**
 * @brief      this function is used to set create connection process timeout.
 * @param	   timeout_ms - timeout threshold
 * @return     status, 0x00:  succeed
 * 					   other: failed
 */
ble_sts_t   blc_ll_setCreateConnectionTimeout (u32 timeout_ms);



#endif /* LL_INIT_H_ */
