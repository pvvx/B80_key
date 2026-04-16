/********************************************************************************************************
 * @file    app_common.h
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
#ifndef APP_COMMON_H_
#define APP_COMMON_H_


#include "vendor/common/user_config.h"


/**
 * @brief  connection device information declaration
 * 		   for Central(master) project only
 */
typedef struct
{
	u16 conn_handle;
	u8 conn_state;
	u8 mac_adrType;
	u8 mac_addr[6];
}dev_info_t;

extern dev_info_t 	cur_conn_device;



/**
 * @brief		this function is used to Check if any Stack(Controller & Host) Initialization error after all BLE initialization done!!!
 * 			    attention that code will stuck in "while(1)" if any error detected in initialization, user need to find what error happens and then fix it.
 * @param[in]	none
 * @return      none
 */
void blc_app_checkControllerHostInitialization(void);


/**
 * @brief      This function is check If IRK(local IRK or peer IRK) valid
 * @param[in]  pIrk - pointer to IRK(local IRK or peer IRK)
 * @return     1: IRK valid
 * 			   0: IRK invalid
 */
bool blc_app_isIrkValid(u8* pIrk);

#endif /* APP_COMMON_H_ */
