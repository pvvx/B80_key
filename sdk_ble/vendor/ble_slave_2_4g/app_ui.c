/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for BLE SDK
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
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "app.h"
#include "app_att.h"
#include "app_ui.h"


/*----------------------------------------------------------------------------*/
/*------------- OTA  Function                                 ----------------*/
/*----------------------------------------------------------------------------*/
#if (BLE_OTA_SERVER_ENABLE)

_attribute_data_retention_ int 	ota_is_working = 0;

/**
 * @brief      this function is used to register the function for OTA start.
 * @param[in]  none
 * @return     none
 */
void app_enter_ota_mode(void)
{
	#if(UI_LED_ENABLE)
	gpio_write(GPIO_LED_BLUE, 1);
	gpio_write(GPIO_LED_GREEN, 1);
	#endif
	ota_is_working = 1;
}


/**
 * @brief       no matter whether the OTA result is successful or fail.
 *              code will run here to tell user the OTA result.
 * @param[in]   result    OTA result:success or fail(different reason)
 * @return      none
 */
void app_ota_result(int result)
{
	#if (0)  //this is only for debug, should disable in mass production code
		if(result == OTA_SUCCESS){  //OTA success
			gpio_write(GPIO_LED_BLUE, 0);
			sleep_us(1000000);  //led off for 1 second
			gpio_write(GPIO_LED_BLUE, 1);
			sleep_us(1000000);  //led on for 1 second
		}
		else{  //OTA fail

		#if 0 //this is only for debug, can not use this in application code
			irq_disable();

			while(1)
			{
				gpio_toggle(GPIO_LED_BLUE);
				sleep_us(1000000);  //led on for 1 second
			}
		#endif

		}
	#endif
}


#endif
