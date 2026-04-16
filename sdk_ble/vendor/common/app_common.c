/********************************************************************************************************
 * @file    app_common.c
 *
 * @brief   This is the source file for BLE SDK
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
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"


/**
 * @brief		this function is used to Check if any Stack(Controller & Host) Initialization error after all BLE initialization done.
 * 			    attention that code will stuck in "while(1)" if any error detected in initialization, user need to find what error happens and then fix it.
 * @param[in]	none
 * @return      none
 */
void blc_app_checkControllerHostInitialization(void)
{
	u32 error_code1 = blc_contr_checkControllerInitialization();
	u32 error_code2 = blc_host_checkHostInitialization();
	if(error_code1 != INIT_SUCCESS || error_code2 != INIT_SUCCESS){
		/* It's recommended that user set some UI alarm to know the exact error, e.g. LED shine, print log */
		printf("[APP][INI] Stack INIT ERROR 0x%04x, 0x%04x", error_code1, error_code2);
		#if (UI_LED_ENABLE)
			#ifdef GPIO_LED_RED
				gpio_write(GPIO_LED_RED, LED_ON_LEVEL);
			#endif
		#endif

		while(1);
	}
}





/**
 * @brief      This function is check If IRK(local IRK or peer IRK) valid
 * @param[in]  pIrk - pointer to IRK(local IRK or peer IRK)
 * @return     1: IRK valid
 * 			   0: IRK invalid
 */
bool blc_app_isIrkValid(u8* pIrk)
{

	u8 byte_all_0xFF[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	u8 byte_all_0x00[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	/* IRK valid: not 16 0x00, not 16 0xFF */
	if(memcmp(pIrk, byte_all_0xFF, 16) && memcmp(pIrk, byte_all_0x00, 16)){
		return TRUE;
	}
	else{
		return FALSE;
	}
}
