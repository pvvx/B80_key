/********************************************************************************************************
 * @file    main.c
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

#if (FEATURE_TEST_MODE == TEST_LL_DATA_LENGTH_EXTENSION_MASTER)


/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */
_attribute_ram_code_ void irq_handler(void)
{
	blm_sdk_irq_handler();
}


/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
int main(void)
{



	cpu_wakeup_init(INTERNAL_CAP_XTAL24M);


	clock_init(SYS_CLK_TYPE);

	gpio_init(1);  //analog resistance will keep available in deepSleep mode, so no need initialize again

	rf_drv_ble_init();

	user_init_normal ();


    irq_enable();

	while (1) {
		main_loop ();
	}
}

#endif  //end of (FEATURE_TEST_MODE == xxx)
